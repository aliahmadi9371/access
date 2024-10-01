#include "mqttclass.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include "encryption/encrypt.h"

using namespace std;

MqttClass::MqttClass(QObject *parent) : QObject(parent)
{
    Init();

    return;
}

void MqttClass::Init()
{
    userName = "";

    client = nullptr;
    checkServerTimer = nullptr;

    subLogResp = nullptr;
    bSubLogResp = false;

    first = false;

    subInpResp = nullptr;
    bSubInpResp = false;

    subApbResp = nullptr;
    bSubApbResp = false;

    subAuthResp = nullptr;
    bSubAuthResp = false;

    //------------------------
    //------------------------
    //------------------------
    subAuthAndResp = nullptr;
    bSubAuthAndResp = false;

    subApbAndResp = nullptr;
    bSubApbAndResp = false;

    subLogAndResp = nullptr;
    bSubLogAndResp = false;
    //------------------------
    //------------------------
    //------------------------

    subStartStopSyncResp = nullptr;
    bSubStartStopSyncResp = false;

    subTimezonesTableSyncResp = nullptr;
    bSubTimezonesTableSyncResp = false;

    subAccessTimezonesTableSyncResp = nullptr;
    bSubAccessTimezonesTableSyncResp = false;

    subAccessShiftsTableSyncResp = nullptr;
    bSubAccessShiftsTableSyncResp = false;

    subtag_UsersTableSyncResp = nullptr;
    bSubtag_UsersTableSyncResp = false;

    subtag_TerminalsTableSyncResp = nullptr;
    bSubtag_TerminalsTableSyncResp = false;

    //------------------------
    subHolidaysTableSyncResp = nullptr;
    bSubHolidaysTableSyncResp = false;

    subHolidaysSetsTableSyncResp = nullptr;
    bSubHolidaysSetsTableSyncResp = false;

    subHolidaysGroupsTableSyncResp = nullptr;
    bSubHolidaysGroupsTableSyncResp = false;
    //--------------------------

    subtag_HolidaysTableSyncResp = nullptr;
    bSubtag_HolidaysTableSyncResp = false;

    subtag_AccessPairsTableSyncResp = nullptr;
    bSubtag_AccessPairsTableSyncResp = false;

    subtagsTableSyncResp = nullptr;
    bSubtagsTableSyncResp = false;

    subLock = nullptr;
    bSubLock = false;

    statusTimer = nullptr;

    bMqttThreadRun = false;
    bMqttThreadJobRun = false;

    ping_proc = false;

    connecting = false;

    Mqtt_Status = false;

    ping_count = 0;

    mqtt_status = -1;
    mqtt_status_old = -1;

    first_status = false;

    checker_mqtt_connecting = nullptr;

    connect_disconnect_counter = 0;

    delay_Timer = nullptr;
    delay = false;



    return;
}

void MqttClass::Config()
{
    client = new QMqttClient(this);

    checkServerTimer = new QTimer(this);
    checkServerTimer->setSingleShot(true);
    checkServerTimer->setInterval((gl_Ini_MqttKeepAlive * 1000) + 1500);
    checkServerTimer->stop();
    QObject::connect(checkServerTimer, &QTimer::timeout, this, &MqttClass::on_checkServerTimerTimeout, Qt::QueuedConnection);

//    qRegisterMetaType<QMqttTopicName>("QMqttTopicName");
//    QObject::connect(client, &QMqttClient::messageReceived, this, &MqttClass::on_messageReceived, Qt::QueuedConnection);

    qRegisterMetaType<QMqttClient::ClientState>("ClientState");
    QObject::connect(client, &QMqttClient::stateChanged, this, &MqttClass::on_stateChanged_client, Qt::QueuedConnection);

    //client->requestPing()
    QObject::connect(client, &QMqttClient::pingResponseReceived, this, &MqttClass::on_pingResponseReceived, Qt::QueuedConnection);


    statusTimer = new QTimer(this);
    statusTimer->setSingleShot(true);
    statusTimer->setInterval(MQTT_STATUS_TIMERTIMEOUT_SEC * 1000);
    statusTimer->stop();
    QObject::connect(statusTimer, &QTimer::timeout, this, &MqttClass::on_statusTimerTimeout, Qt::QueuedConnection);


    checker_mqtt_connecting = new QTimer(this);
    checker_mqtt_connecting->setSingleShot(true);
    checker_mqtt_connecting->setInterval(3 * 1000);
    checker_mqtt_connecting->stop();
    QObject::connect(checker_mqtt_connecting, &QTimer::timeout, this, &MqttClass::on_checker_mqtt_connecting, Qt::QueuedConnection);

    delay_Timer = new QTimer(this);
    delay_Timer->setSingleShot(true);
    delay_Timer->stop();
    QObject::connect(delay_Timer, &QTimer::timeout, this, &MqttClass::on_Delay_Timeout, Qt::QueuedConnection);

    return;
}

void MqttClass::on_StartWorker()
{
    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";
        qInfo() << MQTTCLASS << "on_StartWorker--> " << "start mqtt thread class";
        qInfo() << MQTTCLASS << "on_StartWorker--> " << "mqtt thread id = " << QThread::currentThreadId();
    #endif
    Config();
    MqttConfig();

    #ifdef QT_DEBUG
        qInfo() << MQTTCLASS << "on_StartWorker: emit this->StartedWorker();";
    #endif

    bMqttThreadRun = true;
    emit this->StartedWorker();

    return;
}

void MqttClass::on_StartMqttJob()
{
    if(bMqttThreadRun){


        qInfo() << "";
        qInfo() << "";
        qInfo() << MQTTCLASS << "************************                                                                                                *************************";
        qInfo() << MQTTCLASS << "*************************************************************************************************************************************************";
        qInfo().noquote() << MQTTCLASS << "************************************************************ START  Full Application ************************************************************";
        qInfo() << MQTTCLASS << "*************************************************************************************************************************************************";
        qInfo() << MQTTCLASS << "************************                                                                                                *************************";
        qInfo() << "";
        qInfo() << "";
        qInfo() << "************************ last all app ************************";
        qInfo() << "";
        qInfo() << "";

//        qInfo() << "new app";
//        qInfo() << "";
//        qInfo() << "";

        MqttConnect();

        checker_mqtt_connecting->start();
        checkServerTimer->start();

        bMqttThreadJobRun = true;
        emit this->sig_StartedMqttJob();
//        statusTimer->start();
    }

    return;
}

void MqttClass::MqttHostConfig()
{
    client->setProtocolVersion(QMqttClient::MQTT_3_1);

    client->setHostname(gl_Ini_SrvIp);
    client->setPort(gl_Ini_SrvPort);
    client->setClientId(gl_Eth_MacAddr);


    client->setCleanSession(true);
//    QMqttConnectionProperties con_prop;
//    con_prop.setSessionExpiryInterval(0xFFFFFFFF);
//    client->setConnectionProperties(con_prop);

    userName = gl_Eth_MacAddr;
    userName = userName.remove(":");

    client->setUsername(userName);
    qDebug() << "user name = " << userName;
    std::string msg = userName.toStdString();
    std::string key = "IroTeam";
    std::string encrypted_msg = encrypt(msg, key);


    #ifdef QT_DEBUG
        qInfo() << MQTTCLASS << "MqttHostConfig: mqtt username -> " << userName;
        qInfo() << MQTTCLASS << "MqttHostConfig: mqtt password -> " << QString::fromStdString(encrypted_msg);
    #endif

    client->setPassword(QString::fromStdString(encrypted_msg));


    client->setKeepAlive(gl_Ini_MqttKeepAlive);

    return;
}

void MqttClass::MqttWillConfig()
{
    client->setWillMessage(gl_Ini_WillMsg_Dc.toLocal8Bit());
    client->setWillQoS(gl_Ini_WillQos);
    client->setWillRetain(gl_Ini_WillRetain);
    QString WillTopic = QString("IroTeam/%1/STATUS").arg(userName);
    client->setWillTopic(WillTopic);

    return;
}

void MqttClass::MqttConfig()
{
    MqttHostConfig();
    MqttWillConfig();

    mqtt_status = 0;
    mqtt_status_old = 0;

    return;
}

void MqttClass::MqttConnect()
{
    qInfo().noquote() << MQTTCLASS << "MqttConnect --> " << "connect to " << gl_Ini_SrvIp + QString(":%1").arg(QString::number(gl_Ini_SrvPort));
    client->connectToHost();

    return;
}

void MqttClass::MqttDisconnect()
{
    qInfo().noquote() << MQTTCLASS << "MqttDisconnect --> " << "disconnect from" << gl_Ini_SrvIp + QString(":%1").arg(QString::number(gl_Ini_SrvPort));
    client->disconnectFromHost();

    return;
}

void MqttClass::on_stateChanged_client(QMqttClient::ClientState state)
{
    switch(state){
        case QMqttClient::ClientState::Disconnected:{
            qInfo().noquote() << MQTTCLASS << "on_stateChanged_client--> ClientState::Disconnected From" << QString("[%1:%2]").arg(gl_Ini_SrvIp).arg(QString::number(gl_Ini_SrvPort));
            statusTimer->stop();
            Delay_ms(5000);
            MqttConnect();
            break;
        }
        case QMqttClient::ClientState::Connecting:{
            qInfo().noquote() << MQTTCLASS << "on_stateChanged_client--> ClientState::Connecting to" << QString("[%1:%2]").arg(gl_Ini_SrvIp).arg(QString::number(gl_Ini_SrvPort));
            break;
        }
        case QMqttClient::ClientState::Connected:{
            qInfo().noquote() << MQTTCLASS << "on_stateChanged_client--> ClientState::Connected to" << QString("[%1:%2]").arg(gl_Ini_SrvIp).arg(QString::number(gl_Ini_SrvPort));
            on_statusTimerTimeout();
            mqtt_subscribe();
            break;
        }
        default:{
            break;
        }
    }
//    mqtt_check_subscribe_status();

    return;
}

void MqttClass::on_checker_mqtt_connecting()
{
    checker_mqtt_connecting->stop();
    mqtt_check_subscribe_status();
    checker_mqtt_connecting->start();

    return;
}

void MqttClass::mqtt_check_subscribe_status()
{
    if(client->state() == QMqttClient::ClientState::Connected){
        if(mqtt_check_subscribe()){
            mqtt_status = 2;
            if(mqtt_status != mqtt_status_old){
                qInfo() << MQTTCLASS << "mqtt_check_subscribe_status--> successfull subscription";
                mqtt_status_old = mqtt_status;
                emit this->sig_CyMqttStatus(mqtt_status);
            }
        }
        else{
            mqtt_status = 1;
            if(mqtt_status != mqtt_status_old){
                qInfo() << MQTTCLASS << "mqtt_check_subscribe_status--> unsuccessfull subscription";
                mqtt_status_old = mqtt_status;
                emit this->sig_CyMqttStatus(mqtt_status);
            }
        }
    }
    else{
        mqtt_status = 0;
        if(mqtt_status != mqtt_status_old){
            qInfo() << MQTTCLASS << "mqtt_check_subscribe_status--> not connected to server";
            mqtt_status_old = mqtt_status;
            emit this->sig_CyMqttStatus(mqtt_status);
        }
    }

    return;
}

bool MqttClass::mqtt_check_subscribe()
{
    if(client->state() == QMqttClient::ClientState::Connected){
        if(
            mqtt_check_connectivity()
          ){
            Mqtt_Status = true;
            return true;
        }
        else{
            Mqtt_Status = false;
            return false;
        }
    }
    else{
        Mqtt_Status = false;
        return false;
    }
}

bool MqttClass::mqtt_check_connectivity()
{
    if(
                bSubLogResp && subLogResp && (subLogResp->state() == QMqttSubscription::Subscribed) &&
                bSubInpResp && subInpResp && (subInpResp->state() == QMqttSubscription::Subscribed) &&
                bSubApbResp && subApbResp && (subApbResp->state() == QMqttSubscription::Subscribed) &&
                bSubAuthResp && subAuthResp && (subAuthResp->state() == QMqttSubscription::Subscribed) &&

                //------------------------
                //------------------------
                //------------------------
                bSubAuthAndResp && subAuthAndResp && (subAuthAndResp->state() == QMqttSubscription::Subscribed) &&
                bSubApbAndResp && subApbAndResp && (subApbAndResp->state() == QMqttSubscription::Subscribed) &&
                bSubLogAndResp && subLogAndResp && (subLogAndResp->state() == QMqttSubscription::Subscribed) &&
                //------------------------
                //------------------------
                //------------------------

                bSubStartStopSyncResp && subStartStopSyncResp && (subStartStopSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubTimezonesTableSyncResp && subTimezonesTableSyncResp && (subTimezonesTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubAccessTimezonesTableSyncResp && subAccessTimezonesTableSyncResp && (subAccessTimezonesTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubAccessShiftsTableSyncResp && subAccessShiftsTableSyncResp && (subAccessShiftsTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubtag_UsersTableSyncResp && subtag_UsersTableSyncResp && (subtag_UsersTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubtag_TerminalsTableSyncResp && subtag_TerminalsTableSyncResp && (subtag_TerminalsTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                //--------------
                bSubHolidaysTableSyncResp  && subHolidaysTableSyncResp && (subHolidaysTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubHolidaysSetsTableSyncResp  && subHolidaysTableSyncResp && (subHolidaysTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubHolidaysGroupsTableSyncResp  && subHolidaysTableSyncResp && (subHolidaysTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                //--------------
                bSubtag_HolidaysTableSyncResp  && subtag_HolidaysTableSyncResp && (subtag_HolidaysTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubtag_AccessPairsTableSyncResp && subtag_AccessPairsTableSyncResp && (subtag_AccessPairsTableSyncResp->state() == QMqttSubscription::Subscribed) &&
                bSubtagsTableSyncResp && subtagsTableSyncResp && (subtagsTableSyncResp->state() == QMqttSubscription::Subscribed) &&

                bSubLock && subLock &&  (subLock->state() == QMqttSubscription::Subscribed)

        ){
//        qInfo() << MQTTCLASS << "mqtt_check_connectivity--> trueeee";
        return true;
    }
    else{
//        qInfo() << MQTTCLASS << "mqtt_check_connectivity--> falseee";
        return false;
    }

}

void MqttClass::on_checkServerTimerTimeout()
{
    checkServerTimer->stop();

    QApplication::processEvents(QEventLoop::AllEvents);
    if(client->state() == QMqttClient::ClientState::Connected){
        if(ping_proc){
            ping_count++;
            qInfo().noquote() << MQTTCLASS << "on_checkServerTimerTimeout--> Failed Ping From" << QString("[%1:%2]").arg(gl_Ini_SrvIp).arg(QString::number(gl_Ini_SrvPort));
            if(ping_count > 0){
                ping_count = 0;
                ping_proc = false;
                MqttDisconnect();
            }
        }
        else{
            ping_count = 0;
            ping_proc = true;
            client->requestPing();
        }
    }


    checkServerTimer->start();

    return;
}

void MqttClass::on_pingResponseReceived()
{
    ping_proc = false;

    return;
}

void MqttClass::mqtt_unsubscribe()
{
    qInfo().noquote() << MQTTCLASS << "mqtt_unsubscribe--> ";
    MqttUnSubLogResp();
    MqttUnSubInpResp();
    MqttUnSubAuthResp();
    MqttUnSubApbResp();

    //------------------------
    //------------------------
    //------------------------
    MqttUnSubAuthAndResp();
    MqttUnSubApbAndResp();
    MqttUnSubLogAndResp();
    //------------------------
    //------------------------
    //------------------------

    MqttUnSubTimezonesTableSync();
    MqttUnSubAccessTimezonesTableSync();
    MqttUnSubAccessShiftsTableSync();
    MqttUnSubtag_TerminalsTableSync();
    MqttUnSubtag_UsersTableSync();
    //------------------
    MqttUnSubHolidaysTableSync();
    MqttUnSubHolidaysSetsTableSync();
    MqttUnSubHolidaysGroupsTableSync();
    //------------------
    MqttUnSubtag_HolidaysTableSync();
    MqttUnSubtag_AccessPairsTableSync();
    MqttUnSubtagsTableSync();
    MqttUnSubStartStopSync();


    MqttUnSubLock();

    return;
}


void MqttClass::mqtt_subscribe()
{
    qInfo() << MQTTCLASS << "mqtt_subscribe--> ";
    //Table thread to Mqtt thread and Mqtt thread to Table thread
//    if(!subLogResp)
        MqttSubLogResp();
//    else if(!bSubLogResp && (subLogResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubLogResp();

//    if(!subInpResp)
        MqttSubInpResp();
//    else if(!bSubInpResp && (subInpResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubInpResp();

    //Auth thread to Mqtt thread and Mqtt thread to Auth thread
//    if(!subApbResp)
        MqttSubApbResp();
//    else if(!bSubApbResp && (subApbResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubApbResp();

//    if(!subAuthResp)
        MqttSubAuthResp();
//    else if(!bSubAuthResp && (subAuthResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubAuthResp();

    //------------------
    //------------------
    //------------------
//    if(!subAuthAndResp)
        MqttSubAuthAndResp();
//    else if(!bSubAuthAndResp && (subAuthAndResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubAuthAndResp();

//    if(!subApbAndResp)
        MqttSubApbAndResp();
//    else if(!bSubApbAndResp && (subApbAndResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubApbAndResp();

//    if(!subLogAndResp)
        MqttSubLogAndResp();
//    else if(!bSubLogAndResp && (subLogAndResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubLogAndResp();
    //------------------
    //------------------
    //------------------


    //Job thread to Mqtt thread and Mqtt thread to Job thread
//    if(!subTimezonesTableSyncResp)
        MqttSubTimezonesTableSync();
//    else if(!bSubTimezonesTableSyncResp && (subTimezonesTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubTimezonesTableSync();

//    if(!subAccessTimezonesTableSyncResp)
        MqttSubAccessTimezonesTableSync();
//    else if(!bSubAccessTimezonesTableSyncResp && (subAccessTimezonesTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubAccessTimezonesTableSync();

//    if(!subAccessShiftsTableSyncResp)
        MqttSubAccessShiftsTableSync();
//    else if(!bSubAccessShiftsTableSyncResp && (subAccessShiftsTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubAccessShiftsTableSync();

//    if(!subtag_UsersTableSyncResp)
        MqttSubtag_UsersTableSync();
//    else if(!bSubtag_UsersTableSyncResp && (subtag_UsersTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubtag_UsersTableSync();

//    if(!subtag_TerminalsTableSyncResp)
        MqttSubtag_TerminalsTableSync();
//    else if(!bSubtag_TerminalsTableSyncResp && (subtag_TerminalsTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubtag_TerminalsTableSync();


    //-------------------------------
//    if(!subHolidaysTableSyncResp)
        MqttSubHolidaysTableSync();
//    else if(!bSubHolidaysTableSyncResp && (subHolidaysTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubHolidaysTableSync();


//    if(!subHolidaysSetsTableSyncResp)
        MqttSubHolidaysSetsTableSync();
//    else if(!bSubHolidaysSetsTableSyncResp && (subHolidaysSetsTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubHolidaysSetsTableSync();

//    if(!subHolidaysGroupsTableSyncResp)
        MqttSubHolidaysGroupsTableSync();
//    else if(!bSubHolidaysGroupsTableSyncResp && (subHolidaysGroupsTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubHolidaysGroupsTableSync();
    //-------------------------------

//    if(!subtag_HolidaysTableSyncResp)
        MqttSubtag_HolidaysTableSync();
//    else if(!bSubtag_HolidaysTableSyncResp && (subtag_HolidaysTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubtag_HolidaysTableSync();

//    if(!subtag_AccessPairsTableSyncResp)
        MqttSubtag_AccessPairsTableSync();
//    else if(!bSubtag_AccessPairsTableSyncResp && (subtag_AccessPairsTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubtag_AccessPairsTableSync();

//    if(!subtagsTableSyncResp)
        MqttSubtagsTableSync();
//    if(!bSubtagsTableSyncResp && (subtagsTableSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubtagsTableSync();

//    if(!subStartStopSyncResp)
        MqttSubStartStopSync();
//    else if(!bSubStartStopSyncResp && (subStartStopSyncResp->state() == QMqttSubscription::Unsubscribed))
//        MqttSubStartStopSync();


        MqttSubLock();

    return;
}

void MqttClass::on_statusTimerTimeout()
{
    statusTimer->stop();
    //qDebug() << "on_statusTimerTimeout";
    if(mqtt_check_subscribe()){
//        qInfo() << MQTTCLASS << "on_statusTimerTimeout--> publish IroTeam is Connected";
        QString msg = "";
        if(!first_status){
            first_status = true;
            msg = gl_Ini_WillMsg_Fc;
        }
        else{
            msg = gl_Ini_WillMsg_Cc;
        }
            //qDebug() << "msg = " << msg;
            QString topic = QString("IroTeam/%1/STATUS").arg(userName);
            MqttPub(topic, msg, 2);
    }

    statusTimer->start();

    return;
}

void MqttClass::MqttPub(QString topic, QString msg, quint8 qos)
{
    if(client->state() == QMqttClient::ClientState::Connected){
        QMqttTopicName topicName;
        topicName.setName(topic);
        client->publish(topicName, msg.toLocal8Bit(), qos , false);
    }
    else{
        qInfo() << MQTTCLASS << "MqttPub--> client is not connected to server";
    }
}

//############################################################################################################################
//############################################################################################################################
//##################################################  Log to Server Sig from Table Handler Calss  ############################
void MqttClass::on_sig_SendToServerLog_slot(Log log)
{

//    qDebug() << "on_sig_SendToServerLog_slot " << bSubLogResp << " , " << mqtt_check_subscribe();
    if(bSubLogResp && mqtt_check_subscribe())
        MqttPubLog(log.userUniqueId, log.readerExpId, log.readerId, log.isHoliday, log.isUser, log.errorCode, log.errorString, log.reqDateTime.toString("yyyy-MM-dd hh:mm:ss"), log.UUID, log.isSuccess);

    return;
}

//##################################################  Log to Server  #########################################################
void MqttClass::MqttPubLog(int userUniqueId, int readerExpId, int readerId, bool isHoliday, bool isUser, int errorCode, QString errorString, QString reqDateTime, QString UUID, bool isSuccess)
{
    QString msg = "{\"userUniqueId\":%1, \"readerExpId\":%2, \"readerId\":%3, \"isHoliday\":%4, \"isUser\":%5, \"errorCode\":%6, \"errorString\":\"%7\", \"reqDateTime\":\"%8\", \"isSuccess\":%9, \"UUID\":\"%10\"}";
    msg = msg.arg(QString::number(userUniqueId));
    msg = msg.arg(QString::number(readerExpId));
    msg = msg.arg(QString::number(readerId));
    msg = msg.arg(QString::number(isHoliday));
    msg = msg.arg(QString::number(isUser));
    msg = msg.arg(QString::number(errorCode));
    msg = msg.arg(errorString);
    msg = msg.arg(reqDateTime);
    if(isSuccess){
        msg = msg.arg(QString::number(1));
    }
    else{
        msg = msg.arg(QString::number(0));
    }
    msg = msg.arg(UUID);


    QString topic = QString("IroTeam/%1/LOG").arg(userName);

//    qDebug() << "MqttPubLog --> msg = " << msg;
//    qDebug() << "MqttPubLog --> topic = " << topic;

    MqttPub(topic, msg, 1);

    return;
}

//##################################################  Log Resp from Server  ##################################################
bool MqttClass::MqttSubLogResp()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubLogResp--> ";
    QString topicString = QString("IroTeam/%1/LOGRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subLogResp = client->subscribe(topicFilter,qos);

    if(subLogResp){
        ret = true;
        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subLogResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedLogResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subLogResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedLogResp ,Qt::QueuedConnection);

//        QObject::connect(subLogResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedLogResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubLogResp()
{
    if(subLogResp && bSubLogResp){
        QString topicString = QString("IroTeam/%1/LOGRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subLogResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

//        QObject::disconnect(subLogResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedLogResp);
//        QObject::disconnect(subLogResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedLogResp);

        subLogResp->deleteLater();
        subLogResp = nullptr;
        bSubLogResp = false;
        qInfo() << MQTTCLASS << "MqttUnSubLogResp--> ";
    }

    return;
}

void MqttClass::on_messageReceivedLogResp(QMqttMessage mqttMsg)
{
//    qInfo() << MQTTCLASS << "on_messageReceivedLogResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(bTableThreadJobRun){
                emit this->sig_ServerToLogResp(obj.value("UUID").toString());
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedLogResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedLogResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }


    return;
}

void MqttClass::on_stateChangedLogResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState){
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedLogResp--> " << "QMqttSubscription::Unsubscribed";
        bSubLogResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedLogResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubLogResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedLogResp--> " << "QMqttSubscription::Subscribed";
        bSubLogResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedLogResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubLogResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedLogResp--> " << "QMqttSubscription::Error";
        bSubLogResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedLogResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedLogResp--> " <<  qos;

//    return;
//}

//############################################################################################################################
//############################################################################################################################
//##################################################  Inp to Server Sig from Table Handler Calss  ############################
void MqttClass::on_sig_SendToServerInp_slot(Inp inp)
{
    if(bSubInpResp && mqtt_check_subscribe())
        MqttPubInp(inp.readerExpId, inp.readerId, inp.reqDateTime.toString("yyyy-MM-dd hh:mm:ss"), inp.UUID);

    return;
}

//##################################################  Inp to Server  ##########################################################
void MqttClass::MqttPubInp(int readerExpId, int readerId, QString reqDateTime, QString UUID)
{
    QString msg = "{\"readerExpId\":%1, \"readerId\":%2, \"reqDateTime\":\"%3\", \"UUID\":\"%4\"}";
    msg = msg.arg(QString::number(readerExpId));
    msg = msg.arg(QString::number(readerId));
    msg = msg.arg(reqDateTime);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/INP").arg(userName);


//    qDebug() << msg;
//    qDebug() << topic;
    MqttPub(topic, msg, 1);

    return;
}

//##################################################  Inp Resp from Server  ##################################################
bool MqttClass::MqttSubInpResp()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubInpResp--> ";
    QString topicString = QString("IroTeam/%1/INPRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subInpResp = client->subscribe(topicFilter,qos);

    if(subInpResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subInpResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedInpResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subInpResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedInpResp ,Qt::QueuedConnection);

//        QObject::connect(subInpResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedInpResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubInpResp()
{
    if(subInpResp && bSubInpResp){

        QString topicString = QString("IroTeam/%1/INPRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subInpResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subInpResp->deleteLater();
        subInpResp = nullptr;
        bSubInpResp = false;
        qInfo() << MQTTCLASS << "MqttUnSubInpResp--> ";
    }

    return;
}

void MqttClass::on_messageReceivedInpResp(QMqttMessage mqttMsg)
{
//    qInfo() << MQTTCLASS << "on_messageReceivedInpResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(bTableThreadJobRun)
                emit this->sig_ServerToInpResp(obj.value("UUID").toString());
        }
        else{            
            qWarning() << MQTTCLASS << "on_messageReceivedInpResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedInpResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedInpResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedInpResp--> " << "QMqttSubscription::Unsubscribed";
        bSubInpResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedInpResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubInpResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedInpResp--> " << "QMqttSubscription::Subscribed";
        bSubInpResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        bSubInpResp = false;
        qInfo() << MQTTCLASS << "on_stateChangedInpResp--> " << "QMqttSubscription::UnsubscriptionPending";
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedInpResp--> " << "QMqttSubscription::Error";
        bSubInpResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedInpResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedInpResp--> " <<  qos;

//    return;
//}
//############################################################################################################################
//############################################################################################################################

//############################################################################################################################
//############################################################################################################################
//##################################################  Apb to Server Sig from Authentication Handler Calss  ############################
void MqttClass::on_sig_SendToServerApb_slot(Apb apb)
{
//    qDebug() << "on_sig_SendToServerApb_slot";
    if(bSubApbResp && mqtt_check_subscribe())
        MqttPubApb(apb.userUniqueId, apb.readerExpId, apb.readerId, apb.reqDateTime.toString("yyyy-MM-dd hh:mm:ss"));

    return;
}
//##################################################  Apb to Server  ##################################################

void MqttClass::MqttPubApb(int userUniqueId, int readerExpId, int readerId, QString reqDateTime)
{
    QString msg = "{\"userUniqueId\":%1, \"readerExpId\":%2, \"readerId\":%3, \"reqDateTime\":\"%4\"}";
    msg = msg.arg(QString::number(userUniqueId));
    msg = msg.arg(QString::number(readerExpId));
    msg = msg.arg(QString::number(readerId));
    msg = msg.arg(reqDateTime);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/APB").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);

    return;
}

//##################################################  Antipassback Resp from Server  ##################################################
bool MqttClass::MqttSubApbResp()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubApbResp--> ";
    QString topicString = QString("IroTeam/%1/APBRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subApbResp = client->subscribe(topicFilter,qos);

    if(subApbResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subApbResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedApbResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subApbResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedApbResp ,Qt::QueuedConnection);

//        QObject::connect(subApbResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedApbResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubApbResp()
{
    if(subApbResp && bSubApbResp){
        qInfo() << MQTTCLASS << "MqttUnSubApbResp--> ";
        QString topicString = QString("IroTeam/%1/APBRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subApbResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subApbResp->deleteLater();
        subApbResp = nullptr;
        bSubApbResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedApbResp(QMqttMessage mqttMsg)
{
    qInfo() << MQTTCLASS << "on_messageReceivedApbResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("errorCode").toInt() == 0){
//                QString reqDateTimeString = obj.value("reqDateTime").toVariant().toDateTime();
                if(bTableThreadJobRun)
                    emit this->sig_ServerToApbResp(obj.value("userUniqueId").toInt(),
                                                   obj.value("readerExpId").toInt(),
                                                   obj.value("readerId").toInt(),
                                                   obj.value("reqDateTime").toVariant().toDateTime(),
                                                   obj.value("antiPassBack").toInt());
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedApbResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedApbResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedApbResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedApbResp--> " << "QMqttSubscription::Unsubscribed";
        bSubApbResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedApbResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubApbResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedApbResp--> " << "QMqttSubscription::Subscribed";
        bSubApbResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedApbResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubApbResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedApbResp--> " << "QMqttSubscription::Error";
        bSubApbResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedApbResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedApbResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  Auth to Server Sig from Authentication Calss  ############################
void MqttClass::on_sig_SendToServerAuth_slot(Auth auth)
{
    //    qDebug() << "on_sig_SendToServerAuth_slot";
    if(bSubAuthResp && mqtt_check_subscribe())
        MqttPubAuth(auth.userUniqueId, auth.readerExpId, auth.readerId, auth.reqDateTime.toString("yyyy-MM-dd hh:mm:ss"));

    return;
}

//##################################################  Auth to Server  ##########################################################
void MqttClass::MqttPubAuth(int userUniqueId, int readerExpId, int readerId, QString reqDateTime)
{
    QString msg = "{\"userUniqueId\":%1, \"readerExpId\":%2, \"readerId\":%3, \"reqDateTime\":\"%4\"}";
    msg = msg.arg(QString::number(userUniqueId));
    msg = msg.arg(QString::number(readerExpId));
    msg = msg.arg(QString::number(readerId));
    msg = msg.arg(reqDateTime);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/AUTH").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);

    return;
}

bool MqttClass::MqttSubAuthResp()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubAuthResp--> ";
    QString topicString = QString("IroTeam/%1/AUTHRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subAuthResp = client->subscribe(topicFilter,qos);

    if(subAuthResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subAuthResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedAuthResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subAuthResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedAuthResp ,Qt::QueuedConnection);

//        QObject::connect(subAuthResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedAuthResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubAuthResp()
{
    if(subAuthResp && bSubAuthResp){
        qInfo() << MQTTCLASS << "MqttUnSubAuthResp--> ";
        QString topicString = QString("IroTeam/%1/AUTHRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subAuthResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subAuthResp->deleteLater();
        subAuthResp = nullptr;
        bSubAuthResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedAuthResp(QMqttMessage mqttMsg)
{
    qInfo() << "";
    qInfo() << "";
    qInfo() << MQTTCLASS << "on_messageReceivedAuthResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
//            if(obj.value("errorCode").toInt() == 0){
            if(bTableThreadJobRun)
                emit this->sig_ServerToAuthResp(obj.value("userUniqueId").toInt(),
                                                obj.value("readerExpId").toInt(),
                                                obj.value("readerId").toInt(),
                                                obj.value("reqDateTime").toVariant().toDateTime(),
                                                obj.value("isHoliday").toInt(),
                                                obj.value("errorCode").toInt(),
                                                obj.value("errorString").toString(),
                                                obj.value("isSuccess").toInt());
//            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedAuthResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedAuthResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedAuthResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedAuthResp--> " << "QMqttSubscription::Unsubscribed";
        bSubAuthResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAuthResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubAuthResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedAuthResp--> " << "QMqttSubscription::Subscribed";
        bSubAuthResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAuthResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubAuthResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedAuthResp--> " << "QMqttSubscription::Error";
        bSubAuthResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedAuthResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedAuthResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  StartStop to Server Sig from JobHandler Calss  ###########################
void MqttClass::on_sig_StartStopSyncDatabase(int startStop)
{
    if(bSubStartStopSyncResp && mqtt_check_subscribe())
        MqttPubStartSopSync(startStop);

    return;
}

//##################################################  StartStopSync to Server  ###############################################
void MqttClass::MqttPubStartSopSync(int startStop)
{
    QString msg = "{\"startStop\":%1, \"currentDateTime\":\"%2\"}";
    msg = msg.arg(QString::number(startStop));
    msg = msg.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz t"));
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXSYNC").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);

    return;
}

bool MqttClass::MqttSubStartStopSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubStartStopSync--> ";
    QString topicString = QString("IroTeam/%1/STARTSTOPSYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subStartStopSyncResp = client->subscribe(topicFilter,qos);

    if(subStartStopSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subStartStopSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedStartStopResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subStartStopSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedStartStopResp ,Qt::QueuedConnection);

//        QObject::connect(subStartStopSync, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedStartStopResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubStartStopSync()
{
    if(subStartStopSyncResp && bSubStartStopSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubStartStopSync--> ";
        QString topicString = QString("IroTeam/%1/STARTSTOPSYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subStartStopSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subStartStopSyncResp->deleteLater();
        subStartStopSyncResp = nullptr;
        bSubStartStopSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedStartStopResp(QMqttMessage mqttMsg)
{
    qInfo() << "";
    qInfo() << "";
    qInfo() << MQTTCLASS << "on_messageReceivedStartStopResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
//            if(obj.value("errorCode").toInt() == 0){
            if(bJobThreadJobRun)
                emit this->sig_ServerToStartStopResp(obj.value("startStop").toInt());
//            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedStartStopResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedStartStopResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedStartStopResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedStartStopResp--> " << "QMqttSubscription::Unsubscribed";
        bSubStartStopSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedStartStopResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubStartStopSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedStartStopResp--> " << "QMqttSubscription::Subscribed";
        bSubStartStopSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedStartStopResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubStartStopSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedStartStopResp--> " << "QMqttSubscription::Error";
        bSubStartStopSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedStartStopResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedStartStopResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  TimezonesTableSync to Server Sig from JobHandler Calss  ##################
void MqttClass::on_sig_TimezonesTableSync(QString code, QString startTime, QString stopTime, QString UUID)
{
    if(bSubTimezonesTableSyncResp && mqtt_check_subscribe())
        MqttPubTimezonesTableSync(code, startTime, stopTime, UUID);

    return;
}

//##################################################  TimezonesTableSync to Server  ############################################
void MqttClass::MqttPubTimezonesTableSync(QString code, QString startTime, QString stopTime, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"startTime\":\"%2\", \"stopTime\":\"%3\", \"UUID\":\"%4\"}";
    msg = msg.arg(code);
    msg = msg.arg(startTime);
    msg = msg.arg(stopTime);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXTIMEZONESTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);

    return;
}

bool MqttClass::MqttSubTimezonesTableSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubTimezonesTableSync--> ";
    QString topicString = QString("IroTeam/%1/TIMEZONESTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subTimezonesTableSyncResp = client->subscribe(topicFilter,qos);

    if(subTimezonesTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subTimezonesTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedTimezonesTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subTimezonesTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedTimezonesTableResp ,Qt::QueuedConnection);

//        QObject::connect(subTimezonesTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedTimezonesTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubTimezonesTableSync()
{
    if(subTimezonesTableSyncResp && bSubTimezonesTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubTimezonesTableSync--> ";
        QString topicString = QString("IroTeam/%1/TIMEZONESTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subTimezonesTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subTimezonesTableSyncResp->deleteLater();
        subTimezonesTableSyncResp = nullptr;
        bSubTimezonesTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedTimezonesTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedTimezonesTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerToTimezonesTableResp(0, "",
                                                                  "",  "",
                                                                  obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                        emit this->sig_ServerToTimezonesTableResp(1, obj.value("code").toString(),
                                                                  obj.value("startTime").toString(),  obj.value("stopTime").toString(),
                                                                  obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerToTimezonesTableResp(2, obj.value("code").toString(),
                                                                  obj.value("startTime").toString(),  obj.value("stopTime").toString(),
                                                                  obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedTimezonesTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }

        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedTimezonesTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedTimezonesTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedTimezonesTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedTimezonesTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubTimezonesTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedSTimezonesTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubTimezonesTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedSTimezonesTableResp--> " << "QMqttSubscription::Subscribed";
        bSubTimezonesTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedSTimezonesTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubTimezonesTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedSTimezonesTableResp--> " << "QMqttSubscription::Error";
        bSubTimezonesTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedTimezonesTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedTimezonesTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  AccessTimezonesTableSync to Server Sig from JobHandler Calss  ############
void MqttClass::on_sig_AccessTimezonesTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID06, QString UUID07, QString UUID08, QString UUID09, QString UUID10, QString UUID11, QString UUID12, QString UUID)
{
    if(bSubAccessTimezonesTableSyncResp && mqtt_check_subscribe())
        MqttPubAccessTimezonesTableSync(code, UUID01, UUID02, UUID03, UUID04, UUID05, UUID06, UUID07, UUID08, UUID09, UUID10, UUID11, UUID12, UUID);

    return;
}

//##################################################  AccessTimezonesTableSync to Server  ######################################
void MqttClass::MqttPubAccessTimezonesTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID06, QString UUID07, QString UUID08, QString UUID09, QString UUID10, QString UUID11, QString UUID12, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"UUID01\":\"%2\", \"UUID02\":\"%3\", \"UUID03\":\"%4\", \"UUID04\":\"%5\", \"UUID05\":\"%6\", \"UUID06\":\"%7\", \"UUID07\":\"%8\", \"UUID08\":\"%9\", \"UUID09\":\"%10\", \"UUID10\":\"%11\", \"UUID11\":\"%12\", \"UUID12\":\"%13\", \"UUID\":\"%14\"}";
    msg = msg.arg(code);
    msg = msg.arg(UUID01);
    msg = msg.arg(UUID02);
    msg = msg.arg(UUID03);
    msg = msg.arg(UUID04);
    msg = msg.arg(UUID05);
    msg = msg.arg(UUID06);
    msg = msg.arg(UUID07);
    msg = msg.arg(UUID08);
    msg = msg.arg(UUID09);
    msg = msg.arg(UUID10);
    msg = msg.arg(UUID11);
    msg = msg.arg(UUID12);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXACCESSTIMEZONESTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubAccessTimezonesTableSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubAccessTimezonesTableSync--> ";
    QString topicString = QString("IroTeam/%1/ACCESSTIMEZONESTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subAccessTimezonesTableSyncResp = client->subscribe(topicFilter,qos);

    if(subAccessTimezonesTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subAccessTimezonesTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedAccessTimezonesTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subAccessTimezonesTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedAccessTimezonesTableResp ,Qt::QueuedConnection);

//        QObject::connect(subTimezonesTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedAccessTimezonesTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubAccessTimezonesTableSync()
{
    if(subAccessTimezonesTableSyncResp && bSubAccessTimezonesTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubAccessTimezonesTableSync--> ";
        QString topicString = QString("IroTeam/%1/ACCESSTIMEZONESTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subAccessTimezonesTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subAccessTimezonesTableSyncResp->deleteLater();
        subAccessTimezonesTableSyncResp = nullptr;
        bSubAccessTimezonesTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedAccessTimezonesTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedAccessTimezonesTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerToAccessTimezonesTableResp(0, "",
                                                                        "",  "",
                                                                        "",  "",
                                                                        "",  "",
                                                                        "",  "",
                                                                        "",  "",
                                                                        "",  "",
                                                                        obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerToAccessTimezonesTableResp(1, obj.value("code").toString(),
                                                                       obj.value("UUID01").toString(),  obj.value("UUID02").toString(),
                                                                       obj.value("UUID03").toString(),  obj.value("UUID04").toString(),
                                                                       obj.value("UUID05").toString(),  obj.value("UUID06").toString(),
                                                                       obj.value("UUID07").toString(),  obj.value("UUID08").toString(),
                                                                       obj.value("UUID09").toString(),  obj.value("UUID10").toString(),
                                                                       obj.value("UUID11").toString(),  obj.value("UUID12").toString(),
                                                                       obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerToAccessTimezonesTableResp(2, obj.value("code").toString(), obj.value("UUID01").toString(),  obj.value("UUID02").toString(),
                                                                        obj.value("UUID03").toString(),  obj.value("UUID04").toString(),
                                                                        obj.value("UUID05").toString(),  obj.value("UUID06").toString(),
                                                                        obj.value("UUID07").toString(),  obj.value("UUID08").toString(),
                                                                        obj.value("UUID09").toString(),  obj.value("UUID10").toString(),
                                                                        obj.value("UUID11").toString(),  obj.value("UUID12").toString(),
                                                                        obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedAccessTimezonesTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedAccessTimezonesTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedAccessTimezonesTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedAccessTimezonesTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState){
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedAccessTimezonesTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubAccessTimezonesTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAccessTimezonesTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubAccessTimezonesTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedAccessTimezonesTableResp--> " << "QMqttSubscription::Subscribed";
        bSubAccessTimezonesTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAccessTimezonesTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubAccessTimezonesTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedAccessTimezonesTableResp--> " << "QMqttSubscription::Error";
        bSubAccessTimezonesTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedAccessTimezonesTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedAccessTimezonesTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  AccessShiftsTableSync to Server Sig from JobHandler Calss  ###############
void MqttClass::on_sig_AccessShiftsTableSync(QString code, int type, QString UUIDSat, QString UUIDSun, QString UUIDMon, QString UUIDTue, QString UUIDWed, QString UUIDThu, QString UUIDFri, QString UUIDHol, QString UUID)
{
    if(bSubAccessShiftsTableSyncResp && mqtt_check_subscribe())
        MqttPubAccessShiftsTableSync(code, type, UUIDSat, UUIDSun, UUIDMon, UUIDTue, UUIDWed, UUIDThu, UUIDFri, UUIDHol, UUID);

    return;
}

//##################################################  AccessShiftsTableSync to Server  #########################################
void MqttClass::MqttPubAccessShiftsTableSync(QString code, int type, QString UUIDSat, QString UUIDSun, QString UUIDMon, QString UUIDTue, QString UUIDWed, QString UUIDThu, QString UUIDFri, QString UUIDHol, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"type\":%2, \"UUIDSat\":\"%3\", \"UUIDSun\":\"%4\", \"UUIDMon\":\"%5\", \"UUIDTue\":\"%6\", \"UUIDWed\":\"%7\", \"UUIDThu\":\"%8\", \"UUIDFri\":\"%9\", \"UUIDHol\":\"%10\", \"UUID\":\"%11\"}";
    msg = msg.arg(code);
    msg = msg.arg(QString::number(type));
    msg = msg.arg(UUIDSat);
    msg = msg.arg(UUIDSun);
    msg = msg.arg(UUIDMon);
    msg = msg.arg(UUIDTue);
    msg = msg.arg(UUIDWed);
    msg = msg.arg(UUIDThu);
    msg = msg.arg(UUIDFri);
    msg = msg.arg(UUIDHol);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXACCESSSHIFTSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubAccessShiftsTableSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubAccessShiftsTableSync--> ";
    QString topicString = QString("IroTeam/%1/ACCESSSHIFTSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subAccessShiftsTableSyncResp = client->subscribe(topicFilter,qos);

    if(subAccessShiftsTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subAccessShiftsTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedAccessShiftsTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subAccessShiftsTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedAccessShiftsTableResp ,Qt::QueuedConnection);

//        QObject::connect(subTimezonesTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedAccessAccessShiftsTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubAccessShiftsTableSync()
{
    if(subAccessShiftsTableSyncResp && bSubAccessShiftsTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubAccessShiftsTableSync--> ";
        QString topicString = QString("IroTeam/%1/ACCESSSHIFTSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subAccessShiftsTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subAccessShiftsTableSyncResp->deleteLater();
        subAccessShiftsTableSyncResp = nullptr;
        bSubAccessShiftsTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedAccessShiftsTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedAccessShiftsTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerToAccessShiftsTableResp(0, "", 0,
                                                                     "",  "",
                                                                     "",  "",
                                                                     "",  "",
                                                                     "",  "", obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerToAccessShiftsTableResp(1, obj.value("code").toString(), obj.value("type").toInt(),
                                                                       obj.value("UUIDSat").toString(),  obj.value("UUIDSun").toString(),
                                                                       obj.value("UUIDMon").toString(),  obj.value("UUIDTue").toString(),
                                                                       obj.value("UUIDWed").toString(),  obj.value("UUIDThu").toString(),
                                                                       obj.value("UUIDSat").toString(),  obj.value("UUIDSat").toString(),
                                                                       obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerToAccessShiftsTableResp(2, obj.value("code").toString(), obj.value("type").toInt(),
                                                                     obj.value("UUIDSat").toString(),  obj.value("UUIDSun").toString(),
                                                                     obj.value("UUIDMon").toString(),  obj.value("UUIDTue").toString(),
                                                                     obj.value("UUIDWed").toString(),  obj.value("UUIDThu").toString(),
                                                                     obj.value("UUIDSat").toString(),  obj.value("UUIDSat").toString(),
                                                                     obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedAccessShiftsTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedAccessShiftsTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedAccessShiftsTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedAccessShiftsTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedAccessShiftsTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubAccessShiftsTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAccessShiftsTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubAccessShiftsTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedAccessShiftsTableResp--> " << "QMqttSubscription::Subscribed";
        bSubAccessShiftsTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAccessShiftsTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubAccessShiftsTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedAccessShiftsTableResp--> " << "QMqttSubscription::Error";
        bSubAccessShiftsTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedAccessShiftsTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedAccessShiftsTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  tag_UsersTableSync to Server Sig from JobHandler Calss  ##################
void MqttClass::on_sig_tag_UsersTableSync(int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID)
{
    if(bSubtag_UsersTableSyncResp && mqtt_check_subscribe())
        MqttPubtag_UsersTableSync(userUniqueId, type, active, creditStartDateTime, creditStopDateTime, deactiveByDateTime, deactiveStartDateTime, deactiveStopDateTime, UUID);

    return;
}

//##################################################  tag_UsersTableSync to Server  ############################################
void MqttClass::MqttPubtag_UsersTableSync(int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID)
{
    QString msg = "{\"userUniqueId\":%1, \"type\":%2, \"active\":%3, \"creditStartDateTime\":\"%4\", \"creditStopDateTime\":\"%5\", \"deactiveByDateTime\":%6, \"deactiveStartDateTime\":\"%7\", \"deactiveStopDateTime\":\"%8\", \"UUID\":\"%9\"}";
    msg = msg.arg(QString::number(userUniqueId));
    msg = msg.arg(QString::number(type));
    msg = msg.arg(QString::number(active));
    msg = msg.arg(creditStartDateTime);
    msg = msg.arg(creditStopDateTime);
    msg = msg.arg(QString::number(deactiveByDateTime));
    msg = msg.arg(deactiveStartDateTime);
    msg = msg.arg(deactiveStopDateTime);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXTAG_USERSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubtag_UsersTableSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubtag_UsersTableSync--> ";
    QString topicString = QString("IroTeam/%1/TAG_USERSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subtag_UsersTableSyncResp = client->subscribe(topicFilter,qos);

    if(subtag_UsersTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subtag_UsersTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedtag_UsersTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subtag_UsersTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedtag_UsersTableResp ,Qt::QueuedConnection);

//        QObject::connect(bSubtag_UsersTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedtag_UsersTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubtag_UsersTableSync()
{
    if(bSubtag_UsersTableSyncResp && subtag_UsersTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubtag_UsersTableSync--> ";
        QString topicString = QString("IroTeam/%1/TAG_USERSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subtag_UsersTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subtag_UsersTableSyncResp->deleteLater();
        subtag_UsersTableSyncResp = nullptr;
        bSubtag_UsersTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedtag_UsersTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedtag_UsersTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerTotag_UsersTableResp(0, 0, 0, 0,
                                                                     "",  "",
                                                                     0,
                                                                     "",  "",
                                                                     obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerTotag_UsersTableResp(1, obj.value("userUniqueId").toInt(), obj.value("type").toInt(), obj.value("active").toInt(),
                                                                       obj.value("creditStartDateTime").toString(),  obj.value("creditStopDateTime").toString(),
                                                                       obj.value("deactiveByDateTime").toInt(),
                                                                       obj.value("deactiveStartDateTime").toString(),  obj.value("deactiveStopDateTime").toString(),
                                                                       obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerTotag_UsersTableResp(2, obj.value("userUniqueId").toInt(), obj.value("type").toInt(), obj.value("active").toInt(),
                                                                  obj.value("creditStartDateTime").toString(),  obj.value("creditStopDateTime").toString(),
                                                                  obj.value("deactiveByDateTime").toInt(),
                                                                  obj.value("deactiveStartDateTime").toString(),  obj.value("deactiveStopDateTime").toString(),
                                                                  obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedtag_UsersTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedtag_UsersTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedtag_UsersTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedtag_UsersTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedtag_UsersTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubtag_UsersTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_UsersTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubtag_UsersTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedtag_UsersTableResp--> " << "QMqttSubscription::Subscribed";
        bSubtag_UsersTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_UsersTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubtag_UsersTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedtag_UsersTableResp--> " << "QMqttSubscription::Error";
        bSubtag_UsersTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedtag_UsersTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedtag_UsersTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  tag_TerminalsTableSync to Server Sig from JobHandler Calss  ##############
void MqttClass::on_sig_tag_TerminalsTableSync(int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID)
{
    if(bSubtag_TerminalsTableSyncResp && mqtt_check_subscribe())
        MqttPubtag_TerminalsTableSync(readerId, type, readerExpId, active, deactiveByDateTime, deactiveStartDateTime, deactiveStopDateTime, UUID);

    return;
}

//##################################################  tag_TerminalsTableSync to Server S#######################################
void MqttClass::MqttPubtag_TerminalsTableSync(int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID)
{
    QString msg = "{\"readerId\":%1, \"type\":%2, \"readerExpId\":%3, \"active\":%4, \"deactiveByDateTime\":%5, \"deactiveStartDateTime\":\"%6\", \"deactiveStopDateTime\":\"%7\", \"UUID\":\"%8\"}";
    msg = msg.arg(QString::number(readerId));
    msg = msg.arg(QString::number(type));
    msg = msg.arg(QString::number(readerExpId));
    if(active)
        msg = msg.arg(QString::number(1));
    else
        msg = msg.arg(QString::number(0));

    if(deactiveByDateTime)
        msg = msg.arg(QString::number(1));
    else
        msg = msg.arg(QString::number(0));

    msg = msg.arg(deactiveStartDateTime);
    msg = msg.arg(deactiveStopDateTime);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXTAG_TERMINALSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubtag_TerminalsTableSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubtag_TerminalsTableSync--> ";
    QString topicString = QString("IroTeam/%1/TAG_TERMINALSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subtag_TerminalsTableSyncResp = client->subscribe(topicFilter,qos);

    if(subtag_TerminalsTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subtag_TerminalsTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedtag_TerminalsTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subtag_TerminalsTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedtag_TerminalsTableResp ,Qt::QueuedConnection);

//        QObject::connect(bSubtag_TerminalsTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedtag_TerminalsTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubtag_TerminalsTableSync()
{
    if(bSubtag_TerminalsTableSyncResp && subtag_TerminalsTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubtag_TerminalsTableSync--> ";
        QString topicString = QString("IroTeam/%1/TAG_TERMINALSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subtag_TerminalsTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subtag_TerminalsTableSyncResp->deleteLater();
        subtag_TerminalsTableSyncResp = nullptr;
        bSubtag_TerminalsTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedtag_TerminalsTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedtag_TerminalsTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerTotag_TerminalsTableResp(0, 0, 0, 0, 0, 0,
                                                                     "",  "",
                                                                     obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerTotag_TerminalsTableResp(1, obj.value("readerId").toInt(), obj.value("type").toInt(), obj.value("readerExpId").toInt(), obj.value("active").toInt(),  obj.value("deactiveByDateTime").toInt(),
                                                                       obj.value("deactiveStartDateTime").toString(),  obj.value("deactiveStopDateTime").toString(),
                                                                       obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerTotag_TerminalsTableResp(2, obj.value("readerId").toInt(), obj.value("type").toInt(), obj.value("readerExpId").toInt(), obj.value("active").toInt(),  obj.value("deactiveByDateTime").toInt(),
                                                                      obj.value("deactiveStartDateTime").toString(),  obj.value("deactiveStopDateTime").toString(),
                                                                      obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedtag_TerminalsTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedtag_TerminalsTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedtag_TerminalsTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedtag_TerminalsTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedtag_TerminalsTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubtag_TerminalsTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_TerminalsTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubtag_TerminalsTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedtag_TerminalsTableResp--> " << "QMqttSubscription::Subscribed";
        bSubtag_TerminalsTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_TerminalsTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubtag_TerminalsTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedtag_TerminalsTableResp--> " << "QMqttSubscription::Error";
        bSubtag_TerminalsTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedtag_TerminalsTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedtag_TerminalsTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  HolidaysTableSync to Server Sig from JobHandler Calss  ###############
void MqttClass::on_sig_HolidaysTableSync(QString code, QString date, QString UUID)
{
    if(bSubHolidaysTableSyncResp && mqtt_check_subscribe())
        MqttPubHolidaysTableSync(code, date, UUID);

    return;
}

//##################################################  HolidaysTableSync to Server ##########################################
void MqttClass::MqttPubHolidaysTableSync(QString code, QString date, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"date\":\"%2\", \"UUID\":\"%3\"}";
    msg = msg.arg(code);
    msg = msg.arg(date);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXHOLIDAYSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubHolidaysTableSync()
{
    bool ret =  false;
//    qInfo() << MQTTCLASS << "MqttSubHolidaysTableSync--> ";
    QString topicString = QString("IroTeam/%1/HOLIDAYSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subHolidaysTableSyncResp = client->subscribe(topicFilter,qos);

    if(subHolidaysTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subHolidaysTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedHolidaysTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subHolidaysTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedHolidaysTableResp ,Qt::QueuedConnection);

//        QObject::connect(subHolidaysTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedHolidaysTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubHolidaysTableSync()
{
    if(bSubHolidaysTableSyncResp && subHolidaysTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubHolidaysTableSync--> ";
        QString topicString = QString("IroTeam/%1/HOLIDAYSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subHolidaysTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subHolidaysTableSyncResp->deleteLater();
        subHolidaysTableSyncResp = nullptr;
        bSubHolidaysTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedHolidaysTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedHolidaysTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerToHolidaysTableResp(0, "", "",
                                                                     obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerToHolidaysTableResp(1, obj.value("code").toString(), obj.value("date").toString(),
                                                                       obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerToHolidaysTableResp(2, obj.value("code").toString(), obj.value("date").toString(),
                                                                     obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedHolidaysTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedHolidaysTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedHolidaysTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedHolidaysTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubHolidaysTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubHolidaysTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedHolidaysTableResp--> " << "QMqttSubscription::Subscribed";
        bSubHolidaysTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubHolidaysTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysTableResp--> " << "QMqttSubscription::Error";
        bSubHolidaysTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedHolidaysTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedHolidaysTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  HolidaysTableSync to Server Sig from JobHandler Calss  ###############
void MqttClass::on_sig_HolidaysSetsTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID06, QString UUID07, QString UUID08, QString UUID09, QString UUID10,QString UUID11, QString UUID12, QString UUID13, QString UUID14, QString UUID15, QString UUID16, QString UUID17, QString UUID18, QString UUID19, QString UUID20, QString UUID21, QString UUID22, QString UUID23, QString UUID24, QString UUID25, QString UUID26, QString UUID27, QString UUID28, QString UUID29, QString UUID30, QString UUID31, QString UUID32, QString UUID)
{
    if(bSubHolidaysSetsTableSyncResp && mqtt_check_subscribe())
        MqttPubHolidaysSetsTableSync(code, UUID01, UUID02, UUID03, UUID04, UUID05, UUID06, UUID07, UUID08, UUID09, UUID10, UUID11, UUID12, UUID13, UUID14, UUID15, UUID16, UUID17, UUID18, UUID19, UUID20, UUID21, UUID22, UUID23, UUID24, UUID25, UUID26, UUID27, UUID28, UUID29, UUID30, UUID31, UUID32, UUID);

    return;
}

//##################################################  HolidaysTableSync to Server ##########################################
void MqttClass::MqttPubHolidaysSetsTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID06, QString UUID07, QString UUID08, QString UUID09, QString UUID10,QString UUID11, QString UUID12, QString UUID13, QString UUID14, QString UUID15, QString UUID16, QString UUID17, QString UUID18, QString UUID19, QString UUID20, QString UUID21, QString UUID22, QString UUID23, QString UUID24, QString UUID25, QString UUID26, QString UUID27, QString UUID28, QString UUID29, QString UUID30, QString UUID31, QString UUID32, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"UUID01\":\"%2\", \"UUID02\":\"%3\", \"UUID03\":\"%4\", \"UUID04\":\"%5\", \"UUID05\":\"%6\", \"UUID06\":\"%7\", \"UUID07\":\"%8\", \"UUID08\":\"%9\", \"UUID09\":\"%10\", \"UUID10\":\"%11\", \"UUID11\":\"%12\", \"UUID12\":\"%13\", \"UUID13\":\"%14\", \"UUID14\":\"%15\", \"UUID15\":\"%16\", \"UUID16\":\"%17\", \"UUID17\":\"%18\", \"UUID18\":\"%19\", \"UUID19\":\"%20\", \"UUID20\":\"%21\", \"UUID21\":\"%22\", \"UUID22\":\"%23\", \"UUID23\":\"%24\", \"UUID24\":\"%25\", \"UUID25\":\"%26\", \"UUID26\":\"%27\", \"UUID27\":\"%28\", \"UUID28\":\"%29\", \"UUID29\":\"%30\", \"UUID30\":\"%31\", \"UUID31\":\"%32\", \"UUID32\":\"%33\", \"UUID\":\"%34\"}";
    msg = msg.arg(code);
    msg = msg.arg(UUID01);
    msg = msg.arg(UUID02);
    msg = msg.arg(UUID03);
    msg = msg.arg(UUID04);
    msg = msg.arg(UUID05);
    msg = msg.arg(UUID06);
    msg = msg.arg(UUID07);
    msg = msg.arg(UUID08);
    msg = msg.arg(UUID09);
    msg = msg.arg(UUID10);
    msg = msg.arg(UUID11);
    msg = msg.arg(UUID12);
    msg = msg.arg(UUID13);
    msg = msg.arg(UUID14);
    msg = msg.arg(UUID15);
    msg = msg.arg(UUID16);
    msg = msg.arg(UUID17);
    msg = msg.arg(UUID18);
    msg = msg.arg(UUID19);
    msg = msg.arg(UUID20);
    msg = msg.arg(UUID21);
    msg = msg.arg(UUID22);
    msg = msg.arg(UUID23);
    msg = msg.arg(UUID24);
    msg = msg.arg(UUID25);
    msg = msg.arg(UUID26);
    msg = msg.arg(UUID27);
    msg = msg.arg(UUID28);
    msg = msg.arg(UUID29);
    msg = msg.arg(UUID30);
    msg = msg.arg(UUID31);
    msg = msg.arg(UUID32);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXHOLIDAYSSETSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubHolidaysSetsTableSync()
{
    bool ret =  false;
//    qInfo() << MQTTCLASS << "MqttSubHolidaysSetsTableSync--> ";
    QString topicString = QString("IroTeam/%1/HOLIDAYSSETSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subHolidaysSetsTableSyncResp = client->subscribe(topicFilter,qos);

    if(subHolidaysSetsTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subHolidaysSetsTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedHolidaysSetsTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subHolidaysSetsTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedHolidaysSetsTableResp ,Qt::QueuedConnection);

//        QObject::connect(subHolidaysSetsTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedHolidaysSetsTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubHolidaysSetsTableSync()
{
    if(bSubHolidaysSetsTableSyncResp && subHolidaysSetsTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubHolidaysSetsTableSync--> ";
        QString topicString = QString("IroTeam/%1/HOLIDAYSSETSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subHolidaysSetsTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subHolidaysSetsTableSyncResp->deleteLater();
        subHolidaysSetsTableSyncResp = nullptr;
        bSubHolidaysSetsTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedHolidaysSetsTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedHolidaysSetsTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerToHolidaysSetsTableResp(0, "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
                                                                     obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerToHolidaysSetsTableResp(
                                                                    1,
                                                                    obj.value("code").toString(),
                                                                    obj.value("UUID01").toString(),
                                                                    obj.value("UUID02").toString(),
                                                                    obj.value("UUID03").toString(),
                                                                    obj.value("UUID04").toString(),
                                                                    obj.value("UUID05").toString(),
                                                                    obj.value("UUID06").toString(),
                                                                    obj.value("UUID07").toString(),
                                                                    obj.value("UUID08").toString(),
                                                                    obj.value("UUID09").toString(),
                                                                    obj.value("UUID10").toString(),
                                                                    obj.value("UUID11").toString(),
                                                                    obj.value("UUID12").toString(),
                                                                    obj.value("UUID13").toString(),
                                                                    obj.value("UUID14").toString(),
                                                                    obj.value("UUID15").toString(),
                                                                    obj.value("UUID16").toString(),
                                                                    obj.value("UUID17").toString(),
                                                                    obj.value("UUID18").toString(),
                                                                    obj.value("UUID19").toString(),
                                                                    obj.value("UUID20").toString(),
                                                                    obj.value("UUID21").toString(),
                                                                    obj.value("UUID22").toString(),
                                                                    obj.value("UUID23").toString(),
                                                                    obj.value("UUID24").toString(),
                                                                    obj.value("UUID25").toString(),
                                                                    obj.value("UUID26").toString(),
                                                                    obj.value("UUID27").toString(),
                                                                    obj.value("UUID28").toString(),
                                                                    obj.value("UUID29").toString(),
                                                                    obj.value("UUID30").toString(),
                                                                    obj.value("UUID31").toString(),
                                                                    obj.value("UUID32").toString(),
                                                                    obj.value("UUID").toString()
                                                                   );
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerToHolidaysSetsTableResp(
                                                                     2,
                                                                     obj.value("code").toString(),
                                                                     obj.value("UUID01").toString(),
                                                                     obj.value("UUID02").toString(),
                                                                     obj.value("UUID03").toString(),
                                                                     obj.value("UUID04").toString(),
                                                                     obj.value("UUID05").toString(),
                                                                     obj.value("UUID06").toString(),
                                                                     obj.value("UUID07").toString(),
                                                                     obj.value("UUID08").toString(),
                                                                     obj.value("UUID09").toString(),
                                                                     obj.value("UUID10").toString(),
                                                                     obj.value("UUID11").toString(),
                                                                     obj.value("UUID12").toString(),
                                                                     obj.value("UUID13").toString(),
                                                                     obj.value("UUID14").toString(),
                                                                     obj.value("UUID15").toString(),
                                                                     obj.value("UUID16").toString(),
                                                                     obj.value("UUID17").toString(),
                                                                     obj.value("UUID18").toString(),
                                                                     obj.value("UUID19").toString(),
                                                                     obj.value("UUID20").toString(),
                                                                     obj.value("UUID21").toString(),
                                                                     obj.value("UUID22").toString(),
                                                                     obj.value("UUID23").toString(),
                                                                     obj.value("UUID24").toString(),
                                                                     obj.value("UUID25").toString(),
                                                                     obj.value("UUID26").toString(),
                                                                     obj.value("UUID27").toString(),
                                                                     obj.value("UUID28").toString(),
                                                                     obj.value("UUID29").toString(),
                                                                     obj.value("UUID30").toString(),
                                                                     obj.value("UUID31").toString(),
                                                                     obj.value("UUID32").toString(),
                                                                     obj.value("UUID").toString()
                                                                    );
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedHolidaysSetsTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedHolidaysSetsTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedHolidaysSetsTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedHolidaysSetsTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysSetsTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubHolidaysSetsTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysSetsTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubHolidaysSetsTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedHolidaysSetsTableResp--> " << "QMqttSubscription::Subscribed";
        bSubHolidaysSetsTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysSetsTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubHolidaysSetsTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysSetsTableResp--> " << "QMqttSubscription::Error";
        bSubHolidaysSetsTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedHolidaysSetsTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedHolidaysSetsTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  HolidaysTableSync to Server Sig from JobHandler Calss  ###############
void MqttClass::on_sig_HolidaysGroupsTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID)
{
    if(bSubHolidaysGroupsTableSyncResp && mqtt_check_subscribe())
        MqttPubHolidaysGroupsTableSync(code, UUID01, UUID02, UUID03, UUID04, UUID05, UUID);

    return;
}

//##################################################  HolidaysTableSync to Server ##########################################
void MqttClass::MqttPubHolidaysGroupsTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"UUID01\":\"%2\", \"UUID02\":\"%3\", \"UUID03\":\"%4\", \"UUID04\":\"%5\", \"UUID05\":\"%6\", \"UUID\":\"%7\"}";
    msg = msg.arg(code);
    msg = msg.arg(UUID01);
    msg = msg.arg(UUID02);
    msg = msg.arg(UUID03);
    msg = msg.arg(UUID04);
    msg = msg.arg(UUID05);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXHOLIDAYSGROUPSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubHolidaysGroupsTableSync()
{
    bool ret =  false;
//    qInfo() << MQTTCLASS << "MqttSubHolidaysGroupsTableSync--> ";
    QString topicString = QString("IroTeam/%1/HOLIDAYSGROUPSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subHolidaysGroupsTableSyncResp = client->subscribe(topicFilter,qos);

    if(subHolidaysGroupsTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subHolidaysGroupsTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedHolidaysGroupsTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subHolidaysGroupsTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedHolidaysGroupsTableResp ,Qt::QueuedConnection);

//        QObject::connect(subHolidaysGroupsTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedHolidaysGroupsTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubHolidaysGroupsTableSync()
{
    if(bSubHolidaysGroupsTableSyncResp && subHolidaysGroupsTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubHolidaysGroupsTableSync--> ";
        QString topicString = QString("IroTeam/%1/HOLIDAYSGROUPSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subHolidaysGroupsTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subHolidaysGroupsTableSyncResp->deleteLater();
        subHolidaysGroupsTableSyncResp = nullptr;
        bSubHolidaysGroupsTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedHolidaysGroupsTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedHolidaysGroupsTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerToHolidaysGroupsTableResp(0, "", "", "", "", "", "",
                                                                     obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerToHolidaysGroupsTableResp(
                                                                      1,
                                                                      obj.value("code").toString(),
                                                                      obj.value("UUID01").toString(),
                                                                      obj.value("UUID02").toString(),
                                                                      obj.value("UUID03").toString(),
                                                                      obj.value("UUID04").toString(),
                                                                      obj.value("UUID05").toString(),
                                                                      obj.value("UUID").toString()
                                                                     );
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerToHolidaysGroupsTableResp(
                                                                       2,
                                                                       obj.value("code").toString(),
                                                                       obj.value("UUID01").toString(),
                                                                       obj.value("UUID02").toString(),
                                                                       obj.value("UUID03").toString(),
                                                                       obj.value("UUID04").toString(),
                                                                       obj.value("UUID05").toString(),
                                                                       obj.value("UUID").toString()
                                                                      );
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedHolidaysGroupsTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedHolidaysGroupsTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedHolidaysGroupsTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedHolidaysGroupsTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysGroupsTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubHolidaysGroupsTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysGroupsTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubHolidaysGroupsTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedHolidaysGroupsTableResp--> " << "QMqttSubscription::Subscribed";
        bSubHolidaysGroupsTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysGroupsTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubHolidaysGroupsTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedHolidaysGroupsTableResp--> " << "QMqttSubscription::Error";
        bSubHolidaysGroupsTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedHolidaysGroupsTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedHolidaysGroupsTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  tag_HolidaysTableSync to Server Sig from JobHandler Calss  ###############
void MqttClass::on_sig_tag_HolidaysTableSync(QString code, int type, QString UUIDHolGroups, QString UUID)
{
    if(bSubtag_HolidaysTableSyncResp && mqtt_check_subscribe())
        MqttPubtag_HolidaysTableSync(code, type, UUIDHolGroups, UUID);

    return;
}

//##################################################  tag_HolidaysTableSync to Server ##########################################
void MqttClass::MqttPubtag_HolidaysTableSync(QString code, int type, QString UUIDHolGroups, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"type\":%2, \"UUIDHolGroups\":\"%3\", \"UUID\":\"%4\"}";
    msg = msg.arg(code);
    msg = msg.arg(QString::number(type));
    msg = msg.arg(UUIDHolGroups);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXTAG_HOLIDAYSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubtag_HolidaysTableSync()
{
    bool ret =  false;
//    qInfo() << MQTTCLASS << "MqttSubtag_HolidaysTableSync--> ";
    QString topicString = QString("IroTeam/%1/TAG_HOLIDAYSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subtag_HolidaysTableSyncResp = client->subscribe(topicFilter,qos);

    if(subtag_HolidaysTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subtag_HolidaysTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedtag_HolidaysTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subtag_HolidaysTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedtag_HolidaysTableResp ,Qt::QueuedConnection);

//        QObject::connect(subtag_HolidaysTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedtag_HolidaysTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubtag_HolidaysTableSync()
{
    if(bSubtag_HolidaysTableSyncResp && subtag_HolidaysTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubtag_HolidaysTableSync--> ";
        QString topicString = QString("IroTeam/%1/TAG_HOLIDAYSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subtag_HolidaysTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subtag_HolidaysTableSyncResp->deleteLater();
        subtag_HolidaysTableSyncResp = nullptr;
        bSubtag_HolidaysTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedtag_HolidaysTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedtag_HolidaysTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerTotag_HolidaysTableResp(0, "", 0, "",
                                                                     obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerTotag_HolidaysTableResp(1, obj.value("code").toString(), obj.value("type").toInt(), obj.value("UUIDHolGroups").toString(),
                                                                       obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerTotag_HolidaysTableResp(2, obj.value("code").toString(), obj.value("type").toInt(), obj.value("UUIDHolGroups").toString(),
                                                                     obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedtag_HolidaysTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedtag_HolidaysTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedtag_HolidaysTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedtag_HolidaysTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedtag_HolidaysTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubtag_HolidaysTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_HolidaysTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubtag_HolidaysTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedtag_HolidaysTableResp--> " << "QMqttSubscription::Subscribed";
        bSubtag_HolidaysTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_HolidaysTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubtag_HolidaysTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedtag_HolidaysTableResp--> " << "QMqttSubscription::Error";
        bSubtag_HolidaysTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedtag_HolidaysTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedtag_HolidaysTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  tag_AccessPairsTableSync to Server Sig from JobHandler Calss  ############
void MqttClass::on_sig_tag_AccessPairsTableSync(QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID)
{
    if(bSubtag_AccessPairsTableSyncResp && mqtt_check_subscribe())
        MqttPubtag_AccessPairsTableSync(code, type, UUID_Terminals, UUID_Shifts, UUID);

    return;
}

//##################################################  tag_AccessPairsTableSync to Server #######################################
void MqttClass::MqttPubtag_AccessPairsTableSync(QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"type\":%2, \"UUID_Terminals\":\"%3\", \"UUID_Shifts\":\"%4\", \"UUID\":\"%5\"}";
    msg = msg.arg(code);
    msg = msg.arg(QString::number(type));
    msg = msg.arg(UUID_Terminals);
    msg = msg.arg(UUID_Shifts);
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXTAG_ACCESSPAIRSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}

bool MqttClass::MqttSubtag_AccessPairsTableSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubtag_AccessPairsTableSync--> ";
    QString topicString = QString("IroTeam/%1/TAG_ACCESSPAIRSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subtag_AccessPairsTableSyncResp = client->subscribe(topicFilter,qos);

    if(subtag_AccessPairsTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subtag_AccessPairsTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedtag_AccessPairsTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subtag_AccessPairsTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedtag_AccessPairsTableResp ,Qt::QueuedConnection);

//        QObject::connect(subtag_AccessPairsTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedtag_AccessPairsTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubtag_AccessPairsTableSync()
{
    if(bSubtag_AccessPairsTableSyncResp && subtag_AccessPairsTableSyncResp){
//        qInfo() << MQTTCLASS << "MqttUnSubtag_AccessPairsTableSync--> ";
        QString topicString = QString("IroTeam/%1/TAG_ACCESSPAIRSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subtag_AccessPairsTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subtag_AccessPairsTableSyncResp->deleteLater();
        subtag_AccessPairsTableSyncResp = nullptr;
        bSubtag_AccessPairsTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedtag_AccessPairsTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedtag_AccessPairsTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(bJobThreadJobRun){
                    if(obj.value("action").toString() == "delete"){
                        emit this->sig_ServerTotag_AccessPairsTableResp(0, "", 0, "", "",
                                                                     obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "update"){
                       emit this->sig_ServerTotag_AccessPairsTableResp(1, obj.value("code").toString(), obj.value("type").toInt(), obj.value("UUID_Terminals").toString(), obj.value("UUID_Shifts").toString(),
                                                                       obj.value("UUID").toString());
                    }
                    if(obj.value("action").toString() == "insert"){
                        emit this->sig_ServerTotag_AccessPairsTableResp(2, obj.value("code").toString(), obj.value("type").toInt(), obj.value("UUID_Terminals").toString(), obj.value("UUID_Shifts").toString(),
                                                                        obj.value("UUID").toString());
                    }
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedtag_AccessPairsTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedtag_AccessPairsTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedtag_AccessPairsTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedtag_AccessPairsTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedtag_AccessPairsTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubtag_AccessPairsTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_AccessPairsTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubtag_AccessPairsTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedtag_AccessPairsTableResp--> " << "QMqttSubscription::Subscribed";
        bSubtag_AccessPairsTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtag_AccessPairsTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubtag_AccessPairsTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedtag_AccessPairsTableResp--> " << "QMqttSubscription::Error";
        bSubtag_AccessPairsTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedtag_AccessPairsTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedtag_AccessPairsTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

//##############################################################################################################################
//##############################################################################################################################
//##################################################  tagsTableSync to Server Sig from JobHandler Calss  #######################
void MqttClass::on_sig_tagsTableSync(QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID)
{
    if(bSubtagsTableSyncResp && mqtt_check_subscribe())
        MqttPub_tagsTableSync(code, UUID_Tagged, taggedType, UUID_Tag, tagType, UUID);

    return;
}

//##################################################  tagsTableSync to Server  #################################################
void MqttClass::MqttPub_tagsTableSync(QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID)
{
    QString msg = "{\"code\":\"%1\", \"UUID_Tagged\":\"%2\", \"taggedType\":%3, \"UUID_Tag\":\"%4\", \"tagType\":%5, \"UUID\":\"%6\"}";
    msg = msg.arg(code);
    msg = msg.arg(UUID_Tagged);
    msg = msg.arg(QString::number(taggedType));
    msg = msg.arg(UUID_Tag);
    msg = msg.arg(QString::number(tagType));
    msg = msg.arg(UUID);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/LINUXTAGSTABLE").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);
}
bool MqttClass::MqttSubtagsTableSync()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubtagsTableSync--> ";
    QString topicString = QString("IroTeam/%1/TAGSTABLESYNCRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subtagsTableSyncResp = client->subscribe(topicFilter,qos);

    if(subtagsTableSyncResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subtagsTableSyncResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedtagsTableResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subtagsTableSyncResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedtagsTableResp ,Qt::QueuedConnection);

//        QObject::connect(subtag_AccessPairsTableSyncResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedtagsTableResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubtagsTableSync()
{
    if(bSubtagsTableSyncResp && subtagsTableSyncResp){
        qInfo() << MQTTCLASS << "MqttUnSubtagsTableSync--> ";
        QString topicString = QString("IroTeam/%1/TAGSTABLESYNCRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subtagsTableSyncResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subtagsTableSyncResp->deleteLater();
        subtagsTableSyncResp = nullptr;
        bSubtagsTableSyncResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedtagsTableResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedtagsTableResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("UUID").toString() != ""){
                if(obj.value("action").toString() == "delete"){
                    emit this->sig_ServerTotagsTableResp(0, "", "", 0, "", 0,
                                                                 obj.value("UUID").toString());
                }
                if(obj.value("action").toString() == "update"){
                   emit this->sig_ServerTotagsTableResp(1, obj.value("code").toString(),
                                                                   obj.value("UUID_Tagged").toString(), obj.value("taggedType").toInt(),
                                                                   obj.value("UUID_Tag").toString(), obj.value("tagType").toInt(),
                                                                   obj.value("UUID").toString());
                }
                if(obj.value("action").toString() == "insert"){
                    emit this->sig_ServerTotagsTableResp(2, obj.value("code").toString(),
                                                         obj.value("UUID_Tagged").toString(), obj.value("taggedType").toInt(),
                                                         obj.value("UUID_Tag").toString(), obj.value("tagType").toInt(),
                                                         obj.value("UUID").toString());
                }
            }
            else{
                qWarning() << MQTTCLASS << "on_messageReceivedtagsTableResp--> " << "Invelid Request\n" << mqttMsg.payload();
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedtagsTableResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedtagsTableResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedtagsTableResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedtagsTableResp--> " << "QMqttSubscription::Unsubscribed";
        bSubtagsTableSyncResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtagsTableResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubtagsTableSyncResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedtagsTableResp--> " << "QMqttSubscription::Subscribed";
        bSubtagsTableSyncResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedtagsTableResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubtagsTableSyncResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedtagsTableResp--> " << "QMqttSubscription::Error";
        bSubtagsTableSyncResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedtagsTableResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedtagsTableResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

void MqttClass::on_sig_SendToServerAuthAnd_Slot(Auth_And authand)
{
    //    qDebug() << "on_sig_SendToServerAuth_And";
    if(bSubAuthAndResp && mqtt_check_subscribe())
        MqttPubAuthAnd(authand.userUniqueId_1, authand.userUniqueId_2, authand.readerId_1, authand.readerId_2, authand.readerExpId, authand.reqDateTime.toString("yyyy-MM-dd hh:mm:ss"));


    return;
}

void MqttClass::MqttPubAuthAnd(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, int readerExpId, QString reqDateTime)
{
    QString msg = "{\"userUniqueId1\":%1, \"userUniqueId2\":%2, \"readerId1\":%3, \"readerId2\":%4, \"readerExpId\":%5, \"reqDateTime\":\"%6\"}";
    msg = msg.arg(QString::number(userUniqueId1));
    msg = msg.arg(QString::number(userUniqueId2));
    msg = msg.arg(QString::number(readerId1));
    msg = msg.arg(QString::number(readerId2));
    msg = msg.arg(QString::number(readerExpId));
    msg = msg.arg(reqDateTime);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/AUTHAND").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);

    return;
}

bool MqttClass::MqttSubAuthAndResp()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubAuthAndResp--> ";
    QString topicString = QString("IroTeam/%1/AUTHANDRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subAuthAndResp = client->subscribe(topicFilter,qos);

    if(subAuthAndResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subAuthAndResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedAuthAndResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subAuthAndResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedAuthAndResp ,Qt::QueuedConnection);

//        QObject::connect(subAuthAndResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedAuthAndResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubAuthAndResp()
{
    if(subAuthAndResp && bSubAuthAndResp){
        qInfo() << MQTTCLASS << "MqttUnSubAuthAndResp--> ";
        QString topicString = QString("IroTeam/%1/AUTHANDRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subAuthAndResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subAuthAndResp->deleteLater();
        subAuthAndResp = nullptr;
        bSubAuthAndResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedAuthAndResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedAuthAndResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
//            if(obj.value("errorCode").toInt() == 0){
            if(bTableThreadJobRun)
                emit this->sig_ServerToAuthAndResp(obj.value("userUniqueId1").toInt(),
                                                obj.value("userUniqueId2").toInt(),
                                                obj.value("readerId1").toInt(),
                                                obj.value("readerId2").toInt(),
                                                obj.value("isHoliday1").toInt(),
                                                obj.value("isHoliday2").toInt(),
                                                obj.value("errorCode1").toInt(),
                                                obj.value("errorCode2").toInt(),
                                                obj.value("errorString1").toString(),
                                                obj.value("errorString2").toString(),
                                                obj.value("readerExpId").toInt(),
                                                obj.value("reqDateTime").toVariant().toDateTime(),
                                                obj.value("isSuccess").toInt());
//            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedAuthAndResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedAuthAndResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedAuthAndResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedAuthAndResp--> " << "QMqttSubscription::Unsubscribed";
        bSubAuthAndResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAuthAndResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubAuthAndResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedAuthAndResp--> " << "QMqttSubscription::Subscribed";
        bSubAuthAndResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedAuthAndResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubAuthAndResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedAuthAndResp--> " << "QMqttSubscription::Error";
        bSubAuthAndResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedAuthAndResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedAuthAndResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

void MqttClass::on_sig_SendToServerApbAnd_slot(Apb_And apband)
{
    // qDebug() << "on_sig_SendToServerApbAnd_slot";
    if(bSubApbAndResp && mqtt_check_subscribe())
        MqttPubApbAnd(apband.userUniqueId_1, apband.userUniqueId_2, apband.readerId_1, apband.readerId_2, apband.readerExpId, apband.reqDateTime.toString("yyyy-MM-dd hh:mm:ss"));

    return;
}
//##################################################  Apb to Server  ##################################################

void MqttClass::MqttPubApbAnd(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, int readerExpId, QString reqDateTime)
{

    QString msg = "{\"userUniqueId1\":%1, \"userUniqueId2\":%2, \"readerId1\":%3, \"readerId2\":%4, \"readerExpId\":%5, \"reqDateTime\":\"%6\"}";
    msg = msg.arg(QString::number(userUniqueId1));
    msg = msg.arg(QString::number(userUniqueId2));

    msg = msg.arg(QString::number(readerId1));
    msg = msg.arg(QString::number(readerId2));

    msg = msg.arg(QString::number(readerExpId));
    msg = msg.arg(reqDateTime);
//    qDebug() << msg;

    QString topic = QString("IroTeam/%1/APBAND").arg(userName);

//    qDebug() << msg;
//    qDebug() << topic;

    MqttPub(topic, msg, 1);

    return;
}

//##################################################  Antipassback Resp from Server  ##################################################
bool MqttClass::MqttSubApbAndResp()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubApbAndResp--> ";
    QString topicString = QString("IroTeam/%1/APBANDRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subApbAndResp = client->subscribe(topicFilter,qos);

    if(subApbAndResp){
        ret = true;

        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subApbAndResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedApbAndResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subApbAndResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedApbAndResp ,Qt::QueuedConnection);

//        QObject::connect(subApbAndResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedApbAndResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubApbAndResp()
{
    if(subApbAndResp && bSubApbAndResp){
        qInfo() << MQTTCLASS << "MqttUnSubApbAndResp--> ";
        QString topicString = QString("IroTeam/%1/APBANDRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subApbAndResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

        subApbAndResp->deleteLater();
        subApbAndResp = nullptr;
        bSubApbAndResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedApbAndResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedApbAndResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(obj.value("errorCode").toInt() == 0){
//                QString reqDateTimeString = obj.value("reqDateTime").toVariant().toDateTime();
                if(bTableThreadJobRun)
                    emit this->sig_ServerToApbAndResp(obj.value("userUniqueId1").toInt(),
                                                   obj.value("userUniqueId2").toInt(),
                                                   obj.value("readerId1").toInt(),
                                                   obj.value("readerId2").toInt(),
                                                   obj.value("readerExpId").toInt(),
                                                   obj.value("reqDateTime").toVariant().toDateTime(),
                                                   obj.value("antiPassBack").toInt());
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedApbAndResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedApbAndResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }

    return;
}

void MqttClass::on_stateChangedApbAndResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedApbAndResp--> " << "QMqttSubscription::Unsubscribed";
        bSubApbAndResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedApbAndResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubApbAndResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedApbAndResp--> " << "QMqttSubscription::Subscribed";
        bSubApbAndResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedApbAndResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubApbAndResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedApbAndResp--> " << "QMqttSubscription::Error";
        bSubApbAndResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedApbAndResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedApbAndResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################
void MqttClass::on_sig_SendToServerLogAnd_slot(LogAnd logand)
{

//    qDebug() << "on_sig_SendToServerLogAnd_slot " << bSubLogAndResp << " , " << mqtt_check_subscribe();
    if(bSubLogAndResp && mqtt_check_subscribe())
        MqttPubLogAnd(logand.userUniqueId_1, logand.userUniqueId_2,
                      logand.readerId_1, logand.readerId_2,
                      logand.isHoliday_1, logand.isHoliday_2,
                      logand.isUser_1, logand.isUser_2,
                      logand.errorCode_1, logand.errorCode_2,
                      logand.errorString_1, logand.errorString_2,
                      logand.readerExpId, logand.reqDateTime.toString("yyyy-MM-dd hh:mm:ss"), logand.UUID, logand.isSuccess);

    return;
}

//##################################################  Log to Server  #########################################################
void MqttClass::MqttPubLogAnd(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, bool isHoliday1, bool isHoliday2, bool isUser1, bool isUser2, int errorCode1, int errorCode2, QString errorString1, QString errorString2, int readerExpId, QString reqDateTime, QString UUID, bool isSuccess)
{
    QString msg = "{\"userUniqueId1\":%1, \"userUniqueId2\":%2, \"readerId1\":%3, \"readerId2\":%4, \"isHoliday1\":%5, \"isHoliday2\":%6, \"isUser1\":%7, \"isUser2\":%8, \"errorCode1\":%9, \"errorCode2\":%10, \"errorString1\":\"%11\", \"errorString2\":\"%12\", \"readerExpId\":%13, \"reqDateTime\":\"%14\", \"isSuccess\":%15, \"UUID\":\"%16\"}";
    msg = msg.arg(QString::number(userUniqueId1));
    msg = msg.arg(QString::number(userUniqueId2));

    msg = msg.arg(QString::number(readerId1));
    msg = msg.arg(QString::number(readerId2));

    if(isHoliday1)
        msg = msg.arg(QString::number(1));
    else
        msg = msg.arg(QString::number(0));

    if(isHoliday2)
        msg = msg.arg(QString::number(1));
    else
        msg = msg.arg(QString::number(0));

    if(isUser1)
        msg = msg.arg(QString::number(1));
    else
        msg = msg.arg(QString::number(0));

    if(isUser2)
        msg = msg.arg(QString::number(1));
    else
        msg = msg.arg(QString::number(0));

    msg = msg.arg(QString::number(errorCode1));
    msg = msg.arg(QString::number(errorCode2));

    msg = msg.arg(errorString1);
    msg = msg.arg(errorString2);

    msg = msg.arg(QString::number(readerExpId));
    msg = msg.arg(reqDateTime);

    if(isSuccess)
        msg = msg.arg(QString::number(1));
    else
        msg = msg.arg(QString::number(0));

    msg = msg.arg(UUID);


    QString topic = QString("IroTeam/%1/LOGAND").arg(userName);

//    qDebug() << "MqttPubLogAnd --> msg = " << msg;
//    qDebug() << "MqttPubLogAnd --> topic = " << topic;

    MqttPub(topic, msg, 1);

    return;
}

//##################################################  Log Resp from Server  ##################################################
bool MqttClass::MqttSubLogAndResp()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubLogAndResp--> ";
    QString topicString = QString("IroTeam/%1/LOGANDRESP").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subLogAndResp = client->subscribe(topicFilter,qos);

    if(subLogAndResp){
        ret = true;
        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subLogAndResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedLogAndResp, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subLogAndResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedLogAndResp ,Qt::QueuedConnection);

//        QObject::connect(subLogAndResp, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedLogAndResp, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubLogAndResp()
{
    if(subLogAndResp && bSubLogAndResp){
        qInfo() << MQTTCLASS << "MqttUnSubLogAndResp--> ";
        QString topicString = QString("IroTeam/%1/LOGANDRESP").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subLogAndResp->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

//        QObject::disconnect(subLogAndResp, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedLogAndResp);
//        QObject::disconnect(subLogAndResp, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedLogAndResp);

        subLogAndResp->deleteLater();
        subLogAndResp = nullptr;
        bSubLogAndResp = false;
    }

    return;
}

void MqttClass::on_messageReceivedLogAndResp(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedLogAndResp--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(bTableThreadJobRun){
                emit this->sig_ServerToLogAndResp(obj.value("UUID").toString());
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedLogAndResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedLogAndResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }


    return;
}

void MqttClass::on_stateChangedLogAndResp(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedLogAndResp--> " << "QMqttSubscription::Unsubscribed";
        bSubLogAndResp = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedLogAndResp--> " << "QMqttSubscription::SubscriptionPending";
        bSubLogAndResp = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedLogAndResp--> " << "QMqttSubscription::Subscribed";
        bSubLogAndResp = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedLogAndResp--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubLogAndResp = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedLogAndResp--> " << "QMqttSubscription::Error";
        bSubLogAndResp = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedLogAndResp(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedLogAndResp--> " <<  qos;

//    return;
//}
//##############################################################################################################################
//##############################################################################################################################

void MqttClass::on_sig_InpsPub_slot(QString inputs_str)
{
    QString topic = QString("IroTeam/%1/INPUTS").arg(userName);
    if(mqtt_status == 2){
        MqttPub(topic, inputs_str, 1);
    }
    else{
        qInfo() << MQTTCLASS << "on_sig_InpsPub_slot--> can not publish msg: " <<  inputs_str << " on topic: " << topic;
    }

    return;
}




//##################################################  Lock Resp to server  #########################################################
void MqttClass::on_sig_SendToServerLock_slot(quint8 port, bool ok)
{
    QString msg = "";
    if(port == 1){
        if(ok)
            msg = "{\"p1\":\"ok\"}";
        else
            msg = "{\"p1\":\"nk\"}";
    }
    else if(port == 2){
        if(ok)
            msg = "{\"p2\":\"ok\"}";
        else
            msg = "{\"p2\":\"nk\"}";
    }
    else{
        //invalid port
    }


    QString topic = QString("IroTeam/%1/LOCKCMDRESP").arg(userName);

//    qDebug() << "MqttPubLogAnd --> msg = " << msg;
//    qDebug() << "MqttPubLogAnd --> topic = " << topic;

    MqttPub(topic, msg, 1);

    return;
}

bool MqttClass::MqttSubLock()
{
    bool ret = false;
//    qInfo() << MQTTCLASS << "MqttSubLock--> ";
    QString topicString = QString("IroTeam/%1/LOCKCMD").arg(userName);
    quint8 qos = 1;
    QMqttTopicFilter topicFilter;
    topicFilter.setFilter(topicString);
    subLock = client->subscribe(topicFilter,qos);

    if(subLock){
        ret = true;
        qRegisterMetaType<QMqttMessage>("QMqttMessage");
        QObject::connect(subLock, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedLock, Qt::QueuedConnection);

        qRegisterMetaType<QMqttSubscription::SubscriptionState>("SubscriptionState");
        QObject::connect(subLock, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedLock,Qt::QueuedConnection);

//        QObject::connect(subLock, &QMqttSubscription::qosChanged, this, &MqttClass::on_qosChangedLock, Qt::QueuedConnection);
    }
    else{
        ret = false;
    }

    return ret;
}

void MqttClass::MqttUnSubLock()
{
   if(subLock && bSubLogAndResp){
        qInfo() << MQTTCLASS << "MqttUnSubLock--> ";
        QString topicString = QString("IroTeam/%1/LOCKCMD").arg(userName);
        QMqttTopicFilter topicFilter;
        topicFilter.setFilter(topicString);
        client->unsubscribe(topicFilter);

//        while(subLock->state() == QMqttSubscription::Subscribed){
//            QApplication::processEvents(QEventLoop::AllEvents);
//        }

//        QObject::disconnect(subLock, &QMqttSubscription::messageReceived, this, &MqttClass::on_messageReceivedLock);
//        QObject::disconnect(subLock, &QMqttSubscription::stateChanged, this, &MqttClass::on_stateChangedLoock);

        subLock->deleteLater();
        subLock = nullptr;
        bSubLock = false;
    }

    return;
}


//{"p1":"1"}
//{"p2":"1"}
void MqttClass::on_messageReceivedLock(QMqttMessage mqttMsg)
{
//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MQTTCLASS << "on_messageReceivedLock--> " <<  mqttMsg.payload();

    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(mqttMsg.payload());
    // check validity of the document
    if(!doc.isNull()){
        if(doc.isObject()){
            obj = doc.object();
            if(bTableThreadJobRun){
                if(mqttMsg.payload().length() == 10 && (mqttMsg.payload() == "{\"p1\":\"1\"}" || mqttMsg.payload() == "{\"p2\":\"1\"}")){

                    if(mqttMsg.payload() == "{\"p1\":\"1\"}"){
                        emit this->sig_ServerToCypressLock(1);
                    }
                    else if(mqttMsg.payload() == "{\"p2\":\"1\"}"){
                        emit this->sig_ServerToCypressLock(2);
                    }
                    else{
                        //inavlid pm
                    }
                }
                else{
                    //invalid length
                }
            }
        }
        else{
            qWarning() << MQTTCLASS << "on_messageReceivedLogAndResp--> " << "Document is not an object\n" << mqttMsg.payload();
        }
    }
    else{
        qWarning() << MQTTCLASS << "on_messageReceivedLogAndResp--> " << "Invalid JSON...\n" << mqttMsg.payload() ;
    }


    return;
}

void MqttClass::on_stateChangedLock(QMqttSubscription::SubscriptionState subState)
{
    switch (subState) {
    case QMqttSubscription::Unsubscribed:
        qInfo() << MQTTCLASS << "on_stateChangedLock--> " << "QMqttSubscription::Unsubscribed";
        bSubLock = false;
        break;
    case QMqttSubscription::SubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedLock--> " << "QMqttSubscription::SubscriptionPending";
        bSubLock = false;
        break;
    case QMqttSubscription::Subscribed:
//        qInfo() << MQTTCLASS << "on_stateChangedLock--> " << "QMqttSubscription::Subscribed";
        bSubLock = true;
        break;
    case QMqttSubscription::UnsubscriptionPending:
        qInfo() << MQTTCLASS << "on_stateChangedLock--> " << "QMqttSubscription::UnsubscriptionPending";
        bSubLock = false;
        break;
    case QMqttSubscription::Error:
        qInfo() << MQTTCLASS << "on_stateChangedLock--> " << "QMqttSubscription::Error";
        bSubLock = false;
        break;
    default:
        break;
    }
    mqtt_check_subscribe_status();

    return;
}

//void MqttClass::on_qosChangedLock(quint8 qos)
//{
//    qInfo() << MQTTCLASS << "on_qosChangedLock--> " <<  qos;

//    return;
//}


void MqttClass::on_Delay_Timeout()
{
    delay_Timer->stop();
    delay = false;

    return;
}

void MqttClass::Delay_ms(quint32 ms)
{
    delay_Timer->start(ms);
    delay = true;
    while(delay){
        QApplication::processEvents(QEventLoop::AllEvents);
    }

    return;
}



#include "mainclass.h"

MainClass::MainClass(QObject *parent) : QObject(parent)
{
    return;
}

MainClass::~MainClass()
{
    return;
}

void MainClass::Init()
{
    mainclass_i2c = nullptr;
    
    mainclass_cy_thread = nullptr;
    mainclass_cy_worker = nullptr;
    
    mainclass_auth_thread = nullptr;
    maincalss_auth_worker = nullptr;

    ntp_Timer = nullptr;
    ntp = false;
    rtc_Timer = nullptr;
    watch_Timer = nullptr;

    return;
}

void MainClass::Config()
{
    mainclass_i2c = new I2cmasterClass(this);


    ntp_Timer = new QTimer(this);
    ntp_Timer->setSingleShot(true);
    ntp_Timer->setInterval(MAIN_NTP_TIMEOUT_MIN * 60 * 1000);
    ntp_Timer->stop();

    rtc_Timer = new QTimer(this);
    rtc_Timer->setSingleShot(true);
    rtc_Timer->setInterval(MAIN_RTC_TIMEOUT_SEC * 1000);
    rtc_Timer->stop();

    watch_Timer = new QTimer(this);
    watch_Timer->setSingleShot(true);
    watch_Timer->setInterval(MAIN_WATCH_TIMEOUT_SEC * 1000);
    watch_Timer->stop();

    QObject::connect(ntp_Timer, &QTimer::timeout, this, &MainClass::on_ntpTimerTimeout, Qt::QueuedConnection);
    QObject::connect(rtc_Timer, &QTimer::timeout, this, &MainClass::on_rtcTimerTimeout, Qt::QueuedConnection);
    QObject::connect(watch_Timer, &QTimer::timeout, this, &MainClass::on_watchTimerTimeout, Qt::QueuedConnection);


    //********************** just release **********************
    #ifndef QT_DEBUG
        watch_Timer->start();
    #endif
    return;
}

void MainClass::on_StartWorker()
{
    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";
        qInfo() << MAINCLASS << "on_StartWorker--> " << "start main thread class";
        qInfo() << MAINCLASS << "on_StartWorker--> " << "main thread id = " << QThread::currentThreadId();
    #endif

    Init(); //clear all local objects. global objects in global.h and global.cpp have been initiated.
    Config(); //new all pointer and config other objects

    QObject::connect(this, &MainClass::StartedWorker, this, &MainClass::on_StartedWorker, Qt::QueuedConnection);
    StartApplication(); //if cy thread run continue another -> StartInterrupt();

    return;
}

void MainClass::StartApplication()
{
    GetInfoFromIni();
    GetNetworkFromSys();
    CheckHardware();

    ntp = mainclass_i2c->SetDateTimeToSysWithNtpdateTerminal(true, gl_Ini_NtpUrl);
    if(ntp){
        mainclass_i2c->SetDateTimeToRtcWithI2c(true);
        ntp_Timer->start();
    }
    else{
        GetInfoFromRtc();
    }


    //test that mysql has been activated.
    Database* _myDb;
    queryResult res;
    _myDb = new Database(this);
    bool ret = _myDb->Db_open("TestConnectionMain");
    if(ret){
        res = _myDb->Db_execquery("SELECT UUID()");
    }
    else{
        //reboot board
        Reboot();
        //QApplication::quit();
    }

    if(!res.success){
        QApplication::quit();
    }

    _myDb->Db_close();
    _myDb->deleteLater();
    delete _myDb;


    emit this->StartedWorker();

    return;
}

void MainClass::GetInfoFromIni()
{
    qInfo() << "";
    qInfo() << "";
    QSettings setting(gl_Prog_ConfigFileName, QSettings::IniFormat);

//Database Info
    //database name
    gl_Ini_DbName = setting.value(DB_NAME, DB_NAME_DEFAULT).toString();

    //databaseoption
    gl_Ini_DbOption = setting.value(DB_OPTION, DB_OPTION_DEFAULT).toString();

    //driver name
    gl_Ini_DbDriver = setting.value(DB_DRIVER, DB_DRIVER_DEFAULT).toString();

    //hiost ip, username, password
    gl_Ini_DbHostIp = setting.value(DB_HOSTIP, DB_HOSTNAME_DEFAULT).toString();
    gl_Ini_DbPassword = setting.value(DB_PASS, DB_PASS_DEFAULT).toString();
    gl_Ini_DbUsername = setting.value(DB_USER, DB_USER_DEFAULT).toString();

    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "getConfigFromIni: Database Info "
                << "gl_DbDriver_ini = "      << gl_Ini_DbDriver      << " , "
                << "gl_DbHostIp_ini = "      << gl_Ini_DbHostIp      << " , "
                << "gl_DbUsername_ini = "    << gl_Ini_DbUsername    << " , "
                << "gl_DbPassword_ini = "    << gl_Ini_DbPassword    << " , "
                << "gl_DbName_ini = "        << gl_Ini_DbName        << " , "
                << "gl_DbOption_in i= "      << gl_Ini_DbOption      ;
        qInfo() << "";
    #endif

//Ntp Server Info
    //url
    gl_Ini_NtpUrl = setting.value(NTP_URL, NTP_URL_DEFAULT).toString();

    if(!MAIN_LOG_TO_FILE){
        qInfo() << MAINCLASS << "getConfigFromIni: Ntp Server Info "
                << "gl_Ini_NtpUrl = "      << gl_Ini_NtpUrl;
        qInfo() << "";
    }

//mqtt server
    //ip
    gl_Ini_SrvIp = setting.value(SRV_IP, SRV_IP_DEFAULT).toString();

    //port
    gl_Ini_SrvPort = setting.value(SRV_PORT, SRV_PORT_DEFAULT).toInt();

    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "getConfigFromIni: Mqtt Server Info "
                << "gl_Ini_SrvIp = "      << gl_Ini_SrvIp   << " , "
                << "gl_Ini_SrvPort = "    << gl_Ini_SrvPort;
        qInfo() << "";
    #endif

//sync time
        gl_Ini_SyncTmS = setting.value(SYNC_TM_S, SYNC_TM_S_DEFUALT).toUInt();

        #ifdef QT_DEBUG
            qInfo() << MAINCLASS << "gl_Ini_SyncTmS: Sync time Info "
                    << "gl_Ini_SyncTmS = "      << gl_Ini_SyncTmS;
            qInfo() << "";
        #endif

//Peripherals Info
    //Cypress Uart port
    gl_Ini_SerialCyPort = setting.value(P_CY_UART, P_CY_UART_DEFAULT).toString();

    //Rtc I2C Port
    gl_Ini_I2cDevicePort = setting.value(P_RTC_I2C, P_RTC_I2C_DEFAULT).toString();

    //Rtc Address
    gl_Ini_Ds1307DevAddr = setting.value(P_RTC_ADDR, P_RTC_ADDR_DEFAULT).toInt();

    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "getConfigFromIni: Peripherals Info "
                << "gl_Ini_SerialCyPort = "     << gl_Ini_SerialCyPort   << " , "
                << "gl_Ini_I2cDevicePort = "    << gl_Ini_I2cDevicePort  << " , "
                << "gl_Ini_Ds1307DevAddr = "    << gl_Ini_Ds1307DevAddr  ;
        qInfo() << "";
    #endif

//Will
    //flag
    //gl_Ini_WillFg = setting.value(WILL_FLAG, WILL_FLAG_DEFAULT).toInt();

    //qos
    gl_Ini_WillQos = setting.value(WILL_QOS, WILL_QOS_DEFAULT).toInt();

    //retain
    gl_Ini_WillRetain = setting.value(WILL_RETAIN, WILL_RETAIN_DEFAULT).toBool();

    //will disconnect msg
    gl_Ini_WillMsg_Dc = setting.value(WILL_MSG_DISCONNECT, WILL_MSG_DISCONNECT_DEFAULT).toString();

    //will connect msg
    gl_Ini_WillMsg_Cc = setting.value(WILL_MSG_CONNECT, WILL_MSG_CONNECT_DEFAULT).toString();

    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "getConfigFromIni: Will Info "
                //<< "gl_Ini_WillFg = "       << gl_Ini_WillFg        << " , "
                << "gl_Ini_WillQos = "      << gl_Ini_WillQos       << " , "
                << "gl_Ini_WillRetain = "   << gl_Ini_WillRetain    << " , "
                << "gl_Ini_WillMsg_Dc = "   << gl_Ini_WillMsg_Dc    << " , "
                << "gl_Ini_WillMsg_Cc = "   << gl_Ini_WillMsg_Cc    ;
        qInfo() << "";
    #endif


//topics config
    //keep alive
    gl_Ini_MqttKeepAlive = setting.value(MQTT_KEEP_ALIVE, MQTT_KEEP_ALIVE_DEFAULT).toInt();

    //qos
    gl_Ini_MqttQos = setting.value(MQTT_QOS, MQTT_QOS_DEFAULT).toInt();

    //retain
    gl_Ini_MqttRetain = setting.value(MQTT_RETAIN, MQTT_RETAIN_DEFAULT).toBool();

    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "getConfigFromIni: Topics Config Info "
                << "gl_Ini_MqttKeepAlive = "    << gl_Ini_MqttKeepAlive     << " , "
                << "gl_Ini_MqttQos = "          << gl_Ini_MqttQos           << " , "
                << "gl_Ini_MqttRetain = "       << gl_Ini_MqttRetain        ;
        qInfo() << "";
    #endif


//Port1
    //bits
    gl_Ini_Port1Bits = setting.value(PORT1_BITS, PORT1_BITS_DEFAULT).toInt();

    //timeout
    gl_Ini_Port1Timeout = setting.value(PORT1_TIMEOUT_MS, PORT1_TIMEOUT_MS_DEFAULT).toInt();

    //proc timeout
    gl_Ini_Port1ProcTimeout = setting.value(PORT1_PROC_TIMEOUT_MS, PORT1_PROC_TIMEOUT_MS_DEFAULT).toInt();

    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "getConfigFromIni: Port1 Config Info "
                << "gl_Ini_Port1Bits = "        << gl_Ini_Port1Bits             << " , "
                << "gl_Ini_Port1Timeout = "     << gl_Ini_Port1Timeout          << " , "
                << "gl_Ini_Port1ProcTimeout = " << gl_Ini_Port1ProcTimeout      ;
        qInfo() << "";
    #endif

//Port2
    //bits
    gl_Ini_Port2Bits = setting.value(PORT2_BITS, PORT2_BITS_DEFAULT).toInt();

    //timeout
    gl_Ini_Port2Timeout = setting.value(PORT2_TIMEOUT_MS, PORT2_TIMEOUT_MS_DEFAULT).toInt();

    //proc timeout
    gl_Ini_Port2ProcTimeout = setting.value(PORT2_PROC_TIMEOUT_MS, PORT2_PROC_TIMEOUT_MS_DEFAULT).toInt();

    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "getConfigFromIni: Port2 Config Info "
                << "gl_Ini_Port1Bits = "        << gl_Ini_Port2Bits             << " , "
                << "gl_Ini_Port1Timeout = "     << gl_Ini_Port2Timeout          << " , "
                << "gl_Ini_Port2ProcTimeout = " << gl_Ini_Port2ProcTimeout      ;
        qInfo() << "";
    #endif


    //And
        //and
        gl_ini_And = setting.value(AND_AND, AND_AND_DEFAULT).toBool();

        //port
        gl_ini_AndPort = setting.value(AND_PORT, AND_PORT_DEFAULT).toInt();

        //timeout
        gl_ini_AndGetTimeout = setting.value(AND_GET_TIMEOUT, AND_GET_TIMEOUT_DEFAULT).toInt();

        #ifdef QT_DEBUG
            qInfo() << MAINCLASS << "getConfigFromIni: And Config Info "
                    << "gl_ini_And = "          << gl_ini_And           << " , "
                    << "gl_ini_AndPort = "      << gl_ini_AndPort       << " , "
                    << "gl_ini_AndTimeout = "   << gl_ini_AndGetTimeout    ;
            qInfo() << "";
        #endif

    return;
}

void MainClass::GetNetworkFromSys()
{
    int size = QNetworkInterface::allInterfaces().size();
    for(int i = 0; i < size; i++){
        if(QNetworkInterface::allInterfaces().at(i).type() == QNetworkInterface::Ethernet && QNetworkInterface::allInterfaces().at(i).humanReadableName() == "eth0"){
            gl_Eth_MacAddr = QNetworkInterface::allInterfaces().at(i).hardwareAddress();
            gl_Eth_MacAddr = gl_Eth_MacAddr.toUpper();
            qInfo() << MAINCLASS << "GetNetworkFromSys: Eth0 Mac Address gl_Eth_MacAddr : " <<  gl_Eth_MacAddr;
        }

//        if(QNetworkInterface::allInterfaces().at(i).type() == QNetworkInterface::Wifi && QNetworkInterface::allInterfaces().at(i).humanReadableName() == "wlan0"){
//            gl_Wlan_MacAddr = QNetworkInterface::allInterfaces().at(i).hardwareAddress();
//            gl_Wlan_MacAddr = gl_Wlan_MacAddr.toUpper();
//            qInfo() << MAINCLASS << "GetNetworkFromSys: Wlan0 Mac Address gl_Wlan_MacAddr : " <<  gl_Wlan_MacAddr;
//            qInfo() << "";
//        }
    }

    return;
}

void MainClass::GetInfoFromRtc()
{
    qInfo() << MAINCLASS << "GetInfoFromRtc--> " << "get rtc data and set to system";

    mainclass_i2c->GetDateTimeFromRtcWithI2c(true);
    mainclass_i2c->SetDateTimeToSysWithTerminal();

    on_ntpTimerTimeout();
    qInfo() << "";
    return;
}

void MainClass::on_StartedWorker()
{
    StartCy();

    qInfo() << "";

    return;
}

void MainClass::StartCy()
{
    mainclass_cy_thread = new QThread;
    mainclass_cy_worker = new Cypress_Serial;
    mainclass_cy_worker->moveToThread(mainclass_cy_thread);
    mainclass_cy_thread->start();
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::StartWorker, mainclass_cy_worker, &Cypress_Serial::on_StartWorker, Qt::QueuedConnection);
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::StartedWorker, this, &MainClass::on_StartedCyThread, Qt::QueuedConnection);
    emit mainclass_cy_worker->StartWorker();

    return;
}

void MainClass::on_StartedCyThread()
{
    StartAuth();

    return;
}



void MainClass::StartAuth()
{
    mainclass_auth_thread = new QThread;
    maincalss_auth_worker = new AthenticationClass;
    maincalss_auth_worker->moveToThread(mainclass_auth_thread);
    mainclass_auth_thread->start();
    QObject::connect(maincalss_auth_worker, &AthenticationClass::StartWorker, maincalss_auth_worker, &AthenticationClass::on_StartWorker, Qt::QueuedConnection);
    QObject::connect(maincalss_auth_worker, &AthenticationClass::StartedWorker, this, &MainClass::on_StartedAuthWorker, Qt::QueuedConnection);
    emit maincalss_auth_worker->StartWorker();

    return;
}

 void MainClass::on_StartedAuthWorker()
 {
    StartTable();

    return;
 }

 void MainClass::StartTable()
 {
     mainclass_handler_thread = new QThread;
     maincalss_handler_worker = new TableHandler;
     maincalss_handler_worker->moveToThread(mainclass_handler_thread);
     mainclass_handler_thread->start();
     QObject::connect(maincalss_handler_worker, &TableHandler::StartWorker, maincalss_handler_worker, &TableHandler::on_StartWorker, Qt::QueuedConnection);
     QObject::connect(maincalss_handler_worker, &TableHandler::StartedWorker, this, &MainClass::on_StartedTableWorker, Qt::QueuedConnection);
     emit maincalss_handler_worker->StartWorker();

     return;
 }

void MainClass::on_StartedTableWorker()
{
    StartMqtt();

    return;
}

void MainClass::StartMqtt()
{
    mainclass_mqtt_thread = new QThread;

    maincalss_mqtt_worker = new MqttClass;
    maincalss_mqtt_worker->moveToThread(mainclass_mqtt_thread);
    mainclass_mqtt_thread->start();

    QObject::connect(maincalss_mqtt_worker, &MqttClass::StartWorker, maincalss_mqtt_worker, &MqttClass::on_StartWorker, Qt::QueuedConnection);
    QObject::connect(maincalss_mqtt_worker, &MqttClass::StartedWorker, this, &MainClass::on_StartedMqtt, Qt::QueuedConnection);

    emit maincalss_mqtt_worker->StartWorker();

    return;
}



void MainClass::on_StartedMqtt()
{
    StartJob();

    return;
}

void MainClass::StartJob()
{
    mainclass_job_thread = new QThread;

    maincalss_job_worker = new JobHandler;
    maincalss_job_worker->moveToThread(mainclass_job_thread);
    mainclass_job_thread->start();

    QObject::connect(maincalss_job_worker, &JobHandler::StartWorker, maincalss_job_worker, &JobHandler::on_StartWorker, Qt::QueuedConnection);
    QObject::connect(maincalss_job_worker, &JobHandler::StartedWorker, this, &MainClass::on_StartedJob, Qt::QueuedConnection);

    emit maincalss_job_worker->StartWorker();

    return;
}

void MainClass::on_StartedJob()
{
    qInfo() << "";
    qInfo() << "";
    
    //Cy signals ---------------------------------------------------------------
    //sig_StartCyThreadJob (main thread) --> on_StartCyJob (mainclass_cy_worker)
    QObject::connect(this, &MainClass::sig_StartCyThreadJob, mainclass_cy_worker, &Cypress_Serial::on_StartCyJob, Qt::QueuedConnection);
    //emit this->sig_StartedCyJob();

    //sig_AuthReq (mainclass_cy_worker) --> on_sig_AuthReq_slot (maincalss_auth_worker)
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::sig_AuthReq, maincalss_auth_worker, &AthenticationClass::on_sig_AuthReq_slot, Qt::QueuedConnection);

    //------------------------
    //------------------------
    //------------------------
    //sig_AuthReqAnd (mainclass_cy_worker) --> on_sig_AuthReqAnd_slot (maincalss_auth_worker)
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::sig_AuthReqAnd, maincalss_auth_worker, &AthenticationClass::on_sig_AuthReqAnd_slot, Qt::QueuedConnection);

    //------------------------
    //------------------------
    //------------------------

    //sig_InpReq (mainclass_cy_worker) --> on_sig_InpReq_slot (maincalss_handler_worker)
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::sig_InpReq, maincalss_handler_worker, &TableHandler::on_sig_InpReq_slot, Qt::QueuedConnection);

    //sig_StartedCyJob (mainclass_cy_worker) --> on_sig_StartedCyJob (main thread)
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::sig_StartedCyJob, this, &MainClass::on_sig_StartedCyJob, Qt::QueuedConnection);
    //--------------------------------------------------------------------------


    //Auth signals -------------------------------------------------------------
    //sig_StartAuthThreadJob (main thread) --> on_StartAuthJob (maincalss_auth_worker)
    QObject::connect(this, &MainClass::sig_StartAuthThreadJob, maincalss_auth_worker, &AthenticationClass::on_StartAuthJob, Qt::QueuedConnection);
    //emit this->on_StartAuthJob();

    //sig_toCy (maincalss_auth_worker) --> on_AuthRes (mainclass_cy_worker)
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_toCy, mainclass_cy_worker, &Cypress_Serial::on_AuthRes, Qt::QueuedConnection);

    //sig_toLog (maincalss_auth_worker) --> on_sig_toLog_slot (maincalss_handler_worker)
    qRegisterMetaType<AuthRes>("AuthRes");
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_toLog, maincalss_handler_worker, &TableHandler::on_sig_toLog_slot, Qt::QueuedConnection);

    qRegisterMetaType<Auth>("Auth");
    //sig_SendToServerAuth (maincalss_auth_worker) --> on_sig_SendToServerAuth_slot (maincalss_mqtt_worker)
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_SendToServerAuth, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerAuth_slot, Qt::QueuedConnection);



    //------------------------
    //------------------------
    //------------------------
    //sig_toLogAnd (maincalss_auth_worker) --> on_sig_toLogAnd_slot (maincalss_handler_worker)
    qRegisterMetaType<AuthRes_And>("AuthRes_And");
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_toLogAnd, maincalss_handler_worker, &TableHandler::on_sig_toLogAnd_slot, Qt::QueuedConnection);

    //sig_SendToServerAuth_And (maincalss_auth_worker) --> on_sig_SendToServerAuthAnd_Slot (maincalss_mqtt_worker)
    qRegisterMetaType<Auth_And>("Auth_And");
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_SendToServerAuth_And, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerAuthAnd_Slot, Qt::QueuedConnection);

    //sig_SendToServerApb_And (maincalss_auth_worker) --> on_sig_SendToServerApbAnd_Slot (maincalss_mqtt_worker)
    qRegisterMetaType<Apb_And>("Apb_And");
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_SendToServerApb_And, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerApbAnd_slot, Qt::QueuedConnection);
    //------------------------
    //------------------------
    //------------------------



    qRegisterMetaType<Apb>("Apb");
    //sig_SendToServerApb (maincalss_auth_worker) --> on_sig_SendToServerApb_slot (maincalss_mqtt_worker)
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_SendToServerApb, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerApb_slot, Qt::QueuedConnection);

    //sig_StartedAuthJob (maincalss_auth_worker) --> on_sig_StartedAuthJob (main thread)
    QObject::connect(maincalss_auth_worker, &AthenticationClass::sig_StartedAuthJob, this, &MainClass::on_sig_StartedAuthJob, Qt::QueuedConnection);
    //--------------------------------------------------------------------------


    //Table signals ------------------------------------------------------------
    //sig_StartTableThreadJob (main thread) --> on_StartTableJob (maincalss_handler_worker)
    QObject::connect(this, &MainClass::sig_StartTableThreadJob, maincalss_handler_worker, &TableHandler::on_StartTableJob, Qt::QueuedConnection);
    //emit this->sig_StartTableThreadJob();

    //sig_SendToServerLog (maincalss_handler_worker) --> on_sig_SendToServerLog_slot (maincalss_mqtt_worker)
    qRegisterMetaType<Log>("Log");
    QObject::connect(maincalss_handler_worker, &TableHandler::sig_SendToServerLog, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerLog_slot, Qt::QueuedConnection);

    //------------------------
    //------------------------
    //------------------------
    //sig_SendToServerLogAnd  (maincalss_auth_worker) --> on_sig_SendToServerLogAnd_slot (maincalss_mqtt_worker)
    qRegisterMetaType<LogAnd>("LogAnd");
    QObject::connect(maincalss_handler_worker, &TableHandler::sig_SendToServerLogAnd, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerLogAnd_slot, Qt::QueuedConnection);
    //------------------------
    //------------------------
    //------------------------

    //sig_SendToServerInp (maincalss_handler_worker) --> on_sig_SendToServerInp_slot (maincalss_mqtt_worker)
    qRegisterMetaType<Inp>("Inp");
    QObject::connect(maincalss_handler_worker, &TableHandler::sig_SendToServerInp, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerInp_slot, Qt::QueuedConnection);

    //sig_StartedTableJob (maincalss_handler_worker) --> on_sig_StartedTableJob (main thread)
    QObject::connect(maincalss_handler_worker, &TableHandler::sig_StartedTableJob, this, &MainClass::on_sig_StartedTableJob, Qt::QueuedConnection);
    //--------------------------------------------------------------------------


    //Job signals --------------------------------------------------------------
    //sig_startJobThreadJob (main thread) --> on_StartJobJob (maincalss_job_worker)
    QObject::connect(this, &MainClass::sig_startJobThreadJob, maincalss_job_worker, &JobHandler::on_StartJobJob, Qt::QueuedConnection);
    //emit this->sig_startJobThreadJob();

    //sig_StartSync (maincalss_job_worker) --> on_sig_StartStopSyncDatabase (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_StartSync, maincalss_mqtt_worker, &MqttClass::on_sig_StartStopSyncDatabase, Qt::QueuedConnection);

    //sig_TimezonesTableSync (maincalss_job_worker) --> on_sig_TimezonesTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_TimezonesTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_TimezonesTableSync, Qt::QueuedConnection);

    //sig_AccessTimezonesTableSync (maincalss_job_worker) --> on_sig_AccessTimezonesTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_AccessTimezonesTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_AccessTimezonesTableSync, Qt::QueuedConnection);

    //sig_AccessShiftsTableSync (maincalss_job_worker) --> on_sig_AccessShiftsTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_AccessShiftsTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_AccessShiftsTableSync, Qt::QueuedConnection);

    //sig_Tag_UsersTableSync (maincalss_job_worker) --> on_sig_tag_UsersTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_Tag_UsersTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_tag_UsersTableSync, Qt::QueuedConnection);

    //sig_Tag_TerminalsTableSync (maincalss_job_worker) --> on_sig_tag_TerminalsTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_Tag_TerminalsTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_tag_TerminalsTableSync, Qt::QueuedConnection);

    // -----------------------
    //sig_HolidaysTableSync (maincalss_job_worker) --> on_sig_HolidaysTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_HolidaysTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_HolidaysTableSync, Qt::QueuedConnection);

    //sig_HolidaysSetsTableSync (maincalss_job_worker) --> on_sig_HolidaysSetsTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_HolidaysSetsTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_HolidaysSetsTableSync, Qt::QueuedConnection);

    //sig_HolidaysGroupsTableSync (maincalss_job_worker) --> on_sig_HolidaysGroupsTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_HolidaysGroupsTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_HolidaysGroupsTableSync, Qt::QueuedConnection);
    // -----------------------

    //sig_Tag_HolidaysTableSync (maincalss_job_worker) --> on_sig_tag_HolidaysTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_Tag_HolidaysTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_tag_HolidaysTableSync, Qt::QueuedConnection);

    //sig_Tag_AccessPairsTableSync (maincalss_job_worker) --> on_sig_tag_AccessPairsTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_Tag_AccessPairsTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_tag_AccessPairsTableSync, Qt::QueuedConnection);

    //sig_TagsTableSync (maincalss_job_worker) --> on_sig_tagsTableSync (maincalss_mqtt_worker)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_TagsTableSync, maincalss_mqtt_worker, &MqttClass::on_sig_tagsTableSync, Qt::QueuedConnection);

    //sig_StartedJobJob (maincalss_job_worker) --> on_sig_StartedJobJob (main thread)
    QObject::connect(maincalss_job_worker, &JobHandler::sig_StartedJobJob, this, &MainClass::on_sig_StartedJobJob, Qt::QueuedConnection);
    //--------------------------------------------------------------------------


    //Mqtt signals -------------------------------------------------------------
    //sig_startJobThreadJob (main thread) --> on_StartJobJob (maincalss_mqtt_worker)
    QObject::connect(this, &MainClass::sig_startMqttThreadJob, maincalss_mqtt_worker, &MqttClass::on_StartMqttJob, Qt::QueuedConnection);
    //emit this->sig_startJobThreadJob();

    //sig_ServerToLogResp (maincalss_mqtt_worker) --> on_sig_ServerToLogResp_slot (maincalss_handler_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToLogResp, maincalss_handler_worker, &TableHandler::on_sig_ServerToLogResp_slot, Qt::QueuedConnection);

    //sig_ServerToInpResp (maincalss_mqtt_worker) --> on_sig_ServerToInpResp_slot (maincalss_handler_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToInpResp, maincalss_handler_worker, &TableHandler::on_sig_ServerToInpResp_slot, Qt::QueuedConnection);

    //sig_ServerToApbResp (maincalss_mqtt_worker) --> on_sig_ServerToApbResp_slot (maincalss_auth_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToApbResp, maincalss_auth_worker, &AthenticationClass::on_sig_ServerToApbResp_slot, Qt::QueuedConnection);

    //sig_ServerToAuthResp (maincalss_mqtt_worker) --> on_sig_ServerToAuthResp_slot (maincalss_auth_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToAuthResp, maincalss_auth_worker, &AthenticationClass::on_sig_ServerToAuthResp_slot, Qt::QueuedConnection);

    //sig_ServerToStartStopResp (maincalss_mqtt_worker) --> on_sig_ServerToAuthResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToStartStopResp, maincalss_job_worker, &JobHandler::on_sig_ServerToStartStopResp_slot, Qt::QueuedConnection);

    //------------------------
    //------------------------
    //------------------------
    //sig_ServerToAuthAndResp (maincalss_mqtt_worker) --> on_sig_ServerToAuthRespAnd_slot (maincalss_auth_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToAuthAndResp, maincalss_auth_worker, &AthenticationClass::on_sig_ServerToAuthRespAnd_slot, Qt::QueuedConnection);

    //sig_ServerToApbAndResp (maincalss_mqtt_worker) --> on_sig_ServerToApbRespAnd_slot (maincalss_auth_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToApbAndResp, maincalss_auth_worker, &AthenticationClass::on_sig_ServerToApbRespAnd_slot, Qt::QueuedConnection);

    //sig_ServerToLogAndResp (maincalss_mqtt_worker) --> on_sig_ServerToLogAndResp_slot (maincalss_handler_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToLogAndResp, maincalss_handler_worker, &TableHandler::on_sig_ServerToLogAndResp_slot, Qt::QueuedConnection);
    //------------------------
    //------------------------
    //------------------------


    //sig_ServerToAuthResp (maincalss_mqtt_worker) --> on_sig_ServerToTimezonesTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToTimezonesTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerToTimezonesTableResp_slot, Qt::QueuedConnection);

    //sig_ServerToAccessTimezonesTableResp (maincalss_mqtt_worker) --> on_sig_ServerToAccessTimezonesTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToAccessTimezonesTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerToAccessTimezonesTableResp_slot, Qt::QueuedConnection);

    //sig_ServerToAccessShiftsTableResp (maincalss_mqtt_worker) --> on_sig_ServerToAccessShiftsTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToAccessShiftsTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerToAccessShiftsTableResp_slot, Qt::QueuedConnection);

    //sig_ServerTotag_UsersTableResp (maincalss_mqtt_worker) --> on_sig_ServerTotag_UsersTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerTotag_UsersTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerTotag_UsersTableResp_slot, Qt::QueuedConnection);

    //sig_ServerTotag_TerminalsTableResp (maincalss_mqtt_worker) --> on_sig_ServerTotag_TerminalsTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerTotag_TerminalsTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerTotag_TerminalsTableResp_slot, Qt::QueuedConnection);

    //--------------------
    //sig_ServerToHolidaysTableResp (maincalss_mqtt_worker) --> on_sig_ServerToHolidaysTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToHolidaysTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerToHolidaysTableResp_slot, Qt::QueuedConnection);

    //sig_ServerToHolidaysSetsTableResp (maincalss_mqtt_worker) --> on_sig_ServerToHolidaysSetsTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToHolidaysSetsTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerToHolidaysSetsTableResp_slot, Qt::QueuedConnection);

    //sig_ServerToHolidaysGroupsTableResp (maincalss_mqtt_worker) --> on_sig_ServerTotHolidaysGroupsTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToHolidaysGroupsTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerToHolidaysGroupsTableResp_slot, Qt::QueuedConnection);
    //--------------------

    //sig_ServerTotag_HolidaysTableResp (maincalss_mqtt_worker) --> on_sig_ServerTotag_HolidaysTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerTotag_HolidaysTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerTotag_HolidaysTableResp_slot, Qt::QueuedConnection);

    //sig_ServerTotag_AccessPairsTableResp (maincalss_mqtt_worker) --> on_sig_ServerTotag_AccessPairsTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerTotag_AccessPairsTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerTotag_AccessPairsTableResp_slot, Qt::QueuedConnection);

    //sig_ServerTotagsTableResp (maincalss_mqtt_worker) --> on_sig_ServerTotagsTableResp_slot (maincalss_job_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerTotagsTableResp, maincalss_job_worker, &JobHandler::on_sig_ServerTotagsTableResp_slot, Qt::QueuedConnection);

    //sig_StartedMqttJob (maincalss_mqtt_worker) --> on_sig_StartedMqttJob (main thread)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_StartedMqttJob, this, &MainClass::on_sig_StartedMqttJob, Qt::QueuedConnection);


    //sig_CyMqttStatus (maincalss_mqtt_worker) --> on_sig_CyMqttStatus (mainclass_cy_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_CyMqttStatus, mainclass_cy_worker, &Cypress_Serial::on_sig_CyMqttStatus, Qt::QueuedConnection);
    //--------------------------------------------------------------------------


    //sig_InpsPub (mainclass_cy_worker) --> on_sig_InpsPub_slot (maincalss_mqtt_worker)
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::sig_InpsPub, maincalss_mqtt_worker, &MqttClass::on_sig_InpsPub_slot, Qt::QueuedConnection);




    //sig_ServerToCypressLock (maincalss_mqtt_worker) --> sig_ServerToCypressLock_slot (mainclass_cy_worker)
    QObject::connect(maincalss_mqtt_worker, &MqttClass::sig_ServerToCypressLock, mainclass_cy_worker, &Cypress_Serial::sig_ServerToCypressLock_slot, Qt::QueuedConnection);


    //sig_SendToServerLock (mainclass_cy_worker) --> on_sig_SendToServerLock_slot (maincalss_mqtt_worker)
    QObject::connect(mainclass_cy_worker, &Cypress_Serial::sig_SendToServerLock, maincalss_mqtt_worker, &MqttClass::on_sig_SendToServerLock_slot, Qt::QueuedConnection);


//    void sig_ServerToCypressLock(quint8 port); //mqtt
//    void sig_ServerToCypressLock_slot(quint8 port);  //cypress

//    void sig_SendToServerLock(quint8 port, bool ok); //cypress
//    void on_sig_SendToServerLock_slot(quint8 port, bool ok); //mqtt

    emit this->sig_StartCyThreadJob();

    return;

}


void MainClass::on_sig_StartedCyJob()
{
//   qDebug() << "on_sig_StartedCyJob";
   emit this->sig_StartAuthThreadJob();

    return;
}

void MainClass::on_sig_StartedAuthJob()
{
//    qDebug() << "on_sig_StartedAuthJob";
    emit this->sig_StartTableThreadJob();

     return;
}

void MainClass::on_sig_StartedTableJob()
{
//    qDebug() << "on_sig_StartedTableJob";
    emit this->sig_startJobThreadJob();

    return;
}

void MainClass::on_sig_StartedJobJob()
{
//    qDebug() << "on_sig_StartedJobJob";
    emit this->sig_startMqttThreadJob();

    return;
}

void MainClass::on_sig_StartedMqttJob()
{
//    qDebug() << "on_sig_StartedMqttJob";

//    while(!(maincalss_mqtt_worker->bSubInpResp && maincalss_mqtt_worker->bSubLogResp && maincalss_mqtt_worker->bSubApbResp &&
//            maincalss_mqtt_worker->bSubAccessShiftsTableSyncResp && maincalss_mqtt_worker->bSubAccessTimezonesTableSyncResp && maincalss_mqtt_worker->bSubStartStopSyncResp && maincalss_mqtt_worker->bSubtagsTableSyncResp &&
//            maincalss_mqtt_worker->bSubtag_AccessPairsTableSyncResp && maincalss_mqtt_worker->bSubtag_HolidaysTableSyncResp && maincalss_mqtt_worker->bSubtag_TerminalsTableSyncResp && maincalss_mqtt_worker->bSubtag_UsersTableSyncResp &&
//            maincalss_mqtt_worker->bSubTimezonesTableSyncResp)){
//        QApplication::processEvents(QEventLoop::AllEvents);
//    }

//    qInfo() << "";
//    qInfo() << "";
//    qInfo() << MAIN << "************************                                                                                                *************************";
//    qInfo() << MAIN << "*************************************************************************************************************************************************";
//    qInfo().noquote() << MAIN << "************************************************************ START  Full Application ************************************************************";
//    qInfo() << MAIN << "*************************************************************************************************************************************************";
//    qInfo() << MAIN << "************************                                                                                                *************************";


    return;
}

void MainClass::on_ntpTimerTimeout()
{
    ntp_Timer->stop();
    if(!ntp){
        ntp = mainclass_i2c->SetDateTimeToSysWithNtpdateTerminal(true, gl_Ini_NtpUrl);
    }
    rtc_Timer->start();
}

void MainClass::on_rtcTimerTimeout()
{
    rtc_Timer->stop();
    if(ntp){
        mainclass_i2c->SetDateTimeToRtcWithI2c(true);
        //ntp = false;
    }
    ntp_Timer->start();
}

void MainClass::on_watchTimerTimeout()
{
    watch_Timer->stop();
    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";
    #endif
    QProcess *process = new QProcess(this);
    QString proc = QString("sudo %1").arg(gl_Prog_WatchFileName);
    #ifdef QT_DEBUG
        qInfo() << MAINCLASS << "on_watchTimerTimeout--> " << "cmd string = " << proc;
    #endif

    process->start(proc);
    process->waitForFinished(-1);

    QStringList listout;
    QStringList listerr;

    QString err(process->readAllStandardError());
    if(err != ""){
        listerr.clear();
        listerr = err.split("\n");
        for(int i=0; i<listerr.count(); i++)
            if(listerr.at(i) != "")
                qWarning() << MAINCLASS << "on_watchTimerTimeout--> " << "stderr -> "<< listerr.at(i);
    }

    QString output(process->readAllStandardOutput());
    if(output != ""){
        listout.clear();
        QStringList listout = output.split("\n");
        for(int i=0; i<listout.count(); i++)
            if(listout.at(i) != ""){
                #ifdef QT_DEBUG
                    qInfo() << MAINCLASS << "on_watchTimerTimeout--> " << "stdout -> " << listout.at(i);
                #endif
            }
    }
    watch_Timer->start();

    process->deleteLater();
    return;
}

void MainClass::CheckHardware()
{
    bool ret1 = false;
    bool ret2 = false;

    QStringList listout;
    QStringList listerr;
    QString err;
    QString proc;

    QProcess *process = new QProcess(this);

    proc = QString("ls /dev/ttyS1");
    process->start(proc);
    process->waitForFinished(-1);

    err = process->readAllStandardError();

    if(err != ""){
        ret1 = true;
        listerr.clear();
        listerr = err.split("\n");
        for(int i=0; i<listerr.count(); i++)
            if(listerr.at(i) != "")
                qWarning() << MAINCLASS << "CheckHardware ttyS1--> " << "stderr -> "<< listerr.at(i);
    }

    proc = QString("ls /dev/i2c-0");
    process->start(proc);
    process->waitForFinished(-1);

    err = process->readAllStandardError();
    if(err != ""){
    ret2 = true;
    listerr.clear();
    listerr = err.split("\n");
    for(int i=0; i<listerr.count(); i++)
        if(listerr.at(i) != "")
            qWarning() << MAINCLASS << "CheckHardware i2c-0--> " << "stderr -> "<< listerr.at(i);
    }

    if(ret1 || ret2){
        qWarning() << MAINCLASS << "CheckHardware --> " << "reboot";
        proc = QString("sudo reboot");
        process->start(proc);
        process->waitForFinished(-1);
    }

    process->deleteLater();
    return;
}

void MainClass::Reboot()
{
    qWarning() << MAINCLASS << "Reboot --> " << "reboot";
    QStringList listout;
    QStringList listerr;
    QString err;
    QString proc;

    QProcess *process = new QProcess(this);

    proc = QString("sudo reboot");
    process->start(proc);
    process->waitForFinished(-1);

    err = process->readAllStandardError();

    if(err != ""){
        listerr.clear();
        listerr = err.split("\n");
        for(int i=0; i<listerr.count(); i++)
            if(listerr.at(i) != "")
                qWarning() << MAINCLASS << "Reboot --> " << "stderr -> "<< listerr.at(i);
    }

    process->deleteLater();
    return;
}

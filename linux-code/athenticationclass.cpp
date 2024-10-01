#include "athenticationclass.h"

AthenticationClass::AthenticationClass(QObject *parent) : QObject(parent)
{
    Init();

    return;
}

void AthenticationClass::Init()
{
    _myQuery = nullptr;
    queue = nullptr;
    dequeueTimer = nullptr;
    pendigList = nullptr;
    pedningListTimer = nullptr;

    mutex = nullptr;

    bAuthTreadRun = false;
    bAuthThreadJobRun = false;

    MAC = "";

    _and_Proc_Timer = nullptr;

    delay_Timer = nullptr;
    delay = false;
    return;
}

void AthenticationClass::Config()
{
    _myQuery = new Query(this);

    MAC = gl_Eth_MacAddr;
    MAC = MAC.remove(":");

    _myQuery->Init();
    _myQuery->Config();
    QString connectionName = QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    if(!_myQuery->Query_OpenDb(connectionName)){
        qWarning() << AUTHCLASS << "Db_open--> " << "can not open database";
        QApplication::quit();
    }

    queue = new QQueue<AuthRes>;

    pendigList = new QList<AuthRes>;

    dequeueTimer = new QTimer(this);
    dequeueTimer->setSingleShot(true);
    dequeueTimer->setInterval(AUTH_DEQ_TIMEOUT_MS);
    dequeueTimer->stop();

    pedningListTimer = new QTimer(this);
    pedningListTimer->setSingleShot(true);
    pedningListTimer->setInterval(AUTH_PEND_TIMEOUT_MS);
    pedningListTimer->stop();

    delay_Timer = new QTimer(this);
    delay_Timer->setSingleShot(true);
    delay_Timer->stop();
    QObject::connect(delay_Timer, &QTimer::timeout, this, &AthenticationClass::on_Delay_Timeout, Qt::QueuedConnection);


    _and_Proc_Timer = new QTimer(this);
    _and_Proc_Timer->setSingleShot(true);
    if(gl_ini_AndPort == 1){
        _and_Proc_Timer->setInterval(gl_Ini_Port1ProcTimeout);
    }
    else if(gl_ini_AndPort == 2){
        _and_Proc_Timer->setInterval(gl_Ini_Port2ProcTimeout);
    }
    else{
        _and_Proc_Timer->setInterval(1);
    }
    _and_Proc_Timer->stop();


    mutex = new QMutex;


    QObject::connect(dequeueTimer, &QTimer::timeout, this, &AthenticationClass::on_dequeueTimerTimeout_slot, Qt::QueuedConnection);
    QObject::connect(pedningListTimer, &QTimer::timeout, this, &AthenticationClass::on_pedningListTimerTimeout_slot, Qt::QueuedConnection);

    qRegisterMetaType<AuthRes>("AuthRes");
    QObject::connect(this, &AthenticationClass::sig_Auth, this, &AthenticationClass::on_Auth_slot, Qt::QueuedConnection);

    QObject::connect(_and_Proc_Timer, &QTimer::timeout, this, &AthenticationClass::on_and_Proc_Timer_Timeout, Qt::QueuedConnection);


    return;
}

void AthenticationClass::on_StartWorker()
{
    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";
        qInfo() << AUTHCLASS << "on_StartWorker--> " << "start auth thread class";
        qInfo() << AUTHCLASS << "on_StartWorker--> " << "authentication thread id = " << QThread::currentThreadId();
    #endif

    Init();
    Config();

    #ifdef QT_DEBUG
        qInfo() << AUTHCLASS << "on_StartWorker: emit this->StartedWorker();";
    #endif

    bAuthTreadRun = true;
    emit this->StartedWorker();

    return;
}

void AthenticationClass::on_StartAuthJob()
{
    if(bAuthTreadRun){
        dequeueTimer->start();
        pedningListTimer->start();
        bAuthThreadJobRun =  true;
        emit this->sig_StartedAuthJob();
    }

    return;
}

//bool AthenticationClass::testConnection()
//{
//    bool ret = false;

//    QString queryStringSelect = "";
//    queryResult queryResultResSelect;

//    queryStringSelect = "SELECT UUID();"; // "query/job_query/timezones_query/query_selectall_timezones.sql"
//    if(queryResultResSelect.query.isActive())
//        queryResultResSelect.query.finish();
//    queryResultResSelect = _myQuery->_myDb->Db_execquery(queryStringSelect);
//    if(queryResultResSelect.success)
//        if(queryResultResSelect.query.next())
//            ret = true;


//    return ret;
//}

void AthenticationClass::Auth_PrepareReq(int userUniqueId, int readerId, int readerExpId, AuthRes &res)
{
    res.userUniqueId = userUniqueId;
    res.readerId = readerId;
    res.readerExpId = readerExpId;
    res.reqDateTime = QDateTime::fromString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "yyyy-MM-dd hh:mm:ss")/*.toUTC()*/;

    qInfo() << AUTHCLASS << "Auth_PrepareReq--> userUniqueId: " << res.userUniqueId << " , readerId: " << res.readerId << " , readerExpId: " << res.readerExpId << ", reqDateTime: " << res.reqDateTime;

    return;
}

void AthenticationClass::on_sig_AuthReq_slot(int userUniqueId, int readerId, int readerExpId)
{
//    qInfo() << "";
//    qInfo() << "";

    AuthRes res;
    Auth_PrepareReq(userUniqueId, readerId, readerExpId, res);
    queue->enqueue(res);
    Delay_ms(50);
    return;
}

void AthenticationClass::on_dequeueTimerTimeout_slot()
{
    dequeueTimer->stop();
    if(!queue->isEmpty()){
        emit this->sig_Auth(queue->dequeue());
        Delay_ms(50);
    }
    dequeueTimer->start();
}

void AthenticationClass::on_pedningListTimerTimeout_slot()
{
    pedningListTimer->stop();
    mutex->lock();
    QMutableListIterator<AuthRes> i(*pendigList);
    while(i.hasNext()){
        AuthRes* res = &(i.next());
        //qDebug() << res->reqDateTime.msecsTo(QDateTime::currentDateTime());
        int ms = 0;
        if((res->readerId == 1) && (res->Server))
            ms = gl_Ini_Port1ProcTimeout;
        if((res->readerId == 2) && (res->Server))
            ms = gl_Ini_Port2ProcTimeout;

        if((res->reqDateTime.msecsTo(QDateTime::currentDateTime()) > ms) && (res->Server)){
            //finalize
            if(res->Auth && res->err == ERR_OK){
//                #ifdef QT_DEBUG
                    //qInfo() << AUTHCLASS << "on_pedningListTimerTimeout_slot--> " << "timeout for anti pass back";
//                #endif
                res->Anti = false;
                res->err = ERR_SERVER_TIMEOUT;
                res->errString = EER_STR_SERVER_TIMEOUT;
                res->isSuccess = true;
            }
            else{
//                #ifdef QT_DEBUG
                    //qInfo() << AUTHCLASS << "on_pedningListTimerTimeout_slot--> " << "timeout for auth or apb";
//                #endif
            }
            Auth_Finalize(*res);
            i.remove();
        }
        else if(!res->Server){
            res->Server = true;
            if(res->err == ERR_NO_USER_FOUND || res->err == ERR_QUERY_USER_FIND){
                //sig to mqtt auth req
//                #ifdef QT_DEBUG
                    //qInfo() << AUTHCLASS << "on_pedningListTimerTimeout_slot--> " << "send auth req to server";
//                #endif
                Auth auth;
                auth.userUniqueId = res->userUniqueId;
                auth.readerExpId = res->readerExpId;
                auth.readerId = res->readerId;
                auth.reqDateTime = res->reqDateTime;
                if(bMqttThreadJobRun)
                    emit this->sig_SendToServerAuth(auth);
            }
            if(res->err == ERR_OK){
//                #ifdef QT_DEBUG
                    //qInfo() << AUTHCLASS << "on_pedningListTimerTimeout_slot--> " << "send apb req to server";
//                #endif
                Apb apb;
                apb.userUniqueId = res->userUniqueId;
                apb.readerExpId = res->readerExpId;
                apb.readerId = res->readerId;
                apb.reqDateTime = res->reqDateTime;
                if(bMqttThreadJobRun)
                    emit this->sig_SendToServerApb(apb);
                //sig to mqtt apb req
            }
        }
        QApplication::processEvents(QEventLoop::AllEvents);
    }
    mutex->unlock();
    pedningListTimer->start();
    return;
}

void AthenticationClass::on_Auth_slot(AuthRes res)
{
//    qInfo() << "";
//    qInfo() << "";


    if(!Auth_CheckReader(res)){
        #ifdef QT_DEBUG
            qInfo() << AUTHCLASS << "on_Auth_slot--> " << "Auth_CheckReader failed";
        #endif
        return;
    }

    if(!Auth_CheckUser(res)){
        #ifdef QT_DEBUG
            qInfo() << AUTHCLASS << "on_Auth_slot--> " << "Auth_CheckUser failed";
        #endif
        return;
    }

    if(!Auth_CheckHol(res)){
        #ifdef QT_DEBUG
            qInfo() << AUTHCLASS << "on_Auth_slot--> " << "Auth_CheckHol failed";
        #endif
        return;
    }

    if(!Auth_CheckAccessPair(res)){
        #ifdef QT_DEBUG
            qInfo() << AUTHCLASS << "on_Auth_slot--> " << "Auth_CheckAccessPair failed";
        #endif
        return;
    }

    if(!Auth_CheckAccessTime(res)){
        #ifdef QT_DEBUG
            qInfo() << AUTHCLASS << "on_Auth_slot--> " << "Auth_CheckAccessTime failed";
        #endif
        return;
    }

    Auth_Res(res);
    //add to pending list

    return;
}

void AthenticationClass::Auth_Res(AuthRes res)
{
//    ERR_NO_USER_FOUND
    //return;

    switch (res.err) {
        case ERR_NO_USER_FOUND:{
            //add to pending list for auth in server;
//            #ifdef QT_DEBUG
                //qDebug() << "need auth ERR_NO_USER_FOUND";
//                qDebug() << "pendigList size befor = " << pendigList->size();
//            #endif
            pendigList->append(res);
//            #ifdef QT_DEBUG
//                qDebug() << "pendigList size after = " << pendigList->size();
//            #endif
            break;
        }
        case ERR_QUERY_USER_FIND:{
            //add to pending list for auth in server;
//            #ifdef QT_DEBUG
                //qDebug() << "need auth ERR_QUERY_USER_FIND";
//                qDebug() << "pendigList size befor = " << pendigList->size();
//            #endif
            pendigList->append(res);
//            #ifdef QT_DEBUG
//                qDebug() << "pendigList size after = " << pendigList->size();
//            #endif
            break;
        }
        case ERR_OK:{
            //add to pending list for antipassback in server;
//            #ifdef QT_DEBUG
//                qDebug() << "need anti pass back";
//                qDebug() << "pendigList size befor = " << pendigList->size();
//            #endif
            pendigList->append(res);
//            #ifdef QT_DEBUG
//                qDebug() << "pendigList size after = " << pendigList->size();
//            #endif
            break;
        }
        default:{
            Auth_Finalize(res);
            break;
        }
    }

    return;
}


void AthenticationClass::Auth_Finalize(AuthRes res)
{
    #ifdef QT_DEBUG
        if(res.isSuccess)
            qDebug() << "have auth and send to cypress";
        else
            qDebug() << "not have auth and send to cypress";
    #endif
    if(bCyThreadJobRun)
        emit this->sig_toCy(res.readerExpId, res.readerId, res.isSuccess);
    if(bTableThreadJobRun)
        emit this->sig_toLog(res);

    return;
}

bool AthenticationClass::Auth_CheckReader(AuthRes &res)
{
    bool ret = true;
    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckReader(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        Auth_Res(res);
        ret = false;
    }

    return ret;
}


bool AthenticationClass::Auth_CheckUser(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckUser(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        Auth_Res(res);
        ret = false;
    }

     return ret;
}

bool AthenticationClass::Auth_CheckHol(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckHol(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        Auth_Res(res);
        ret = false;
    }

    return ret;
}

bool AthenticationClass::Auth_CheckAccessPair(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckAccesspair(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        Auth_Res(res);
        ret = false;
    }

    return ret;
}

bool AthenticationClass::Auth_CheckAccessTime(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckAccesstime(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        Auth_Res(res);
        ret = false;
    }

    return ret;
}

void AthenticationClass::on_MqttResp_slot()
{

    return;
}

void AthenticationClass::on_sig_ServerToApbResp_slot(int userUniqueId, int readerExpId, int readerId, QDateTime reqDateTime, bool apb)
{
//    qDebug() << "userUniqueId = " << userUniqueId;
//    qDebug() << "readerExpId = " << readerExpId;
//    qDebug() << "readerId = " << readerId;
//    qDebug() << "reqDateTime = " << reqDateTime;
//    qDebug() << "apb = " << apb;

    mutex->lock();
    QMutableListIterator<AuthRes> i(*pendigList);
    while(i.hasNext()){
        AuthRes* res = &(i.next());
//        qDebug() << "res->Server = " << res->Server;
//        qDebug() << "res->userUniqueId = " << res->userUniqueId;
//        qDebug() << "res->readerExpId = " << res->readerExpId;
//        qDebug() << "res->readerId = " << res->readerId;
//        qDebug() << "res->reqDateTime = " << res->reqDateTime;
        if(res->Server && res->userUniqueId == userUniqueId && res->readerExpId == readerExpId && res->readerId == readerId && res->reqDateTime.toString("yyyy-MM-dd hh:mm:ss") == reqDateTime.toString("yyyy-MM-dd hh:mm:ss")){
            //finalize
            //qDebug() << "find in list";
            if(res->Auth && res->err == ERR_OK){
                if(apb){
                    res->Anti = true;
                    res->isSuccess = true;
                    res->err = ERR_OK;
                    res->errString = ERR_STR_OK;
                }
                else{
                    res->Anti = false;
                    res->isSuccess = false;
                    res->err = ERR_CHECK_ANTIPASSBACK;
                    res->errString = EER_STR_CHECK_ANTIPASSBACK;
                }
            }
            Auth_Finalize(*res);
            i.remove();
        }
        QApplication::processEvents(QEventLoop::AllEvents);
    }
    mutex->unlock();

    return;
}

void AthenticationClass::on_sig_ServerToAuthResp_slot(int userUniqueId, int readerExpId, int readerId, QDateTime reqDateTime, bool isHoliday, int errorCode, QString errorString, bool isSuccess)
{
//    qDebug() << "userUniqueId = " << userUniqueId;
//    qDebug() << "readerExpId = " << readerExpId;
//    qDebug() << "readerId = " << readerId;
//    qDebug() << "reqDateTime = " << reqDateTime;
//    qDebug() << "isHoliday = " << isHoliday;
//    qDebug() << "errorCode = " << errorCode;
//    qDebug() << "errorString = " << errorString;
//    qDebug() << "isSuccess = " << isSuccess;

        mutex->lock();
        QMutableListIterator<AuthRes> i(*pendigList);
        while(i.hasNext()){
            AuthRes* res = &(i.next());
//        qDebug() << "res->Server = " << res->Server;
//        qDebug() << "res->userUniqueId = " << res->userUniqueId;
//        qDebug() << "res->readerExpId = " << res->readerExpId;
//        qDebug() << "res->readerId = " << res->readerId;
//        qDebug() << "res->reqDateTime = " << res->reqDateTime;
            if(res->Server && res->userUniqueId == userUniqueId && res->readerExpId == readerExpId && res->readerId == readerId && res->reqDateTime.toString("yyyy-MM-dd hh:mm:ss") == reqDateTime.toString("yyyy-MM-dd hh:mm:ss") && (res->err == ERR_NO_USER_FOUND || res->err == ERR_QUERY_USER_FIND)){
                //finalize
                res->userUUID = ""; //unknown
                res->isSuccess = isSuccess;
                res->isHoliday = isHoliday;
                res->err = errorCode;
                res->errString = errorString;

                Auth_Finalize(*res);
                i.remove();
            }
            QApplication::processEvents(QEventLoop::AllEvents);
        }
//        qDebug() << "dont find";
        mutex->unlock();

        return;
}

void AthenticationClass::on_sig_AuthReqAnd_slot(int userUniqueId_1, int userUniqueId_2, int readerId_1, int readerId_2, int readerExpId, bool succ)
{
    if(!succ){
        AuthRes_And_Auth.AuthResPort1.userUniqueId = userUniqueId_1;
        AuthRes_And_Auth.AuthResPort2.userUniqueId = userUniqueId_2;

        AuthRes_And_Auth.AuthResPort1.readerId = readerId_1;
        AuthRes_And_Auth.AuthResPort2.readerId = readerId_2;

        AuthRes_And_Auth.AuthResPort1.readerExpId = readerExpId;
        AuthRes_And_Auth.AuthResPort2.readerExpId = readerExpId;


        QDateTime curr = QDateTime::fromString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "yyyy-MM-dd hh:mm:ss")/*.toUTC()*/;

        AuthRes_And_Auth.AuthResPort1.reqDateTime = curr;
        AuthRes_And_Auth.AuthResPort2.reqDateTime = curr;

        if(userUniqueId_1 == -1){
            AuthRes_And_Auth.AuthResPort1.err = ERR_AND_2USER_NOTFOUND;
            AuthRes_And_Auth.AuthResPort1.errString = ERR_STR_AND_2USER_NOTFOUND;
        }

        if(userUniqueId_2 == -1){
            AuthRes_And_Auth.AuthResPort2.err = ERR_AND_2USER_NOTFOUND;
            AuthRes_And_Auth.AuthResPort2.errString = ERR_STR_AND_2USER_NOTFOUND;
        }




        //send to log
        emit this->sig_toLogAnd(AuthRes_And_Auth);

        //send to cy
        emit this->sig_toCy(1, 1, false);
    }
    else{
        AuthRes_And_Auth.AuthResPort1.userUniqueId = userUniqueId_1;
        AuthRes_And_Auth.AuthResPort2.userUniqueId = userUniqueId_2;

        AuthRes_And_Auth.AuthResPort1.readerId = readerId_1;
        AuthRes_And_Auth.AuthResPort2.readerId = readerId_2;

        AuthRes_And_Auth.AuthResPort1.readerExpId = readerExpId;
        AuthRes_And_Auth.AuthResPort2.readerExpId = readerExpId;


        QDateTime curr = QDateTime::fromString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "yyyy-MM-dd hh:mm:ss")/*.toUTC()*/;

        AuthRes_And_Auth.AuthResPort1.reqDateTime = curr;
        AuthRes_And_Auth.AuthResPort2.reqDateTime = curr;


        qInfo() << AUTHCLASS << "Auth_PrepareReq--> << userUniqueId1: " << AuthRes_And_Auth.AuthResPort1.userUniqueId << " , userUniqueId2: " << AuthRes_And_Auth.AuthResPort1.userUniqueId << " >> , readerExpId: " << AuthRes_And_Auth.AuthResPort1.readerExpId << ", reqDateTime: " << curr;

        //run proc timer
        _and_Proc_Timer->start();


        if(Auth_CheckReader_And(AuthRes_And_Auth.AuthResPort1)){
            if(Auth_CheckReader_And(AuthRes_And_Auth.AuthResPort2)){
                if(Auth_CheckUser_And(AuthRes_And_Auth.AuthResPort1)){
                    if(Auth_CheckUser_And(AuthRes_And_Auth.AuthResPort2)){
                        if(Auth_CheckHol(AuthRes_And_Auth.AuthResPort1)){
                            if(Auth_CheckHol(AuthRes_And_Auth.AuthResPort2)){
                                if(Auth_CheckAccessPair(AuthRes_And_Auth.AuthResPort1)){
                                    if(Auth_CheckAccessPair(AuthRes_And_Auth.AuthResPort2)){
                                        if(Auth_CheckAccessTime(AuthRes_And_Auth.AuthResPort1)){
                                            if(Auth_CheckAccessTime(AuthRes_And_Auth.AuthResPort2)){
                                                //check anti pass back
                                                Apb_And apband;
                                                apband.readerId_1 = AuthRes_And_Auth.AuthResPort1.readerId;
                                                apband.readerId_2 = AuthRes_And_Auth.AuthResPort2.readerId;

                                                apband.userUniqueId_1 = AuthRes_And_Auth.AuthResPort1.userUniqueId;
                                                apband.userUniqueId_2 = AuthRes_And_Auth.AuthResPort2.userUniqueId;

                                                apband.readerExpId = AuthRes_And_Auth.AuthResPort1.readerExpId;
                                                apband.reqDateTime = AuthRes_And_Auth.AuthResPort1.reqDateTime;

                                                qInfo() << AUTHCLASS << "send apb and request to server";
                                                emit this->sig_SendToServerApb_And(apband);
                                            }
                                            else{
                                                //error access time user 2
                                                //terminate proc and log
                                                on_and_Proc_Timer_Timeout();
                                            }
                                        }
                                        else{
                                            //error access time user 1
                                            //terminate proc and log
                                            on_and_Proc_Timer_Timeout();
                                        }
                                    }
                                    else{
                                        //error access pair user 2
                                        //terminate proc and log
                                        on_and_Proc_Timer_Timeout();
                                    }
                                }
                                else{
                                    //error access pair user 1
                                    //terminate proc and log
                                    on_and_Proc_Timer_Timeout();
                                }
                            }
                            else{
                                //error hol user 2
                                //terminate proc and log
                                on_and_Proc_Timer_Timeout();
                            }
                        }
                        else{
                            //error hol user 1
                            //terminate proc and log
                            on_and_Proc_Timer_Timeout();
                        }
                    }
                    else{
                        //error check user 2
                        //send to server 2 packet for check user
                        Auth_And authand;
                        authand.readerId_1 = AuthRes_And_Auth.AuthResPort1.readerId;
                        authand.readerId_2 = AuthRes_And_Auth.AuthResPort2.readerId;

                        authand.userUniqueId_1 = AuthRes_And_Auth.AuthResPort1.userUniqueId;
                        authand.userUniqueId_2 = AuthRes_And_Auth.AuthResPort2.userUniqueId;

                        authand.readerExpId = AuthRes_And_Auth.AuthResPort1.readerExpId;
                        authand.reqDateTime = AuthRes_And_Auth.AuthResPort1.reqDateTime;

                        emit this->sig_SendToServerAuth_And(authand);
                    }
                }
                else{
                    //error check user 1
                    //send to server 2 packet for check user
                    Auth_And authand;
                    authand.readerId_1 = AuthRes_And_Auth.AuthResPort1.readerId;
                    authand.readerId_2 = AuthRes_And_Auth.AuthResPort2.readerId;

                    authand.userUniqueId_1 = AuthRes_And_Auth.AuthResPort1.userUniqueId;
                    authand.userUniqueId_2 = AuthRes_And_Auth.AuthResPort2.userUniqueId;

                    authand.readerExpId = AuthRes_And_Auth.AuthResPort1.readerExpId;
                    authand.reqDateTime = AuthRes_And_Auth.AuthResPort1.reqDateTime;

                    emit this->sig_SendToServerAuth_And(authand);
                }
            }
            else{
                //error check reader 2
                //terminate proc and log
                on_and_Proc_Timer_Timeout();
            }
        }
        else{
            //error check reader 1
            //terminate proc and log
            on_and_Proc_Timer_Timeout();
        }
    }

    return;
}

void AthenticationClass::on_sig_ServerToApbRespAnd_slot(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, int readerExpId, QDateTime reqDateTime, bool apb)
{

    if(_and_Proc_Timer->isActive()){
        if(
            userUniqueId1 == AuthRes_And_Auth.AuthResPort1.userUniqueId     &&
            userUniqueId2 == AuthRes_And_Auth.AuthResPort2.userUniqueId     &&
            readerId1 == AuthRes_And_Auth.AuthResPort1.readerId             &&
            readerId2 == AuthRes_And_Auth.AuthResPort2.readerId             &&
            readerExpId == AuthRes_And_Auth.AuthResPort1.readerExpId        &&
            readerExpId == AuthRes_And_Auth.AuthResPort2.readerExpId        &&
            reqDateTime == AuthRes_And_Auth.AuthResPort1.reqDateTime        &&
            reqDateTime == AuthRes_And_Auth.AuthResPort2.reqDateTime
          )
        {
            if(apb){
                AuthRes_And_Auth.AuthResPort1.Anti = true;
                AuthRes_And_Auth.AuthResPort2.Anti = true;

                AuthRes_And_Auth.AuthResPort1.isSuccess = true;
                AuthRes_And_Auth.AuthResPort2.isSuccess = true;

                AuthRes_And_Auth.AuthResPort1.err = ERR_OK;
                AuthRes_And_Auth.AuthResPort2.err = ERR_OK;

                AuthRes_And_Auth.AuthResPort1.errString = ERR_STR_OK;
                AuthRes_And_Auth.AuthResPort2.errString = ERR_STR_OK;

                //send to cy
                emit this->sig_toCy(1, 1, true);
                //send to log
                emit this->sig_toLogAnd(AuthRes_And_Auth);
            }
            else{

                AuthRes_And_Auth.AuthResPort1.Anti = false;
                AuthRes_And_Auth.AuthResPort2.Anti = false;

                AuthRes_And_Auth.AuthResPort1.isSuccess = false;
                AuthRes_And_Auth.AuthResPort2.isSuccess = false;

                AuthRes_And_Auth.AuthResPort1.err = ERR_CHECK_ANTIPASSBACK;
                AuthRes_And_Auth.AuthResPort2.err = ERR_CHECK_ANTIPASSBACK;

                AuthRes_And_Auth.AuthResPort1.errString = EER_STR_CHECK_ANTIPASSBACK;
                AuthRes_And_Auth.AuthResPort2.errString = EER_STR_CHECK_ANTIPASSBACK;


                //send to log
                emit this->sig_toLogAnd(AuthRes_And_Auth);

                //send to cy
                emit this->sig_toCy(1, 1, false);
            }

            _and_Proc_Timer->stop();
            init_AuthRes_And_Auth();
        }
    }


    return;
}

void AthenticationClass::on_sig_ServerToAuthRespAnd_slot(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, bool isHoliday1, bool isHoliday2, int errorCode1, int errorCode2, QString errorString1, QString errorString2, int readerExpId, QDateTime reqDateTime, bool isSuccess)
{
//    qDebug() << "on_sig_ServerToAuthRespAnd_slot";
//    qDebug() << reqDateTime;
    if(_and_Proc_Timer->isActive()){
        if(
            userUniqueId1 == AuthRes_And_Auth.AuthResPort1.userUniqueId     &&
            userUniqueId2 == AuthRes_And_Auth.AuthResPort2.userUniqueId     &&
            readerId1 == AuthRes_And_Auth.AuthResPort1.readerId             &&
            readerId2 == AuthRes_And_Auth.AuthResPort2.readerId             &&
            readerExpId == AuthRes_And_Auth.AuthResPort1.readerExpId        &&
            readerExpId == AuthRes_And_Auth.AuthResPort2.readerExpId        &&
            reqDateTime == AuthRes_And_Auth.AuthResPort1.reqDateTime        &&
            reqDateTime == AuthRes_And_Auth.AuthResPort2.reqDateTime
          )
        {
            if(isSuccess){
                AuthRes_And_Auth.AuthResPort1.Anti = true;
                AuthRes_And_Auth.AuthResPort2.Anti = true;

                AuthRes_And_Auth.AuthResPort1.isSuccess = true;
                AuthRes_And_Auth.AuthResPort2.isSuccess = true;

                AuthRes_And_Auth.AuthResPort1.err = errorCode1;
                AuthRes_And_Auth.AuthResPort2.err = errorCode2;

                AuthRes_And_Auth.AuthResPort1.errString = errorString1;
                AuthRes_And_Auth.AuthResPort2.errString = errorString2;

                AuthRes_And_Auth.AuthResPort1.isHoliday = isHoliday1;
                AuthRes_And_Auth.AuthResPort2.isHoliday = isHoliday2;


                //send to cy
                emit this->sig_toCy(1, 1, true);
                //send to log
                emit this->sig_toLogAnd(AuthRes_And_Auth);
            }
            else{

                AuthRes_And_Auth.AuthResPort1.Anti = false;
                AuthRes_And_Auth.AuthResPort2.Anti = false;

                AuthRes_And_Auth.AuthResPort1.isSuccess = false;
                AuthRes_And_Auth.AuthResPort2.isSuccess = false;

                AuthRes_And_Auth.AuthResPort1.err = errorCode1;
                AuthRes_And_Auth.AuthResPort2.err = errorCode2;

                AuthRes_And_Auth.AuthResPort1.errString = errorString1;
                AuthRes_And_Auth.AuthResPort2.errString = errorString2;

                AuthRes_And_Auth.AuthResPort1.isHoliday = isHoliday1;
                AuthRes_And_Auth.AuthResPort2.isHoliday = isHoliday2;

                //send to log
                emit this->sig_toLogAnd(AuthRes_And_Auth);

                //send to cy
                emit this->sig_toCy(1, 1, false);
            }
            _and_Proc_Timer->stop();
            init_AuthRes_And_Auth();
        }
    }

    return;
}


bool AthenticationClass::Auth_CheckReader_And(AuthRes &res)
{
    bool ret = true;
    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckReader(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK)
        ret = false;

    return ret;
}

bool AthenticationClass::Auth_CheckUser_And(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckUser(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK)
        ret = false;


     return ret;
}

bool AthenticationClass::Auth_CheckHol_And(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckHol(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        ret = false;
    }

    return ret;
}

bool AthenticationClass::Auth_CheckAccessPair_And(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckAccesspair(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        ret = false;
    }

    return ret;
}

bool AthenticationClass::Auth_CheckAccessTime_And(AuthRes &res)
{
    bool ret = true;

    //QString connectionName = MAC + QMYSL_AUTH + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    //_myQuery->Query_OpenDb(connectionName);
    _myQuery->Query_CheckAccesstime(res);
    //_myQuery->Query_CloseDb();
    if(res.err != ERR_OK){
        ret = false;
    }

    return ret;
}

void AthenticationClass::on_and_Proc_Timer_Timeout()
{
    _and_Proc_Timer->stop();

    //check timeout for anti passback
    if(AuthRes_And_Auth.AuthResPort1.Auth && AuthRes_And_Auth.AuthResPort2.Auth){

        qDebug() << "APB AND Req Timeout";
        AuthRes_And_Auth.AuthResPort1.isSuccess = true;
        AuthRes_And_Auth.AuthResPort2.isSuccess = true;

        AuthRes_And_Auth.AuthResPort1.Anti = false;
        AuthRes_And_Auth.AuthResPort2.Anti = false;

        AuthRes_And_Auth.AuthResPort1.err = ERR_SERVER_TIMEOUT;
        AuthRes_And_Auth.AuthResPort2.err = ERR_SERVER_TIMEOUT;

        AuthRes_And_Auth.AuthResPort1.errString = EER_STR_SERVER_TIMEOUT;
        AuthRes_And_Auth.AuthResPort2.errString = EER_STR_SERVER_TIMEOUT;

        //send to log
        emit this->sig_toLogAnd(AuthRes_And_Auth);


        //send to cy
        emit this->sig_toCy(AuthRes_And_Auth.AuthResPort1.readerExpId, AuthRes_And_Auth.AuthResPort1.readerId, true);
    }
    else{
        //send to log
        emit this->sig_toLogAnd(AuthRes_And_Auth);


        //send to cy
        emit this->sig_toCy(AuthRes_And_Auth.AuthResPort1.readerExpId, AuthRes_And_Auth.AuthResPort1.readerId, false);
    }

    init_AuthRes_And_Auth();




    return;
}

void AthenticationClass::init_AuthRes_And_Auth()
{
    AuthRes_And_Auth.AuthResPort1.err = -2;
    AuthRes_And_Auth.AuthResPort1.errString = "";
    AuthRes_And_Auth.AuthResPort1.userRowId = -1;
    AuthRes_And_Auth.AuthResPort1.userUUID = "";
    AuthRes_And_Auth.AuthResPort1.userUniqueId = -1;
    AuthRes_And_Auth.AuthResPort1.isUser = false;
    AuthRes_And_Auth.AuthResPort1.userTableType = -1;
    AuthRes_And_Auth.AuthResPort1.userActivation = false;
    AuthRes_And_Auth.AuthResPort1.readerUUID = "";
    AuthRes_And_Auth.AuthResPort1.readerRowId = -1;
    AuthRes_And_Auth.AuthResPort1.readerId = 1;
    AuthRes_And_Auth.AuthResPort1.readerExpId = 1;
    AuthRes_And_Auth.AuthResPort1.readerTableType = -1;
    AuthRes_And_Auth.AuthResPort1.readerActivation = false;
    AuthRes_And_Auth.AuthResPort1.readerDeactivationByDateTime = false;
    AuthRes_And_Auth.AuthResPort1.isHoliday = false;
    AuthRes_And_Auth.AuthResPort1.holidayTableType = -1;
    AuthRes_And_Auth.AuthResPort1.holidayCode = "";
    AuthRes_And_Auth.AuthResPort1.userDayOfWeek = -1;
    AuthRes_And_Auth.AuthResPort1.userDayOfWeekStr = "";
    AuthRes_And_Auth.AuthResPort1.Auth = false;
    AuthRes_And_Auth.AuthResPort1.Anti = false;
    AuthRes_And_Auth.AuthResPort1.Server = false;
    AuthRes_And_Auth.AuthResPort1.isSuccess = false;
    AuthRes_And_Auth.AuthResPort1.userDeactivationByDateTime = false;

    AuthRes_And_Auth.AuthResPort2.err = -2;
    AuthRes_And_Auth.AuthResPort2.errString = "";
    AuthRes_And_Auth.AuthResPort2.userRowId = -1;
    AuthRes_And_Auth.AuthResPort2.userUUID = "";
    AuthRes_And_Auth.AuthResPort2.userUniqueId = -1;
    AuthRes_And_Auth.AuthResPort2.isUser = false;
    AuthRes_And_Auth.AuthResPort2.userTableType = -1;
    AuthRes_And_Auth.AuthResPort2.userActivation = false;
    AuthRes_And_Auth.AuthResPort2.readerUUID = "";
    AuthRes_And_Auth.AuthResPort2.readerRowId = -1;
    AuthRes_And_Auth.AuthResPort2.readerId = 2;
    AuthRes_And_Auth.AuthResPort2.readerExpId = 1;
    AuthRes_And_Auth.AuthResPort2.readerTableType = -1;
    AuthRes_And_Auth.AuthResPort2.readerActivation = false;
    AuthRes_And_Auth.AuthResPort2.readerDeactivationByDateTime = false;
    AuthRes_And_Auth.AuthResPort2.isHoliday = false;
    AuthRes_And_Auth.AuthResPort2.holidayTableType = -1;
    AuthRes_And_Auth.AuthResPort2.holidayCode = "";
    AuthRes_And_Auth.AuthResPort2.userDayOfWeek = -1;
    AuthRes_And_Auth.AuthResPort2.userDayOfWeekStr = "";
    AuthRes_And_Auth.AuthResPort2.Auth = false;
    AuthRes_And_Auth.AuthResPort2.Anti = false;
    AuthRes_And_Auth.AuthResPort2.Server = false;
    AuthRes_And_Auth.AuthResPort2.isSuccess = false;
    AuthRes_And_Auth.AuthResPort2.userDeactivationByDateTime = false;
}

void AthenticationClass::on_Delay_Timeout()
{
    delay_Timer->stop();
    delay = false;

    return;
}

void AthenticationClass::Delay_ms(quint32 ms)
{
    delay_Timer->start(ms);
    delay = true;
    while(delay){
        QApplication::processEvents(QEventLoop::AllEvents);
    }

    return;
}

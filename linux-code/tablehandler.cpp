#include "tablehandler.h"

TableHandler::TableHandler(QObject *parent) : QObject(parent)
{

}

void TableHandler::Init()
{
    _myDb = nullptr;
    dbConnection = false;
    logTimer = nullptr;
    deleteTimer = nullptr;

    bTableThreadRun = false;
    bTableThreadJobRun = false;

    MAC = "";

    return;
}

void TableHandler::Config()
{
    _myDb = new Database(this);

    MAC = gl_Eth_MacAddr;
    MAC = MAC.remove(":");

    logTimer = new QTimer(this);
    logTimer->setSingleShot(true);
    logTimer->setInterval(TABLE_LOG_TIMEOUT_MS);
    QObject::connect(logTimer, &QTimer::timeout, this, &TableHandler::on_logTimerTimeout, Qt::QueuedConnection);


    deleteTimer = new QTimer(this);
    deleteTimer->setSingleShot(true);
    deleteTimer->setInterval(TABLE_LOG_DELETE_TIMEOUT_SEC * 1000);
    QObject::connect(deleteTimer, &QTimer::timeout, this, &TableHandler::on_deleteTimerTimeout, Qt::QueuedConnection);
    if(!TableHandler_OpenDb()){
        qWarning() << TABLE << "Db_open--> " << "can not open database";
        QApplication::quit();
    }



    return;
}

void TableHandler::on_StartWorker()
{
    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";
        qInfo() << TABLE << "on_StartWorker--> " << "start table handler thread class";
        qInfo() << TABLE << "on_StartWorker--> " << "table handle thread id = " << QThread::currentThreadId();
    #endif

    Init();
    Config();

    #ifdef QT_DEBUG
        qInfo() << TABLE << "on_StartWorker: emit this->StartedWorker();";
    #endif

    bTableThreadRun = true;
    emit this->StartedWorker();

    return;
}

void TableHandler::on_StartTableJob()
{
    if(bTableThreadRun){
        logTimer->start();
        deleteTimer->start();

        bTableThreadJobRun = true;
        emit this->sig_StartedTableJob();

    }

    return;
}

bool TableHandler::TableHandler_OpenDb()
{
    QString connectionName = MAC + QMYSL_TABLE + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
    bool ret = _myDb->Db_open(connectionName);
    dbConnection = ret;

    return ret;
}

bool TableHandler::TableHandler_CloseDb()
{
    bool ret = false;
    if(dbConnection)
        ret = _myDb->Db_close();
    dbConnection = !ret;

    return ret;
}

void TableHandler::on_sig_toLog_slot(AuthRes res)
{
    Insert__logsTable(res.userUUID, res.userUniqueId, res.readerUUID, res.readerId, res.readerExpId, res.reqDateTime, res.isHoliday, res.err, res.errString, res.isUser, res.isSuccess);

    #ifdef QT_DEBUG
        qDebug() << "insert log";
    #endif

    return;
}

void TableHandler::on_sig_InpReq_slot(int readerExpId, int readerId)
{
    Insert__inpsTable(readerExpId, readerId);

    #ifdef QT_DEBUG
        qDebug() << "insert inp";
    #endif

    return;
}

bool TableHandler::Insert__logsTable(QString userUUID, int userUniqueId, QString readerUUID, int readerId, int readerExpId, QDateTime reqDateTime, bool isHoliday, int errorCode, QString errorString, bool isUser, bool isSuccess)
{
    bool ret = false;

    //TableHandler_OpenDb();

    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    if(reqDateTime.toString("yyyy-MM-dd hh:mm:ss") == ""){
        reqDateTime = QDateTime::fromString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "yyyy-MM-dd hh:mm:ss");
    }

    int ishol = 0;
    if(isHoliday)
        ishol = 1;

    int isuser = 0;
    if(isUser)
        isuser = 1;

    int issuc = 0;
    if(isSuccess)
        issuc = 1;

    queryString = _myDb->Db_loadquery(gl_prog_QueryInsertLog); // "query/table_query/query_insert_log.sql"
    queryString = queryString.arg(gl_Ini_DbName)
                             .arg(userUUID)
                             .arg(QString::number(userUniqueId))
                             .arg(readerUUID)
                             .arg(QString::number(readerId))
                             .arg(QString::number(readerExpId))
                             .arg(reqDateTime.toString("yyyy-MM-dd hh:mm:ss"))
                             .arg(QString::number(ishol))
                             .arg(QString::number(errorCode))
                             .arg(errorString)
                             .arg(QString::number(isuser))
                             .arg(QString::number(issuc));

    queryResultRes = _myDb->Db_execquery(queryString);
    if(queryResultRes.success){
        ret = true;
    }
    else{
        qWarning() << TABLE << "Insert__logsTable--> Error: " << queryResultRes.query.lastError().text();
    }


    //TableHandler_CloseDb();

    return ret;
}


bool TableHandler::Insert__inpsTable(int readerExpId, int readerId)
{
    bool ret = false;

    //TableHandler_OpenDb();

    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    queryString = _myDb->Db_loadquery(gl_prog_QueryInsertInp); // "query/table_query/query_insert_inp.sql"
    queryString = queryString.arg(gl_Ini_DbName)
                             .arg(QString::number(readerExpId))
                             .arg(QString::number(readerId));

    queryResultRes = _myDb->Db_execquery(queryString);
    if(queryResultRes.success){
        ret = true;
    }
    else{
        qWarning() << TABLE << "Insert__inpsTable--> Error: " << queryResultRes.query.lastError().text();
    }

    //TableHandler_CloseDb();

    return ret;
}

bool TableHandler::Find_NotSentLog(Log &log)
{
    bool ret = false;

    //TableHandler_OpenDb();

    //variable definition
    QString queryString = "";
    queryResult queryResultRes;


    queryString = _myDb->Db_loadquery(gl_prog_QueryFindLog); // "query/table_query/query_find_not_sent_log.sql"
    queryString = queryString.arg(gl_Ini_DbName);


    queryResultRes = _myDb->Db_execquery(queryString);
    if(queryResultRes.success){
        if(queryResultRes.query.next()){
//            qDebug() << "find new log";

            int id = queryResultRes.query.value("id").toInt();
            log.userUniqueId = queryResultRes.query.value("userUniqueId").toInt();
            log.readerExpId = queryResultRes.query.value("readerExpId").toInt();
            log.readerId = queryResultRes.query.value("readerId").toInt();
            log.reqDateTime = queryResultRes.query.value("reqDateTime").toDateTime();
            log.isHoliday = queryResultRes.query.value("isHoliday").toBool();
            log.isUser = queryResultRes.query.value("isUser").toBool();
            log.errorCode = queryResultRes.query.value("errorCode").toInt();
            log.errorString = queryResultRes.query.value("errorString").toString();
            log.UUID = queryResultRes.query.value("UUID").toString();
            log.isSuccess = queryResultRes.query.value("isSuccess").toBool();

            QString queryStringUpdated = "";
            queryStringUpdated = _myDb->Db_loadquery(gl_prog_QueryUpdatedAtLogs); // "query/table_query/query_log_updatedAt.sql"
            queryStringUpdated = queryStringUpdated.arg(gl_Ini_DbName).arg(QString::number(id));
            queryResult queryResultResUpdatedAt;

            queryResultResUpdatedAt = _myDb->Db_execquery(queryStringUpdated);
            if(queryResultRes.success)
                ret = true;
            else
                qWarning() << TABLE << "Find_NotSentLog--> Error2: " <<  queryResultRes.query.lastError().text();
        }
    }
    else{
        qWarning() << TABLE << "Find_NotSentLog--> Error1: " <<  queryResultRes.query.lastError().text();
    }

    //TableHandler_CloseDb();

    return ret;
}

bool TableHandler::Find_NotSentInp(Inp &inp)
{
    bool ret = false;

    //TableHandler_OpenDb();

    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    queryString = _myDb->Db_loadquery(gl_prog_QueryFindInp); // "query/table_query/query_find_not_sent_inp.sql"
    queryString = queryString.arg(gl_Ini_DbName);

    queryResultRes = _myDb->Db_execquery(queryString);
    if(queryResultRes.success){
        if(queryResultRes.query.next()){
            //qDebug() << "find new inp";
            int id = queryResultRes.query.value("id").toInt();
            inp.readerExpId = queryResultRes.query.value("readerExpId").toInt();
            inp.readerId = queryResultRes.query.value("readerId").toInt();
            inp.reqDateTime = queryResultRes.query.value("reqDateTime").toDateTime();
            inp.UUID = queryResultRes.query.value("UUID").toString();

            QString queryStringUpdated = "";
            queryStringUpdated = _myDb->Db_loadquery(gl_prog_QueryUpdatedAtInps); // "query/table_query/query_inp_updatedAt.sql"
            queryStringUpdated = queryStringUpdated.arg(gl_Ini_DbName).arg(QString::number(id));
            queryResult queryResultResUpdatedAt;

            queryResultResUpdatedAt = _myDb->Db_execquery(queryStringUpdated);
            if(queryResultRes.success)
                ret = true;
            else
                qWarning() << TABLE << "Find_NotSentInp--> Error2: " <<  queryResultRes.query.lastError().text();
        }
    }
    else{
        qWarning() << TABLE << "Find_NotSentInp--> Error1: " <<  queryResultRes.query.lastError().text();
    }

    //TableHandler_CloseDb();

    return ret;
}

void TableHandler::on_logTimerTimeout()
{
    logTimer->stop();
    Log log;
    Inp inp;
    LogAnd logand;


    if(Mqtt_Status){
        if(Find_NotSentLog(log)){
            //send not sent log to server
            emit this->sig_SendToServerLog(log);
        }


        //-------------------------
        //-------------------------
        //-------------------------
        if(Find_NotSentLogAnd(logand)){
            //send not sent log to server
            emit this->sig_SendToServerLogAnd(logand);
        }
        //-------------------------
        //-------------------------
        //-------------------------


        if(Find_NotSentInp(inp)){
            //send not sent log to server
            emit this->sig_SendToServerInp(inp);
        }
    }

    logTimer->start();

    return;
}

void TableHandler::on_deleteTimerTimeout()
{
    deleteTimer->stop();

    //TableHandler_OpenDb();

    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteLogs); // "query/table_query/query_delete_logs.sql"
    queryString = queryString.arg(gl_Ini_DbName);
    queryResultRes = _myDb->Db_execquery(queryString);
    if(!queryResultRes.success){
        qWarning() << TABLE << "on_deleteTimerTimeout--> Error Log: " <<  queryResultRes.query.lastError().text();
    }

    queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteInps); // "query/table_query/query_delete_inps.sql"
    queryString = queryString.arg(gl_Ini_DbName);
    queryResultRes = _myDb->Db_execquery(queryString);
    if(!queryResultRes.success){
        qWarning() << TABLE << "on_deleteTimerTimeout--> Error Inp: " <<  queryResultRes.query.lastError().text();
    }


    //-------------------------
    //-------------------------
    //-------------------------
    queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteLogsAnd); // "query/table_query/query_delete_logsand.sql"
    queryString = queryString.arg(gl_Ini_DbName);
    queryResultRes = _myDb->Db_execquery(queryString);
    if(!queryResultRes.success){
        qWarning() << TABLE << "on_deleteTimerTimeout--> Error LogAnd: " <<  queryResultRes.query.lastError().text();
    }
    //-------------------------
    //-------------------------
    //-------------------------

    //TableHandler_CloseDb();
    deleteTimer->start();
    
    return;
}

void TableHandler::on_sig_ServerToLogResp_slot(QString UUID)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //TableHandler_OpenDb();

    queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateLog); // "query/table_query/query_update_log.sql"
    queryString = queryString.arg(gl_Ini_DbName)
                             .arg(UUID);

//    qDebug() << "on_sig_ServerToLogResp_slot --> " <<  UUID << queryString;


    queryResultRes = _myDb->Db_execquery(queryString);
    if(!queryResultRes.success){
        qWarning() << TABLE << "on_sig_ServerToLogResp_slot--> Error: " <<  queryResultRes.query.lastError().text();
    }

    //TableHandler_CloseDb();

    return;
}



void TableHandler::on_sig_ServerToInpResp_slot(QString UUID)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //TableHandler_OpenDb();

    queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateInp); // "query/table_query/query_update_inp.sql"
    queryString = queryString.arg(gl_Ini_DbName)
                             .arg(UUID);

    queryResultRes = _myDb->Db_execquery(queryString);
    if(!queryResultRes.success){
        qWarning() << TABLE << "on_sig_ServerToInpResp_slot--> Error: " <<  queryResultRes.query.lastError().text();
    }

    //TableHandler_CloseDb();

    return;
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
void TableHandler::on_sig_toLogAnd_slot(AuthRes_And res)
{
    Insert__logsTableAnd(res.AuthResPort1.userUUID, res.AuthResPort2.userUUID,
                         res.AuthResPort1.userUniqueId, res.AuthResPort2.userUniqueId,
                         res.AuthResPort1.readerId, res.AuthResPort2.readerId,
                         res.AuthResPort1.readerUUID, res.AuthResPort2.readerUUID,
                         res.AuthResPort1.isHoliday, res.AuthResPort2.isHoliday,
                         res.AuthResPort1.err, res.AuthResPort2.err,
                         res.AuthResPort1.errString, res.AuthResPort2.errString,
                         res.AuthResPort1.isUser, res.AuthResPort2.isUser,
                         res.AuthResPort1.readerExpId, res.AuthResPort1.reqDateTime, res.AuthResPort1.isSuccess);

    #ifdef QT_DEBUG
        qDebug() << "insert logAnd";
    #endif

    return;
}

bool TableHandler::Insert__logsTableAnd(QString userUUID1, QString userUUID2, int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, QString readerUUID1, QString readerUUID2, bool isHoliday1, bool isHoliday2, int errorCode1, int errorCode2, QString errorString1, QString errorString2, bool isUser1, bool isUser2, int readerExpId, QDateTime reqDateTime, bool isSuccess)
{
    bool ret = false;

    //TableHandler_OpenDb();

    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    if(reqDateTime.toString("yyyy-MM-dd hh:mm:ss") == ""){
        reqDateTime = QDateTime::fromString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "yyyy-MM-dd hh:mm:ss");
    }

    queryString = _myDb->Db_loadquery(gl_prog_QueryInsertLogAnd); // "query/table_query/query_insert_logand.sql"
    queryString = queryString.arg(gl_Ini_DbName)
                             .arg(QString::number(userUniqueId1))
                             .arg(QString::number(userUniqueId2))
                             .arg(userUUID1)
                             .arg(userUUID2)
                             .arg(QString::number(readerId1))
                             .arg(QString::number(readerId2))
                             .arg(readerUUID1)
                             .arg(readerUUID2)
                             .arg(QString::number(isHoliday1))
                             .arg(QString::number(isHoliday2))
                             .arg(QString::number(errorCode1))
                             .arg(QString::number(errorCode2))
                             .arg(errorString1)
                             .arg(errorString2)
                             .arg(QString::number(isUser1))
                             .arg(QString::number(isUser2))
                             .arg(QString::number(readerExpId))
                             .arg(reqDateTime.toString("yyyy-MM-dd hh:mm:ss"))
                             .arg(QString::number(isSuccess));

    queryResultRes = _myDb->Db_execquery(queryString);
    if(queryResultRes.success){
        ret = true;
    }
    else{
        qWarning() << TABLE << "Insert__logsTableAnd--> Error: " << queryResultRes.query.lastError().text();
    }


    //TableHandler_CloseDb();

    return ret;
}


bool TableHandler::Find_NotSentLogAnd(LogAnd &logand)
{
    bool ret = false;

    //TableHandler_OpenDb();

    //variable definition
    QString queryString = "";
    queryResult queryResultRes;


    queryString = _myDb->Db_loadquery(gl_prog_QueryFindLogAnd); // "query/table_query/query_find_not_sent_logand.sql"
    queryString = queryString.arg(gl_Ini_DbName);


    queryResultRes = _myDb->Db_execquery(queryString);
    if(queryResultRes.success){
        if(queryResultRes.query.next()){
            //qDebug() << "find new log";
            int id = queryResultRes.query.value("id").toInt();
            logand.userUniqueId_1 = queryResultRes.query.value("userUniqueId1").toInt();
            logand.userUniqueId_2 = queryResultRes.query.value("userUniqueId2").toInt();
            logand.readerExpId = queryResultRes.query.value("readerExpId").toInt();
            logand.readerId_1 = queryResultRes.query.value("readerId1").toInt();
            logand.readerId_2 = queryResultRes.query.value("readerId2").toInt();
            logand.reqDateTime = queryResultRes.query.value("reqDateTime").toDateTime();
            logand.isHoliday_1 = queryResultRes.query.value("isHoliday1").toBool();
            logand.isHoliday_2 = queryResultRes.query.value("isHoliday1").toBool();
            logand.isUser_1 = queryResultRes.query.value("isUser1").toBool();
            logand.isUser_2 = queryResultRes.query.value("isUser2").toBool();
            logand.errorCode_1 = queryResultRes.query.value("errorCode1").toInt();
            logand.errorCode_2 = queryResultRes.query.value("errorCode2").toInt();
            logand.errorString_1 = queryResultRes.query.value("errorString1").toString();
            logand.errorString_2 = queryResultRes.query.value("errorString2").toString();
            logand.UUID = queryResultRes.query.value("UUID").toString();
            logand.isSuccess = queryResultRes.query.value("isSuccess").toBool();

            QString queryStringUpdated = "";
            queryStringUpdated = _myDb->Db_loadquery(gl_prog_QueryUpdatedAtLogsAnd); // "query/table_query/query_log_updatedAtand.sql"
            queryStringUpdated = queryStringUpdated.arg(gl_Ini_DbName).arg(QString::number(id));
            queryResult queryResultResUpdatedAt;

            queryResultResUpdatedAt = _myDb->Db_execquery(queryStringUpdated);
            if(queryResultRes.success)
                ret = true;
            else
                qWarning() << TABLE << "Find_NotSentLogAnd--> Error2: " <<  queryResultRes.query.lastError().text();
        }
    }
    else{
        qWarning() << TABLE << "Find_NotSentLogAnd--> Error1: " <<  queryResultRes.query.lastError().text();
    }

    //TableHandler_CloseDb();

    return ret;
}

void TableHandler::on_sig_ServerToLogAndResp_slot(QString UUID)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //qDebug() << "on_sig_ServerToLogAndResp_slot";
    //TableHandler_OpenDb();

    queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateLogAnd); // "query/table_query/query_update_logand.sql"
    queryString = queryString.arg(gl_Ini_DbName)
                             .arg(UUID);


    queryResultRes = _myDb->Db_execquery(queryString);
    if(!queryResultRes.success){
        qWarning() << TABLE << "on_sig_ServerToLogAndResp_slot--> Error: " <<  queryResultRes.query.lastError().text();
    }

    //TableHandler_CloseDb();

    return;
}

#include "query.h"

Query::Query(QObject *parent) : QObject(parent)
{

}

void Query::Init()
{
    _myDb = nullptr;
    dbConnection = false;
}

void Query::Config()
{
    _myDb = new Database(this);
}

bool Query::Query_OpenDb(QString connection_name)
{
    bool ret = _myDb->Db_open(connection_name);
    dbConnection = ret;

    return ret;
}

bool Query::Query_CloseDb()
{
    bool ret = false;
    if(dbConnection)
        ret = _myDb->Db_close();
    dbConnection = !ret;

    return ret;
}


void Query::Query_CheckUser(AuthRes &res)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //query prepare
    queryString = _myDb->Db_loadquery(gl_prog_QueryFindUser); // "query/auth_query/query_find_user.sql"
    queryString = queryString = queryString.arg(gl_Ini_DbName).arg(QString::number(res.userUniqueId));

    //query execution
    queryResultRes = _myDb->Db_execquery(queryString);

    //query get result
    if(queryResultRes.success){
        _queryResultFindUsertoStruct(queryResultRes, res);
    }
    else{
        qWarning() << QUERYCLASS << "Query_CheckUserFind--> " << queryResultRes.query.lastError().text();
        res.err = ERR_QUERY_USER_FIND;
        res.errString = ERR_STR_QUERY_USER_FIND;
    }

    if(res.err == ERR_NO_USER_FOUND || res.err == ERR_QUERY_USER_FIND)
        res.isUser = false;
    else
        res.isUser = true;
    //query show result
    #ifdef QT_DEBUG
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user row id : " << res.userRowId;
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user UUID : " << res.userUUID;
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user unique id : " << res.userUniqueId;
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user table type : " << res.userTableType;
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user activation status : " << res.userActivation;
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user credit start date time : " << res.userCreditStartDateTime.toString("yyyy-MM-dd hh:mm:ss");
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user credit stop date time : " << res.userCreditStopDateTime.toString("yyyy-MM-dd hh:mm:ss");
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user deactivation by date time status : " << res.userDeactivationByDateTime;
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user deactivation by date time status - start date time: " <<  res.userDeactivationStartDateTime.toString("yyyy-MM-dd hh:mm:ss");
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "user deactivation by date time status - stop date time: " <<  res.userDeactivationStopDateTime.toString("yyyy-MM-dd hh:mm:ss");
    #endif
   if(res.err != ERR_OK){
//        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "authentication result of request error code : " <<  res.err << QString("(0x%1)").arg(res.err,2,16,QLatin1Char('0'));
        qInfo().noquote() << QUERYCLASS << "Query_CheckUser--> " << "authentication result of request error string : " <<  res.errString;
    }

    return;
}

void Query::_queryResultFindUsertoStruct(queryResult &queryResultRes, AuthRes &res)
{
    res.err = ERR_OK;
    res.errString = ERR_STR_OK;

    if(queryResultRes.query.next()){
        res.userRowId = queryResultRes.query.value("id").toInt();
        res.userUUID = queryResultRes.query.value("UUID").toString();
        res.userUniqueId = queryResultRes.query.value("uniqueId").toInt();
        res.userTableType = queryResultRes.query.value("type").toInt();
        res.userActivation = queryResultRes.query.value("active").toInt();
        res.userCreditStartDateTime = queryResultRes.query.value("creditStartDateTime").toDateTime();
        res.userCreditStopDateTime = queryResultRes.query.value("creditStopDateTime").toDateTime();
        res.userDeactivationByDateTime = queryResultRes.query.value("deactiveByDateTime").toInt();
        res.userDeactivationStartDateTime = queryResultRes.query.value("deactiveStartDateTime").toDateTime();
        res.userDeactivationStopDateTime = queryResultRes.query.value("deactiveStopDateTime").toDateTime();
    }
    else{
        //no user id
        res.err = ERR_NO_USER_FOUND;
        res.errString = ERR_STR_UER_FOUND;
    }

    if(res.err == ERR_OK){
        if(!res.userActivation){
            //user not active
            res.err = ERR_USER_NOT_ACTIVE;
            res.errString = ERR_STR_USER_NOT_ACITVE;
        }

        //if user creditStartDateTime is valid and not null -> continue
        if(res.userCreditStartDateTime.isValid() && !res.userCreditStartDateTime.isNull()){
            if(res.reqDateTime < res.userCreditStartDateTime){
                //user req time is  earlier than user creditStartDateTime
                res.err = ERR_USER_NO_CREDIT_NOTSTART;
                res.errString = ERR_STR_USER_NO_CREDIT_NOTSTART;
            }
            //if user creditStopDateTime is valid and not null -> continue
            else if(res.userCreditStopDateTime.isValid() && !res.userCreditStopDateTime.isNull()){
                if(res.reqDateTime > res.userCreditStopDateTime){
                    //user req time is later than user creditStopDateTime
                    res.err = ERR_USER_NO_CREDIT_ISSTOP;
                    res.errString = ERR_STR_USER_NO_CREDIT_ISSTOP;
                }
            }
        }

        //if user deactiveByDateTime is true and user deactiveStartDateTime is valid and is not null and user deactiveStartDateTime is valid and is not null -> continue
        if(
            res.userDeactivationByDateTime &&
            (!res.userDeactivationStartDateTime.isNull() && res.userDeactivationStartDateTime.isValid()) &&
            (!res.userDeactivationStopDateTime.isNull() && res.userDeactivationStopDateTime.isValid())
          ){
                if(res.reqDateTime > res.userDeactivationStartDateTime && res.reqDateTime < res.userDeactivationStopDateTime){
                    res.err = ERR_USER_DEACIVE_BY_DATETIME;
                    res.errString = ERR_STR_USER_DEACIVE_BY_DATETIME;
                }
        }
    }

    return;
}

void Query::Query_CheckReader(AuthRes &res)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //query prepare
    queryString = _myDb->Db_loadquery(gl_prog_QueryFindReader); // "/query/auth_query/query_find_reader.sql"
    queryString = queryString = queryString.arg(gl_Ini_DbName).arg(QString::number(res.readerId)).arg(QString::number(res.readerExpId));

    //query execution
    queryResultRes = _myDb->Db_execquery(queryString);

    //query get result
    if(queryResultRes.success){
        _queryResultFindReadertoStruct(queryResultRes, res);
    }
    else{
        qWarning() << QUERYCLASS << "Query_CheckReader--> " << queryResultRes.query.lastError().text();
        res.err = ERR_QUERY_READER_FIND;
        res.errString = ERR_STR_QUERY_READER_FIND;
    }


    //query show result
    #ifdef QT_DEBUG
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader row id : " << res.readerRowId;
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader uuid : " << res.readerUUID;
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader id: " << res.readerId;
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader exp id : " << res.readerExpId;
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader table type : " << res.readerTableType;
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader activation status : " << res.readerActivation;
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader deactivation by date time status : " << res.readerDeactivationByDateTime;
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader deactivation by date time status - start date time: " <<  res.readerDeactivationStartDateTime.toString("yyyy-MM-dd hh:mm:ss");
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "reader deactivation by date time status - stop date time: " <<  res.readerDeactivationStopDateTime.toString("yyyy-MM-dd hh:mm:ss");
    #endif
    if(res.err != ERR_OK){
//        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "authentication result of request error code : " <<  res.err << QString("(0x%1)").arg(res.err,2,16,QLatin1Char('0'));
        qInfo().noquote() << QUERYCLASS << "Query_CheckReader--> " << "authentication result of request error string : " <<  res.errString;
    }

    return;
}

void Query::_queryResultFindReadertoStruct(queryResult &queryResultRes, AuthRes &res)
{
    res.err = ERR_OK;
    res.errString = ERR_STR_OK;

    if(queryResultRes.query.next()){
        res.err = 0;
        res.readerRowId = queryResultRes.query.value("id").toInt();
        res.readerUUID = queryResultRes.query.value("UUID").toString();
        res.readerId = queryResultRes.query.value("readerId").toInt();
        res.readerTableType = queryResultRes.query.value("type").toInt();
        res.readerExpId = queryResultRes.query.value("readerExpId").toInt();
        res.readerActivation = queryResultRes.query.value("active").toInt();
        res.readerDeactivationByDateTime = queryResultRes.query.value("deactiveByDateTime").toInt();
        res.readerDeactivationStartDateTime = queryResultRes.query.value("deactiveStartDateTime").toDateTime();
        res.readerDeactivationStopDateTime = queryResultRes.query.value("deactiveStopDateTime").toDateTime();
    }
    else{
        //no user id
        res.err = ERR_NO_READER_FOUND;
        res.errString = ERR_STR_NO_READER_FOUND;
    }

    if(res.err == ERR_OK){
        if(!res.readerActivation){
            res.err = ERR_READER_NOT_ACTIVE;
            res.errString = ERR_STR_READER_NOT_ACTIVE;
        }

        if(
            res.readerDeactivationByDateTime &&
            (res.readerDeactivationStartDateTime.isValid() && !res.readerDeactivationStartDateTime.isNull()) &&
            (res.readerDeactivationStopDateTime.isValid() && !res.readerDeactivationStopDateTime.isNull())
          ){
            if(res.reqDateTime > res.readerDeactivationStartDateTime && res.reqDateTime < res.readerDeactivationStopDateTime){
                res.err = ERR_READER_DEACTIVE_BY_DATETIME;
                res.errString = ERR_STR_READER_DEACTIVE_BY_DATETIME;
            }
        }
    }

    return;
}

void Query::Query_CheckHol(AuthRes &res)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //query prepare
    queryString = _myDb->Db_loadquery(gl_prog_QueryCheckHol); // "query/auth_query/query_find_holidays.sql"
    queryString = queryString = queryString.arg(gl_Ini_DbName).arg(res.userUUID).arg(res.reqDateTime.toString("MM-dd"));

    //query execution
    queryResultRes = _myDb->Db_execquery(queryString);

    //query get result
    if(queryResultRes.success){
        _queryResultCheckUserHoltoStruct(queryResultRes, res);
    }
    else{
        qWarning() << QUERYCLASS << "Query_CheckHol--> " << queryResultRes.query.lastError().text();
        res.err = ERR_QUERY_CHECK_HOL;
        res.errString = EER_STR_QUERY_CHECK_HOL;
    }


    //query show result
    #ifdef QT_DEBUG
        qInfo().noquote() << QUERYCLASS << "Query_CheckHol--> " << "user is holiday : " << res.isHoliday;
    #endif

//    if(res.isHoliday){
//        qInfo().noquote() << QUERYCLASS << "Query_CheckHol--> " << "user holiday table type : " << res.holidayTableType;
//        qInfo().noquote() << QUERYCLASS << "Query_CheckHol--> " << "user holiday code: " << res.holidayCode;
//    }

    #ifdef QT_DEBUG
        qInfo().noquote() << QUERYCLASS << "Query_CheckHol--> " << "user day of week : " << res.userDayOfWeek;
        qInfo().noquote() << QUERYCLASS << "Query_CheckHol--> " << "user day of week str: " << res.userDayOfWeekStr;
    #endif

    if(res.err != ERR_OK){
//        qInfo().noquote() << QUERYCLASS << "Query_CheckHol--> " << "authentication result of request error code : " <<  res.err << QString("(0x%1)").arg(res.err,2,16,QLatin1Char('0'));
        qInfo().noquote() << QUERYCLASS << "Query_CheckHol--> " << "authentication result of request error string : " <<  res.errString;
    }

    return;
}

void Query::_queryResultCheckUserHoltoStruct(queryResult &queryResultRes, AuthRes &res)
{
    res.err = ERR_OK;
    res.errString = ERR_STR_OK;

    if(queryResultRes.query.next()){
        res.holidayTableType = queryResultRes.query.value("type").toInt();
        res.holidayCode = queryResultRes.query.value("code").toString();

        res.isHoliday = true;
    }

    if(res.isHoliday){
        res.userDayOfWeekStr = "Hol";
        res.userDayOfWeek = 8;
    }
    else{
        res.userDayOfWeek = res.reqDateTime.date().dayOfWeek();
        switch(res.userDayOfWeek){
            case 1:{
                res.userDayOfWeekStr = "Mon";
                break;
            }
            case 2:{
                res.userDayOfWeekStr = "Tue";
                break;
            }
            case 3:{
                res.userDayOfWeekStr = "Wed";
                break;
            }
            case 4:{
                res.userDayOfWeekStr = "Thu";
                break;
            }
            case 5:{
                res.userDayOfWeekStr = "Fri";
                break;
            }
            case 6:{
                res.userDayOfWeekStr = "Sat";
                break;
            }
            case 7:{
                res.userDayOfWeekStr = "Sun";
                break;
            }
            default:{
                break;
            }
        }
    }

    return;
}

void Query::Query_CheckAccesspair(AuthRes &res)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //query prepare
    queryString = _myDb->Db_loadquery(gl_prog_QueryCheckPair); // "query/auth_query/query_find_accesspairs.sql"
    queryString = queryString = queryString.arg(gl_Ini_DbName).arg(res.userUUID).arg(res.readerUUID);

    //query execution
    queryResultRes = _myDb->Db_execquery(queryString);

    //query get result
    if(queryResultRes.success){
        _queryResultCheckUserPairtoStruct(queryResultRes, res);
    }
    else{
        qWarning() << QUERYCLASS << "Query_CheckAccesspair--> " << queryResultRes.query.lastError().text();
        res.err = ERR_QUERY_CHECK_ACCESS;
        res.errString = EER_STR_QUERY_CHECK_ACCESS;
    }

    if(res.err != ERR_OK){
//        qInfo().noquote() << QUERYCLASS << "Query_CheckAccesspair--> " << "authentication result of request error code : " <<  res.err << QString("(0x%1)").arg(res.err,2,16,QLatin1Char('0'));
        qInfo().noquote() << QUERYCLASS << "Query_CheckAccesspair--> " << "authentication result of request error string : " <<  res.errString;
    }

    return;
}

void Query::_queryResultCheckUserPairtoStruct(queryResult &queryResultRes, AuthRes &res)
{

    if(queryResultRes.query.next()){
        res.err = ERR_OK;
        res.errString = ERR_STR_OK;
    }
    else{
        res.err = ERR_USER_NOACCESSPAIR;
        res.errString = ERR_STR_USER_NOACCESSPAIR;
    }


    return;
}

void Query::Query_CheckAccesstime(AuthRes &res)
{
    //variable definition
    QString queryString = "";
    queryResult queryResultRes;

    //query prepare
    queryString = _myDb->Db_loadquery(gl_prog_QueryCheckAccess); // "query/auth_query/query_find_accesstimes.sql"
    queryString = queryString = queryString.arg(gl_Ini_DbName).arg(res.userDayOfWeekStr).arg(res.userUUID).arg(res.readerUUID);
    //query execution
    queryResultRes = _myDb->Db_execquery(queryString);

    //query get result
    if(queryResultRes.success){
        _queryResultCheckUserAccesstoStruct(queryResultRes, res);
    }
    else{
        qWarning() << QUERYCLASS << "qWarning: Query_CheckAccesstime--> " << queryResultRes.query.lastError().text();
        res.err = ERR_QUERY_CHECK_TIME;
        res.errString = EER_STR_QUERY_CHECK_TIME;
    }

    if(res.err != ERR_OK){
//        qInfo().noquote() << QUERYCLASS << "Query_CheckAccesstime--> " << "authentication result of request error code : " <<  res.err << QString("(0x%1)").arg(res.err,2,16,QLatin1Char('0'));
        qInfo().noquote() << QUERYCLASS << "Query_CheckAccesstime--> " << "authentication result of request error string : " <<  res.errString;
    }


    return;
}

void Query::_queryResultCheckUserAccesstoStruct(queryResult &queryResultRes, AuthRes &res)
{
    res.err = ERR_USER_NOACCESS_TIME;
    res.errString = ERR_STR_USER_NOACCESS_TIME;

    while(queryResultRes.query.next()){
        QApplication::processEvents(QEventLoop::AllEvents);
        QTime start = queryResultRes.query.value("startTime").toTime();
        QTime stop = queryResultRes.query.value("stopTime").toTime();

        #ifdef QT_DEBUG
            qInfo().noquote() << QUERYCLASS << "Query_CheckAccesstime--> " << "startTime: " << start << " , stopTime:" << stop;
        #endif

        if(start.isValid() && !start.isNull() && stop.isValid() && !stop.isNull()){
            if(res.reqDateTime.time() >= start && res.reqDateTime.time() <= stop){
                res.Auth = true;
                res.err = ERR_OK;
                res.errString = ERR_STR_OK;
                break;
            }
        }
    }

    return;
}

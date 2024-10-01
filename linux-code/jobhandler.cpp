#include "jobhandler.h"

JobHandler::JobHandler(QObject *parent) : QObject(parent)
{
    Init();

    return;
}

void JobHandler::Init()
{
    _myDb = nullptr;
    dbConnection = false;

    checkTimer = nullptr;

    bJobThreadRun = false;
    bJobThreadJobRun = false;

    in_sync = false;
    server_start_sync = false;
    count_server_unsync = 0;

    MAC = "";

    return;

}

void JobHandler::Config()
{
    _myDb = new Database(this);

    MAC = gl_Eth_MacAddr;
    MAC = MAC.remove(":");

    QString connectionName = MAC + QMYSL_JOB + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    if(!Query_OpenDb(connectionName)){
        qWarning() << JOBHANDLER << "Db_open--> " << "can not open database";
        QApplication::quit();
    }

    checkTimer = new QTimer(this);
    checkTimer->setSingleShot(true);
    checkTimer->setInterval(gl_Ini_SyncTmS * 1000);
//    checkTimer->setInterval(10 * 1000);
    checkTimer->stop();

    delay_Timer = new QTimer(this);
    delay_Timer->setSingleShot(true);
    delay_Timer->stop();
    QObject::connect(delay_Timer, &QTimer::timeout, this, &JobHandler::on_Delay_Timeout, Qt::QueuedConnection);

    QObject::connect(checkTimer, &QTimer::timeout, this, &JobHandler::on_checkTimerTimeout_slot, Qt::QueuedConnection);

    return;
}

bool JobHandler::Query_OpenDb(QString connection_name)
{
    bool ret = _myDb->Db_open(connection_name);
    dbConnection = ret;

    return ret;
}

bool JobHandler::Query_CloseDb()
{
    bool ret = false;
    if(dbConnection)
        ret = _myDb->Db_close();
    dbConnection = !ret;

    return ret;
}

void JobHandler::on_StartWorker()
{
    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";
        qInfo() << JOBHANDLER << "on_StartWorker--> " << "start JobHandler thread class";
        qInfo() << JOBHANDLER << "on_StartWorker--> " << "jbbhandler thread id = " << QThread::currentThreadId();
    #endif

    Init();
    Config();

    #ifdef QT_DEBUG
        qInfo() << JOBHANDLER << "on_StartWorker: emit this->StartedWorker();";
    #endif

    bJobThreadRun = true;
    emit this->StartedWorker();

    return;
}

void JobHandler::on_StartJobJob()
{
    if(bJobThreadRun){
//        qDebug() << "on_startCheckTimer_slot";
        checkTimer->start();
        bJobThreadJobRun = true;
        emit this->sig_StartedJobJob();
    }


    return;
}

bool JobHandler::testConnection()
{
    bool ret = false;

    QString queryStringSelect = "";
    queryResult queryResultResSelect;

    queryStringSelect = "SELECT UUID();"; // "query/job_query/timezones_query/query_selectall_timezones.sql"
    if(queryResultResSelect.query.isActive())
        queryResultResSelect.query.finish();
    queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
    if(queryResultResSelect.success)
        if(queryResultResSelect.query.next())
            ret = true;


    return ret;
}

void JobHandler::on_checkTimerTimeout_slot()
{
    checkTimer->stop();

    if(bMqttThreadJobRun){
        if(!testConnection()){
            qWarning() << JOBHANDLER << "on_checkTimerTimeout_slot--> failed testConnection --> quit app";
            //qWarning() << DBCLASS << "on_checkTimerTimeout_slot--> " << "can not open database ERROR: " << _Sqldb->lastError().text();
            QApplication::quit();
            //Query_CloseDb();
            //QString connectionName = MAC + QMYSL_JOB + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.zzz");
            //Query_OpenDb(connectionName);
        }

        if(!in_sync){
            in_sync = true;
            db_in_sync = true;

            server_start_sync = false;
            count_server_unsync = 0;

            qInfo() << JOBHANDLER << "linux sync = 1 (start)";
            emit this->sig_StartSync(1);
            Delay_ms(500);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_TimezonesTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_AccessTimezonesTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_AccessShiftsTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_Tag_UsersTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_Tag_TerminalsTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_HolidaysTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_HolidaysSetsTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_HolidaysGroupsTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_Tag_HolidaysTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_Tag_AccessPairsTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Sync_TagsTable();
            Delay_ms(50);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

            Delay_ms(500);
            qInfo() << JOBHANDLER << "linux sync = finish (stop)";
            emit this->sig_StartSync(0);
            QApplication::processEvents(QEventLoop::AllEvents);
            Delay_ms(50);

        }
        else{
//            qInfo() << JOBHANDLER << "linux sync = 2 (busy)";
            if(!server_start_sync){
                count_server_unsync++;
                if(count_server_unsync % 10 == 0){
                    qInfo() << JOBHANDLER << "server sync = no response";
                }
//                if(count_server_unsync > 30){
//                    count_server_unsync = 0;
//                    on_sig_ServerToStartStopResp_slot(false);
//                }
            }
            emit this->sig_StartSync(2);

        }


    }


    if(in_sync){
        checkTimer->setInterval(2000);
    }
    else{
        checkTimer->setInterval(JOB_CHECK_JOBTIMER_SEC * 1000);
    }
    checkTimer->start();

}

void JobHandler::Sync_TimezonesTable()
{
    qInfo() << JOBHANDLER << "Sync_TimezonesTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;



    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromTimezonesTable); // "query/job_query/timezones_query/query_selectall_timezones.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);


    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromTimezonesTable); // "query/job_query/timezones_query/query_updateall_timezones.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_TimezonesTableSync(queryResultResSelect.query.value("code").toString(), queryResultResSelect.query.value("startTime").toTime().toString("hh:mm:ss"), queryResultResSelect.query.value("stopTime").toTime().toString("hh:mm:ss"), queryResultResSelect.query.value("UUID").toString());

                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_AccessTimezonesTable()
{
    qInfo() << JOBHANDLER << "Sync_AccessTimezonesTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;



    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromAccessTimezonesTable); // "query/job_query/accesstimezones_query/query_selectall_accesstimezones.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);


    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromAccessTimezonesTable); // "query/job_query/accesstimezones_query/query_updateall_accesstimezones.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_AccessTimezonesTableSync(queryResultResSelect.query.value("code").toString(),
                                                      queryResultResSelect.query.value("uuid01__TimezonesTableUuid").toString(), queryResultResSelect.query.value("uuid02__TimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuid03__TimezonesTableUuid").toString(), queryResultResSelect.query.value("uuid04__TimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuid05__TimezonesTableUuid").toString(), queryResultResSelect.query.value("uuid06__TimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuid07__TimezonesTableUuid").toString(), queryResultResSelect.query.value("uuid08__TimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuid09__TimezonesTableUuid").toString(), queryResultResSelect.query.value("uuid10__TimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuid11__TimezonesTableUuid").toString(), queryResultResSelect.query.value("uuid12__TimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_AccessShiftsTable()
{
    qInfo() << JOBHANDLER << "Sync_AccessShiftsTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;

    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromAccessShiftsTable); // "query/job_query/accessshifts_query/query_selectall_accessshifts.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromAccessShiftsTable); // "query/job_query/accessshifts_query/query_updateall_accessshifts.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_AccessShiftsTableSync(queryResultResSelect.query.value("code").toString(), queryResultResSelect.query.value("type").toInt(),
                                                      queryResultResSelect.query.value("uuidSat__AccessTimezonesTableUuid").toString(), queryResultResSelect.query.value("uuidSun__AccessTimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuidMon__AccessTimezonesTableUuid").toString(), queryResultResSelect.query.value("uuidTue__AccessTimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuidWed__AccessTimezonesTableUuid").toString(), queryResultResSelect.query.value("uuidThu__AccessTimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("uuidFri__AccessTimezonesTableUuid").toString(), queryResultResSelect.query.value("uuidHol__AccessTimezonesTableUuid").toString(),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_Tag_UsersTable()
{
    qInfo() << JOBHANDLER << "Sync_Tag_UsersTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;



    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromTagUsersTable); // "query/job_query/tag_users_query/query_selectall_tagusers.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){

                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromTagUsersTable); // "query/job_query/tag_users_query/query_updateall_tagusers.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_Tag_UsersTableSync(queryResultResSelect.query.value("uniqueId").toInt(), queryResultResSelect.query.value("type").toInt(),
                                                      queryResultResSelect.query.value("active").toInt(),
                                                      queryResultResSelect.query.value("creditStartDateTime").toDateTime().toString("yyyy-MM-dd hh:mm:ss"), queryResultResSelect.query.value("creditStopDateTime").toDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                                                      queryResultResSelect.query.value("deactiveByDateTime").toInt(),
                                                      queryResultResSelect.query.value("deactiveStartDateTime").toDateTime().toString("yyyy-MM-dd hh:mm:ss"), queryResultResSelect.query.value("deactiveStopDateTime").toDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_Tag_TerminalsTable()
{
    qInfo() << JOBHANDLER << "Sync_Tag_TerminalsTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;



    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromTagTerminalsTable); // "query/job_query/tag_terminals_query/query_selectall_tagterminals.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromTagTerminalsTable); // "query/job_query/tag_terminals_query/query_updateall_tagterminals.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_Tag_TerminalsTableSync(queryResultResSelect.query.value("readerId").toInt(), queryResultResSelect.query.value("type").toInt(), queryResultResSelect.query.value("readerExpId").toInt(),
                                                      queryResultResSelect.query.value("active").toInt(), queryResultResSelect.query.value("deactiveByDateTime").toInt(),
                                                      queryResultResSelect.query.value("deactiveStartDateTime").toDateTime().toString("yyyy-MM-dd hh:mm:ss"), queryResultResSelect.query.value("deactiveStopDateTime").toDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_HolidaysTable()
{
    qInfo() << JOBHANDLER << "Sync_HolidaysTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;

    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromHolidaysTable); // "query/job_query/holidays_query/query_selectall_holidays.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromHolidaysTable); // "query/job_query/holidays_query/query_updateall_holidays.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_HolidaysTableSync(queryResultResSelect.query.value("code").toString(),
                                                      queryResultResSelect.query.value("date").toDate().toString("yyyy-MM-dd"),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_HolidaysSetsTable()
{
    qInfo() << JOBHANDLER << "Sync_HolidaysSetsTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;

    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromHolidaysSetsTable); // "query/job_query/holidayssets_query/query_selectall_holidayssets.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);



    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromHolidaysSetsTable); // "query/job_query/holidayssets_query/query_updateall_holidayssets.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    QList<QString> stringList;
                    stringList.clear();
                    for(int i = 1; i < 33; i++){
                        stringList.append(QString("uuid%1_HolidaysTableUuid").arg(i, 2, 10, QLatin1Char('0')));
                    }
                    emit this->sig_HolidaysSetsTableSync(
                                                         queryResultResSelect.query.value("code").toString(),
                                                         queryResultResSelect.query.value(stringList.at(0)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(1)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(2)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(3)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(4)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(5)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(6)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(7)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(8)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(9)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(10)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(11)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(12)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(13)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(14)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(15)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(16)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(17)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(18)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(19)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(20)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(21)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(22)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(23)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(24)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(25)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(26)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(27)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(28)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(29)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(30)).toString(),
                                                         queryResultResSelect.query.value(stringList.at(31)).toString(),
                                                         queryResultResSelect.query.value("UUID").toString()
                                                        );
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_HolidaysGroupsTable()
{
    qInfo() << JOBHANDLER << "Sync_HolidaysGroupsTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;

    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromHolidaysGroupsTable); // "query/job_query/holidaysgroups_query/query_selectall_holidaysgroups.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromHolidaysGroupsTable); // "query/job_query/holidaysgroups_query/query_updateall_holidaysgroups.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    QList<QString> stringList;
                    stringList.clear();
                    for(int i = 1; i < 6; i++){
                        stringList.append(QString("uuid%1_HolidaysSetsTableUuid").arg(i, 2, 10, QLatin1Char('0')));
                    }
                    emit this->sig_HolidaysGroupsTableSync(
                                                           queryResultResSelect.query.value("code").toString(),
                                                           queryResultResSelect.query.value(stringList.at(00)).toString(),
                                                           queryResultResSelect.query.value(stringList.at(01)).toString(),
                                                           queryResultResSelect.query.value(stringList.at(02)).toString(),
                                                           queryResultResSelect.query.value(stringList.at(03)).toString(),
                                                           queryResultResSelect.query.value(stringList.at(04)).toString(),
                                                           queryResultResSelect.query.value("UUID").toString()
                                                          );
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_Tag_HolidaysTable()
{
    qInfo() << JOBHANDLER << "Sync_Tag_HolidaysTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;



    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromTagHolidaysTable); // "query/job_query/tag_holidays_query/query_selectall_tagholidays.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromTagHolidaysTable); // "query/job_query/tag_holidays_query/query_updateall_tagholidays.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_Tag_HolidaysTableSync(queryResultResSelect.query.value("code").toString(), queryResultResSelect.query.value("type").toInt(),
                                                      queryResultResSelect.query.value("uuid_HolidaysGroupsTableUuid").toString(),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_Tag_AccessPairsTable()
{
    qInfo() << JOBHANDLER << "Sync_Tag_AccessPairsTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;



    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromTagAccessPairsTable); // "query/job_query/tag_accesspairs_query/query_selectall_tagaccesspairs.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromTagAccessPairsTable); // "query/job_query/tag_accesspairs_query/query_updateall_tagaccesspairs.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_Tag_AccessPairsTableSync(queryResultResSelect.query.value("code").toString(), queryResultResSelect.query.value("type").toInt(),
                                                      queryResultResSelect.query.value("uuid__tag_terminalsTableUuid").toString(), queryResultResSelect.query.value("uuid__AccessShiftsTableUuid").toString(),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}

void JobHandler::Sync_TagsTable()
{
    qInfo() << JOBHANDLER << "Sync_TagsTable--> ";
    QString currdt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //variable definition
    QString queryStringSelect = "";
    queryResult queryResultResSelect;



    //query prepare
    queryStringSelect = _myDb->Db_loadquery(gl_prog_QuerySelectAllFromTagsTable); // "query/job_query/tags_query/query_selectall_tags.sql"
    queryStringSelect = queryStringSelect.arg(gl_Ini_DbName).arg(currdt);

    bool sync = true;
    while(sync){
        //query execution
        if(queryResultResSelect.query.isActive())
            queryResultResSelect.query.finish();
        queryResultResSelect = _myDb->Db_execquery(queryStringSelect);
        if(queryResultResSelect.success){
            if(queryResultResSelect.query.next()){
                QString uuid = queryResultResSelect.query.value("UUID").toString();
                QString queryStringUpdate = "";
                queryResult queryResultResUpdatet;
                queryStringUpdate = _myDb->Db_loadquery(gl_prog_QueryUpdateAllFromTagsTable); // "query/job_query/tags_query/query_updateall_tags.sql"
                queryStringUpdate = queryStringUpdate.arg(gl_Ini_DbName).arg(uuid);
                queryResultResUpdatet = _myDb->Db_execquery(queryStringUpdate);
                if(queryResultResUpdatet.success){
                    emit this->sig_TagsTableSync(queryResultResSelect.query.value("code").toString(),
                                                      queryResultResSelect.query.value("uuidTagged").toString(), queryResultResSelect.query.value("taggedType").toInt(),
                                                      queryResultResSelect.query.value("uuidTag").toString(), queryResultResSelect.query.value("tagType").toInt(),
                                                      queryResultResSelect.query.value("UUID").toString());
                }
                else{
                    sync = false;
                }
            }
            else{
                sync = false;
            }
        }
        else{
            sync = false;
        }
        QApplication::processEvents(QEventLoop::AllEvents);
        Delay_ms(50);
    }

    return;
}


//###############################################################################################################################
//###############################################################################################################################

void JobHandler::on_sig_ServerToStartStopResp_slot(bool startStop)
{
    if(startStop){
        qInfo() << JOBHANDLER << "on_sig_ServerToStartStopResp_slot--> " << "start sync from server";
        server_start_sync = true;
        count_server_unsync = 0;
    }
    else{
        qInfo() << JOBHANDLER << "on_sig_ServerToStartStopResp_slot--> " << "stop sync from server";
        in_sync = false;
        db_in_sync = false;
        server_start_sync = false;
        count_server_unsync = 0;

        checkTimer->stop();
        checkTimer->setInterval(JOB_CHECK_JOBTIMER_SEC * 1000);
        checkTimer->start();
    }

    qInfo() << "";
    qInfo() << "";

    return;
}

void JobHandler::on_sig_ServerToTimezonesTableResp_slot(int action, QString code, QString startTime, QString stopTime, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(startTime == "")
            startTime = "NULL";
        else
            startTime = "\'" + startTime + "\'";

        if(stopTime == "")
            stopTime = "NULL";
        else
            stopTime = "\'" + stopTime + "\'";
    }


    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "delete UUID " << UUID << "from TimezonesTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromTimezonesTable); // "query/job_query/timezones_query/query_delete_from_timezonestable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "delete UUID " << UUID << "from TimezonesTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "update UUID " << UUID << "to Timezones Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToTimezonesTable); // "query/job_query/timezones_query/query_update_to_timezonestable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(startTime).arg(stopTime).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "update UUID " << UUID << "to TimezonesTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }

    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "insert UUID " << UUID << "to TimezonesTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToTimezonesTable); // "query/job_query/timezones_query/query_insert_to_timezonestable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(startTime).arg(stopTime).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "insert UUID " << UUID << "to TimezonesTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerToAccessTimezonesTableResp_slot(int action, QString code, QString uuid01, QString uuid02, QString uuid03, QString uuid04, QString uuid05, QString uuid06, QString uuid07, QString uuid08, QString uuid09, QString uuid10, QString uuid11, QString uuid12, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(uuid01 == "")
            uuid01 = "NULL";
        else
            uuid01 = "\'" + uuid01 + "\'";

        if(uuid02 == "")
            uuid02 = "NULL";
        else
            uuid02 = "\'" + uuid02 + "\'";

        if(uuid03 == "")
            uuid03 = "NULL";
        else
            uuid03 = "\'" + uuid03 + "\'";

        if(uuid04 == "")
            uuid04 = "NULL";
        else
            uuid04 = "\'" + uuid04 + "\'";

        if(uuid05 == "")
            uuid05 = "NULL";
        else
            uuid05 = "\'" + uuid05 + "\'";

        if(uuid06 == "")
            uuid06 = "NULL";
        else
            uuid06 = "\'" + uuid06 + "\'";

        if(uuid07 == "")
            uuid07 = "NULL";
        else
            uuid07 = "\'" + uuid07 + "\'";

        if(uuid08 == "")
            uuid08 = "NULL";
        else
            uuid08 = "\'" + uuid08 + "\'";

        if(uuid09 == "")
            uuid09 = "NULL";
        else
            uuid09 = "\'" + uuid09 + "\'";

        if(uuid10 == "")
            uuid10 = "NULL";
        else
            uuid10 = "\'" + uuid10 + "\'";

        if(uuid11 == "")
            uuid11 = "NULL";
        else
            uuid11 = "\'" + uuid11 + "\'";

        if(uuid12 == "")
            uuid12 = "NULL";
        else
            uuid12 = "\'" + uuid12 + "\'";
    }


    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerToAccessTimezonesTableResp_slot--> " << "delete UUID " << UUID << "from AccessTimezonesTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromAccessTimezonesTable); // "query/job_query/accesstimezones_query/query_delete_from_accesstimezonestable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToAccessTimezonesTableResp_slot--> " << "delete UUID " << UUID << "from AccessTimezonesTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "update UUID " << UUID << "to AccessTimezonesTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToAccessTimezonesTable); // "query/job_query/accesstimezones_query/query_update_to_accesstimezonestable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).
                arg(uuid01).arg(uuid02).
                arg(uuid03).arg(uuid04).
                arg(uuid05).arg(uuid06).
                arg(uuid07).arg(uuid08).
                arg(uuid09).arg(uuid10).
                arg(uuid11).arg(uuid12).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToAccessTimezonesTableResp_slot--> " << "update UUID " << UUID << "to AccessTimezonesTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }

    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerToTimezonesTableResp_slot--> " << "insert UUID " << UUID << "to AccessTimezonesTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToAccessTimezonesTable); // "query/job_query/accesstimezones_query/query_insert_to_accesstimezonestable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).
                arg(uuid01).arg(uuid02).
                arg(uuid03).arg(uuid04).
                arg(uuid05).arg(uuid06).
                arg(uuid07).arg(uuid08).
                arg(uuid09).arg(uuid10).
                arg(uuid11).arg(uuid12).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToAccessTimezonesTableResp_slot--> " << "insert UUID " << UUID << "to AccessTimezonesTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerToAccessShiftsTableResp_slot(int action, QString code, int type, QString uuidSat, QString uuidSun, QString uuidMon, QString uuidTue, QString uuidWed, QString uuidThu, QString uuidFri, QString uuidHol, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    QString stype = "";

    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(type == 0)
            stype = "NULL";
        else
            stype = QString::number(type);

        if(uuidSat == "")
            uuidSat = "NULL";
        else
            uuidSat = "\'" + uuidSat + "\'";

        if(uuidSun == "")
            uuidSun = "NULL";
        else
            uuidSun = "\'" + uuidSun + "\'";

        if(uuidMon == "")
            uuidMon = "NULL";
        else
            uuidMon = "\'" + uuidMon + "\'";

        if(uuidTue == "")
            uuidTue = "NULL";
        else
            uuidTue = "\'" + uuidTue + "\'";

        if(uuidWed == "")
            uuidWed = "NULL";
        else
            uuidWed = "\'" + uuidWed + "\'";

        if(uuidThu == "")
            uuidThu = "NULL";
        else
            uuidThu = "\'" + uuidThu + "\'";

        if(uuidFri == "")
            uuidFri = "NULL";
        else
            uuidFri = "\'" + uuidFri + "\'";

        if(uuidHol == "")
            uuidHol = "NULL";
        else
            uuidHol = "\'" + uuidHol + "\'";
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerToAccessShiftsTableResp_slot--> " << "delete UUID " << UUID << "from AccessShiftsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromAccessShiftsTable); // "query/job_query/accessshifts_query/query_delete_from_accessshiftstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToAccessShiftsTableResp_slot--> " << "delete UUID " << UUID << "from AccessShiftsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerToAccessShiftsTableResp_slot--> " << "update UUID " << UUID << "to AccessShiftsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToAccessShiftsTable); // "query/job_query/accessshifts_query/query_update_to_accessshiftstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(stype).
                arg(uuidSat).arg(uuidSun).
                arg(uuidMon).arg(uuidTue).
                arg(uuidWed).arg(uuidThu).
                arg(uuidFri).arg(uuidHol).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToAccessShiftsTableResp_slot--> " << "update UUID " << UUID << "to AccessShiftsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }

    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerToAccessShiftsTableResp_slot--> " << "insert UUID " << UUID << "to AccessShiftsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToAccessShiftsTable); // "query/job_query/accessshifts_query/query_insert_to_accessshiftstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(stype).
                arg(uuidSat).arg(uuidSun).
                arg(uuidMon).arg(uuidTue).
                arg(uuidWed).arg(uuidThu).
                arg(uuidFri).arg(uuidHol).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToAccessShiftsTableResp_slot--> " << "insert UUID " << UUID << "to AccessShiftsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerTotag_UsersTableResp_slot(int action, int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    QString suseruniqueId = "";
    QString stype = "";

    if(action == 1 || action == 2){
        if(userUniqueId == 0)
            suseruniqueId = "NULL";
        else
            suseruniqueId = QString::number(userUniqueId);

        if(type == 0)
            stype = "NULL";
        else
            stype = QString::number(type);

        if(creditStartDateTime == "")
            creditStartDateTime = "NULL";
        else
            creditStartDateTime = "\'" + creditStartDateTime + "\'";

        if(creditStopDateTime == "")
            creditStopDateTime = "NULL";
        else
            creditStopDateTime = "\'" + creditStopDateTime + "\'";

        if(deactiveStartDateTime == "")
            deactiveStartDateTime = "NULL";
        else
            deactiveStartDateTime = "\'" + deactiveStartDateTime + "\'";

        if(deactiveStopDateTime == "")
            deactiveStopDateTime = "NULL";
        else
            deactiveStopDateTime = "\'" + deactiveStopDateTime + "\'";
    }


    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "delete UUID " << UUID << "from Tag_UsersTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromTagUsersTable); // "query/job_query/tag_users_query/query_delete_from_taguserstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "delete UUID " << UUID << "from Tag_UsersTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "update UUID " << UUID << "to Tag_UsersTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToTagUsersTable); // "query/job_query/tag_users_query/query_update_to_taguserstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(suseruniqueId).arg(stype).arg(QString::number(active)).
                arg(creditStartDateTime).arg(creditStopDateTime).
                arg(QString::number(deactiveByDateTime)).
                arg(deactiveStartDateTime).arg(deactiveStopDateTime).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "update UUID " << UUID << "to Tag_UsersTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }

    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "insert UUID " << UUID << "to Tag_UsersTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToTagUsersTable); // "query/job_query/tag_users_query/query_insert_to_taguserstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(suseruniqueId).arg(stype).arg(QString::number(active)).
                arg(creditStartDateTime).arg(creditStopDateTime).
                arg(QString::number(deactiveByDateTime)).
                arg(deactiveStartDateTime).arg(deactiveStopDateTime).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "insert UUID " << UUID << "to Tag_UsersTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerTotag_TerminalsTableResp_slot(int action, int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    QString sreaderId = "";
    QString stype = "";
    QString sreaderExpId = "";


    if(action == 1 || action == 2){
        if(readerId == 0)
            sreaderId = "NULL";
        else
            sreaderId = QString::number(readerId);

        if(type == 0)
            stype = "NULL";
        else
            stype = QString::number(type);

        if(readerExpId == 0)
            sreaderExpId = "NULL";
        else
            sreaderExpId = QString::number(readerExpId);

        if(deactiveStartDateTime == "")
            deactiveStartDateTime = "NULL";
        else
            deactiveStartDateTime = "\'" + deactiveStartDateTime + "\'";

        if(deactiveStopDateTime == "")
            deactiveStopDateTime = "NULL";
        else
            deactiveStopDateTime = "\'" + deactiveStopDateTime + "\'";
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_TerminalsTableResp_slot--> " << "delete UUID " << UUID << "from Tag_TerminalsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromTagTerminalsTable); // "query/job_query/tag_terminals_query/query_delete_from_tagterminalstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_TerminalsTableResp_slot--> " << "delete UUID " << UUID << "from Tag_TerminalsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_TerminalsTableResp_slot--> " << "update UUID " << UUID << "to Tag_TerminalsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToTagTerminalsTable); // "query/job_query/tag_terminals_query/query_update_to_tagterminalstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(sreaderId).arg(stype).arg(sreaderExpId).arg(QString::number(active)).arg(QString::number(deactiveByDateTime)).
                arg(deactiveStartDateTime).arg(deactiveStopDateTime).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_TerminalsTableResp_slot--> " << "update UUID " << UUID << "to Tag_TerminalsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }

    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_TerminalsTableResp_slot--> " << "insert UUID " << UUID << "to Tag_TerminalsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToTagTerminalsTable); // "query/job_query/tag_terminals_query/query_insert_to_tagterminalstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(sreaderId).arg(stype).arg(sreaderExpId).arg(QString::number(active)).arg(QString::number(deactiveByDateTime)).
                arg(deactiveStartDateTime).arg(deactiveStopDateTime).
                arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_TerminalsTableResp_slot--> " << "insert UUID " << UUID << "to Tag_TerminalsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerToHolidaysTableResp_slot(int action, QString code, QString date, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    QString stype = "";


    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(date == "")
            date = "NULL";
        else
            date = "\'" + date + "\'";
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerToHolidaysTableResp_slot--> " << "delete UUID " << UUID << "from HolidaysTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromHolidaysTable); // "query/job_query/holidays_query/query_delete_from_holidaystable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysTableResp_slot--> " << "delete UUID " << UUID << "from HolidaysTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerTo_HolidaysTableResp_slot--> " << "update UUID " << UUID << "to HolidaysTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToHolidaysTable); // "query/job_query/holidays_query/query_update_to_holidaystable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(date).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysTableResp_slot--> " << "update UUID " << UUID << "to HolidaysTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerToHolidaysTableResp_slot--> " << "insert UUID " << UUID << "to HolidaysTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToHolidaysTable); // "query/job_query/holidays_query/query_insert_to_holidaystable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(date).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysTableResp_slot--> " << "insert UUID " << UUID << "to HolidaysTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerToHolidaysSetsTableResp_slot(int action, QString code, QString uuid01, QString uuid02, QString uuid03, QString uuid04, QString uuid05, QString uuid06, QString uuid07, QString uuid08, QString uuid09, QString uuid10, QString uuid11, QString uuid12, QString uuid13, QString uuid14, QString uuid15, QString uuid16, QString uuid17, QString uuid18, QString uuid19, QString uuid20, QString uuid21, QString uuid22, QString uuid23, QString uuid24, QString uuid25, QString uuid26, QString uuid27, QString uuid28, QString uuid29, QString uuid30, QString uuid31, QString uuid32, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;


    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(uuid01 == "")
            uuid01 = "NULL";
        else
            uuid01 = "\'" + uuid01 + "\'";

        if(uuid02 == "")
            uuid02 = "NULL";
        else
            uuid02 = "\'" + uuid02 + "\'";

        if(uuid03 == "")
            uuid03 = "NULL";
        else
            uuid03 = "\'" + uuid03 + "\'";

        if(uuid04 == "")
            uuid04 = "NULL";
        else
            uuid04 = "\'" + uuid04 + "\'";

        if(uuid05 == "")
            uuid05 = "NULL";
        else
            uuid05 = "\'" + uuid05 + "\'";

        if(uuid06 == "")
            uuid06 = "NULL";
        else
            uuid06 = "\'" + uuid06 + "\'";

        if(uuid07 == "")
            uuid07 = "NULL";
        else
            uuid07 = "\'" + uuid07 + "\'";

        if(uuid08 == "")
            uuid08 = "NULL";
        else
            uuid08 = "\'" + uuid08 + "\'";

        if(uuid09 == "")
            uuid09 = "NULL";
        else
            uuid09 = "\'" + uuid09 + "\'";

        if(uuid10 == "")
            uuid10 = "NULL";
        else
            uuid10 = "\'" + uuid10 + "\'";

        if(uuid11 == "")
            uuid11 = "NULL";
        else
            uuid11 = "\'" + uuid11 + "\'";

        if(uuid12 == "")
            uuid12 = "NULL";
        else
            uuid12 = "\'" + uuid12 + "\'";

        if(uuid13 == "")
            uuid13 = "NULL";
        else
            uuid13 = "\'" + uuid13 + "\'";

        if(uuid14 == "")
            uuid14 = "NULL";
        else
            uuid14 = "\'" + uuid14 + "\'";

        if(uuid15 == "")
            uuid15 = "NULL";
        else
            uuid15 = "\'" + uuid15 + "\'";

        if(uuid16 == "")
            uuid16 = "NULL";
        else
            uuid16 = "\'" + uuid16 + "\'";

        if(uuid17 == "")
            uuid17 = "NULL";
        else
            uuid17 = "\'" + uuid17 + "\'";

        if(uuid18 == "")
            uuid18 = "NULL";
        else
            uuid18 = "\'" + uuid18 + "\'";

        if(uuid19 == "")
            uuid19 = "NULL";
        else
            uuid19 = "\'" + uuid19 + "\'";

        if(uuid20 == "")
            uuid20 = "NULL";
        else
            uuid20 = "\'" + uuid20 + "\'";

        if(uuid21 == "")
            uuid21 = "NULL";
        else
            uuid21 = "\'" + uuid21 + "\'";

        if(uuid22 == "")
            uuid22 = "NULL";
        else
            uuid22 = "\'" + uuid22 + "\'";

        if(uuid23 == "")
            uuid23 = "NULL";
        else
            uuid23 = "\'" + uuid23 + "\'";

        if(uuid24 == "")
            uuid24 = "NULL";
        else
            uuid24 = "\'" + uuid24 + "\'";

        if(uuid25 == "")
            uuid25 = "NULL";
        else
            uuid25 = "\'" + uuid25 + "\'";

        if(uuid26 == "")
            uuid26 = "NULL";
        else
            uuid26 = "\'" + uuid26 + "\'";

        if(uuid27 == "")
            uuid27 = "NULL";
        else
            uuid27 = "\'" + uuid27 + "\'";

        if(uuid28 == "")
            uuid28 = "NULL";
        else
            uuid28 = "\'" + uuid28 + "\'";

        if(uuid29 == "")
            uuid29 = "NULL";
        else
            uuid29 = "\'" + uuid29 + "\'";

        if(uuid30 == "")
            uuid30 = "NULL";
        else
            uuid30 = "\'" + uuid30 + "\'";

        if(uuid31 == "")
            uuid31 = "NULL";
        else
            uuid31 = "\'" + uuid31 + "\'";

        if(uuid32 == "")
            uuid32 = "NULL";
        else
            uuid32 = "\'" + uuid32 + "\'";
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerToHolidaysSetsTableResp_slot--> " << "delete UUID " << UUID << "from HolidaysSetsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromHolidaysSetsTable); // "query/job_query/holidayssets_query/query_delete_from_holidayssetstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysSetsTableResp_slot--> " << "delete UUID " << UUID << "from HolidaysSetsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerTo_HolidaysSetsTableResp_slot--> " << "update UUID " << UUID << "to HolidaysSetsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToHolidaysSetsTable); // "query/job_query/holidayssets_query/query_update_to_holidayssetstable.sql"
        queryString = queryString
                                 .arg(gl_Ini_DbName)
                                 .arg(code)
                                 .arg(uuid01)
                                 .arg(uuid02)
                                 .arg(uuid03)
                                 .arg(uuid04)
                                 .arg(uuid05)
                                 .arg(uuid06)
                                 .arg(uuid07)
                                 .arg(uuid08)
                                 .arg(uuid09)
                                 .arg(uuid10)
                                 .arg(uuid11)
                                 .arg(uuid12)
                                 .arg(uuid13)
                                 .arg(uuid14)
                                 .arg(uuid15)
                                 .arg(uuid16)
                                 .arg(uuid17)
                                 .arg(uuid18)
                                 .arg(uuid19)
                                 .arg(uuid20)
                                 .arg(uuid21)
                                 .arg(uuid22)
                                 .arg(uuid23)
                                 .arg(uuid24)
                                 .arg(uuid25)
                                 .arg(uuid26)
                                 .arg(uuid27)
                                 .arg(uuid28)
                                 .arg(uuid29)
                                 .arg(uuid30)
                                 .arg(uuid31)
                                 .arg(uuid32)
                                 .arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysSetsTableResp_slot--> " << "update UUID " << UUID << "to HolidaysSetsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerToHolidaysSetsTableResp_slot--> " << "insert UUID " << UUID << "to HolidaysSetsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToHolidaysSetsTable); // "query/job_query/holidayssets_query/query_insert_to_holidayssetstable.sql"
        queryString = queryString
                                 .arg(gl_Ini_DbName)
                                 .arg(code)
                                 .arg(uuid01)
                                 .arg(uuid02)
                                 .arg(uuid03)
                                 .arg(uuid04)
                                 .arg(uuid05)
                                 .arg(uuid06)
                                 .arg(uuid07)
                                 .arg(uuid08)
                                 .arg(uuid09)
                                 .arg(uuid10)
                                 .arg(uuid11)
                                 .arg(uuid12)
                                 .arg(uuid13)
                                 .arg(uuid14)
                                 .arg(uuid15)
                                 .arg(uuid16)
                                 .arg(uuid17)
                                 .arg(uuid18)
                                 .arg(uuid19)
                                 .arg(uuid20)
                                 .arg(uuid21)
                                 .arg(uuid22)
                                 .arg(uuid23)
                                 .arg(uuid24)
                                 .arg(uuid25)
                                 .arg(uuid26)
                                 .arg(uuid27)
                                 .arg(uuid28)
                                 .arg(uuid29)
                                 .arg(uuid30)
                                 .arg(uuid31)
                                 .arg(uuid32)
                                 .arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysSetsTableResp_slot--> " << "insert UUID " << UUID << "to HolidaysSetsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerToHolidaysGroupsTableResp_slot(int action, QString code, QString uuid01, QString uuid02, QString uuid03, QString uuid04, QString uuid05, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(uuid01 == "")
            uuid01 = "NULL";
        else
            uuid01 = "\'" + uuid01 + "\'";

        if(uuid02 == "")
            uuid02 = "NULL";
        else
            uuid02 = "\'" + uuid02 + "\'";

        if(uuid03 == "")
            uuid03 = "NULL";
        else
            uuid03 = "\'" + uuid03 + "\'";

        if(uuid04 == "")
            uuid04 = "NULL";
        else
            uuid04 = "\'" + uuid04 + "\'";

        if(uuid05 == "")
            uuid05 = "NULL";
        else
            uuid05 = "\'" + uuid05 + "\'";
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerToHolidaysGroupsTableResp_slot--> " << "delete UUID " << UUID << "from HolidaysGroupsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromHolidaysGroupsTable); // "query/job_query/holidaysgroups_query/query_delete_from_holidaysgroupstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysGroupsTableResp_slot--> " << "delete UUID " << UUID << "from HolidaysGroupsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerToHolidaysGroupsTableResp_slot--> " << "update UUID " << UUID << "to HolidaysGroupsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToHolidaysGroupsTable); // "query/job_query/holidaysgroups_query/query_update_to_holidaysgroupstable.sql"
        queryString = queryString.arg(gl_Ini_DbName)
                                 .arg(code)
                                 .arg(uuid01)
                                 .arg(uuid02)
                                 .arg(uuid03)
                                 .arg(uuid04)
                                 .arg(uuid05)
                                 .arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysGroupsTableResp_slot--> " << "update UUID " << UUID << "to HolidaysGroupsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerToHolidaysGroupsTableResp_slot--> " << "insert UUID " << UUID << "to HolidaysGroupsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToHolidaysGroupsTable); // "query/job_query/holidaysgroups_query/query_insert_to_holidaysgroupstable.sql"
        queryString = queryString.arg(gl_Ini_DbName)
                                 .arg(code)
                                 .arg(uuid01)
                                 .arg(uuid02)
                                 .arg(uuid03)
                                 .arg(uuid04)
                                 .arg(uuid05)
                                 .arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerToHolidaysGroupsTableResp_slot--> " << "insert UUID " << UUID << "to HolidaysGroupsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerTotag_HolidaysTableResp_slot(int action, QString code, int type, QString uuid, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    QString stype = "";


    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(type == 0)
            stype = "NULL";
        else
            stype = QString::number(type);

        if(uuid == "")
            uuid = "NULL";
        else
            uuid = "\'" + uuid + "\'";
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_HolidaysTableResp_slot--> " << "delete UUID " << UUID << "from Tag_HolidaysTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromTagHolidaysTable); // "query/job_query/tag_holidays_query/query_delete_from_tagholidaystable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_HolidaysTableResp_slot--> " << "delete UUID " << UUID << "from Tag_HolidaysTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_HolidaysTableResp_slot--> " << "update UUID " << UUID << "to Tag_HolidaysTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToTagHolidaysTable); // "query/job_query/tag_holidays_query/query_update_to_tagholidaystable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(stype).arg(uuid).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_HolidaysTableResp_slot--> " << "update UUID " << UUID << "to Tag_HolidaysTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_HolidaysTableResp_slot--> " << "insert UUID " << UUID << "to Tag_HolidaysTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToTagHolidaysTable); // "query/job_query/tag_holidays_query/query_insert_to_tagholidaystable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(stype).arg(uuid).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_HolidaysTableResp_slot--> " << "insert UUID " << UUID << "to Tag_HolidaysTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerTotag_AccessPairsTableResp_slot(int action, QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    QString stype = "";


    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(type == 0)
            stype = "NULL";
        else
            stype = QString::number(type);

        if(UUID_Terminals == "")
            UUID_Terminals = "NULL";
        else
            UUID_Terminals = "\'" + UUID_Terminals + "\'";

        if(UUID_Shifts == "")
            UUID_Shifts = "NULL";
        else
            UUID_Shifts = "\'" + UUID_Shifts + "\'";
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_AccessPairsTableResp_slot--> " << "delete UUID " << UUID << "from Tag_AccessPairsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromTagAccessPairsTable); // "query/job_query/tag_accesspairs_query/query_delete_from_tagaccesspairstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_AccessPairsTableResp_slot--> " << "delete UUID " << UUID << "from Tag_AccessPairsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_AccessPairsTableResp_slot--> " << "update UUID " << UUID << "to Tag_AccessPairsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToTagAccessPairsTable); // "query/job_query/tag_accesspairs_query/query_update_to_tagaccesspairstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(stype).arg(UUID_Terminals).arg(UUID_Shifts).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_AccessPairsTableResp_slot--> " << "update UUID " << UUID << "to Tag_AccessPairsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }

    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_AccessPairsTableResp_slot--> " << "insert UUID " << UUID << "to Tag_AccessPairsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToTagAccessPairsTable); // "query/job_query/tag_accesspairs_query/query_insert_to_tagaccesspairstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(stype).arg(UUID_Terminals).arg(UUID_Shifts).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_AccessPairsTableResp_slot--> " << "insert UUID " << UUID << "to Tag_AccessPairsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}

void JobHandler::on_sig_ServerTotagsTableResp_slot(int action, QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID)
{
    QString queryString = "";
    queryResult queryResultRes;

    QString staggedType = "";
    QString stagType = "";

    if(action == 1 || action == 2){
        if(code == "")
            code = "NULL";
        else
            code = "\'" + code + "\'";

        if(UUID_Tagged == 0)
            UUID_Tagged = "NULL";
        else
            UUID_Tagged = "\'" + UUID_Tagged + "\'";

        if(taggedType == 0)
            staggedType = "NULL";
        else
            staggedType = QString::number(taggedType);

        if(UUID_Tag == "")
            UUID_Tag = "NULL";
        else
            UUID_Tag = "\'" + UUID_Tag + "\'";

        if(tagType == 0)
            stagType = "NULL";
        else
            stagType = QString::number(tagType);
    }

    if(action == 0){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "delete UUID " << UUID << "from TagsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryDeleteFromTagsTable); // "query/job_query/tags_query/query_delete_from_tagstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "delete UUID " << UUID << "from TagsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }
    if(action == 1){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "update UUID " << UUID << "to TagsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryUpdateToTagsTable); // "query/job_query/tags_query/query_update_to_tagstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(UUID_Tagged).arg(taggedType).arg(UUID_Tag).arg(stagType).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "update UUID " << UUID << "to TagsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }

    }
    if(action == 2){
//        qInfo() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "insert UUID " << UUID << "to TagsTable Table";
        queryString = _myDb->Db_loadquery(gl_prog_QueryInsertToTagsTable); // "query/job_query/tags_query/query_insert_to_tagstable.sql"
        queryString = queryString.arg(gl_Ini_DbName).arg(code).arg(UUID_Tagged).arg(taggedType).arg(UUID_Tag).arg(stagType).arg(UUID);
        queryResultRes = _myDb->Db_execquery(queryString);
        if(!queryResultRes.success){
            qWarning() << JOBHANDLER << "on_sig_ServerTotag_UsersTableResp_slot--> " << "insert UUID " << UUID << "to TagsTable Table error: " <<  queryResultRes.query.lastError().text();
            qInfo() << "";
            qInfo() << "";
        }
    }

    return;
}


void JobHandler::on_Delay_Timeout()
{
    delay_Timer->stop();
    delay = false;

    return;
}

void JobHandler::Delay_ms(quint32 ms)
{
    delay_Timer->start(ms);
    delay = true;
    while(delay){
        QApplication::processEvents(QEventLoop::AllEvents);
    }

    return;
} 

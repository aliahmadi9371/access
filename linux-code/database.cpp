#include "database.h"

Database::Database(QObject *parent) : QObject(parent)
{
    Init();
    Config();
}

void Database::Init()
{
    _Sqldb = nullptr;
}

void Database::Config()
{
    _Sqldb = new QSqlDatabase();
}

void Database::Db_showavail()
{
    QSqlDatabase database;
    for(int i=0; i<(database.drivers()).size(); i++){
        qInfo() << DBCLASS << "Db_showavail--> " << (database.drivers()).at(i);
    }
}

bool Database::Db_open(QString con_name)
{
    return Db_open(gl_Ini_DbDriver, con_name, gl_Ini_DbHostIp, gl_Ini_DbUsername, gl_Ini_DbPassword, gl_Ini_DbName, gl_Ini_DbOption);
}

bool Database::Db_open(QString drv_name, QString con_name, QString host_name, QString user_name, QString pass_word, QString db_name, QString db_option)
{
    bool ret = false;

    *_Sqldb = QSqlDatabase::addDatabase(drv_name, con_name);
    _Sqldb->setHostName(host_name);
    _Sqldb->setUserName(user_name);
    _Sqldb->setPassword(pass_word);
    _Sqldb->setDatabaseName(db_name);

    _Sqldb->setConnectOptions(db_option);
    if(_Sqldb->open()) {
//        #ifdef QT_DEBUG
//            qInfo() << DBCLASS << "Db_open--> " << "can open data base";
//        #endif
        ret = true;
    }
    else{
        qWarning() << DBCLASS << "Db_open--> " << "can not open database ERROR: " << _Sqldb->lastError().text();
        //QApplication::quit();
    }
    return ret;
}

bool Database::Db_close()
{
    if(_Sqldb->isOpen())
        _Sqldb->close();
    return true;
}

QString Database::Db_loadquery(QString sFilename)
{
    QString squery;
    QFile scriptFile(sFilename);
    if (scriptFile.open(QIODevice::ReadOnly)){
        squery = QString(scriptFile.readAll());
        scriptFile.close();
        return squery;
    }
    else{
        return "";
    }
}

queryResult Database::Db_execquery(QString string_query)
{
    queryResult result;
    if(_Sqldb){
        QSqlQuery query(*_Sqldb);
        if(query.exec(string_query)){
//            qInfo() << DBCLASS << "Db_execquery: " << "success query execution";
            result.success = true;
            result.query = query;
            result.size = query.size();
        }
        else{
            QString error_query = query.lastError().text();
//            QString error_db = _Sqldb->lastError().text();
            qWarning() << DBCLASS << "Db_execquery--> " << "can not execute Query ERROR: " << error_query;
//            qWarning() << DBCLASS << "Db_execquery--> " << "can not execute Database ERROR: " << error_db;
//            qWarning() << DBCLASS << "Db_execquery--> " << "query: " << string_query;

            if(!(error_query.contains("Duplicate entry")))
                QApplication::quit();
        }
    }
    else{
        qCritical() << DBCLASS << "Db_execquery--> " << "memory is not allocated!!!";
        QApplication::quit();
    }
    return result;
}

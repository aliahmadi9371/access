#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "defines.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    void Init();
    void Config();

    void Db_showavail();
    bool Db_open(QString con_name);
    bool Db_open(QString drv_name, QString con_name, QString host_name, QString user_name, QString pass_word, QString db_name, QString db_option);
    bool Db_close();

    QString Db_loadquery(QString sFilename);
    queryResult Db_execquery(QString string_query);

    QSqlDatabase* _Sqldb;


signals:

};

#endif // DATABASE_H

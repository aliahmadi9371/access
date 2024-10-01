#ifndef QUERY_H
#define QUERY_H

#include <QObject>

#include "defines.h"
#include "database.h"

class Query : public QObject
{
    Q_OBJECT
public:
    explicit Query(QObject *parent = nullptr);
    void Init();
    void Config();

    bool Query_OpenDb(QString connection_name);
    bool Query_CloseDb();

    void Query_CheckUser(AuthRes &res);
    void _queryResultFindUsertoStruct(queryResult &queryResultRes, AuthRes &res);

    void Query_CheckReader(AuthRes &res);
    void _queryResultFindReadertoStruct(queryResult &queryResultRes, AuthRes &res);

    void Query_CheckHol(AuthRes &res);
    void _queryResultCheckUserHoltoStruct(queryResult &queryResultRes, AuthRes &res);

    void Query_CheckAccesspair(AuthRes &res);
    void _queryResultCheckUserPairtoStruct(queryResult &queryResultRes, AuthRes &res);

    void Query_CheckAccesstime(AuthRes &res);
    void _queryResultCheckUserAccesstoStruct(queryResult &queryResultRes, AuthRes &res);


    Database* _myDb;
    bool dbConnection;

signals:

};

#endif // QUERY_H

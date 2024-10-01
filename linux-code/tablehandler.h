#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include <QObject>
#include "defines.h"
#include "database.h"

class TableHandler : public QObject
{
    Q_OBJECT
public:
    explicit TableHandler(QObject *parent = nullptr);
    void Init();
    void Config();

    bool TableHandler_OpenDb();
    bool TableHandler_CloseDb();

    bool Insert__logsTable(QString userUUID, int userUniqueId, QString readerUUID, int readerId, int readerExpId, QDateTime reqDateTime, bool isHoliday, int errorCode, QString errorString, bool isUser, bool isSuccess);
    bool Insert__inpsTable(int readerExpId, int readerId);


    bool Find_NotSentLog(Log &log);
    bool Find_NotSentInp(Inp &inp);


    bool Insert__logsTableAnd(QString userUUID1, QString userUUID2, int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, QString readerUUID1, QString readerUUID2, bool isHoliday1, bool isHoliday2, int errorCode1, int errorCode2, QString errorString1, QString errorString2, bool isUser1, bool isUser2, int readerExpId, QDateTime reqDateTime, bool isSuccess);
    bool Find_NotSentLogAnd(LogAnd &logand);



    QTimer *logTimer;

    QTimer *deleteTimer;

    Database* _myDb;
    bool dbConnection;
    QString MAC;

signals:
    void StartedWorker();
    void StartWorker();

    void sig_SendToServerLog(Log log);
    void sig_SendToServerInp(Inp inp);
    void sig_SendToServerLogAnd(LogAnd logand);


    void sig_ChangeLogState(int id); //from mqtt

    void sig_StartedTableJob();

public slots:
    void on_StartWorker();

    void on_logTimerTimeout();
    void on_deleteTimerTimeout();

    void on_sig_toLog_slot(AuthRes res);


    void on_sig_InpReq_slot(int readerExpId, int readerId);

    void on_sig_toLogAnd_slot(AuthRes_And res);
    void on_sig_ServerToLogResp_slot(QString UUID);
    void on_sig_ServerToInpResp_slot(QString UUID);
    void on_sig_ServerToLogAndResp_slot(QString UUID);

    void on_StartTableJob();
};

#endif // TABLEHANDLER_H

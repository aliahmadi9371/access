#ifndef ATHENTICATIONCLASS_H
#define ATHENTICATIONCLASS_H

#include <QObject>
#include "defines.h"
#include "query.h"


class AthenticationClass : public QObject
{
    Q_OBJECT
public:
    explicit AthenticationClass(QObject *parent = nullptr);
    void Init();
    void Config();

    void Auth_PrepareReq(int userUniqueId, int readerId, int readerExpId, AuthRes &res);
    void Auth_Res(AuthRes res);

    bool Auth_CheckReader(AuthRes &res);
    bool Auth_CheckUser(AuthRes &res);
    bool Auth_CheckHol(AuthRes &res);
    bool Auth_CheckAccessPair(AuthRes &res);
    bool Auth_CheckAccessTime(AuthRes &res);

    void Auth_Finalize(AuthRes res);


    QTimer* delay_Timer;
    bool delay;
    void Delay_ms(quint32 ms);


    Query* _myQuery;

    QQueue<AuthRes>* queue;
    QList<AuthRes>* pendigList;

    QTimer* dequeueTimer;
    QTimer* pedningListTimer;

    QMutex* mutex;

    QString MAC;

    AuthRes_And AuthRes_And_Auth;
    void init_AuthRes_And_Auth();
    bool Auth_CheckReader_And(AuthRes &res);
    bool Auth_CheckUser_And(AuthRes &res);
    bool Auth_CheckHol_And(AuthRes &res);
    bool Auth_CheckAccessPair_And(AuthRes &res);
    bool Auth_CheckAccessTime_And(AuthRes &res);
    QTimer* _and_Proc_Timer;

//    bool testConnection();
//    QTimer *testDbTimer;

signals:
    void StartWorker();
    void StartedWorker();

    void sig_toLog(AuthRes res);


    void sig_toCy(int readerExpId, int readId, bool success);

    void sig_Auth(AuthRes res);

    void sig_SendToServerApb(Apb apb);
    void sig_SendToServerAuth(Auth auth);

    void sig_toLogAnd(AuthRes_And res);
    void sig_SendToServerApb_And(Apb_And apband);
    void sig_SendToServerAuth_And(Auth_And authand);


    void sig_StartedAuthJob();
public slots:
    void on_StartWorker();

    void on_sig_AuthReq_slot(int userUniqueId, int readerId, int readerExpId);
    void on_sig_AuthReqAnd_slot(int userUniqueId_1, int userUniqueId_2, int readerId_1, int readerId_2, int readerExpId, bool succ);
    void on_Auth_slot(AuthRes res);

    void on_dequeueTimerTimeout_slot();
    void on_pedningListTimerTimeout_slot();

    void on_MqttResp_slot();


    void on_sig_ServerToApbResp_slot(int userUniqueId, int readerExpId, int readerId, QDateTime reqDateTime, bool apb);
    void on_sig_ServerToAuthResp_slot(int userUniqueId, int readerExpId, int readerId, QDateTime reqDateTime, bool isHoliday, int errorCode, QString errorString, bool isSuccess);

    void on_sig_ServerToApbRespAnd_slot(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, int readerExpId,QDateTime reqDateTime, bool apb);
    void on_sig_ServerToAuthRespAnd_slot(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, bool isHoliday1, bool isHoliday2, int errorCode1, int errorCode2, QString errorString1, QString errorString2, int readerExpId, QDateTime reqDateTime, bool isSuccess);

    void on_StartAuthJob();

//    void on_testDbTimerTimeout();

    void on_and_Proc_Timer_Timeout();
    void on_Delay_Timeout();
};

#endif // ATHENTICATIONCLASS_H

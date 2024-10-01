#ifndef MQTTCLASS_H
#define MQTTCLASS_H

#include <QObject>
#include "defines.h"

class MqttClass : public QObject
{
    Q_OBJECT
public:
    explicit MqttClass(QObject *parent = nullptr);
    void Init();
    void Config();
    void MqttConfig();
    void MqttHostConfig();
    void MqttWillConfig();
    void MqttConnect();
    void MqttDisconnect();
    void MqttPub(QString topic, QString msg, quint8 qos);
    void MqttPubLog(int userUniqueId, int readerExpId, int readerId, bool isHoliday, bool isUser, int errorCode, QString errorString, QString reqDateTime, QString UUID, bool isSuccess);
    void MqttPubInp(int readerExpId, int readerId, QString reqDateTime, QString UUID);
    void MqttPubApb(int userUniqueId, int readerExpId, int readerId, QString reqDateTime);
    void MqttPubAuth(int userUniqueId, int readerExpId, int readerId, QString reqDateTime);
    void MqttPubStartSopSync(int startStop);
    void MqttPubTimezonesTableSync(QString code, QString startTime, QString stopTime, QString UUID);
    void MqttPubAccessTimezonesTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID06, QString UUID07, QString UUID08, QString UUID09, QString UUID10, QString UUID11, QString UUID12, QString UUID);
    void MqttPubAccessShiftsTableSync(QString code, int type, QString UUIDSat, QString UUIDSun, QString UUIDMon, QString UUIDTue, QString UUIDWed, QString UUIDThu, QString UUIDFri, QString UUIDHol, QString UUID);
    void MqttPubtag_UsersTableSync(int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);
    void MqttPubtag_TerminalsTableSync(int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);

    //-------------------------
    void MqttPubHolidaysTableSync(QString code, QString date, QString UUID);
    void MqttPubHolidaysSetsTableSync(QString code,
                                      QString UUID01,
                                      QString UUID02,
                                      QString UUID03,
                                      QString UUID04,
                                      QString UUID05,
                                      QString UUID06,
                                      QString UUID07,
                                      QString UUID08,
                                      QString UUID09,
                                      QString UUID10,
                                      QString UUID11,
                                      QString UUID12,
                                      QString UUID13,
                                      QString UUID14,
                                      QString UUID15,
                                      QString UUID16,
                                      QString UUID17,
                                      QString UUID18,
                                      QString UUID19,
                                      QString UUID20,
                                      QString UUID21,
                                      QString UUID22,
                                      QString UUID23,
                                      QString UUID24,
                                      QString UUID25,
                                      QString UUID26,
                                      QString UUID27,
                                      QString UUID28,
                                      QString UUID29,
                                      QString UUID30,
                                      QString UUID31,
                                      QString UUID32,
                                      QString UUID
                                     );
    void MqttPubHolidaysGroupsTableSync(QString code,
                                        QString UUID01,
                                        QString UUID02,
                                        QString UUID03,
                                        QString UUID04,
                                        QString UUID05,
                                        QString UUID
                                       );
    //-------------------------

    void MqttPubtag_HolidaysTableSync(QString code, int type, QString UUIDHolGroups, QString UUID);
    void MqttPubtag_AccessPairsTableSync(QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID);
    void MqttPub_tagsTableSync(QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID);

    bool connecting;

    bool MqttSubLogResp();
    void MqttUnSubLogResp();
    bool bSubLogResp;
    QMqttSubscription* subLogResp;

    bool MqttSubInpResp();
    void MqttUnSubInpResp();
    bool bSubInpResp;
    QMqttSubscription* subInpResp;

    bool MqttSubApbResp();
    void MqttUnSubApbResp();
    bool bSubApbResp;
    QMqttSubscription* subApbResp;

    bool MqttSubAuthResp();
    void MqttUnSubAuthResp();
    bool bSubAuthResp;
    QMqttSubscription* subAuthResp;

    bool MqttSubStartStopSync();
    void MqttUnSubStartStopSync();
    bool bSubStartStopSyncResp;
    QMqttSubscription* subStartStopSyncResp;

    bool MqttSubTimezonesTableSync();
    void MqttUnSubTimezonesTableSync();
    bool bSubTimezonesTableSyncResp;
    QMqttSubscription* subTimezonesTableSyncResp;

    bool MqttSubAccessTimezonesTableSync();
    void MqttUnSubAccessTimezonesTableSync();
    bool bSubAccessTimezonesTableSyncResp;
    QMqttSubscription* subAccessTimezonesTableSyncResp;

    bool MqttSubAccessShiftsTableSync();
    void MqttUnSubAccessShiftsTableSync();
    bool bSubAccessShiftsTableSyncResp;
    QMqttSubscription* subAccessShiftsTableSyncResp;

    bool MqttSubtag_UsersTableSync();
    void MqttUnSubtag_UsersTableSync();
    bool bSubtag_UsersTableSyncResp;
    QMqttSubscription* subtag_UsersTableSyncResp;

    bool MqttSubtag_TerminalsTableSync();
    void MqttUnSubtag_TerminalsTableSync();
    bool bSubtag_TerminalsTableSyncResp;
    QMqttSubscription* subtag_TerminalsTableSyncResp;

    //------------------
    bool MqttSubHolidaysTableSync();
    void MqttUnSubHolidaysTableSync();
    bool bSubHolidaysTableSyncResp;
    QMqttSubscription* subHolidaysTableSyncResp;

    bool MqttSubHolidaysSetsTableSync();
    void MqttUnSubHolidaysSetsTableSync();
    bool bSubHolidaysSetsTableSyncResp;
    QMqttSubscription* subHolidaysSetsTableSyncResp;

    bool MqttSubHolidaysGroupsTableSync();
    void MqttUnSubHolidaysGroupsTableSync();
    bool bSubHolidaysGroupsTableSyncResp;
    QMqttSubscription* subHolidaysGroupsTableSyncResp;
    //------------------

    bool MqttSubtag_HolidaysTableSync();
    void MqttUnSubtag_HolidaysTableSync();
    bool bSubtag_HolidaysTableSyncResp;
    QMqttSubscription* subtag_HolidaysTableSyncResp;

    bool MqttSubtag_AccessPairsTableSync();
    void MqttUnSubtag_AccessPairsTableSync();
    bool bSubtag_AccessPairsTableSyncResp;
    QMqttSubscription* subtag_AccessPairsTableSyncResp;

    bool MqttSubtagsTableSync();
    void MqttUnSubtagsTableSync();
    bool bSubtagsTableSyncResp;
    QMqttSubscription* subtagsTableSyncResp;

    int mqtt_status;
    int mqtt_status_old;


    QMqttClient* client;

    QTimer* checkServerTimer;

    QTimer* checker_mqtt_connecting;

    QMqttClient::ClientState serverState;

    QTimer *statusTimer;

    bool ping_proc;
    int ping_count;
    bool first;
    bool first_status;
    int connect_disconnect_counter;

    void mqtt_unsubscribe();
    void mqtt_subscribe();

    bool mqtt_check_subscribe();
    void mqtt_check_subscribe_status();

    bool mqtt_check_connectivity();

    QString userName;

    QTimer* delay_Timer;
    bool delay;
    void Delay_ms(quint32 ms);


    void MqttPubAuthAnd(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, int readerExpId, QString reqDateTime);
    bool MqttSubAuthAndResp();
    void MqttUnSubAuthAndResp();
    bool bSubAuthAndResp;
    QMqttSubscription* subAuthAndResp;

    void MqttPubApbAnd(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, int readerExpId, QString reqDateTime);
    bool MqttSubApbAndResp();
    void MqttUnSubApbAndResp();
    bool bSubApbAndResp;
    QMqttSubscription* subApbAndResp;


    void MqttPubLogAnd(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, bool isHoliday1, bool isHoliday2, bool isUser1, bool isUser2, int errorCode1, int errorCode2, QString errorString1, QString errorString2, int readerExpId, QString reqDateTime, QString UUID, bool isSuccess);
    bool MqttSubLogAndResp();
    void MqttUnSubLogAndResp();
    bool bSubLogAndResp;
    QMqttSubscription* subLogAndResp;


    bool MqttSubLock();
    void MqttUnSubLock();
    bool bSubLock;
    QMqttSubscription* subLock;

signals:
    void StartWorker();
    void StartedWorker();

    void toTable();
    void toCy();

    void sig_ServerToLogResp(QString UUID);
    void sig_ServerToInpResp(QString UUID);

    void sig_ServerToApbResp(int userUniqueId, int readerExpId, int readerId, QDateTime reqDateTime, bool apb);
    void sig_ServerToAuthResp(int userUniqueId, int readerExpId, int readerId, QDateTime reqDateTime, bool isHoliday, int errorCode, QString errorString, bool isSuccess);

    void sig_ServerToStartStopResp(bool startStop);
    void sig_ServerToTimezonesTableResp(int action, QString code, QString startTime, QString stopTime, QString UUID);
    void sig_ServerToAccessTimezonesTableResp(int action, QString code, QString uuid01, QString uuid02, QString uuid03, QString uuid04, QString uuid05, QString uuid06, QString uuid07, QString uuid08, QString uuid09, QString uuid10, QString uuid11, QString uuid12, QString UUID);
    void sig_ServerToAccessShiftsTableResp(int action, QString code, int type, QString uuidSat, QString uuidSun, QString uuidMon, QString uuidTue, QString uuidWed, QString uuidThu, QString uuidFri, QString uuidHol, QString UUID);
    void sig_ServerTotag_UsersTableResp(int action, int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);
    void sig_ServerTotag_TerminalsTableResp(int action, int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);

    //---------------------
    void sig_ServerToHolidaysTableResp(int action, QString code, QString date, QString UUID);
    void sig_ServerToHolidaysSetsTableResp(int action,
                                           QString code,
                                           QString uuid01,
                                           QString uuid02,
                                           QString uuid03,
                                           QString uuid04,
                                           QString uuid05,
                                           QString uuid06,
                                           QString uuid07,
                                           QString uuid08,
                                           QString uuid09,
                                           QString uuid10,
                                           QString uuid11,
                                           QString uuid12,
                                           QString uuid13,
                                           QString uuid14,
                                           QString uuid15,
                                           QString uuid16,
                                           QString uuid17,
                                           QString uuid18,
                                           QString uuid19,
                                           QString uuid20,
                                           QString uuid21,
                                           QString uuid22,
                                           QString uuid23,
                                           QString uuid24,
                                           QString uuid25,
                                           QString uuid26,
                                           QString uuid27,
                                           QString uuid28,
                                           QString uuid29,
                                           QString uuid30,
                                           QString uuid31,
                                           QString uuid32,
                                           QString UUID
                                          );
    void sig_ServerToHolidaysGroupsTableResp(int action,
                                             QString code,
                                             QString uuid01,
                                             QString uuid02,
                                             QString uuid03,
                                             QString uuid04,
                                             QString uuid05,
                                             QString UUID
                                            );
    //---------------------

    void sig_ServerTotag_HolidaysTableResp(int action, QString code, int type, QString date, QString UUID);
    void sig_ServerTotag_AccessPairsTableResp(int action, QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID);
    void sig_ServerTotagsTableResp(int action, QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID);

    void sig_StartedMqttJob();

    void sig_CyMqttStatus(int status);



    //---------------------
    //---------------------
    //---------------------
    void sig_ServerToAuthAndResp(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, bool isHoliday1, bool isHoliday2, int errorCode1, int errorCode2, QString errorString1, QString errorString2, int readerExpId, QDateTime reqDateTime, bool isSuccess);
    void sig_ServerToApbAndResp(int userUniqueId1, int userUniqueId2, int readerId1, int readerId2, int readerExpId, QDateTime reqDateTime, bool apb);
    void sig_ServerToLogAndResp(QString UUID);
    //---------------------
    //---------------------
    //---------------------


    void sig_ServerToCypressLock(quint8 port);

public slots:
    void on_StartWorker();

    void on_checkServerTimerTimeout();
    void on_stateChanged_client(QMqttClient::ClientState state);
    void on_statusTimerTimeout();

    void on_messageReceivedLogResp(QMqttMessage mqttMsg);
    void on_stateChangedLogResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedLogResp(quint8 qos);


    void on_messageReceivedInpResp(QMqttMessage mqttMsg);
    void on_stateChangedInpResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedInpResp(quint8 qos);


    void on_messageReceivedApbResp(QMqttMessage mqttMsg);
    void on_stateChangedApbResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedApbResp(quint8 qos);


    void on_messageReceivedAuthResp(QMqttMessage mqttMsg);
    void on_stateChangedAuthResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedAuthResp(quint8 qos);

    void on_messageReceivedStartStopResp(QMqttMessage mqttMsg);
    void on_stateChangedStartStopResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedStartStopResp(quint8 qos);

    void on_messageReceivedTimezonesTableResp(QMqttMessage mqttMsg);
    void on_stateChangedTimezonesTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedTimezonesTableResp(quint8 qos);

    void on_messageReceivedAccessTimezonesTableResp(QMqttMessage mqttMsg);
    void on_stateChangedAccessTimezonesTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedAccessTimezonesTableResp(quint8 qos);

    void on_messageReceivedAccessShiftsTableResp(QMqttMessage mqttMsg);
    void on_stateChangedAccessShiftsTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedAccessShiftsTableResp(quint8 qos);

    void on_messageReceivedtag_UsersTableResp(QMqttMessage mqttMsg);
    void on_stateChangedtag_UsersTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedtag_UsersTableResp(quint8 qos);

    void on_messageReceivedtag_TerminalsTableResp(QMqttMessage mqttMsg);
    void on_stateChangedtag_TerminalsTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedTerminalsTableResp(quint8 qos);

    //---------------------
    void on_messageReceivedHolidaysTableResp(QMqttMessage mqttMsg);
    void on_stateChangedHolidaysTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedHolidaysTableResp(quint8 qos);

    void on_messageReceivedHolidaysSetsTableResp(QMqttMessage mqttMsg);
    void on_stateChangedHolidaysSetsTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedHolidaysSetsTableResp(quint8 qos);

    void on_messageReceivedHolidaysGroupsTableResp(QMqttMessage mqttMsg);
    void on_stateChangedHolidaysGroupsTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedHolidaysGroupsTableResp(quint8 qos);
    //---------------------

    void on_messageReceivedtag_HolidaysTableResp(QMqttMessage mqttMsg);
    void on_stateChangedtag_HolidaysTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedtag_HolidaysTableResp(quint8 qos);

    void on_messageReceivedtag_AccessPairsTableResp(QMqttMessage mqttMsg);
    void on_stateChangedtag_AccessPairsTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedtag_AccessPairsTableResp(quint8 qos);

    void on_messageReceivedtagsTableResp(QMqttMessage mqttMsg);
    void on_stateChangedtagsTableResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedtagsTableResp(quint8 qos);


    void on_sig_SendToServerLog_slot(Log log);
    void on_sig_SendToServerInp_slot(Inp inp);

    void on_sig_SendToServerApb_slot(Apb apb);
    void on_sig_SendToServerAuth_slot(Auth auth);

    void on_sig_StartStopSyncDatabase(int startStop);
    void on_sig_TimezonesTableSync(QString code, QString startTime, QString stopTime, QString UUID);
    void on_sig_AccessTimezonesTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID06, QString UUID07, QString UUID08, QString UUID09, QString UUID10, QString UUID11, QString UUID12, QString UUID);
    void on_sig_AccessShiftsTableSync(QString code, int type, QString UUIDSat, QString UUIDSun, QString UUIDMon, QString UUIDTue, QString UUIDWed, QString UUIDThu, QString UUIDFri, QString UUIDHol, QString UUID);
    void on_sig_tag_UsersTableSync(int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);
    void on_sig_tag_TerminalsTableSync(int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);

    //----------------------------
    void on_sig_HolidaysTableSync(QString code, QString date, QString UUID);
    void on_sig_HolidaysSetsTableSync(QString code,
                                      QString UUID01,
                                      QString UUID02,
                                      QString UUID03,
                                      QString UUID04,
                                      QString UUID05,
                                      QString UUID06,
                                      QString UUID07,
                                      QString UUID08,
                                      QString UUID09,
                                      QString UUID10,
                                      QString UUID11,
                                      QString UUID12,
                                      QString UUID13,
                                      QString UUID14,
                                      QString UUID15,
                                      QString UUID16,
                                      QString UUID17,
                                      QString UUID18,
                                      QString UUID19,
                                      QString UUID20,
                                      QString UUID21,
                                      QString UUID22,
                                      QString UUID23,
                                      QString UUID24,
                                      QString UUID25,
                                      QString UUID26,
                                      QString UUID27,
                                      QString UUID28,
                                      QString UUID29,
                                      QString UUID30,
                                      QString UUID31,
                                      QString UUID32,
                                      QString UUID
                                     );
    void on_sig_HolidaysGroupsTableSync(QString code,
                                        QString UUID01,
                                        QString UUID02,
                                        QString UUID03,
                                        QString UUID04,
                                        QString UUID05,
                                        QString UUID
                                       );
    //----------------------------

    void on_sig_tag_HolidaysTableSync(QString code, int type, QString UUIDHolGroups, QString UUID);
    void on_sig_tag_AccessPairsTableSync(QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID);
    void on_sig_tagsTableSync(QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID);

    void on_StartMqttJob();

    void on_pingResponseReceived();
    void on_checker_mqtt_connecting();


    void on_sig_SendToServerAuthAnd_Slot(Auth_And authand);
    void on_messageReceivedAuthAndResp(QMqttMessage mqttMsg);
    void on_stateChangedAuthAndResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedAuthAndResp(quint8 qos);

    void on_sig_SendToServerApbAnd_slot(Apb_And apband);
    void on_messageReceivedApbAndResp(QMqttMessage mqttMsg);
    void on_stateChangedApbAndResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedApbAndResp(quint8 qos);


    void on_sig_SendToServerLogAnd_slot(LogAnd logand);
    void on_messageReceivedLogAndResp(QMqttMessage mqttMsg);
    void on_stateChangedLogAndResp(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedLogAndResp(quint8 qos);

//------------------------------------------------------
    void on_sig_InpsPub_slot(QString inputs_str);
//------------------------------------------------------

    void on_sig_SendToServerLock_slot(quint8 port, bool ok);
    void on_messageReceivedLock(QMqttMessage mqttMsg);
    void on_stateChangedLock(QMqttSubscription::SubscriptionState subState);
//    void on_qosChangedLLock(quint8 qos);


    void on_Delay_Timeout();
};

#endif // MQTTCLASS_H

#ifndef JOBHANDLER_H
#define JOBHANDLER_H

#include <QObject>
#include "defines.h"
#include "database.h"

class JobHandler : public QObject
{
    Q_OBJECT
public:
    explicit JobHandler(QObject *parent = nullptr);

    void Init();
    void Config();

    bool Query_OpenDb(QString connection_name);
    bool Query_CloseDb();


    void Sync_TimezonesTable();
    void Sync_AccessTimezonesTable();
    void Sync_AccessShiftsTable();
    void Sync_Tag_UsersTable();
    void Sync_Tag_TerminalsTable();
    void Sync_HolidaysTable();
    void Sync_HolidaysSetsTable();
    void Sync_HolidaysGroupsTable();
    void Sync_Tag_HolidaysTable();
    void Sync_Tag_AccessPairsTable();
    void Sync_TagsTable();



    Database* _myDb;
    bool dbConnection;

    QTimer* checkTimer;

    void Delay_ms(quint32 ms);
    QTimer* delay_Timer;
    bool delay;

    bool in_sync;
    bool server_start_sync;
    int count_server_unsync;

    bool testConnection();
    QString MAC;

signals:
    void StartWorker();
    void StartedWorker();

    void sig_StartSync(int startStop);
    void sig_TimezonesTableSync(QString code, QString startTime, QString stopTime, QString UUID);
    void sig_AccessTimezonesTableSync(QString code, QString UUID01, QString UUID02, QString UUID03, QString UUID04, QString UUID05, QString UUID06, QString UUID07, QString UUID08, QString UUID09, QString UUID10, QString UUID11, QString UUID12, QString UUID);
    void sig_AccessShiftsTableSync(QString code, int type, QString UUIDSat, QString UUIDSun, QString UUIDMon, QString UUIDTue, QString UUIDWed, QString UUIDThu, QString UUIDFri, QString UUIDHol, QString UUID);
    void sig_Tag_UsersTableSync(int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);
    void sig_Tag_TerminalsTableSync(int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);

    void sig_HolidaysTableSync(QString code, QString date, QString UUID);
    void sig_HolidaysSetsTableSync(
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
    void sig_HolidaysGroupsTableSync(
                                     QString code,
                                     QString uuid01,
                                     QString uuid02,
                                     QString uuid03,
                                     QString uuid04,
                                     QString uuid05,
                                     QString UUID
                                    );
    void sig_Tag_HolidaysTableSync(QString code, int type, QString uuid, QString UUID);

    void sig_Tag_AccessPairsTableSync(QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID);
    void sig_TagsTableSync(QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID);

    void sig_StartedJobJob();

    //void sig_SendTimezonesTableRowsToMqtt(int id, QString code, );
public slots:
    void on_StartWorker();

    void on_checkTimerTimeout_slot();

    void on_StartJobJob();

    void on_sig_ServerToStartStopResp_slot(bool startStop);
    void on_sig_ServerToTimezonesTableResp_slot(int action, QString code, QString startTime, QString stopTime, QString UUID);
    void on_sig_ServerToAccessTimezonesTableResp_slot(int action, QString code, QString uuid01, QString uuid02, QString uuid03, QString uuid04, QString uuid05, QString uuid06, QString uuid07, QString uuid08, QString uuid09, QString uuid10, QString uuid11, QString uuid12, QString UUID);
    void on_sig_ServerToAccessShiftsTableResp_slot(int action, QString code, int type, QString uuidSat, QString uuidSun, QString uuidMon, QString uuidTue, QString uuidWed, QString uuidThu, QString uuidFri, QString uuidHol, QString UUID);
    void on_sig_ServerTotag_UsersTableResp_slot(int action, int userUniqueId, int type, bool active, QString creditStartDateTime, QString creditStopDateTime, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);
    void on_sig_ServerTotag_TerminalsTableResp_slot(int action, int readerId, int type, int readerExpId, bool active, bool deactiveByDateTime, QString deactiveStartDateTime, QString deactiveStopDateTime, QString UUID);
    void on_sig_ServerToHolidaysTableResp_slot(int action, QString code, QString date, QString UUID);
    void on_sig_ServerToHolidaysSetsTableResp_slot(int action, QString code, QString uuid01, QString uuid02, QString uuid03, QString uuid04, QString uuid05, QString uuid06, QString uuid07, QString uuid08, QString uuid09, QString uuid10, QString uuid11, QString uuid12, QString uuid13, QString uuid14, QString uuid15, QString uuid16, QString uuid17, QString uuid18, QString uuid19, QString uuid20, QString uuid21, QString uuid22, QString uuid23, QString uuid24, QString uuid25, QString uuid26, QString uuid27, QString uuid28, QString uuid29, QString uuid30, QString uuid31, QString uuid32, QString UUID);
    void on_sig_ServerToHolidaysGroupsTableResp_slot(int action, QString code, QString uuid01, QString uuid02, QString uuid03, QString uuid04, QString uuid05, QString UUID);
    void on_sig_ServerTotag_HolidaysTableResp_slot(int action, QString code, int type, QString uuid, QString UUID);
    void on_sig_ServerTotag_AccessPairsTableResp_slot(int action, QString code, int type, QString UUID_Terminals, QString UUID_Shifts, QString UUID);
    void on_sig_ServerTotagsTableResp_slot(int action, QString code, QString UUID_Tagged, int taggedType, QString UUID_Tag, int tagType, QString UUID);

    void on_Delay_Timeout();
};

#endif // JOBHANDLER_H

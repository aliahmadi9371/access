#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include "defines.h"

#include "i2cmasterclass.h"
#include "cypress_serial.h"
#include "athenticationclass.h"
#include "tablehandler.h"
#include "mqttclass.h"
#include "jobhandler.h"

class MainClass : public QObject
{
    Q_OBJECT
public:
    explicit MainClass(QObject *parent = nullptr);
    ~MainClass();
    void Init();
    void Config();

    void StartApplication();
    void StartCy();
    void StartAuth();
    void StartTable();
    void StartMqtt();
    void StartJob();

    //this tread
    void GetInfoFromIni();
    void GetNetworkFromSys();
    void CheckHardware();
    void GetInfoFromRtc();

    void Reboot();

    I2cmasterClass* mainclass_i2c;

    QThread* mainclass_cy_thread;
    Cypress_Serial* mainclass_cy_worker;

    QThread* mainclass_auth_thread;
    AthenticationClass* maincalss_auth_worker;

    QThread* mainclass_handler_thread;
    TableHandler* maincalss_handler_worker;

    QThread* mainclass_mqtt_thread;
    MqttClass* maincalss_mqtt_worker;

    QThread* mainclass_job_thread;
    JobHandler* maincalss_job_worker;


    QTimer* ntp_Timer;
    bool ntp;
    QTimer* rtc_Timer;
    QTimer* watch_Timer;

    bool ntp_synced;


//    QTimer test;

signals:
    void StartWorker();
    void StartedWorker();
    void StartMqttTransmition();
    void StopMqttTransmition();

    void testsig(int userUniqueId, int readerId, int readerExpId);



    
    
    void sig_StartCyThreadJob();
    void sig_StartAuthThreadJob();
    void sig_StartTableThreadJob();
    void sig_startJobThreadJob();
    void sig_startMqttThreadJob();

private slots:
    void on_StartWorker();
    void on_StartedWorker();

    void on_StartedCyThread();
    void on_StartedAuthWorker();
    void on_StartedTableWorker();
    void on_StartedMqtt();
    void on_StartedJob();


    //event of this thread
    void on_ntpTimerTimeout();
    void on_rtcTimerTimeout();
    void on_watchTimerTimeout();

    void on_sig_StartedCyJob();
    void on_sig_StartedAuthJob();
    void on_sig_StartedTableJob();
    void on_sig_StartedJobJob();
    void on_sig_StartedMqttJob();

};

#endif // MAINCLASS_H

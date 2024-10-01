#ifndef CYPRESS_SERIAL_H
#define CYPRESS_SERIAL_H

#include <QObject>
#include "defines.h"
#include "serial.h"

//26 bits format
//bit[0]             --> Leading Parity Bit (Even)(over bits 1 to 12)       -- 1 bit
//bit[1 ... 8]       --> Site Code                                          -- 8 bits  (MSB)
//bit[9 ... 24]      --> Unique Id                                          -- 16 bits (MSB)
//bit[25]            --> Trailing Parity Bit (Odd)(over bits 13 to 24)      -- 1 bit

//39 bits format
//bit[0]             --> Leading Parity Bit (Even)(over bits 1 to 21)       -- 1 bit
//bit[1 ... 8]       --> OEM                                                -- 8 bits  (MSB)
//bit[9 ... 20]      --> Site Code                                          -- 8 bits  (MSB)
//bit[21 ... 37]     --> Unique Id                                          -- 16 bits (MSB)
//bit[38]            --> Trailing Parity Bit (Odd)(over bits 21 to 38)      -- 1 bit

class Cypress_Serial : public QObject
{
    Q_OBJECT
public:
    explicit Cypress_Serial(QObject *parent = nullptr);
    void Init();
    void Config();

    void Delay_ms(quint32 ms);

    QByteArray CreateMsg(QByteArray msg, unsigned char kind, unsigned char error_code);
    void Init_Ports();

    void dequeue();
    void enqueue(QString msg);
    QString peek();

    int size();
    bool isEmpty();
    bool isFull();
    void initQ();



    QString *arr; // array to store queue elements
    QString mainItem;

    int capacity;	// maximum capacity of the queue
    int front;		// front points to front element in the queue (if any)
    int rear;		// rear points to last element in the queue
    int count;		// current size of the queue

    Serial* _serial;
    QTimer* _timeoutTimer;
    QTimer* _queueChecker;
    QString tmp_msg;

    QMutex* mutex;   

    bool start;

    bool Port1_Lock;
    bool Port1_Lock_Old;
    bool Port1_Relay_on_off;
    bool Port1_Relay_on_off_Old;
    int Port1_Wbits;
    int Port1_Wbits_Old;
    int Port1_Timeout_Relay_Ms = 0;
    int Port1_Timeout_Proc_Ms = 0;
    QTimer* Port1_Timer_Relay_on;
    QTimer* Port1_Timeout_Proc;

    bool Port2_Lock;
    bool Port2_Lock_Old;
    bool Port2_Relay_on_off;
    bool Port2_Relay_on_off_Old;
    int Port2_Wbits;
    int Port2_Wbits_Old;
    int Port2_Timeout_Relay_Ms = 0;
    int Port2_Timeout_Proc_Ms = 0;
    QTimer* Port2_Timer_Relay_on;
    QTimer* Port2_Timeout_Proc;

    QTimer* JobTimer;

    QTimer* delay_Timer;
    bool delay;

    QTimer* and_Timer;
    bool in_And;
    struct Cy_AuthReq_And AuthReqAnd;


    quint32 read_port1;
    quint32 read_port2;
    quint32 read_total;


    bool locked_out_1;
    bool locked_out_2;
    
    int RGB_color;
    int RGB_color_Old;

    QString inputs;
    QString inputs_old;


    //QTimer* CheckTimer;
    //bool check;

    QTimer* ResetTimer;
    bool reset;



    QTimer* ResetAllTimer;
    bool quit;
    
    void Cy_Connect();
    void Cy_Reset();
    void Port_Wbits(bool port);
    void Port_Output(bool port);
    void Port_Lock(bool port);
    void Port_Input(bool port);
    void Port_Read(bool port);
    void Port_Inputs();
    void Port_Color();
    void Port_Reset(bool port);
    void Port_Reset_Proc(bool port);
    AuthReq Port_Proc_Weigand(bool port, QString weigand);
    void Port_Proc_WeigandAnd(bool port, QString weigand);
    InpReq Port_Proc_Input(bool port);
    Weigand Port_Proc_w(int numberBitsFormat, QString weigand);
signals:
    void StartWorker();
    void StartedWorker();

    void sig_InpReq(int readerExpId, int readerId);
    void sig_AuthReq(int userUniqueId, int readerId, int readerExpId);
    void sig_AuthReqAnd(int userUniqueId_1, int userUniqueId_2, int readerId_1, int readerId_2, int readerExpId, bool succ);

    void sig_StartedCyJob();

    void sig_InpsPub(QString inputs_str);
    void sig_SendToServerLock(quint8 port, bool ok);
    
public slots:
    void on_StartWorker();
    void on_CheckBuffer();
    void on_Timeout();
    void on_CheckerTimeout();

    void on_Port1_Timeout();
    void on_Port2_Timeout();

    void on_Port1_Proc_Timeout();
    void on_Port2_Proc_Timeout();

    void on_Job_Timeout();
    void on_Delay_Timeout();

    void on_StartCyJob();


    void on_AuthRes(int readerExpId, int readerId, bool success);

    void on_sig_CyMqttStatus(int status);

    void on_and_Timer_Timeout();


    void sig_ServerToCypressLock_slot(quint8 port);

    //void on_Check_Timer_Timeout();
    void on_Reset_Timer_Timeout();
    void on_ResetAll_Timer_Timeout();
};

#endif // CYPRESS_SERIAL_H

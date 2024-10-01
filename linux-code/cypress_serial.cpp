#include "cypress_serial.h"

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
Cypress_Serial::Cypress_Serial(QObject *parent) : QObject(parent)
{
    Init();
}

void Cypress_Serial::Init()
{
    _serial = nullptr;
    _timeoutTimer = nullptr;

    arr = nullptr;
    mutex = nullptr;

    start = false;

    Port1_Lock = false;
    Port1_Lock_Old = false;
    Port1_Relay_on_off = false;
    Port1_Relay_on_off_Old = false;
    Port1_Wbits = 0;
    Port1_Wbits_Old = 0;
    Port1_Timeout_Relay_Ms = 0;
    Port1_Timeout_Proc_Ms = 0;
    Port1_Timer_Relay_on = nullptr;
    Port1_Timeout_Proc = nullptr;

    Port2_Lock = false;
    Port2_Lock_Old = false;
    Port2_Relay_on_off = false;
    Port2_Relay_on_off_Old = false;
    Port2_Wbits = 0;
    Port2_Wbits_Old = 0;
    Port2_Timeout_Relay_Ms = 0;
    Port2_Timeout_Proc_Ms = 0;
    Port2_Timer_Relay_on = nullptr;
    Port2_Timeout_Proc = nullptr;


    read_port1 = 0;
    read_port2 = 0;
    read_total = 0;

    JobTimer = nullptr;

    delay_Timer = nullptr;
    delay = false;

    and_Timer = nullptr;
    in_And = false;

    inputs = "";
    inputs_old = "";

    locked_out_1 = false;
    locked_out_2 = false;

    mainItem = "";

    //CheckTimer = nullptr;
    //check = false;

    ResetTimer = nullptr;
    reset = false;

    ResetAllTimer = nullptr;
    quit = false;

    return;
}

void Cypress_Serial::Config()
{
    bCyThreadRun = false;
    bCyThreadJobRun = false;

    arr = new QString[CY_MSG_SIZE];
    capacity = CY_MSG_SIZE;
    initQ();
    mutex = new QMutex(QMutex::NonRecursive);


    _serial = new Serial(this);

    _timeoutTimer = new QTimer(this);
    _timeoutTimer->setSingleShot(true);
    _timeoutTimer->stop();
    _timeoutTimer->setInterval(CY_TIMEOUT_MS);

    _queueChecker = new QTimer(this);
    _queueChecker->setSingleShot(true);
    _queueChecker->setInterval(CY_CHECK_MS);


    and_Timer = new QTimer(this);
    and_Timer->setSingleShot(true);
    and_Timer->stop();
    and_Timer->setInterval(gl_ini_AndGetTimeout);
    QObject::connect(and_Timer, &QTimer::timeout, this, &Cypress_Serial::on_and_Timer_Timeout, Qt::QueuedConnection);

    //CheckTimer = new QTimer(this);
    //CheckTimer->setSingleShot(true);
    //CheckTimer->stop();
    //CheckTimer->setInterval(60 * 1000);
    //QObject::connect(CheckTimer, &QTimer::timeout, this, &Cypress_Serial::on_Check_Timer_Timeout, Qt::QueuedConnection);


    ResetTimer = new QTimer(this);
    ResetTimer->setSingleShot(true);
    ResetTimer->stop();
    ResetTimer->setInterval(10 * 60 * 1000);
    QObject::connect(ResetTimer, &QTimer::timeout, this, &Cypress_Serial::on_Reset_Timer_Timeout, Qt::QueuedConnection);

    ResetAllTimer = new QTimer(this);
    ResetAllTimer->setSingleShot(true);
    ResetAllTimer->stop();
    ResetAllTimer->setInterval(60 * 60 * 1000);
    QObject::connect(ResetAllTimer, &QTimer::timeout, this, &Cypress_Serial::on_ResetAll_Timer_Timeout, Qt::QueuedConnection);

    Port1_Lock = false;
    Port1_Lock_Old = false;
    Port1_Relay_on_off = false;
    Port1_Relay_on_off_Old = false;
    Port1_Wbits = gl_Ini_Port1Bits;
    Port1_Wbits_Old = gl_Ini_Port1Bits;
    Port1_Timeout_Relay_Ms = gl_Ini_Port1Timeout;
    Port1_Timer_Relay_on = new QTimer(this);
    Port1_Timer_Relay_on->setSingleShot(true);
    Port1_Timer_Relay_on->setInterval(Port1_Timeout_Relay_Ms);
    Port1_Timer_Relay_on->stop();
    QObject::connect(Port1_Timer_Relay_on, &QTimer::timeout, this, &Cypress_Serial::on_Port1_Timeout, Qt::QueuedConnection);
    Port1_Timeout_Proc_Ms = gl_Ini_Port1ProcTimeout;
    Port1_Timeout_Proc = new QTimer(this);
    Port1_Timeout_Proc->setSingleShot(true);
    Port1_Timeout_Proc->setInterval(Port1_Timeout_Proc_Ms);
    Port1_Timeout_Proc->stop();
    QObject::connect(Port1_Timeout_Proc, &QTimer::timeout, this, &Cypress_Serial::on_Port1_Proc_Timeout, Qt::QueuedConnection);


    Port2_Lock = false;
    Port2_Lock_Old = false;
    Port2_Relay_on_off = false;
    Port2_Relay_on_off_Old = false;
    Port2_Wbits = gl_Ini_Port2Bits;
    Port2_Wbits_Old = gl_Ini_Port2Bits;
    Port2_Timeout_Relay_Ms = gl_Ini_Port2Timeout;
    Port2_Timer_Relay_on = new QTimer(this);
    Port2_Timer_Relay_on->setSingleShot(true);
    Port2_Timer_Relay_on->setInterval(Port2_Timeout_Relay_Ms);
    Port2_Timer_Relay_on->stop();
    QObject::connect(Port2_Timer_Relay_on, &QTimer::timeout, this, &Cypress_Serial::on_Port2_Timeout, Qt::QueuedConnection);
    Port2_Timeout_Proc_Ms = gl_Ini_Port2ProcTimeout;
    Port2_Timeout_Proc = new QTimer(this);
    Port2_Timeout_Proc->setSingleShot(true);
    Port2_Timeout_Proc->setInterval(Port2_Timeout_Proc_Ms);
    Port2_Timeout_Proc->stop();
    QObject::connect(Port2_Timeout_Proc, &QTimer::timeout, this, &Cypress_Serial::on_Port2_Proc_Timeout, Qt::QueuedConnection);


    JobTimer = new QTimer(this);
    JobTimer->setSingleShot(true);
    JobTimer->setInterval(30);
    JobTimer->stop();
    QObject::connect(JobTimer, &QTimer::timeout, this, &Cypress_Serial::on_Job_Timeout, Qt::QueuedConnection);

    delay_Timer = new QTimer(this);
    delay_Timer->setSingleShot(true);
    delay_Timer->stop();
    QObject::connect(delay_Timer, &QTimer::timeout, this, &Cypress_Serial::on_Delay_Timeout, Qt::QueuedConnection);

    RGB_color = 2;
    RGB_color_Old = 2;

    return;
}

void Cypress_Serial::on_StartWorker()
{
    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";
        qInfo() << CYCLASS << "on_StartWorker--> " << "start Cypress thread class";
        qInfo() << CYCLASS << "on_StartWorker--> " << "cypress thread id = " << QThread::currentThreadId();
    #endif

    Config();

    if(_serial->Serial_findport(gl_Ini_SerialCyPort)){
        _serial->Serial_openport();
        _serial->Serial_configport(QSerialPort::NoParity, QSerialPort::Data8, QSerialPort::OneStop, QSerialPort::NoFlowControl, QSerialPort::Baud57600);
//        _serial->Serial_configport(QSerialPort::NoParity, QSerialPort::Data8, QSerialPort::OneStop, QSerialPort::NoFlowControl, QSerialPort::Baud115200);
        _serial->Serial_clearport();
        _serial->Serial_ConnectToSlot(true);

        QObject::connect(_serial, &Serial::CheckBuffer, this, &Cypress_Serial::on_CheckBuffer, Qt::QueuedConnection);
        QObject::connect(_timeoutTimer, &QTimer::timeout, this, &Cypress_Serial::on_Timeout, Qt::QueuedConnection);
        QObject::connect(_queueChecker, &QTimer::timeout, this, &Cypress_Serial::on_CheckerTimeout, Qt::QueuedConnection);

        #ifdef QT_DEBUG
            qInfo() << CYCLASS << "on_StartWorker: emit this->StartedWorker();";
        #endif

        bCyThreadRun = true;
        //CheckTimer->start();
        ResetTimer->start();
        ResetAllTimer->start();
        emit this->StartedWorker();
    }

    return;
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
void Cypress_Serial::on_StartCyJob()
{
    if(bCyThreadRun){
        _queueChecker->start();
        Init_Ports();
    }
    
    return;
}

void Cypress_Serial::Init_Ports()
{
    gl_ini_And = false;
//    _serial->Serial_clearport();

    Cy_Reset();
    Delay_ms(300);

    qInfo() << CYCLASS << "Init_Ports --> " << "Cypress Port 1 Close";
    _serial->Serial_writetoserial("Port 1 Close");
    Delay_ms(50);

    qInfo() << CYCLASS << "Init_Ports --> " << "Cypress Port 1 UnLock";
    _serial->Serial_writetoserial("Port 1 UnLock");
    Delay_ms(50);

    qInfo() << CYCLASS << "Init_Ports --> " << "Cypress Port 1" + QString("Port 1 Wbits:%1").arg(Port1_Wbits);
    _serial->Serial_writetoserial(QString("Port 1 Wbits:%1").arg(Port1_Wbits));
    Delay_ms(50);

    qInfo() << CYCLASS << "Init_Ports --> " << "Cypress Port 2 Close";
    _serial->Serial_writetoserial("Port 2 Close");
    Delay_ms(50);

    qInfo() << CYCLASS << "Init_Ports --> " << "Cypress Port 2 UnLock";
    _serial->Serial_writetoserial("Port 2 UnLock");
    Delay_ms(50);

    qInfo() << CYCLASS << "Init_Ports --> " << "Cypress Port 2" + QString("Port 2 Wbits:%1").arg(Port2_Wbits);
    _serial->Serial_writetoserial(QString("Port 2 Wbits:%1").arg(Port2_Wbits));
    Delay_ms(50);

    qInfo() << CYCLASS << "Init_Ports --> " << "Cypress " + QString("Cy RGB:%1").arg(RGB_color);
    _serial->Serial_writetoserial(QString("Cy RGB:%1").arg(RGB_color));
    Delay_ms(50);

    start = true;

    JobTimer->start();
    emit this->sig_StartedCyJob();
    bCyThreadJobRun = true;

    return;
}


void Cypress_Serial::on_Job_Timeout()
{
    JobTimer->stop();
    QApplication::processEvents(QEventLoop::AllEvents);

//    if(!Port2_Lock && !Port1_Lock){
        Port_Color();
        QApplication::processEvents(QEventLoop::AllEvents);
//    }

//    Port_Inputs();
//    QApplication::processEvents(QEventLoop::AllEvents);

    if(!Port1_Lock){
        Port_Wbits(true);
        QApplication::processEvents(QEventLoop::AllEvents);
    }

    Port_Lock(true);
    QApplication::processEvents(QEventLoop::AllEvents);

    Port_Output(true);
    QApplication::processEvents(QEventLoop::AllEvents);

//    if(Port1_Lock){
//        Port_Input(true);
//        QApplication::processEvents(QEventLoop::AllEvents);
//    }

//    if(!Port1_Lock){
//        Port_Read(true);
//        QApplication::processEvents(QEventLoop::AllEvents);
//    }




//    if(!Port2_Lock){
        Port_Wbits(false);
        QApplication::processEvents(QEventLoop::AllEvents);
//    }

    Port_Lock(false);
    QApplication::processEvents(QEventLoop::AllEvents);

    Port_Output(false);
    QApplication::processEvents(QEventLoop::AllEvents);

//    if(Port2_Lock){
//        Port_Input(false);
//        QApplication::processEvents(QEventLoop::AllEvents);
//    }

//    if(!Port2_Lock){
//        Port_Read(false);
//        QApplication::processEvents(QEventLoop::AllEvents);
//    }








    if(!Port1_Lock && !Port2_Lock && !db_in_sync){
        /*if(check){
            _serial->Serial_writetoserial(QString("Port 1 Wbits:%1").arg(Port1_Wbits));
            Delay_ms(50);

            _serial->Serial_writetoserial(QString("Port 2 Wbits:%1").arg(Port2_Wbits));
            Delay_ms(50);

            Port1_Relay_on_off = false;
            Port1_Relay_on_off_Old = Port1_Relay_on_off;
            _serial->Serial_writetoserial("Port 1 Close");
            Delay_ms(50);

            Port2_Relay_on_off = false;
            Port2_Relay_on_off_Old = Port2_Relay_on_off;
            _serial->Serial_writetoserial("Port 2 Close");
            Delay_ms(50);

            Port1_Lock = false;
            Port1_Lock_Old = Port1_Lock;
            _serial->Serial_writetoserial("Port 1 UnLock");

            Port2_Lock = false;
            Port2_Lock_Old = Port2_Lock;
            _serial->Serial_writetoserial("Port 2 UnLock");
            Delay_ms(50);

            if(Port1_Timer_Relay_on->isActive())
                Port1_Timer_Relay_on->stop();

            if(Port2_Timer_Relay_on->isActive())
                Port2_Timer_Relay_on->stop();


            check = false;
            qInfo() << CYCLASS << "                                           on_Check_Timer_Timeout --> reset state exec";
            CheckTimer->start();
        }*/

        if(reset){
//            Cy_Reset();
            _serial->Serial_closeport();
            _serial->Serial_openport();
            _serial->Serial_clearport();
            _serial->_serialPort->flush();
//            Delay_ms(300);


            reset = false;
            qInfo() << CYCLASS << "                                           on_Reset_Timer_Timeout --> reset micro exec";
            ResetTimer->start();
        }
        else if(quit){
            qInfo() << CYCLASS << "                                           on_ResetAll_Timer_Timeout --> reset app exec";
//            QApplication::quit();

            quit = false;
            ResetAllTimer->start();
        }

    }

    JobTimer->start();
    return;
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
void Cypress_Serial::Cy_Connect()
{
    //qInfo() << CYCLASS << "Cy Connect";
    _serial->Serial_writetoserial("Cy Connect");
    Delay_ms(75);

    return;
}

void Cypress_Serial::Cy_Reset()
{
//    qInfo() << CYCLASS << "Cy Reset";
    _serial->Serial_writetoserial("Cy Reset");
    Delay_ms(75);

    return;
}


void Cypress_Serial::Port_Wbits(bool port)
{
    if(port){
        if(Port1_Wbits_Old != Port1_Wbits){
            Port1_Wbits_Old = Port1_Wbits;
            _serial->Serial_writetoserial(QString("Port 1 Wbits:%1").arg(Port1_Wbits));
            Delay_ms(75);
            qInfo() << CYCLASS << "Port_Wbits --> " << QString("Port 1 Wbits:%1").arg(Port1_Wbits);
        }

    }
    else{
        if(Port2_Wbits_Old != Port2_Wbits){
            Port1_Wbits_Old = Port2_Wbits;
            _serial->Serial_writetoserial(QString("Port 2 Wbits:%1").arg(Port2_Wbits));
            Delay_ms(75);
            qInfo() << CYCLASS << "Port_Wbits --> " << QString("Port 2 Wbits:%1").arg(Port2_Wbits);
        }

    }

    return;
}

void Cypress_Serial::Port_Output(bool port)
{
    if(port){
        if(Port1_Relay_on_off != Port1_Relay_on_off_Old){
            Port1_Relay_on_off_Old = Port1_Relay_on_off;
            if(Port1_Relay_on_off){
                _serial->Serial_writetoserial("Port 1 Open");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Output --> " << "Port 1 Open";
            }
            else{
                _serial->Serial_writetoserial("Port 1 Close");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Output --> " << "Port 1 Close";
//                qInfo() << "------------------------------------------------------------------------------------------";
            }
        }
//        if(Port1_Relay_on_off){

//        }
//        else{

//        }
    }
    else{
        if(Port2_Relay_on_off != Port2_Relay_on_off_Old){
            Port2_Relay_on_off_Old = Port2_Relay_on_off;
            if(Port2_Relay_on_off){
                _serial->Serial_writetoserial("Port 2 Open");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Output --> " << "Port 2 Open";
            }
            else{
                _serial->Serial_writetoserial("Port 2 Close");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Output --> " << "Port 2 Close";
//                qInfo() << "------------------------------------------------------------------------------------------";
            }
        }
//        if(Port2_Relay_on_off){

//        }
//        else{

//        }
    }

    return;
}

void Cypress_Serial::Port_Lock(bool port)
{
    if(port){
        if(Port1_Lock != Port1_Lock_Old){
            Port1_Lock_Old = Port1_Lock;
            if(Port1_Lock){
                _serial->Serial_writetoserial("Port 1 Lock");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Lock --> " << "Port 1 Lock";
            }
            else{
                _serial->Serial_writetoserial("Port 1 UnLock");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Lock --> " << "Port 1 UnLock";
            }

        }
//        if(Port1_Lock){


//        }
//        else{


//        }
    }
    else{
        if(Port2_Lock != Port2_Lock_Old){
            Port2_Lock_Old = Port2_Lock;
            if(Port2_Lock){
                _serial->Serial_writetoserial("Port 2 Lock");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Lock --> " << "Port 2 Lock";
            }
            else{
                _serial->Serial_writetoserial("Port 2 UnLock");
                Delay_ms(75);
                qInfo() << CYCLASS << "Port_Lock --> " << "Port 2 UnLock";
            }
        }
//        if(Port2_Lock){

//        }
//        else{

//        }
    }

    return;
}

void Cypress_Serial::Port_Input(bool port)
{
    if(port){
        _serial->Serial_writetoserial("Port 1 Input");
        Delay_ms(75);
    }
    else{
        _serial->Serial_writetoserial("Port 2 Input");
        Delay_ms(75);
    }

    return;
}

void Cypress_Serial::Port_Read(bool port)
{
    if(port){
        _serial->Serial_writetoserial("Port 1 Read");
        Delay_ms(75);
    }
    else{
        _serial->Serial_writetoserial("Port 2 Read");
        Delay_ms(75);
    }

    return;
}

void Cypress_Serial::Port_Inputs()
{
    _serial->Serial_writetoserial("Inputs");
    Delay_ms(75);

    return;
}


void Cypress_Serial::Port_Color()
{

    _serial->Serial_writetoserial(QString("Cy RGB:%1").arg(QString::number(RGB_color)));
    Delay_ms(75);
    if(RGB_color_Old != RGB_color){
        RGB_color_Old = RGB_color;
        qInfo() << CYCLASS << "Port_Color --> " << QString("Cy RGB:%1").arg(QString::number(RGB_color));
    }

    return;
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
void Cypress_Serial::on_Port1_Timeout()
{
    qInfo() << CYCLASS << "on_Port1_Timeout --> ";
    if(!gl_ini_And){
        Port_Reset(true);
        QApplication::processEvents(QEventLoop::AllEvents);
    }
    else{
        if(in_And){

            Port_Reset(true);
            QApplication::processEvents(QEventLoop::AllEvents);

            Port_Reset(false);
            QApplication::processEvents(QEventLoop::AllEvents);

            in_And = false;
        }
    }

    return;
}

void Cypress_Serial::on_Port2_Timeout()
{
    qInfo() << CYCLASS << "on_Port2_Timeout --> ";
    if(!gl_ini_And){
        Port_Reset(false);
        QApplication::processEvents(QEventLoop::AllEvents);
    }
    else{
        if(in_And){
            Port_Reset(true);
            QApplication::processEvents(QEventLoop::AllEvents);

            Port_Reset(false);
            QApplication::processEvents(QEventLoop::AllEvents);

            in_And = false;
        }
    }
    return;
}

void Cypress_Serial::on_Port1_Proc_Timeout()
{
    Port_Reset_Proc(true);
    QApplication::processEvents(QEventLoop::AllEvents);

    return;
}

void Cypress_Serial::on_Port2_Proc_Timeout()
{
    Port_Reset_Proc(false);
    QApplication::processEvents(QEventLoop::AllEvents);

    return;
}

void Cypress_Serial::Port_Reset(bool port)
{
    if(!gl_ini_And){
        if(port){
            if(Port1_Lock && Port1_Relay_on_off){
                locked_out_1 = 0;
                Port1_Relay_on_off = 0;
                Port1_Lock = 0;
                qInfo() << CYCLASS << "Port_Reset --> " << "Port 1";
                if(Port1_Timer_Relay_on->isActive())
                    Port1_Timer_Relay_on->stop();
            }
        }
        else{
            if(Port2_Lock && Port2_Relay_on_off){
                locked_out_2 = 0;
                Port2_Relay_on_off = 0;
                Port2_Lock = 0;
                qInfo() << CYCLASS << "Port_Reset --> " << "Port 2";
                if(Port2_Timer_Relay_on->isActive())
                    Port2_Timer_Relay_on->stop();
            }
        }
    }
    else if(in_And){
        locked_out_1 = 0;
        locked_out_2 = 0;
        if(port){
            if(Port1_Lock){
                Port1_Relay_on_off = 0;
                Port1_Lock = 0;
                qInfo() << CYCLASS << "Port_Reset --> " << "Port 1";
                if(Port1_Timer_Relay_on->isActive())
                    Port1_Timer_Relay_on->stop();
            }
        }
        else{
            if(Port2_Lock){
                Port2_Relay_on_off = 0;
                Port2_Lock = 0;
                qInfo() << CYCLASS << "Port_Reset --> " << "Port 2";
                if(Port2_Timer_Relay_on->isActive())
                    Port2_Timer_Relay_on->stop();
            }
        }
    }
    else{
        locked_out_1 = 0;
        locked_out_2 = 0;
    }

    return;
}

void Cypress_Serial::Port_Reset_Proc(bool port)
{
    if(port){
        if(Port1_Lock && !Port1_Relay_on_off){
            qInfo() << CYCLASS << "Port_Reset_Proc --> " << "Port 1";
            Port1_Lock = 0;
            if(Port1_Timeout_Proc->isActive())
                Port1_Timeout_Proc->stop();
        }
    }
    else{
        if(Port2_Lock && !Port2_Relay_on_off){
            qInfo() << CYCLASS << "Port_Reset_Proc --> " << "Port 2";
            Port2_Lock = 0;
            if(Port2_Timeout_Proc->isActive())
                Port2_Timeout_Proc->stop();
        }
    }

    return;
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
void Cypress_Serial::on_AuthRes(int readerExpId, int readerId, bool success)
{
    //CheckTimer->stop();
    //check = false;
    //CheckTimer->start();
//    qInfo() << CYCLASS << "on_AuthRes --> " << "readerExpId = " << readerExpId << " , readerId = " << readerId << " , success = " << success;
    if(readerExpId == 1){
        if(!gl_ini_And){
            if(readerId == 1){
                if(success && Port1_Lock){
                    QApplication::processEvents(QEventLoop::AllEvents);
                    Port1_Relay_on_off = true;
                    if(Port1_Timeout_Proc->isActive())
                        Port1_Timeout_Proc->stop();
                    if(!Port1_Timer_Relay_on->isActive())
                        Port1_Timer_Relay_on->start();

//                    QApplication::processEvents(QEventLoop::AllEvents);
//                    Port2_Relay_on_off = true;
//                    if(Port2_Timeout_Proc->isActive())
//                        Port2_Timeout_Proc->stop();
//                    if(!Port2_Timer_Relay_on->isActive())
//                        Port2_Timer_Relay_on->start();
                }
                else{
                    Port_Reset_Proc(true);
                    QApplication::processEvents(QEventLoop::AllEvents);

//                    Port_Reset_Proc(false);
//                    QApplication::processEvents(QEventLoop::AllEvents);
                }
            }
            else if(readerId == 2){
                if(success && Port2_Lock){
                    QApplication::processEvents(QEventLoop::AllEvents);
                    Port2_Relay_on_off = true;
                    if(Port2_Timeout_Proc->isActive())
                        Port2_Timeout_Proc->stop();
                    if(!Port2_Timer_Relay_on->isActive())
                        Port2_Timer_Relay_on->start();

//                    QApplication::processEvents(QEventLoop::AllEvents);
//                    Port1_Relay_on_off = true;
//                    if(Port1_Timeout_Proc->isActive())
//                        Port1_Timeout_Proc->stop();
//                    if(!Port1_Timer_Relay_on->isActive())
//                        Port1_Timer_Relay_on->start();
                }
                else{
                    Port_Reset_Proc(false);
                    QApplication::processEvents(QEventLoop::AllEvents);

//                    Port_Reset_Proc(true);
//                    QApplication::processEvents(QEventLoop::AllEvents);
                }
            }
            else{
                qDebug() << "wrong readerId " << readerId;
                QApplication::processEvents(QEventLoop::AllEvents);
            }
        }
        else{
            if(readerId == 1){
                if(in_And){
                    if(success){
                        if(gl_ini_AndPort == 1){
                            if(Port1_Lock && Port2_Lock){
                                QApplication::processEvents(QEventLoop::AllEvents);
                                Port1_Relay_on_off = true;
                                if(Port1_Timeout_Proc->isActive())
                                    Port1_Timeout_Proc->stop();
                                if(!Port1_Timer_Relay_on->isActive())
                                    Port1_Timer_Relay_on->start();
                            }
                            else{
                                Port_Reset_Proc(true);
                                QApplication::processEvents(QEventLoop::AllEvents);

                                Port_Reset_Proc(false);
                                QApplication::processEvents(QEventLoop::AllEvents);

                                in_And = false;
                            }
                        }
                        else if(gl_ini_AndPort == 2){
                            if(Port1_Lock && Port2_Lock){
                                QApplication::processEvents(QEventLoop::AllEvents);
                                Port2_Relay_on_off = true;
                                if(Port2_Timeout_Proc->isActive())
                                    Port2_Timeout_Proc->stop();
                                if(!Port2_Timer_Relay_on->isActive())
                                    Port2_Timer_Relay_on->start();
                            }
                            else{
                                Port_Reset_Proc(true);
                                QApplication::processEvents(QEventLoop::AllEvents);

                                Port_Reset_Proc(false);
                                QApplication::processEvents(QEventLoop::AllEvents);

                                in_And = false;
                            }
                        }
                        else{
                            Port_Reset_Proc(true);
                            QApplication::processEvents(QEventLoop::AllEvents);

                            Port_Reset_Proc(false);
                            QApplication::processEvents(QEventLoop::AllEvents);

                            in_And = false;
                        }
                    }
                    else{
                        Port_Reset_Proc(true);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        Port_Reset_Proc(false);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        in_And = false;
                    }
                }
                else{
                    Port_Reset_Proc(true);
                    QApplication::processEvents(QEventLoop::AllEvents);

                    Port_Reset_Proc(false);
                    QApplication::processEvents(QEventLoop::AllEvents);

                    in_And = false;
                }
            }
            else{
                qDebug() << "in and wrong readerId " << readerId;
                QApplication::processEvents(QEventLoop::AllEvents);
            }
        }

    }
    else{
        qDebug() << "wrong readerExpId " << readerExpId;
        QApplication::processEvents(QEventLoop::AllEvents);
    }

    return;
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
void Cypress_Serial::on_CheckBuffer()
{
    if(!start){
        _serial->_buffer.clear();
        return;
    }

    _timeoutTimer->start();
//    if(_serial->_buffer.count() > 0){
        if(_serial->_buffer.at(_serial->_buffer.count() - 1) == '\0'){
            _timeoutTimer->stop();
//            qDebug() << "on_CheckBuffer -->                                                new messageeeeeeeeeeeeeeeeeee: " << _serial->_buffer;
//            qDebug() << "on_CheckBuffer --> enqueue msg";
            enqueue(_serial->_buffer);
            _serial->_buffer.clear();
        }
//    }
    QApplication::processEvents(QEventLoop::AllEvents);

    return;
}

void Cypress_Serial::on_Timeout()
{
    qDebug() << "on_Timeout -->                               ********** failed message: " << _serial->_buffer;
    _timeoutTimer->stop();
    _serial->_buffer.clear();

    return;
}

void Cypress_Serial::on_CheckerTimeout()
{
    _queueChecker->stop();
    if (!isEmpty()){
        QApplication::processEvents(QEventLoop::AllEvents);
        QString msg = peek();

        if(msg.contains("P1W")){

            if(!reset){
                ResetTimer->stop();
                reset = false;
                ResetTimer->start();
            }

            if(!quit){
                ResetAllTimer->stop();
                quit = false;
                ResetAllTimer->start();
            }

            //if(!check){
                //CheckTimer->stop();
                //check = false;
                //CheckTimer->start();
            //}

            qInfo() << "";
            qInfo() << "------------------------------------------------------------------------------------------";
//            read_port1++;
//            read_total = read_port1 + read_port2;
//            qInfo() << CYCLASS << "read_port1 = " << read_port1 << ", read_port2 = " << read_port2 << ", read_total = " << read_total;

            if(!gl_ini_And){
                if(!Port1_Lock){
                    Port1_Lock = 1;
//                    Port2_Lock = 1;

                    qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "Port 1 Read Weigand: " << msg;
                    QApplication::processEvents(QEventLoop::AllEvents);

                    AuthReq auth = Port_Proc_Weigand(true, msg);
                    QApplication::processEvents(QEventLoop::AllEvents);
                    //Port1_Timeout_Proc->start();
                    if(bAuthThreadJobRun){
                        QApplication::processEvents(QEventLoop::AllEvents);
                        emit this->sig_AuthReq(auth.userUniqueId, auth.readerId, auth.readerExpId);
                        QApplication::processEvents(QEventLoop::AllEvents);
                    }
                    else{
                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Mqtt Thread is not running";
                        QApplication::processEvents(QEventLoop::AllEvents);
                    }
                }
            }
            else{
                if(!in_And){
                    if(!Port1_Lock && !Port2_Lock){
                        in_And = true;
                        Port1_Lock = 1;
                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "in and proc: Port 1 Read Weigand: " << msg;
                        QApplication::processEvents(QEventLoop::AllEvents);

                        Port_Proc_WeigandAnd(true, msg);
                        QApplication::processEvents(QEventLoop::AllEvents);
                        and_Timer->start();
                    }
                    else{
                        qWarning() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "1 wrong and proc port 1";
                        and_Timer->stop();

                        Port_Reset(true);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        Port_Reset(false);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        AuthReqAnd.userUniqueId_1 = -1;
                        AuthReqAnd.userUniqueId_2 = -1;
                    }
                }
                else{
                    if(!Port1_Lock && Port2_Lock){
                        Port1_Lock = 1;

                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "in and proc: Port 1 Read Weigand: " << msg;
                        Port_Proc_WeigandAnd(true, msg);

                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "in and proc: Port 2 and Port 1 Read Weigand finish ";

                        QApplication::processEvents(QEventLoop::AllEvents);
                        emit this->sig_AuthReqAnd(AuthReqAnd.userUniqueId_1, AuthReqAnd.userUniqueId_2, AuthReqAnd.readerId_1, AuthReqAnd.readerId_2, AuthReqAnd.readerExpId, true);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        and_Timer->stop();
                    }
                    else{
                        qWarning() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "2 wrong and proc port 1";

                        and_Timer->stop();

                        Port_Reset(true);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        Port_Reset(false);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        AuthReqAnd.userUniqueId_1 = -1;
                        AuthReqAnd.userUniqueId_2 = -1;
                    }
                }
            }
        }

        else if(msg.contains("P2W")){
            if(!reset){
                ResetTimer->stop();
                reset = false;
                ResetTimer->start();
            }

            if(!quit){
                ResetAllTimer->stop();
                quit = false;
                ResetAllTimer->start();
            }

            //if(!check){
                //CheckTimer->stop();
                //check = false;
                //CheckTimer->start();
            //}

            qInfo() << "";
            qInfo() << "------------------------------------------------------------------------------------------";
//            read_port2++;
//            read_total = read_port1 + read_port2;
//            qInfo() << CYCLASS << "read_port1 = " << read_port1 << ", read_port2 = " << read_port2 << ", read_total = " << read_total;

            if(!gl_ini_And){
                if(!Port2_Lock){
                    Port2_Lock = 1;
//                    Port1_Lock = 1;

                    qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "Port 2 Read Weigand: " << msg;
                    QApplication::processEvents(QEventLoop::AllEvents);

                    AuthReq auth = Port_Proc_Weigand(false, msg);
                    QApplication::processEvents(QEventLoop::AllEvents);
                    //Port2_Timeout_Proc->start();
                    if(bAuthThreadJobRun){
                        QApplication::processEvents(QEventLoop::AllEvents);
                        emit this->sig_AuthReq(auth.userUniqueId, auth.readerId, auth.readerExpId);
                        QApplication::processEvents(QEventLoop::AllEvents);
                    }
                    else{
                        QApplication::processEvents(QEventLoop::AllEvents);
                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Mqtt Thread is not running";
                    }
                }
            }
            else{
                if(!in_And){
                    if(!Port1_Lock && !Port2_Lock){
                        in_And = true;
                        Port2_Lock = 1;

                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "in and proc: Port 2 Read Weigand: " << msg;
                        QApplication::processEvents(QEventLoop::AllEvents);

                        Port_Proc_WeigandAnd(false, msg);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        and_Timer->start();
                    }
                    else{
                        qWarning() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "1 wrong and proc port 2";
                        and_Timer->stop();

                        Port_Reset(true);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        Port_Reset(false);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        AuthReqAnd.userUniqueId_1 = -1;
                        AuthReqAnd.userUniqueId_2 = -1;

                    }
                }
                else{
                    if(!Port2_Lock && Port1_Lock){
                        //finish and proc
                        Port2_Lock = 1;

                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "in and proc: Port 2 Read Weigand: " << msg;
                        QApplication::processEvents(QEventLoop::AllEvents);
                        Port_Proc_WeigandAnd(false, msg);

                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "in and proc: Port 2 and Port 1 Read Weigand finish ";

                        QApplication::processEvents(QEventLoop::AllEvents);
                        emit this->sig_AuthReqAnd(AuthReqAnd.userUniqueId_1, AuthReqAnd.userUniqueId_2, AuthReqAnd.readerId_1, AuthReqAnd.readerId_2, AuthReqAnd.readerExpId, true);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        and_Timer->stop();
                    }
                    else{
                        qWarning() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "2 wrong and proc port 2";
                        and_Timer->stop();

                        Port_Reset(true);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        Port_Reset(false);
                        QApplication::processEvents(QEventLoop::AllEvents);

                        AuthReqAnd.userUniqueId_1 = -1;
                        AuthReqAnd.userUniqueId_2 = -1;
                    }
                }
            }
        }

        else if(msg.contains("I1")){
            if(!reset){
                ResetTimer->stop();
                reset = false;
                ResetTimer->start();
            }

            if(!quit){
                ResetAllTimer->stop();
                quit = false;
                ResetAllTimer->start();
            }

            //if(!check){
                //CheckTimer->stop();
                //check = false;
                //CheckTimer->start();
            //}

            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "Port 1 Read Input: " << msg;
            if(!gl_ini_And){
                if(Port1_Lock && Port1_Relay_on_off){
                    Port_Reset(true);
                    QApplication::processEvents(QEventLoop::AllEvents);

//                    Port_Reset(false);
//                    QApplication::processEvents(QEventLoop::AllEvents);

                    InpReq inp;
                    inp.readerId = 1;
                    if(bAuthThreadJobRun){
                        QApplication::processEvents(QEventLoop::AllEvents);
                        emit this->sig_InpReq(inp.readerExpId, inp.readerId);
                        QApplication::processEvents(QEventLoop::AllEvents);
                    }
                    else{
                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Mqtt Thread is not running";
                        QApplication::processEvents(QEventLoop::AllEvents);
                    }
                }
                else{
                    qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Cy Port1 is not Locked";
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
            }
            else{
                if(in_And){
                    if(gl_ini_AndPort == 1){
                        if(Port1_Lock && Port2_Lock && Port1_Relay_on_off){
                            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " input 1: in And Proc";
                            Port_Reset(true);
                            QApplication::processEvents(QEventLoop::AllEvents);

                            Port_Reset(false);
                            QApplication::processEvents(QEventLoop::AllEvents);

                            in_And = false;

                            InpReq inp;
                            inp.readerId = 1;
                            if(bAuthThreadJobRun){
                                QApplication::processEvents(QEventLoop::AllEvents);
                                emit this->sig_InpReq(inp.readerExpId, inp.readerId);
                                QApplication::processEvents(QEventLoop::AllEvents);
                            }
                            else{
                                qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Mqtt Thread is not running";
                                QApplication::processEvents(QEventLoop::AllEvents);
                            }
                        }
                        else{
                            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Cy Port1 is not Locked";
                            QApplication::processEvents(QEventLoop::AllEvents);
                        }
                    }
                }
            }
        }

        else if(msg.contains("I2")){
            if(!reset){
                ResetTimer->stop();
                reset = false;
                ResetTimer->start();
            }

            if(!quit){
                ResetAllTimer->stop();
                quit = false;
                ResetAllTimer->start();
            }

            //if(!check){
                //CheckTimer->stop();
                //check = false;
                //CheckTimer->start();
            //}

            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << "Port 2 Read Input: " << msg;
            if(!gl_ini_And){
                if(Port2_Lock && Port2_Relay_on_off){
                    Port_Reset(false);
                    QApplication::processEvents(QEventLoop::AllEvents);

//                    Port_Reset(true);
//                    QApplication::processEvents(QEventLoop::AllEvents);

                    InpReq inp;
                    inp.readerId = 2;
                    if(bAuthThreadJobRun){
                        QApplication::processEvents(QEventLoop::AllEvents);
                        emit this->sig_InpReq(inp.readerExpId, inp.readerId);
                        QApplication::processEvents(QEventLoop::AllEvents);
                    }
                    else{
                        qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Mqtt Thread is not running";
                        QApplication::processEvents(QEventLoop::AllEvents);
                    }
                }
                else{
                    qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Cy Port 2 is not Locked";
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
            }
            else{
                if(in_And){
                    if(gl_ini_AndPort == 2){
                        if(Port1_Lock && Port2_Lock && Port2_Relay_on_off){
                            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " input 2: in And Proc";

                            Port_Reset(true);
                            QApplication::processEvents(QEventLoop::AllEvents);

                            Port_Reset(false);
                            QApplication::processEvents(QEventLoop::AllEvents);

                            in_And = false;

                            InpReq inp;
                            inp.readerId = 1;
                            if(bAuthThreadJobRun){
                                QApplication::processEvents(QEventLoop::AllEvents);
                                emit this->sig_InpReq(inp.readerExpId, inp.readerId);
                                QApplication::processEvents(QEventLoop::AllEvents);
                            }
                            else{
                                qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Mqtt Thread is not running";
                                QApplication::processEvents(QEventLoop::AllEvents);
                            }
                        }
                        else{
                            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Cy Port1 is not Locked";
                            QApplication::processEvents(QEventLoop::AllEvents);
                        }
                    }
                }
            }
        }

        else if(msg.contains("Cy Connected")){
            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> Cy Connected";
            QApplication::processEvents(QEventLoop::AllEvents);
        }
        else if(msg.contains("{\"inps\":\"") && msg.length() == 13){

            if(!reset){
                ResetTimer->stop();
                reset = false;
                ResetTimer->start();
            }

            if(!quit){
                ResetAllTimer->stop();
                quit = false;
                ResetAllTimer->start();
            }

            //if(!check){
                //CheckTimer->stop();
                //check = false;
                //CheckTimer->start();
            //}

            inputs = msg;
//            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> new inputs: " << msg;
            if(inputs != inputs_old){
                inputs_old = inputs;
                qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> new inputs: " << inputs_old;
                if(bAuthThreadJobRun){
                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_InpsPub(inputs_old);
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
                else{
                    qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> " << " Auth Thread is not running";

                }
            }
        }
        else{
            qInfo() << CYCLASS << "on_CheckerTimeout Read Queue --> wrong message ************************-->"<< msg;
            QApplication::processEvents(QEventLoop::AllEvents);
        }
        dequeue();
    }
    _queueChecker->start();

    return;
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
AuthReq Cypress_Serial::Port_Proc_Weigand(bool port, QString weigand)
{
    AuthReq req;
    Weigand w;
    if(port){
        req.readerId = 1;
        weigand = weigand.remove("P1W");
        w = Port_Proc_w(Port1_Wbits, weigand);
    }
    else{
        req.readerId = 2;
        weigand = weigand.remove("P2W");
        w = Port_Proc_w(Port2_Wbits, weigand);
    }
    req.userUniqueId = w.uinqueId;

    return req;
}

void Cypress_Serial::Port_Proc_WeigandAnd(bool port, QString weigand)
{
    Weigand w;
    if(port){
        weigand = weigand.remove("P1W");
        w = Port_Proc_w(Port1_Wbits, weigand);
        AuthReqAnd.userUniqueId_1 = w.uinqueId;
    }
    else{
        weigand = weigand.remove("P2W");
        w = Port_Proc_w(Port2_Wbits, weigand);
        AuthReqAnd.userUniqueId_2 = w.uinqueId;
    }

    return;
}

InpReq Cypress_Serial::Port_Proc_Input(bool port)
{
    InpReq req;
    if(port){
        req.readerId = 1;
    }
    else{
        req.readerId = 2;
    }

    return req;
}

Weigand Cypress_Serial::Port_Proc_w(int numberBitsFormat, QString weigand)
{
    Weigand w;

    switch(numberBitsFormat){
        case 26:{
            QString forEven = weigand.mid(1, 12);
            QString forOdd = weigand.mid(13, 12);

            int even = forEven.count(QLatin1Char('1'));

            if((even % 2 == 0) && (weigand.at(0) == '0')){    
            }
            else if((even % 2 != 0) && (weigand.at(0) == '1')){   
            }
            else{
                qInfo() << CYCLASS << "Port_Proc_w --> " << "even section parity faild";
                break;
            }

            int odd = forOdd.count(QLatin1Char('1'));
            if((odd % 2 != 0) && (weigand.at(weigand.count() - 1) == '0')){
            }
            else if((odd % 2 == 0) && (weigand.at(weigand.count() - 1) == '1')){    
            }
            else{
                qInfo() << CYCLASS << "Port_Proc_w --> " << "odd section parity faild";
                break;
            }

            bool ok;
            w.siteCode = weigand.mid(1, 8).toUInt(&ok, 2);
//            qInfo() << CYCLASS << "Port_Proc_w --> " << "w.siteCode = " << w.siteCode;

            w.uinqueId = weigand.mid(9, 16).toUInt(&ok, 2);
//            qInfo() << CYCLASS << "Port_Proc_w --> " << "w.uinqueId = " << w.uinqueId;
            break;
        }
        default:{
            break;
        }
    }

    return w;
}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//RGB_color = 1 //micro up
//RGB_color = 2 //linux connected to micro
//RGB_color = 3 //connecting to server
//RGB_color = 4 //connected to server
void Cypress_Serial::on_sig_CyMqttStatus(int status)
{
    qInfo() << CYCLASS << "on_sig_CyMqttStatus --> mqtt status = " << status;
    switch(status){
        case 0:{
            RGB_color = 2; //connecting
            break;
        }
        case 1:{
            RGB_color = 3; //connected
            //Port_Color();
            break;
        }
        case 2:{
            RGB_color = 4; //subscribed
            break;
        }
        default:{
            RGB_color = 2;
            break;
        }
    }

//    Port_Color();


    return;
}

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
void Cypress_Serial::dequeue()
{
    if (!isEmpty()){
        mutex->lock();
        front = (front + 1) % capacity;
        count--;
        mutex->unlock();
    }

    return;
}

void Cypress_Serial::enqueue(QString msg)
{
    if (!isFull()){
        mutex->lock();
        rear = (rear + 1) % capacity;
        arr[rear] = msg;
        count++;
        mutex->unlock();
    }
    else{
        dequeue();
        mutex->lock();
        rear = (rear + 1) % capacity;
        arr[rear]= msg;
        count++;
        mutex->unlock();
    }

    return;
}

QString Cypress_Serial::peek()
{
    if (!isEmpty())
        return arr[front];
    else
        return mainItem; // ?????????????????
}

int Cypress_Serial::size()
{
    return count;
}

bool Cypress_Serial::isEmpty()
{
    return (size() == 0);
}

bool Cypress_Serial::isFull()
{
    return (size() == capacity);
}

void Cypress_Serial::initQ()
{
    front = 0;
    rear = -1;
    count = 0;

    return;
}

void Cypress_Serial::on_Delay_Timeout()
{
    delay_Timer->stop();
    delay = false;

    return;
}

void Cypress_Serial::Delay_ms(quint32 ms)
{
    delay_Timer->start(ms);
    delay = true;
    while(delay){
        QApplication::processEvents(QEventLoop::AllEvents);
    }

    return;
}

void Cypress_Serial::on_and_Timer_Timeout()
{
    and_Timer->stop();
    emit this->sig_AuthReqAnd(AuthReqAnd.userUniqueId_1, AuthReqAnd.userUniqueId_2, AuthReqAnd.readerId_1, AuthReqAnd.readerId_2, AuthReqAnd.readerExpId, false);

    Port_Reset(true);
    Port_Reset(false);

    in_And = false;

    AuthReqAnd.userUniqueId_1 = -1;
    AuthReqAnd.userUniqueId_2 = -1;

    return;
}


void Cypress_Serial::sig_ServerToCypressLock_slot(quint8 port)
{
    if(!gl_ini_And){
        if(port == 1){
            qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> open port 1";
            if(Port1_Lock){
                if(locked_out_1){
                    qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> again open port 1";
                    if(Port1_Timer_Relay_on->isActive()){
                        Port1_Timer_Relay_on->stop();
                        Port1_Timer_Relay_on->start();
                    }

                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(1, true);
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
                else{
                    qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> port 1 is locked -> can not be open";
                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(1, false);
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
            }
            else{
                locked_out_1 = 1;
                Port1_Lock = 1;
                Port1_Relay_on_off = true;

                if(Port1_Timeout_Proc->isActive())
                    Port1_Timeout_Proc->stop();
                if(!Port1_Timer_Relay_on->isActive())
                    Port1_Timer_Relay_on->start();

                QApplication::processEvents(QEventLoop::AllEvents);
                emit this->sig_SendToServerLock(1, true);
                QApplication::processEvents(QEventLoop::AllEvents);
            }
        }
        else if (port == 2){
            qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> open port 2";
            if(Port2_Lock){
                if(locked_out_2){
                    qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> again open port 2";
                    if(Port2_Timer_Relay_on->isActive()){
                        Port2_Timer_Relay_on->stop();
                        Port2_Timer_Relay_on->start();
                    }

                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(2, true);
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
                else{
                    qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> port 2 is locked -> can not be open";
                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(2, false);
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
            }
            else{
                locked_out_2 = 1;
                Port2_Lock = 1;
                Port2_Relay_on_off = true;

                if(Port2_Timeout_Proc->isActive())
                    Port2_Timeout_Proc->stop();
                if(!Port2_Timer_Relay_on->isActive())
                    Port2_Timer_Relay_on->start();

                QApplication::processEvents(QEventLoop::AllEvents);
                emit this->sig_SendToServerLock(2, true);
                QApplication::processEvents(QEventLoop::AllEvents);

            }
        }
        else{
            //invalid port
        }
    }
    else{
        if(port == 1 || port == 2){
            qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> open ports";
            if(Port1_Lock || Port2_Lock){
                if(locked_out_1 || locked_out_2){
                    qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> again open ports";
                    if(Port1_Timer_Relay_on->isActive()){
                        Port1_Timer_Relay_on->stop();
                        Port1_Timer_Relay_on->start();
                    }


                    if(Port2_Timer_Relay_on->isActive()){
                        Port2_Timer_Relay_on->stop();
                        Port2_Timer_Relay_on->start();
                    }

                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(1, true);
                    QApplication::processEvents(QEventLoop::AllEvents);

                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(2, true);
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
                else{
                    qInfo() << CYCLASS << "sig_ServerToCypressLock_slot --> ports are locked -> can not be open";
                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(1, false);
                    QApplication::processEvents(QEventLoop::AllEvents);

                    QApplication::processEvents(QEventLoop::AllEvents);
                    emit this->sig_SendToServerLock(2, false);
                    QApplication::processEvents(QEventLoop::AllEvents);
                }
            }
            else{
                locked_out_1 = 1;
                locked_out_2 = 2;

                Port1_Lock = 1;
                Port2_Lock = 1;

                Port1_Relay_on_off = true;
                Port2_Relay_on_off = true;

                if(Port1_Timeout_Proc->isActive())
                    Port1_Timeout_Proc->stop();
                if(!Port1_Timer_Relay_on->isActive())
                    Port1_Timer_Relay_on->start();

                if(Port2_Timeout_Proc->isActive())
                    Port2_Timeout_Proc->stop();
                if(!Port2_Timer_Relay_on->isActive())
                    Port2_Timer_Relay_on->start();

            }
        }
        else{
            //invalid port
        }
    }

    return;
}

//void Cypress_Serial::on_Check_Timer_Timeout()
//{
    //qInfo() << CYCLASS << "                                           on_Check_Timer_Timeout --> reset state timer";
    //CheckTimer->stop();
    //check = true;
    //return;
//}

void Cypress_Serial::on_Reset_Timer_Timeout()
{
    qInfo() << CYCLASS << "                                           on_Reset_Timer_Timeout --> reset micro timer";
    ResetTimer->stop();
    reset = true;
    return;
}

void Cypress_Serial::on_ResetAll_Timer_Timeout()
{
    qInfo() << CYCLASS << "                                          on_ResetAll_Timer_Timeout --> reset app timer";
    ResetAllTimer->stop();
    quit = true;
    return;
}

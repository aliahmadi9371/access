#include "serial.h"

Serial::Serial(QObject *parent) : QObject(parent)
{
    Init();
    Config();
}

void Serial::Init()
{
    _serialportname.clear();
    _buffer.clear();
    _serialPort = nullptr;
    lock = false;

    return;
}

void Serial::Config()
{
    _serialPort = new QSerialPort();
}

bool Serial::Serial_findport(QString myport)
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    bool ret = false;
    for(const QSerialPortInfo& info: ports){        
        if(info.portName() == myport){
            qInfo() << SERIALCLASS << "Seial_findport--> " << "ports name: " << info.portName();
            _serialportname = info.portName();
            ret = true;
        }
        //qDebug() << info.portName();
    }
    return ret;
}

bool Serial::Serial_openport()
{
    bool ret = true;
    _serialPort->setPortName(_serialportname);
    //_serialPort = new QSerialPort();
    if(!(_serialPort->open(QIODevice::ReadWrite))){
        qWarning() << SERIALCLASS << "Serial_openport--> " << _serialPort->errorString();
        ret = false;
        qWarning() << SERIALCLASS << "Serial_openport--> " << "can not open serial: quit app";
        QApplication::quit();
    }
    return ret;
}

void Serial::Serial_closeport()
{
    if(_serialPort->isOpen()){
        _serialPort->close();
//        qInfo() << SERIALCLASS << "Seial_closeport--> " << "close serial port name " << _serialportname;
    }
}


void Serial::Serial_configport(QSerialPort::Parity parity, QSerialPort::DataBits databit, QSerialPort::StopBits stopbit, QSerialPort::FlowControl flowcontrol, QSerialPort::BaudRate baudrate)
{
    _serialPort->setParity(parity);
    _serialPort->setDataBits(databit);
    _serialPort->setStopBits(stopbit);
    _serialPort->setFlowControl(flowcontrol);
    _serialPort->setBaudRate(baudrate);
    if(baudrate == QSerialPort::Baud57600)
//    if(baudrate == QSerialPort::Baud115200)
        qInfo() << SERIALCLASS << "Serial_configport--> " << "can config serial port name " << _serialportname;
}

void Serial::Serial_ConnectToSlot(bool connect)
{
    //qDebug() << "Serial_ConnectToSlot = " << connect;
    if(connect)
        QObject::connect(_serialPort, &QSerialPort::readyRead, this, &Serial::on_ReadyRead, Qt::QueuedConnection);
    else
        QObject::disconnect(_serialPort, &QSerialPort::readyRead, this, &Serial::on_ReadyRead);

    return;
}

bool Serial::Serial_writetoserial(QString message)
{
//    int write =_serialPort->write(message.toLocal8Bit(), message.length());
//    _serialPort->flush();
//    if(_serialPort->waitForBytesWritten()){
//        if(write != message.length()){
//            qDebug() << SERIALCLASS << "Serial_writetoserial--> " << "wrong write lng = " << write;
//        }
//    }
//    else{
//        qDebug() << SERIALCLASS << "Serial_writetoserial--> " << "can not write = " << write;
//    }
//    _serialPort->flush();
    //bool res = true;
//    _serialPort->flush();
//    char msg[message.length()];
//    for(int i = 0; i < message.length(); i++){
//        msg[i] = message[i].toLatin1();
//    }

//    for(int i = 0; i < message.length(); i++){
//        _serialPort->write(&msg[i], 1);
//        _serialPort->flush();
//    }
    _serialPort->write(message.toLocal8Bit(), message.length() + 1);
    _serialPort->flush();
//    int write1 = _serialPort->write("\0", 1);
    //_serialPort->flush();
//    _serialPort->waitForBytesWritten();
    //qDebug() << SERIALCLASS << "Serial_writetoserial--> " << "write count = " << write;
//    if(write < 0 /*|| write1 < 0*/){
//        lock = false;
//        qWarning() << SERIALCLASS << "Serial_writetoserial-> error: " << _serialPort->errorString();
//        res = false;
//    }
//    else
//        lock = true;
//    _serialPort->flush();
//    Serial_clearport();

    return true;
}

void Serial::Serial_clearport()
{
    _serialPort->clear(QSerialPort::AllDirections);
}

void Serial::on_ReadyRead()
{
    _buffer = _buffer + _serialPort->readAll();
//    qDebug() << "on_ReadyRead:                  ------------- : " << _buffer;
    emit this->CheckBuffer();

    return;
}


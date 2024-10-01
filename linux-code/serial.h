#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include "defines.h"

class Serial : public QObject
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = nullptr);
    void Init();
    void Config();


    bool Serial_findport(QString myport);
    bool Serial_openport();
    void Serial_configport(QSerialPort::Parity parity = QSerialPort::NoParity, QSerialPort::DataBits databit = QSerialPort::Data8, QSerialPort::StopBits stopbit = QSerialPort::OneStop, QSerialPort::FlowControl flowcontrol = QSerialPort::NoFlowControl, QSerialPort::BaudRate baudrate = QSerialPort::Baud57600);
//    void Serial_configport(QSerialPort::Parity parity = QSerialPort::NoParity, QSerialPort::DataBits databit = QSerialPort::Data8, QSerialPort::StopBits stopbit = QSerialPort::OneStop, QSerialPort::FlowControl flowcontrol = QSerialPort::NoFlowControl, QSerialPort::BaudRate baudrate = QSerialPort::Baud115200);
    void Serial_closeport();
    void Serial_clearport();
    void Serial_ConnectToSlot(bool connect);

    bool Serial_writetoserial(QString message);

    QSerialPort *_serialPort;
    QString _serialportname;
    QByteArray _buffer;

    bool lock;
signals:
    void CheckBuffer();
public slots:
    void on_ReadyRead();

};

#endif // SERIAL_H

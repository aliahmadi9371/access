#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include <QApplication>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFile>
#include <QTimer>
#include <QThread>
#include <QProcess>

class mainclass : public QObject
{
    Q_OBJECT
public:
    explicit mainclass(QObject *parent = nullptr, QString fileName = QString("update.hex"));
    ~mainclass();

    void f_init();
    void f_config();

    void f_proc();

    bool f_findport(QString myport);
    bool f_openport();
    void f_configport(QSerialPort::Parity parity = QSerialPort::NoParity, QSerialPort::DataBits databit = QSerialPort::Data8, QSerialPort::StopBits stopbit = QSerialPort::OneStop, QSerialPort::FlowControl flowcontrol = QSerialPort::NoFlowControl, QSerialPort::BaudRate baudrate = QSerialPort::QSerialPort::Baud57600);
    void f_closeport();
    void f_clearport();
    void f_connectToSlot(bool connect);
    void f_writeToSerial(QString message);

    void f_setFileName();
    void f_countLines();

    void f_checkBuffer();

    void f_serialConfig();

    void f_sendLine(quint32 lineNumber);
    quint16 f_lineCkecksum(QString msg);

    int Last_percent{};

    QString m_fileName{};
    QFile m_file{};

    QSerialPort m_serialPort{};
    QByteArray m_receiptBuffer{};
    QList<QByteArray> m_lines;

    QTimer m_timerReceipt{nullptr};
    QTimer m_timerProc{nullptr};

    quint32 m_lineCount{};
    quint32 m_byteCount{};
    QString m_cmd{};
    qint32 m_procState{};

    void f_turnOnService();
    void f_turnOffService();
    void f_turnOffWatchdog();

//    QByteArray data{};


public slots:
    void f_on_readyRead();
    void f_on_timerReceiptTimerTimeout();
    void f_on_timerProcTimerTimeout();
signals:

};

#endif // MAINCLASS_H

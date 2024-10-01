#include "mainclass.h"

mainclass::mainclass(QObject *parent, QString fileName) : QObject(parent), m_fileName{fileName}
{
//    qDebug() << m_fileName;
//    f_init();

//    f_turnOffService();
//    f_turnOffWatchdog();
//    f_turnOffWatchdog();
//    f_turnOffWatchdog();
//    f_turnOffWatchdog();

    f_config();
    f_serialConfig();
    f_proc();


}

mainclass::~mainclass()
{

}

void mainclass::f_setFileName()
{
    m_fileName = "/opt/fota/bin/" + m_fileName;
    m_file.setFileName(m_fileName);
    qDebug() << m_fileName;
}

void mainclass::f_countLines()
{
    m_lineCount = 0;
    m_receiptBuffer.clear();
    if(m_file.open(QIODevice::ReadOnly)){
        while(!m_file.atEnd()){
            QByteArray arr = m_file.readLine();
            qDebug() << arr;
            m_lines.append(arr);
            ++m_lineCount;
        }
        qInfo() << "f_countLines--> " << "m_lineCount = " << m_lineCount;
        qInfo() << "f_countLines--> " << "m_lines.size()" << m_lines.size();
        m_file.close();
    }
    else{
        qWarning() << "f_countLines--> " << "m_file can not be opened: " << m_file.errorString();
    }

    m_byteCount = 0;
    if(m_file.open(QIODevice::ReadOnly)){
        while(!m_file.atEnd()){
//            qDebug() << m_file.readLine();
            char data{};
            m_file.read(&data, 1);
            ++m_byteCount;
        }
        qInfo() << "f_countLines--> " << "m_byteCount = " << m_byteCount;
        m_file.close();
    }
    else{
        qWarning() << "f_countLines--> " << "m_file can not be opened: " << m_file.errorString();
    }
}

void mainclass::f_proc()
{
    m_procState = -3;
    f_setFileName();
    f_countLines();


    m_byteCount = (m_byteCount - (m_lineCount * (11 + 4))) / 2;
    qInfo() << "f_proc--> " << "m_byteCount = " << m_byteCount;
    m_cmd = "{SET_FOT}:{\"id\":\"00\",\"data\":[\"1\",\"999999999999\",\"%1\",\"%2\",\"0XFFFFFFFF\"]}";
    m_cmd = m_cmd.arg(QString::number(m_lineCount)).arg(QString::number(m_byteCount));
    qInfo() << "f_proc--> " << "m_cmd = " << m_cmd;
    Last_percent = 0;
    m_timerProc.start();
    f_writeToSerial(m_cmd);
}

void mainclass::f_init()
{

}

void mainclass::f_config()
{
    m_timerReceipt.setSingleShot(true);
    m_timerReceipt.setInterval(5);
    m_timerReceipt.stop();
    QObject::connect(&m_timerReceipt, &QTimer::timeout, this, &mainclass::f_on_timerReceiptTimerTimeout, Qt::QueuedConnection);

    m_timerProc.setSingleShot(true);
    m_timerProc.setInterval(3000);
    m_timerProc.stop();
    QObject::connect(&m_timerProc, &QTimer::timeout, this, &mainclass::f_on_timerProcTimerTimeout, Qt::QueuedConnection);
}

void mainclass::f_serialConfig()
{
    if(f_findport("ttyS1")){
        if(f_openport()){
            f_configport(QSerialPort::NoParity, QSerialPort::Data8, QSerialPort::OneStop, QSerialPort::NoFlowControl, QSerialPort::Baud57600);
            f_clearport();
            f_connectToSlot(true);
        }
    }

}

bool mainclass::f_findport(QString myport)
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    bool ret = false;
    for(const QSerialPortInfo& info: ports){
        if(info.portName() == myport){
            qInfo() << "f_findport--> " << "ports name: " << info.portName();
            ret = true;
        }
//        qInfo() << "f_findport--> " << "ports name: " << info.portName();
    }
    return ret;
}

bool mainclass::f_openport()
{
    bool ret = true;
    QSerialPortInfo info("ttyS1");

    m_serialPort.setPort(info);
    if(!(m_serialPort.open(QIODevice::ReadWrite))){
        qWarning() << "f_openport--> " << m_serialPort.errorString();
        ret = false;
    }
    else{
        qInfo() << "f_openport--> " << "ports name: ttyS1";
    }
    return ret;
}

void mainclass::f_closeport()
{
    if(m_serialPort.isOpen()){
        m_serialPort.close();
        qInfo() << "pf_closeport--> " << "close serial port name ttyS1";
    }
}

void mainclass::f_configport(QSerialPort::Parity parity, QSerialPort::DataBits databit, QSerialPort::StopBits stopbit, QSerialPort::FlowControl flowcontrol, QSerialPort::BaudRate baudrate)
{
    m_serialPort.setParity(parity);
    m_serialPort.setDataBits(databit);
    m_serialPort.setStopBits(stopbit);
    m_serialPort.setFlowControl(flowcontrol);
    m_serialPort.setBaudRate(baudrate);
    qInfo() << "f_configport--> " << "can config serial port name ttyS1";
}


void mainclass::f_clearport()
{
    m_serialPort.clear(QSerialPort::AllDirections);
}

void mainclass::f_connectToSlot(bool connect)
{
    //qDebug() << "Serial_ConnectToSlot = " << connect;
    if(connect)
        QObject::connect(&m_serialPort, &QSerialPort::readyRead, this, &mainclass::f_on_readyRead, Qt::QueuedConnection);
    else
        QObject::disconnect(&m_serialPort, &QSerialPort::readyRead, this, &mainclass::f_on_readyRead);

    return;
}

void mainclass::f_on_readyRead()
{
    m_timerReceipt.start();

    m_receiptBuffer = m_receiptBuffer + m_serialPort.readAll();

//    if(m_receiptBuffer.at(m_receiptBuffer.size() - 1) == '\0'){
//        m_timerReceipt.stop();
//        f_checkBuffer();
//        m_receiptBuffer.clear();
//    }

    return;
}

void mainclass::f_writeToSerial(QString message)
{
    m_serialPort.write(message.toLocal8Bit(), message.length() + 1);
    m_serialPort.flush();
    //_serialPort->write("\0", 1);

//    m_serialPort->write(message.toLocal8Bit(), message.length());
//    m_serialPort->write("\0", 1);

    return;
}

void mainclass::f_on_timerReceiptTimerTimeout()
{
//    qInfo() << "f_on_timerReceiptTimerTimeout: " << m_receiptBuffer;
    m_timerProc.stop();
    m_timerProc.start();
    f_checkBuffer();
    m_timerReceipt.stop();
    m_receiptBuffer.clear();
}

void mainclass::f_on_timerProcTimerTimeout()
{
    qInfo() << "f_on_timerProcTimerTimeout: ";
//    f_turnOnService();
    QApplication::quit();

}

void mainclass::f_checkBuffer()
{
//    qInfo() << "f_checkBuffer: " << m_receiptBuffer;
    QString msg = m_receiptBuffer;

    /*if(msg.contains("{SET_FOT}:{\"status\":\"connect\"}") && m_procState == -3){
        m_procState = -2;
    }
    else */if(msg.contains("{FOT_REQ}:{\"num\":\"")){
        msg = msg.remove("{FOT_REQ}:{\"num\":\"");
        msg = msg.remove("\"}");

        int current_percent = static_cast<int>((static_cast<double>(msg.toUInt()) / static_cast<double>(m_lines.size())) * 100);
        if(Last_percent != current_percent){
            qDebug() << current_percent << ", " << msg.toUInt();
            Last_percent = current_percent;

        }
        f_sendLine(msg.toUInt());
    }
}

void mainclass::f_sendLine(quint32 lineNumber)
{
    if((lineNumber) <= (static_cast<quint32>(m_lines.size()))){
        QString pureLine(m_lines.at(lineNumber - 1));
//        qDebug() << pureLine;
        pureLine = pureLine.remove(0, 1);
        pureLine = pureLine.remove(pureLine.size() - 2, 2);
//        qDebug() << pureLine;

        QString send;
        send.clear();
        send = "{FOT_FOT}:{\"%1\",\"0x%2\",\"";
        send = send.arg(lineNumber, 5, 10, QLatin1Char('0')).arg(f_lineCkecksum(pureLine), 4, 16, QLatin1Char('0'));
        send = send + pureLine + "\"}";
//        qDebug() << "send = " << send;
//        QThread::msleep(30);
        f_writeToSerial(send);
    }
}

quint16 mainclass::f_lineCkecksum(QString msg)
{
    quint16 ret = 0;
    if(msg.size() % 2 == 0){

        for(int i{}; i < msg.size(); i = i + 2){
            QString tmp;
            tmp.clear();
            tmp.append(msg.at(i));
            tmp.append(msg.at(i + 1));
            quint16 byte = 0;
            byte = (static_cast<quint16>(tmp.toLocal8Bit().data()[0]) << 8) | (static_cast<quint16>(tmp.toLocal8Bit().data()[1]) << 0);

            ret ^= byte;

        }
    }
    else{
        qWarning() << "mainclassf_lineCkecksum: " << "line count is not even!";
    }

//    qDebug() << QString("0x%1").arg(ret, 4, 16, QLatin1Char('0'));
    return ret;
}

void mainclass::f_turnOnService()
{
    QProcess *process = new QProcess(this);
    QString proc = QString("sudo systemctl start IroTeam-Web.service");
    qInfo() << "f_turnOffWatchdog--> " << "cmd string = " << proc;

    process->start(proc);
    process->waitForFinished(-1);

    QStringList listout;
    QStringList listerr;

    QString err(process->readAllStandardError());
    if(err != ""){
        listerr.clear();
        listerr = err.split("\n");
        for(int i=0; i<listerr.count(); i++)
            if(listerr.at(i) != "")
                qWarning() << "f_turnOffService--> " << "stderr -> "<< listerr.at(i);
    }

    QString output(process->readAllStandardOutput());
    if(output != ""){
        listout.clear();
        QStringList listout = output.split("\n");
        for(int i=0; i<listout.count(); i++)
            if(listout.at(i) != ""){
                qInfo() << "f_turnOffService--> " << "stdout -> " << listout.at(i);
            }
    }

    delete process;
    process = nullptr;
}

void mainclass::f_turnOffService()
{
    QProcess *process = new QProcess(this);
    QString proc = QString("sudo systemctl stop IroTeam-Web.service");
    qInfo() << "f_turnOffWatchdog--> " << "cmd string = " << proc;

    process->start(proc);
    process->waitForFinished(-1);

    QStringList listout;
    QStringList listerr;

    QString err(process->readAllStandardError());
    if(err != ""){
        listerr.clear();
        listerr = err.split("\n");
        for(int i=0; i<listerr.count(); i++)
            if(listerr.at(i) != "")
                qWarning() << "f_turnOffService--> " << "stderr -> "<< listerr.at(i);
    }

    QString output(process->readAllStandardOutput());
    if(output != ""){
        listout.clear();
        QStringList listout = output.split("\n");
        for(int i=0; i<listout.count(); i++)
            if(listout.at(i) != ""){
                qInfo() << "f_turnOffService--> " << "stdout -> " << listout.at(i);
            }
    }

    delete process;
    process = nullptr;
}

void mainclass::f_turnOffWatchdog()
{
    QProcess *process = new QProcess(this);
    QString proc = QString("sudo /opt/fota/bin/watch.sh");
    qInfo() << "f_turnOffWatchdog--> " << "cmd string = " << proc;

    process->start(proc);
    process->waitForFinished(-1);

    QStringList listout;
    QStringList listerr;

    QString err(process->readAllStandardError());
    if(err != ""){
        listerr.clear();
        listerr = err.split("\n");
        for(int i=0; i<listerr.count(); i++)
            if(listerr.at(i) != "")
                qWarning() << "f_turnOffWatchdog--> " << "stderr -> "<< listerr.at(i);
    }

    QString output(process->readAllStandardOutput());
    if(output != ""){
        listout.clear();
        QStringList listout = output.split("\n");
        for(int i=0; i<listout.count(); i++)
            if(listout.at(i) != ""){
                qInfo() << "f_turnOffWatchdog--> " << "stdout -> " << listout.at(i);
            }
    }

    delete process;
    process = nullptr;
}

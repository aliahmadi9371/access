#ifndef I2CMASTERCLASS_H
#define I2CMASTERCLASS_H

#include <QObject>
#include "defines.h"
#include "i2c.h"

class I2cmasterClass : public QObject
{
    Q_OBJECT
public:
    explicit I2cmasterClass(QObject *parent = nullptr);
    void Init();
    void Config();

    void PrepareAddr(quint16 Addr, char & AddrH, char AddrL);
    void PreparePortData(quint16 Data, char & MSB, char & LSB);

    //rtc
    void GetDateTimeFromSysWithQt(bool log);
    void ShowDateTimeFromSys();

    void GetDateTimeFromRtcWithI2c(bool log);
    void SetDateTimeToRtcWithI2c(bool log);
    void ShowDateTimeNumbersRtc();

    void SetDateTimeToSysWithTerminal();
    bool SetDateTimeToSysWithNtpdateTerminal(bool log, QString URL);


    QProcess *myprocess;

    I2c* _i2c;

    QString date_sys;
    QString time_sys;

    QString date_rtc;
    QString time_rtc;

    int s_sec; //0-59
    int s_min; //0-59
    int s_hour;//0-23

    int s_week_day; //1-7
    int s_day; //1-31
    int s_month; //1-12
    int s_year; //0-99

    int r_sec; //0-59
    int r_min; //0-59
    int r_hour;//0-23

    int r_week_day; //1-7
    int r_day; //1-31
    int r_month; //1-12
    int r_year; //0-99


signals:

};

#endif // I2CMASTERCLASS_H

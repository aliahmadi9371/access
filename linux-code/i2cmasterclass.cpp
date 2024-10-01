#include "i2cmasterclass.h"

I2cmasterClass::I2cmasterClass(QObject *parent) : QObject(parent)
{
    Init();
    Config();
}

void I2cmasterClass::Init()
{
    _i2c = nullptr;
    myprocess = nullptr;

    s_sec = -1; //0-59
    s_min = -1; //0-59
    s_hour = -1;//0-23

    s_week_day = -1; //1-7
    s_day = -1; //1-31
    s_month = -1; //1-12
    s_year = -1; //0-99

    r_sec = -1; //0-59
    r_min = -1; //0-59
    r_hour = -1;//0-23

    r_week_day = -1; //1-7
    r_day = -1; //1-31
    r_month = -1; //1-12
    r_year = -1; //0-99

    date_sys = "";
    time_sys = "";

    date_rtc = "";
    time_rtc = "";
    return;
}

void I2cmasterClass::Config()
{
    _i2c = new I2c(this);
    myprocess = new QProcess(this);
    return;
}

//#################################################################################################################
//#################################################################################################################
//get date time from Qt
void I2cmasterClass::GetDateTimeFromSysWithQt(bool log)
{
    s_year = QDateTime::currentDateTime().toString("yyyy").toInt();
    s_month = QDateTime::currentDateTime().toString("MM").toInt();
    s_day = QDateTime::currentDateTime().toString("dd").toInt();

    s_week_day = QDate::currentDate().dayOfWeek();

    s_hour = QDateTime::currentDateTime().toString("hh").toInt();
    s_min = QDateTime::currentDateTime().toString("mm").toInt();
    s_sec = QDateTime::currentDateTime().toString("ss").toInt();

    if(log){
        date_sys = QString("%1%2%3").arg(s_year,4,10,QLatin1Char('0'))
                           .arg(s_month, 2, 10, QLatin1Char('0'))
                           .arg(s_day, 2, 10, QLatin1Char('0'));

        time_sys = QString("%1:%2:%3").arg(s_hour, 2, 10, QLatin1Char('0'))
                                      .arg(s_min, 2, 10, QLatin1Char('0'))
                                      .arg(s_sec, 2, 10, QLatin1Char('0'));

//        #ifdef QT_DEBUG
                if(log)
                    qInfo() << I2CMASTER << "GetDateTimeFromSysWithQt--> " << "system time: " << date_sys << " DoW: " << s_week_day << " - " << time_sys;
//        #endif
    }

    return;
}

void I2cmasterClass::ShowDateTimeFromSys()
{
    qInfo() << I2CMASTER << "ShowDateTimeFromSysWithQt--> " << "sys year is: " << s_year << " , "
                                                           << "sys month is: " << s_month << " , "
                                                           << "sys day is: " << s_day << " , "
                                                           << "sys week_day is: " << s_week_day << " , "
                                                           << "sys hour is: " << s_hour << " , "
                                                           << "sys min is: " << s_min << " , "
                                                           << "sys sec is: " << s_sec;
    return;
}
//#################################################################################################################
//#################################################################################################################

//#################################################################################################################
//#################################################################################################################
//get and set to ds1307 0x86 with I2C
void I2cmasterClass::GetDateTimeFromRtcWithI2c(bool log)
{
//    qDebug() << gl_Ini_Ds1307DevAddr;
    if(_i2c->I2c_read_all_rtc(gl_Ini_Ds1307DevAddr, RTC_YEAR_REG, RTC_MOTNh_REG, RTC_DAY_REG, RTC_WEEK_REG, RTC_HOUR_REG, RTC_MINUTE_REG, RTC_SEC_REG, r_year, r_month, r_day, r_week_day, r_hour, r_min, r_sec)){
        int year1 = r_year & 0b00001111;
        int year2 = (r_year & 0b11110000) >> 4;
        r_year = year1 + (10 * year2);
        r_year = 2000 + r_year; //critical


        int month1 = r_month & 0b00001111;
        int month2 = (r_month & 0b00010000) >> 4;
        r_month = month1 + 10 * month2;

        int day1 = r_day & 0b00001111;
        int day2 = (r_day & 0b00110000) >> 4;
        r_day = day1 + 10 * day2;

        r_week_day = r_week_day & 0b00000111;

        int hour1 = r_hour & 0b00001111;
        int hour2;
        bool _1224 = (r_hour & 0b0100000) >> 5; //12 or 24 (12 is high)
        if(_1224){ //12
//            qDebug() << "12 hours AM PM";
            bool PM = (r_hour & 0b00100000) >> 4; //PM is high
            if(PM){
//                    qDebug() << "PM";
                hour2 = ((r_hour & 0b00010000) >> 4) + 12;
            }
            else{
//                qDebug() << "AM";
                hour2 = (r_hour & 0b00010000) >> 4;
            }
        }
        else{ //24
//            qDebug() << "24 hours";
            hour2 = (r_hour & 0b00110000) >> 4;
        }
        r_hour = hour1 + 10 * hour2;

        int min1 = r_min & 0b00001111;
        int min2 = (r_min & 0b01110000) >> 4;
        r_min = min1 + 10 * min2;

        int sec1 = r_sec & 0b00001111;
        int sec2 = (r_sec & 0b01110000) >> 4;
        r_sec = sec1 + 10 * sec2;


        if(log){
            date_rtc = QString("%1/%2/%3").arg(r_year, 4, 10, QLatin1Char('0'))
                                          .arg(r_month, 2, 10, QLatin1Char('0'))
                                          .arg(r_day, 2, 10, QLatin1Char('0'));


            time_rtc = QString("%1:%2:%3").arg(r_hour, 2, 10, QLatin1Char('0'))
                                          .arg(r_min, 2, 10, QLatin1Char('0'))
                                          .arg(r_sec, 2, 10, QLatin1Char('0'));

            QDateTime m = QDateTime::currentDateTime();
            qInfo() << I2CMASTER << "GetDateTimeFromRtcWithI2c--> " << "sys time: " << m.toString("yyyy/MM/dd") << "DoW: " << m.date().dayOfWeek() << " - " << m.toString("hh:mm:ss");
            qInfo() << I2CMASTER << "GetDateTimeFromRtcWithI2c--> " << "rtc time: " << date_rtc << "DoW: " << r_week_day << " - " <<  time_rtc;
        }


    }
    else{
            qWarning() << I2CMASTER << "GetDateTimeFromRtcWithI2c--> " << "rtc time: " << "can not read all";
    }
    return;
}

void I2cmasterClass::SetDateTimeToRtcWithI2c(bool log)
{
    GetDateTimeFromSysWithQt(log);

    int year2 = (s_year % 100) / 10;
    int year1 = (s_year % 100) % 10;
//    qDebug() << year2 << year1;
    int ryear = year1 + ( (year2 << 4) & 0b11110000);
//    qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> " << "year2 = " << year2 << ", year1 = " << year1 << ", ryear = " << ryear << QString("(0b%1)").arg(ryear,8,2,QLatin1Char('0'));

    int month2 = s_month / 10;
    int month1 = s_month % 10;
//    qDebug() << month2 << month1;
    int rmonth = month1 + ( (month2 << 4) & 0b11110000);
//    qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> " << "month2 = " << month2 << ", month1 = " << month1 << ", rmonth = " << rmonth << QString("(0b%1)").arg(rmonth,8,2,QLatin1Char('0'));


    int day2 = s_day / 10;
    int day1 = s_day % 10;
    //    qDebug() << day2 << day1;
    int rday = day1 + ( (day2 << 4) & 0b11110000);
//    qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> " << "day2 = " << day2 << ", day1 = " << day1 << ", rday = " << rday << QString("(0b%1)").arg(rday,8,2,QLatin1Char('0'));

    int wd2 = s_week_day / 10;
    int wd1 = s_week_day % 10;
//    qDebug() << wd2 << wd1;
    int rwd = wd1 + ( (wd2 << 4) & 0b11110000);
    rwd = rwd & 0b00000111;
//    qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> " << "wd2 = " << wd2 << ", wd1 = " << wd1 << ", rwd = " << rwd << QString("(0b%1)").arg(rwd,8,2,QLatin1Char('0'));
//

    int hour2 = s_hour / 10;
    int hour1 = s_hour % 10;
//    qDebug() << hour2 << hour1;
    int rhour = hour1 + ( (hour2 << 4) & 0b11110000 );
//    qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> " << "hour2 = " << hour2 << ", hour1 = " << hour1 << ", rhour = " << rhour << QString("(0b%1)").arg(rhour,8,2,QLatin1Char('0'));

    int min2 = s_min / 10;
    int min1 = s_min % 10;
    int rmin = min1 + ( (min2 << 4) & 0b11110000 );
//        qDebug() << rmin;
//    qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> " << "min2 = " << min2 << ", min1 = " << min1 << ", rmin = " << rmin << QString("(0b%1)").arg(rmin,8,2,QLatin1Char('0'));

    int sec2 = s_sec / 10;
    int sec1 = s_sec % 10;
    int rsec = sec1 + (( sec2 << 4) & 0b01110000 ); //CH = 0
//    qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> " << "sec2 = " << sec2 << ", sec1 = " << sec1 << ", rsec = " << rsec << QString("(0b%1)").arg(rsec,8,2,QLatin1Char('0'));

//    rtc_show_datetime_numbers_sys();

//    if(_i2c->I2c_write_wiring_byte(gl_Db_Ds1307DevAddr, 0x06, ryear) &&
//       _i2c->I2c_write_wiring_byte(gl_Db_Ds1307DevAddr, 0x05, rmonth) &&
//       _i2c->I2c_write_wiring_byte(gl_Db_Ds1307DevAddr, 0x04, rday) &&
//       _i2c->I2c_write_wiring_byte(gl_Db_Ds1307DevAddr, 0x03, rwd) &&
//       _i2c->I2c_write_wiring_byte(gl_Db_Ds1307DevAddr, 0x02, rhour) &&
//       _i2c->I2c_write_wiring_byte(gl_Db_Ds1307DevAddr, 0x01, rmin) &&
//       _i2c->I2c_write_wiring_byte(gl_Db_Ds1307DevAddr, 0x00, rsec))

//    qDebug() << "write";
    if(_i2c->I2c_write_all_rtc(gl_Ini_Ds1307DevAddr, RTC_YEAR_REG, RTC_MOTNh_REG, RTC_DAY_REG, RTC_WEEK_REG, RTC_HOUR_REG, RTC_MINUTE_REG, RTC_SEC_REG, ryear, rmonth, rday, rwd, rhour, rmin, rsec)){

//        qDebug() << "write";
//        #ifdef QT_DEBUG
            if(log)
                qInfo() << I2CMASTER << "SetDateTimeToRtcWithI2c--> write data to rtc";
//        #endif
    }
    return;
}

void I2cmasterClass::ShowDateTimeNumbersRtc()
{
    qInfo() << I2CMASTER << "ShowDateTimeNumbersRtc--> " << "rtc year is: " << r_year << " , "
                                                           << "rtc month is: " << r_month << " , "
                                                           << "rtc day is: " << r_day << " , "
                                                           << "rtc week_day is: " << r_week_day << " , "
                                                           << "rtc hour is: " << r_hour << " , "
                                                           << "rtc min is: " << r_min << " , "
                                                           << "rtc sec is: " << r_sec;
    return;
}
//#################################################################################################################
//#################################################################################################################


//get/set datetime to system with termina
//date: (yyyy/mm/dd) (+%Y/%m/%d)
//time (hh:MM:ss) (+%T)
//datetime (yyyy/mm/dd hh:MM:ss) ("+%Y/%m/%d %T")
//#################################################################################################################
//#################################################################################################################
void I2cmasterClass::SetDateTimeToSysWithTerminal()
{
    QProcess *process = new QProcess(this);
    QString proc = QString("sudo date \"+%Y/%m/%d %T\" --set=\"%1 %2\"").arg(date_rtc).arg(time_rtc);
    qInfo() << I2CMASTER << "SetDateTimeToSysWithTerminal--> " << "cmd string = " << proc;

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
            qWarning() << I2CMASTER << "SetDateTimeToSysWithTerminal--> " << "stderr -> "<< listerr.at(i);
    }

    QString output(process->readAllStandardOutput());
    if(output != ""){
        listout.clear();
        QStringList listout = output.split("\n");
        for(int i=0; i<listout.count(); i++)
            if(listout.at(i) != "")
            qInfo() << I2CMASTER << "SetDateTimeToSysWithTerminal--> " << "stdout -> " << listout.at(i);
    }

    process->deleteLater();

    return;
}

bool I2cmasterClass::SetDateTimeToSysWithNtpdateTerminal(bool log, QString URL)
{
    bool ret = true;
    QProcess *process = new QProcess(this);
    QString proc = QString("sudo ntpdate " + URL);
    #ifdef QT_DEBUG
        qInfo() << I2CMASTER << "SetDateTimeToSysWithNtpdateTerminal--> " << "cmd string = " << proc;
    #endif

    //myprocess->start(proc);
    process->start(proc);
    process->waitForFinished(-1);

    QStringList listout;
    QStringList listerr;

    QString err(process->readAllStandardError());
    if(err != ""){
        ret = false;
        listerr.clear();
        listerr = err.split("\n");
        for(int i=0; i<listerr.count(); i++)
            if(listerr.at(i) != "")
                qWarning() << I2CMASTER << "SetDateTimeToSysWithNtpdateTerminal--> " << "stderr -> "<< listerr.at(i);
    }

    if(log){
//    #ifdef QT_DEBUG
        QString output(process->readAllStandardOutput());
        if(output != ""){
            listout.clear();
            QStringList listout = output.split("\n");
            for(int i=0; i<listout.count(); i++)
                if(listout.at(i) != "")
                    qInfo() << I2CMASTER << "SetDateTimeToSysWithNtpdateTerminal--> " << "stdout -> " << listout.at(i);
        }
    }
//    #endif

//    qDebug() << ret;
    process->deleteLater();
    return ret;
}




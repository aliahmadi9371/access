#include "i2c.h"

I2c::I2c(QObject *parent) : QObject(parent)
{
    _fd = -1;
    _open_close = false;
}

bool I2c::I2c_open_wirngig(int dev_addr)
{
    bool ret = true;
    //qDebug() << gl_Ini_I2cDevicePort;
    //gl_Ini_I2cDevicePort = "/dev/i2c-0";
    _fd = wiringPiI2CSetupInterface(gl_Ini_I2cDevicePort.toLocal8Bit().data(), dev_addr);
    if(_fd < 0){
        qWarning() << I2CCLASS << "I2c_open_wirngig--> " << strerror(errno);
        ret = false;
    }
//    else{
//    #ifdef QT_DEBUG
//            qDebug() << I2CCLASS << "I2c_open_wirngig--> fd: " << _fd;
//    #endif
//    }

    _open_close = ret;
    return ret;
}

bool I2c::I2c_open_file(int dev_addr)
{
    bool ret = true;
    _fd = open(gl_Ini_I2cDevicePort.toLocal8Bit().data(), O_RDWR);
    if(_fd < 0){
        qWarning() << I2CCLASS << "I2c_open_file--> " << strerror(errno);
        ret = false;
    }
    else{
        if(ioctl(_fd, I2C_SLAVE, dev_addr) < 0)
        {
            qWarning() << I2CCLASS << "I2c_open_file--> " << strerror(errno);
            I2c_close();
            ret = false;
        }
    }
    _open_close = ret;
    return ret;
}

void I2c::I2c_close()
{
    close(_fd);
    _open_close = false;
    _fd = -1;
}

bool I2c::I2c_read_wiring_byte(int dev_addr, int & data)
{
    bool ret = false;
    if(!_open_close){
        if(I2c_open_wirngig(dev_addr)){
            data = wiringPiI2CRead(_fd);
            if(data < 0){
                qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " can not read 1 byte from port";
            }
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_read_wiring_byte(int dev_addr, char addr, int & data)
{
    bool ret = false;
    if(!_open_close){
        if(I2c_open_wirngig(dev_addr)){
            data = wiringPiI2CReadReg8(_fd, addr);
            if(data < 0){
                qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " can not read 1 byte from port with addr " << QString("0x%1").arg(addr,2,16,QLatin1Char('0'));

            }
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_read_wiring_2bytes(int dev_addr, char addr, int & data)
{
    bool ret = false;
    if(!_open_close){
        if(I2c_open_wirngig(dev_addr)){
            data = wiringPiI2CReadReg16(_fd, addr);
            if(data < 0){
                qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " can not read 2 byte from port with addr " << QString("0x%1").arg(addr,2,16,QLatin1Char('0'));

            }
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_write_wiring_byte(int dev_addr, int data)
{
    bool ret = false;
    if(!_open_close){
        if(I2c_open_wirngig(dev_addr)){
            int res = wiringPiI2CWrite(_fd, data);
            if(res < 0){
                qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " can not write 1 byte to port";
            }
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_write_wiring_byte(int dev_addr, char addr, int data)
{
    bool ret = false;
    if(!_open_close){
        if(I2c_open_wirngig(dev_addr)){
            int res = wiringPiI2CWriteReg8(_fd, addr, data);
            if(res < 0){
                qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " can not write 1 byte to port with addr " << QString("0x%1").arg(addr,2,16,QLatin1Char('0'));

            }
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
        else{

        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_write_wiring_2bytes(int dev_addr, char addr, int data)
{
    bool ret = false;
    if(!_open_close){
        if(I2c_open_wirngig(dev_addr)){
            int res = wiringPiI2CWriteReg16(_fd, addr, data);
            if(res < 0){
                qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " can not write 2 byte to port with addr " << QString("0x%1").arg(addr,2,16,QLatin1Char('0'));
                I2c_close();
            }
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_wiring_byte--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_write_all_rtc(int dev_addr, int addr_year, int addr_month, int addr_day, int addr_wd, int addr_hour, int addr_min, int addr_sec, int ryear, int rmonth, int rday, int rwd, int rhour, int rmin, int rsec)
{
    bool ret = false;
//    int control_addr = 0x07;
//    int control = 0b00000011;
    if(!_open_close){
        if(I2c_open_wirngig(dev_addr)){

//            qDebug() << "******************************************** ryear" << QString("0b%1").arg(ryear,8,2,QLatin1Char('0')) << QString::number(ryear);
//            qDebug() << "******************************************** rmonth" << QString("0b%1").arg(rmonth,8,2,QLatin1Char('0')) << QString::number(rmonth);
//            qDebug() << "******************************************** rday" << QString("0b%1").arg(rday,8,2,QLatin1Char('0')) << QString::number(rday);
//            qDebug() << "******************************************** rwd" << QString("0b%1").arg(rwd,8,2,QLatin1Char('0')) << QString::number(rwd);
//            qDebug() << "******************************************** rhour" << QString("0b%1").arg(rhour,8,2,QLatin1Char('0')) << QString::number(rhour);
//            qDebug() << "******************************************** rmin" << QString("0b%1").arg(rmin,8,2,QLatin1Char('0')) << QString::number(rmin);
//            qDebug() << "******************************************** rsec" << QString("0b%1").arg(rsec,8,2,QLatin1Char('0')) << QString::number(rsec);
//            qDebug() << "******************************************** control" << QString("0b%1").arg(control,8,2,QLatin1Char('0')) << QString::number(control);

//            qDebug() << wiringPiI2CWriteReg8(_fd, addr_year, ryear);
//            qDebug() << wiringPiI2CWriteReg8(_fd, addr_month, rmonth);
//            qDebug() << wiringPiI2CWriteReg8(_fd, addr_day, rday);
//            qDebug() << wiringPiI2CWriteReg8(_fd, addr_wd, rwd);
//            qDebug() << wiringPiI2CWriteReg8(_fd, addr_hour, rhour);
//            qDebug() << wiringPiI2CWriteReg8(_fd, addr_min, rmin);
//            qDebug() << wiringPiI2CWriteReg8(_fd, addr_sec, rsec);
//            wiringPiI2CWriteReg8(_fd, control_addr, control);

            int rett = -1;
            rett = wiringPiI2CWriteReg8(_fd, addr_year, ryear);
            if(rett >= 0){
//                qDebug() << "write year";

                rett = -1;
                rett = wiringPiI2CWriteReg8(_fd, addr_month, rmonth);

                if(rett >= 0){
//                    qDebug() << "write month";

                    rett = -1;
                    rett = wiringPiI2CWriteReg8(_fd, addr_day, rday);

                    if(rett >= 0){
//                        qDebug() << "write day";

                        rett = -1;
                        rett = wiringPiI2CWriteReg8(_fd, addr_wd, rwd);

                        if(rett >= 0){
//                            qDebug() << "write week day";

                            rett = -1;
                            rett = wiringPiI2CWriteReg8(_fd, addr_hour, rhour);

                            if(rett >= 0){
//                                qDebug() << "write hour";

                                rett = -1;
                                rett = wiringPiI2CWriteReg8(_fd, addr_min, rmin);

                                if(rett >= 0){
//                                    qDebug() << "write min";

                                    rett = -1;
                                    rett = wiringPiI2CWriteReg8(_fd, addr_sec, rsec);

                                    if(rett >= 0){
//                                        qDebug() << "write sec";
                                        ret = true;
                                    }
                                    else{
                                        qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << "can not write sec: " << rsec << QString("(0b%1)").arg(rsec, 8, 2, QLatin1Char('0')) << " in address: " << QString("0x%1").arg(addr_sec, 2, 16, QLatin1Char('0'));
                                    }
                                }
                                else{
                                    qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << "can not write min: " << rmin << QString("(0b%1)").arg(rmin, 8, 2, QLatin1Char('0')) << " in address: " << QString("0x%1").arg(addr_min, 2, 16, QLatin1Char('0'));
                                }
                            }
                            else{
                                qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << "can not write hour: " << rhour << QString("(0b%1)").arg(rhour, 8, 2, QLatin1Char('0')) << " in address: " << QString("0x%1").arg(addr_hour, 2, 16, QLatin1Char('0'));
                            }
                        }
                        else{
                            qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << "can not write week day: " << rwd << QString("(0b%1)").arg(rwd, 8, 2, QLatin1Char('0')) << " in address: " << QString("0x%1").arg(addr_wd, 2, 16, QLatin1Char('0'));
                        }
                    }
                    else{
                        qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << "can not write day: " << rday << QString("(0b%1)").arg(rday, 8, 2, QLatin1Char('0')) << " in address: " << QString("0x%1").arg(addr_day, 16, 2, QLatin1Char('0'));
                    }
                }
                else{
                    qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << "can not write month: " << rmonth << QString("(0b%1)").arg(rmonth, 8, 2, QLatin1Char('0')) << " in address: " << QString("0x%1").arg(addr_month, 2, 16, QLatin1Char('0'));
                }
            }
            else{
                qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << "can not write year: " << ryear << QString("(0b%1)").arg(ryear, 8, 2, QLatin1Char('0')) << " in address: " << QString("0x%1").arg(addr_year, 2, 16, QLatin1Char('0'));
            }

//            if(
//               wiringPiI2CWriteReg8(_fd, addr_year, ryear) >= 0 &&
//               wiringPiI2CWriteReg8(_fd, addr_month, rmonth) >= 0 &&
//               wiringPiI2CWriteReg8(_fd, addr_day, rday) >= 0 &&
//               wiringPiI2CWriteReg8(_fd, addr_wd, rwd) >= 0 &&
//               wiringPiI2CWriteReg8(_fd, addr_hour, rhour) >= 0 &&
//               wiringPiI2CWriteReg8(_fd, addr_min, rmin) >= 0 &&
//               wiringPiI2CWriteReg8(_fd, addr_sec, rsec) >= 0 //&&
////               wiringPiI2CWriteReg8(_fd, 0x/*0*/7, 0b10010000) >= 0
//              ){
//                ret = true;
//            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_write_all_rtc--> " << gl_Ini_I2cDevicePort << " is under operation";
    }
    return ret;
}

bool I2c::I2c_read_all_rtc(int dev_addr, int addr_year, int addr_month, int addr_day, int addr_wd, int addr_hour, int addr_min, int addr_sec, int &ryear, int &rmonth, int &rday, int &rwd, int &rhour, int &rmin, int &rsec)
{
    bool ret = false;
//    int control_addr = 0x07;
//    int control = 0;
    if(!_open_close){
//        qDebug() << "close";
        if(I2c_open_wirngig(dev_addr)){

            ryear = wiringPiI2CReadReg8(_fd, addr_year);
//            qDebug() << "******************************************** ryear" << QString("0b%1").arg(ryear,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** ryear" << QString("%1").arg(ryear,2,10,QLatin1Char('0'));
            rmonth = wiringPiI2CReadReg8(_fd, addr_month);
//            qDebug() << "******************************************** rmonth" << QString("0b%1").arg(rmonth,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** rmonth" << QString("%1").arg(rmonth,2,10,QLatin1Char('0'));
            rday = wiringPiI2CReadReg8(_fd, addr_day);
//            qDebug() << "******************************************** rday" << QString("0b%1").arg(rday,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** rday" << QString("%1").arg(rday,2,10,QLatin1Char('0'));
            rwd = wiringPiI2CReadReg8(_fd, addr_wd);
//            qDebug() << "******************************************** rwd" << QString("0b%1").arg(rwd,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** rwd" << QString("%1").arg(rwd,2,10,QLatin1Char('0'));
            rhour = wiringPiI2CReadReg8(_fd, addr_hour);
//            qDebug() << "******************************************** rhour" << QString("0b%1").arg(rhour,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** rhour" << QString("%1").arg(rhour,2,10,QLatin1Char('0'));
            rmin = wiringPiI2CReadReg8(_fd, addr_min);
//            qDebug() << "******************************************** rmin" << QString("0b%1").arg(rmin,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** rmin" << QString("%1").arg(rmin,2,10,QLatin1Char('0'));
            rsec= wiringPiI2CReadReg8(_fd, addr_sec);
//            qDebug() << "******************************************** rsec" << QString("0b%1").arg(rsec,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** rsec" << QString("%1").arg(rsec,2,10,QLatin1Char('0'));
//            int control = wiringPiI2CReadReg8(_fd, RTC_CTRL_REG);
//            qDebug() << "******************************************** control" << QString("0b%1").arg(control,8,2,QLatin1Char('0'));
//            qDebug() << "******************************************** control" << QString("%1").arg(control,2,10,QLatin1Char('0'));
            if(ryear >= 0 && rmonth >= 0 && rday >= 0 && rwd >= 0 && rhour >= 0 && rmin >=0 && rsec >= 0)
                ret = true;
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_all_rtc--> " << gl_Ini_I2cDevicePort << " is under operation";
    }
    return ret;
}

bool I2c::I2c_read_file(int dev_addr, char * buf, int len)
{
    bool ret = false;

    if(!_open_close){
        if(I2c_open_file(dev_addr)){
            struct i2c_rdwr_ioctl_data msg_rdwr;
            struct i2c_msg i2cmsg;

            i2cmsg.addr  = dev_addr;
            i2cmsg.flags = I2C_M_RD;
            i2cmsg.len   = len;
            i2cmsg.buf   = (__u8*)buf;

            msg_rdwr.msgs = &i2cmsg;
            msg_rdwr.nmsgs = 1;

            if(ioctl(_fd, I2C_RDWR, &msg_rdwr) < 0)
                qWarning() << I2CCLASS << "I2c_read_file--> " <<  strerror(errno);
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_file--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_write_file(int dev_addr, char * buf, int len)
{
    bool ret = false;

    if(!_open_close){
        if(I2c_open_file(dev_addr)){
            struct i2c_rdwr_ioctl_data msg_rdwr;
            struct i2c_msg i2cmsg;


            i2cmsg.addr  = dev_addr;
            i2cmsg.flags = 0;
            i2cmsg.len   = len;
            i2cmsg.buf   = (__u8*)buf;

            msg_rdwr.msgs = &i2cmsg;
            msg_rdwr.nmsgs = 1;

            if(ioctl(_fd, I2C_RDWR, &msg_rdwr) < 0)
                qWarning() << I2CCLASS << "I2c_write_file--> " <<  strerror(errno);
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_file--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_read_file_address(int dev_addr, char addrhi, char addrlow, char * buf, int len)
{
    bool ret = false;

    if(!_open_close){
        if(I2c_open_file(dev_addr)){

            if(I2c_read_file_setaddress(dev_addr, addrhi, addrlow)){
                struct i2c_rdwr_ioctl_data msg_rdwr;
                struct i2c_msg             i2cmsg;

                i2cmsg.addr  = dev_addr;
                i2cmsg.flags = I2C_M_RD;
                i2cmsg.len   = len;
                i2cmsg.buf   = (__u8*)buf;

                msg_rdwr.msgs = &i2cmsg;
                msg_rdwr.nmsgs = 1;

                if(ioctl(_fd, I2C_RDWR, &msg_rdwr) < 0){
                    qWarning() << I2CCLASS << "I2c_read_file_address--> " <<  strerror(errno);
                }
                else{
                    QThread::msleep(I2C_RW_DELAY_MS);
                    ret = true;
                }

                I2c_close();
            }
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_read_file--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}

bool I2c::I2c_read_file_setaddress(int dev_addr, char addrhi, char addrlow)
{
    struct i2c_rdwr_ioctl_data msg_rdwr;
    struct i2c_msg i2cmsg;

    char my_buf[2];
    my_buf[0] = addrhi;
    my_buf[1] = addrlow;

    i2cmsg.addr  = dev_addr;
    i2cmsg.flags = 0;
    i2cmsg.len   = 2;
    i2cmsg.buf   = (__u8*)my_buf;

    msg_rdwr.msgs = &i2cmsg;
    msg_rdwr.nmsgs = 1;

    if(ioctl(_fd, I2C_RDWR, &msg_rdwr) < 0){
        qWarning() << I2CCLASS << "I2c_read_file_setaddress--> " <<  strerror(errno);
        QThread::msleep(I2C_RW_DELAY_MS);
        return false;
    }
    else{
        return true;
    }
}

bool I2c::I2c_write_file_address(int dev_addr, char addrhi, char addrlow, char * buf, int len)
{
    bool ret = false;

    if(!_open_close){
        if(I2c_open_file(dev_addr)){
            struct i2c_rdwr_ioctl_data msg_rdwr;
            struct i2c_msg i2cmsg;

            char my_buf[len + 2];
            my_buf[0] = addrhi;
            my_buf[1] = addrlow;
            for(int i= 0; i < len; i++)
               my_buf[2+i] = buf[i];

            i2cmsg.addr  = dev_addr;
            i2cmsg.flags = 0;
            i2cmsg.len   = 2 + len;
            i2cmsg.buf   = (__u8*)my_buf;

            msg_rdwr.msgs = &i2cmsg;
            msg_rdwr.nmsgs = 1;

            if(ioctl(_fd, I2C_RDWR, &msg_rdwr) < 0)
                qWarning() << I2CCLASS << "I2c_write_file_address--> " <<  strerror(errno);
            else{
                QThread::msleep(I2C_RW_DELAY_MS);
                ret = true;
            }
            I2c_close();
        }
    }
    else{
        qWarning() << I2CCLASS << "I2c_write_file_address--> " << gl_Ini_I2cDevicePort << " is under operation";
    }

    return ret;
}


#ifndef I2C_H
#define I2C_H

#include <QObject>
#include "defines.h"

using namespace std;
class I2c : public QObject
{
    Q_OBJECT
public:
    explicit I2c(QObject *parent = nullptr);
    bool _open_close;

    bool I2c_open_wirngig(int dev_addr);
    bool I2c_open_file(int dev_addr);
    void I2c_close();

    bool I2c_read_wiring_byte(int dev_addr, int & data);
    bool I2c_read_wiring_byte(int dev_addr, char addr, int & data);
    bool I2c_read_wiring_2bytes(int dev_addr, char addr, int & data);

    bool I2c_write_wiring_byte(int dev_addr, int data);
    bool I2c_write_wiring_byte(int dev_addr, char addr, int data);
    bool I2c_write_wiring_2bytes(int dev_addr, char addr, int data);

    bool I2c_read_file(int dev_addr, char * buf, int len);
    bool I2c_write_file(int dev_addr, char * buf, int len);

    bool I2c_read_file_address(int dev_addr, char addrhi, char addrlow, char * buf, int len);
    bool I2c_write_file_address(int dev_addr, char addrhi, char addrlow, char * buf, int len);
    bool I2c_read_file_setaddress(int dev_addr, char addrhi, char addrlow);

    bool I2c_write_all_rtc(int dev_addr, int addr_year, int addr_month, int addr_day, int addr_wd, int addr_hour, int addr_min, int addr_sec, int ryear, int rmonth, int rday, int rwd, int rhour, int rmin, int rsec);
    bool I2c_read_all_rtc(int dev_addr, int addr_year, int addr_month, int addr_day, int addr_wd, int addr_hour, int addr_min, int addr_sec, int &ryear, int &rmonth, int &rday, int &rwd, int &rhour, int &rmin, int &rsec);

    int _fd;

signals:

};

#endif // I2C_H

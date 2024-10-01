#ifndef DEFINES_H
#define DEFINES_H
//******************************************all defines and global variables******************************************
//--------------------------------------------------------------------
//includes
    //Qt main
    #include <QCoreApplication>
    #include <QApplication>
    #include <QThread>

    //Qt Network
    #include <QNetworkInterface>

    //Qt tools
    #include <QDebug>
    #include <QString>
    #include <QDateTime>
    #include <QSettings>
    #include <QFile>
    #include <QTextStream>
    #include <QDir>
    #include <QHash>
    #include <QProcess>
    #include <QTimer>
    #include <QMutex>

    //Qt Sql Database
    #include <QSqlDatabase>
    #include <QDebug>
    #include <QSqlQuery>
    #include <QSqlError>

    //Qt Mqtt
    #include <QtMqtt/QtMqtt>

    //Qt Queue
    #include <QQueue>
    #include <QMutex>

    //Qt Serial
    #include <QSerialPort>
    #include <QSerialPortInfo>

    //Qt Json
    #include <QJsonDocument>
    #include <QJsonObject>

    //Qt i2c
    #include <stdio.h>
    #include <stdlib.h>
    #include <linux/i2c-dev.h>
    #include <fcntl.h>
    #include <string.h>
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <errno.h>
    #include <linux/i2c.h>

    //Qt WiringPi
    #include "wiringPiI2C.h"
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//show version
    #define APP_NAME "Access-V1-0-0"
    #define COMPANY_NAME "IroTeam"
    #define APP_PROFILE "AccessControl"
    #define APP_VERSION "V1-0-0"
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//sizes
    //log config
        //size and if log
        #define MAIN_LOGSIZE 1024 * 10000
        #ifdef QT_DEBUG
           #define MAIN_LOG_TO_FILE false
        #else
          #define MAIN_LOG_TO_FILE true
        #endif

    //size of msg queue from cy
        #define CY_MSG_SIZE 100
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//Log for start if all functions of each class
    #define MAIN "Main: "
    #define MAINCLASS   "MainClass: "
    #define I2CMASTER   "I2cMClass: "
    #define MQTTCLASS   "MqttClass: "
    #define AUTHCLASS   "AuthClass: "
    #define TABLE       "TableClass: "
    #define JOBHANDLER  "JobClass: "
    #define CYCLASS     "CyClass: "

    #define I2CCLASS "I2c: "
    #define DBCLASS "Database: "
    #define QUERYCLASS "Query: "
    #define SERIALCLASS "Serial: "
    #define GPIOCLASS "Gpio: "
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//db connecteion name
    #define QMYSL_AUTH "QMYSL_AUTH"
    #define QMYSL_TABLE "QMYSL_TABLE"
    #define QMYSL_JOB "QMYSL_JOB"
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//folders
    #define FOLDER_CONFIG "config"
    #define FOLDER_SCRIPT "script"
    #define FOLDER_LOG "log"
    #define FOLDER_QUERY "query"
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//files name
    //config file name
        #define FILE_INI "config.ini"

    //watch dog script
        #define FILE_WATCHDOG "watch.sh"

    //sql query
        //in auth-thread
            #define QUERY_FIND_READER                       "auth_query/query_find_reader.sql"
            #define QUERY_FIND_USER                         "auth_query/query_find_user.sql"
            #define QUERY_CHECK_HOL                         "auth_query/query_find_holidays.sql"
            #define QUERY_CHECK_PAIR                        "auth_query/query_find_accesspairs.sql"
            #define QUERY_CHECK_ACCESS                      "auth_query/query_find_accesstimes.sql"

        //in table-thread
            #define QUERY_INSERT_LOG                        "table_query/query_insert_log.sql"
            #define QUERY_INSERT_INP                        "table_query/query_insert_inp.sql"

            #define QUERY_FIND_LOG                          "table_query/query_find_not_sent_log.sql"
            #define QUERY_UPDATEDAT_LOGS                    "table_query/query_log_updatedAt.sql"

            #define QUERY_FIND_INP                          "table_query/query_find_not_sent_inp.sql"
            #define QUERY_UPDATEDAT_INPS                    "table_query/query_inp_updatedAt.sql"

            #define QUERY_DELETE_LOGS                       "table_query/query_delete_logs.sql"
            #define QUERY_DELETE_INPS                       "table_query/query_delete_inps.sql"

            #define QUERY_UPDATE_LOG                        "table_query/query_update_log.sql"
            #define QUERY_UPDATE_INP                        "table_query/query_update_inp.sql"

            //And
                #define QUERY_INSERT_LOGAND                        "table_query/query_insert_logand.sql"
                #define QUERY_FIND_LOGAND                          "table_query/query_find_not_sent_logand.sql"
                #define QUERY_UPDATEDAT_LOGSAND                    "table_query/query_log_updatedAtand.sql"
                #define QUERY_DELETE_LOGSAND                       "table_query/query_delete_logsand.sql"
                #define QUERY_UPDATE_LOGAND                        "table_query/query_update_logand.sql"


        //in job-thread
            #define QUERY_SELECTALL_TIMEZONES               "job_query/timezones_query/query_selectall_timezones.sql"
            #define QUERY_UPDATETALL_TIMEZONES              "job_query/timezones_query/query_updateall_timezones.sql"

            #define QUERY_SELECTALL_ACCESSTIMEZONES         "job_query/accesstimezones_query/query_selectall_accesstimezones.sql"
            #define QUERY_UPDATEALL_ACCESSTIMEZONES         "job_query/accesstimezones_query/query_updateall_accesstimezones.sql"

            #define QUERY_SELECTALL_ACCESSHIFTS             "job_query/accessshifts_query/query_selectall_accessshifts.sql"
            #define QUERY_UPDATEALL_ACCESSHIFTS             "job_query/accessshifts_query/query_updateall_accessshifts.sql"

            #define QUERY_SELECTALL_TAGUSER                 "job_query/tag_users_query/query_selectall_tagusers.sql"
            #define QUERY_UPDATEALL_TAGUSER                 "job_query/tag_users_query/query_updateall_tagusers.sql"

            #define QUERY_SELECTALL_TAGTERMINAL             "job_query/tag_terminals_query/query_selectall_tagterminals.sql"
            #define QUERY_UPDATEALL_TAGTERMINAL             "job_query/tag_terminals_query/query_updateall_tagterminals.sql"

            #define QUERY_SELECTALL_HOLIDAYS                "job_query/holidays_query/query_selectall_holidays.sql"
            #define QUERY_UPDATEALL_HOLIDAYS                "job_query/holidays_query/query_updateall_holidays.sql"

            #define QUERY_SELECTALL_HOLIDAYSSETS            "job_query/holidayssets_query/query_selectall_holidayssets.sql"
            #define QUERY_UPDATEALL_HOLIDAYSSETS            "job_query/holidayssets_query/query_updateall_holidayssets.sql"

            #define QUERY_SELECTALL_HOLIDAYSGROUPS          "job_query/holidaysgroups_query/query_selectall_holidaysgroups.sql"
            #define QUERY_UPDATEALL_HOLIDAYSGROUPS          "job_query/holidaysgroups_query/query_updateall_holidaysgroups.sql"

            #define QUERY_SELECTALL_TAGHOLIDAY              "job_query/tag_holidays_query/query_selectall_tagholidays.sql"
            #define QUERY_UPDATEALL_TAGHOLIDAY              "job_query/tag_holidays_query/query_updateall_tagholidays.sql"

            #define QUERY_SELECTALL_TAGACCESSPAIR           "job_query/tag_accesspairs_query/query_selectall_tagaccesspairs.sql"
            #define QUERY_UPDATEALL_TAGACCESSPAIR           "job_query/tag_accesspairs_query/query_updateall_tagaccesspairs.sql"

            #define QUERY_SELECTALL_TAG                     "job_query/tags_query/query_selectall_tags.sql"
            #define QUERY_UPDATEALL_TAG                     "job_query/tags_query/query_updateall_tags.sql"

            #define QUERY_DELETE_FROM_TIMEZONES             "job_query/timezones_query/query_delete_from_timezonestable.sql"
            #define QUERY_UPDATE_TO_TIMEZONES               "job_query/timezones_query/query_update_to_timezonestable.sql"
            #define QUERY_INSERT_TO_TIMEZONES               "job_query/timezones_query/query_insert_to_timezonestable.sql"

            #define QUERY_DELETE_FROM_ACCESSTIMEZONES       "job_query/accesstimezones_query/query_delete_from_accesstimezonestable.sql"
            #define QUERY_UPDATE_TO_ACCESSTIMEZONES         "job_query/accesstimezones_query/query_update_to_accesstimezonestable.sql"
            #define QUERY_INSERT_TO_ACCESSTIMEZONES         "job_query/accesstimezones_query/query_insert_to_accesstimezonestable.sql"

            #define QUERY_DELETE_FROM_ACCESSSHIFTS          "job_query/accessshifts_query/query_delete_from_accessshiftstable.sql"
            #define QUERY_UPDATE_TO_ACCESSSHIFTS            "job_query/accessshifts_query/query_update_to_accessshiftstable.sql"
            #define QUERY_INSERT_TO_ACCESSSHIFTS            "job_query/accessshifts_query/query_insert_to_accessshiftstable.sql"

            #define QUERY_DELETE_FROM_TAGUSERS              "job_query/tag_users_query/query_delete_from_taguserstable.sql"
            #define QUERY_UPDATE_TO_TAGUSERS                "job_query/tag_users_query/query_update_to_taguserstable.sql"
            #define QUERY_INSERT_TO_TAGUSERS                "job_query/tag_users_query/query_insert_to_taguserstable.sql"

            #define QUERY_DELETE_FROM_TAGTERMINALS          "job_query/tag_terminals_query/query_delete_from_tagterminalstable.sql"
            #define QUERY_UPDATE_TO_TAGTERMINALS            "job_query/tag_terminals_query/query_update_to_tagterminalstable.sql"
            #define QUERY_INSERT_TO_TAGTERMINALS            "job_query/tag_terminals_query/query_insert_to_tagterminalstable.sql"

            #define QUERY_DELETE_FROM_HOLIDAYS              "job_query/holidays_query/query_delete_from_holidaystable.sql"
            #define QUERY_UPDATE_TO_HOLIDAYS                "job_query/holidays_query/query_update_to_holidaystable.sql"
            #define QUERY_INSERT_TO_HOLIDAYS                "job_query/holidays_query/query_insert_to_holidaystable.sql"

            #define QUERY_DELETE_FROM_HOLIDAYSSETS          "job_query/holidayssets_query/query_delete_from_holidayssetstable.sql"
            #define QUERY_UPDATE_TO_HOLIDAYSSETS            "job_query/holidayssets_query/query_update_to_holidayssetstable.sql"
            #define QUERY_INSERT_TO_HOLIDAYSSETS            "job_query/holidayssets_query/query_insert_to_holidayssetstable.sql"

            #define QUERY_DELETE_FROM_HOLIDAYSGROUPS        "job_query/holidaysgroups_query/query_delete_from_holidaysgroupstable.sql"
            #define QUERY_UPDATE_TO_HOLIDAYSGROUPS          "job_query/holidaysgroups_query/query_update_to_holidaysgroupstable.sql"
            #define QUERY_INSERT_TO_HOLIDAYSGROUPS          "job_query/holidaysgroups_query/query_insert_to_holidaysgroupstable.sql"

            #define QUERY_DELETE_FROM_TAGHOLIDAYS           "job_query/tag_holidays_query/query_delete_from_tagholidaystable.sql"
            #define QUERY_UPDATE_TO_TAGHOLIDAYS             "job_query/tag_holidays_query/query_update_to_tagholidaystable.sql"
            #define QUERY_INSERT_TO_TAGHOLIDAYS             "job_query/tag_holidays_query/query_insert_to_tagholidaystable.sql"

            #define QUERY_DELETE_FROM_TAGACCESSPAIRS        "job_query/tag_accesspairs_query/query_delete_from_tagaccesspairstable.sql"
            #define QUERY_UPDATE_TO_TAGACCESSPAIRS          "job_query/tag_accesspairs_query/query_update_to_tagaccesspairstable.sql"
            #define QUERY_INSERT_TO_TAGACCESSPAIRS          "job_query/tag_accesspairs_query/query_insert_to_tagaccesspairstable.sql"

            #define QUERY_DELETE_FROM_TAGS                  "job_query/tags_query/query_delete_from_tagstable.sql"
            #define QUERY_UPDATE_TO_TAGS                    "job_query/tags_query/query_update_to_tagstable.sql"
            #define QUERY_INSERT_TO_TAGS                    "job_query/tags_query/query_insert_to_tagstable.sql"
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//timeouts
    //delay for I2C read/write d
        #define I2C_RW_DELAY_MS 30

    //timeout for ntp sync datetime timer
        #define MAIN_NTP_TIMEOUT_MIN 5
    //timeout fou write to RTC last datetime timer
        #define MAIN_RTC_TIMEOUT_SEC 10
    //timeout for write to wdt reg timer
        #define MAIN_WATCH_TIMEOUT_SEC 3

    //timeout for sync log and inp timers
        #define TABLE_LOG_TIMEOUT_MS 200
    //timeout for delete sent logs and inps timer
        #define TABLE_LOG_DELETE_TIMEOUT_SEC 20

    //timeout for check connectivity to server timer
        #define MQTT_SERVER_CHECKTIMEOUT_SEC 2
    //timeout for send status to server timers
        #define MQTT_STATUS_TIMERTIMEOUT_SEC 5

    //timeout for dequeue auth reqs from auth req queue timer
        #define AUTH_DEQ_TIMEOUT_MS 50
    //timeout for check pending auth list timer
        #define AUTH_PEND_TIMEOUT_MS 50
    //timeout for delete index in pending auth list
        #define AUTH_PEND_DIFFLIMIT_SEC 4

    //timeout for sync database timers
        #define JOB_CHECK_JOBTIMER_SEC 180

    //timeout for read serial from cy timer
        #define CY_TIMEOUT_MS 10
    //timeout for check msg queue from cy timer
        #define CY_CHECK_MS 20
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//config sections ini

//log
    #define MAIN_LOG                        "Main/log"


//-->fields
    //database
    #define DB_NAME                         "DbMysql/db_name"
    #define DB_OPTION                       "DbMysql/db_option"
    #define DB_DRIVER                       "DbMysql/driver_name"
    #define DB_HOSTIP                       "DbMysql/host_ip"
    #define DB_PASS                         "DbMysql/pass_word"
    #define DB_USER                         "DbMysql/user_name"

    //ntp
    #define NTP_URL                         "Ntp/url"

    //mqtt server
    #define SRV_IP                          "Server/ip"
    #define SRV_PORT                        "Server/port"

    //sync
    #define SYNC_TM_S                       "Sync/time_s"

    //Peripheral
    #define P_CY_UART                       "Peripheral/cypress_port"
    #define P_RTC_I2C                       "Peripheral/i2c_port"
    #define P_RTC_ADDR                      "Peripheral/rtc_addr"

    //Will
    #define WILL_FLAG                       "Will/flag"
    #define WILL_QOS                        "Will/qos"
    #define WILL_RETAIN                     "Will/retain"
    #define WILL_MSG_CONNECT                "Will/msg_c"
    #define WILL_MSG_DISCONNECT             "Will/msg_dc"

    //Mqtt
    #define MQTT_KEEP_ALIVE                 "Mqtt/keep_alive"
    #define MQTT_QOS                        "Mqtt/qos"
    #define MQTT_RETAIN                     "Mqtt/retain"

    //Port1
    #define PORT1_BITS                      "Port1/bits"
    #define PORT1_TIMEOUT_MS                "Port1/timeout_ms"
    #define PORT1_PROC_TIMEOUT_MS           "Port1/proc_timeout_ms"

    //Port2
    #define PORT2_BITS                      "Port2/bits"
    #define PORT2_TIMEOUT_MS                "Port2/timeout_ms"
    #define PORT2_PROC_TIMEOUT_MS           "Port2/proc_timeout_ms"

    //AND
    #define AND_AND                         "And/and"
    #define AND_PORT                        "And/port"
    #define AND_GET_TIMEOUT                 "And/get_timeout_ms"

//defualts
    //log
    #define MAIN_LOG_DEFAULT                false

    //database
    #define DB_NAME_DEFAULT                 "IroTeam"
    #define DB_OPTION_DEFAULT               "MYSQL_OPT_CONNECT_TIMEOUT=1"
    #define DB_DRIVER_DEFAULT               "QMYSQL"
    #define DB_HOSTNAME_DEFAULT             "localhost"
    #define DB_PASS_DEFAULT                 "1qaz@2wsx"
    #define DB_USER_DEFAULT                 "root"


    //ntp
    #define NTP_URL_DEFAULT                 "time.windows.com"

    //mqtt server
    #define SRV_IP_DEFAULT                  "185.53.140.26"
    #define SRV_PORT_DEFAULT                31536

    //sync
    #define SYNC_TM_S_DEFUALT               180

    //Peripheral
    #define P_CY_UART_DEFAULT               "ttyS1"
    #define P_RTC_I2C_DEFAULT               "/dev/i2c-0"
    #define P_RTC_ADDR_DEFAULT              0x68

    //Will
    #define WILL_FLAG_DEFAULT               1
    #define WILL_QOS_DEFAULT                2
    #define WILL_RETAIN_DEFAULT             true
    #define WILL_MSG_CONNECT_DEFAULT        "IroTeam is connected"
    #define WILL_MSG_DISCONNECT_DEFAULT     "IroTeam is disconnected"

    //Mqtt
    #define MQTT_KEEP_ALIVE_DEFAULT         120
    #define MQTT_QOS_DEFAULT                1
    #define MQTT_RETAIN_DEFAULT             false

    //Port1
    #define PORT1_BITS_DEFAULT              26
    #define PORT1_TIMEOUT_MS_DEFAULT        2000
    #define PORT1_PROC_TIMEOUT_MS_DEFAULT   2000

    //Port2
    #define PORT2_BITS_DEFAULT              26
    #define PORT2_TIMEOUT_MS_DEFAULT        2000
    #define PORT2_PROC_TIMEOUT_MS_DEFAULT   2000

    //AND
    #define AND_AND_DEFAULT                 false
    #define AND_PORT_DEFAULT                1
    #define AND_GET_TIMEOUT_DEFAULT         10000
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//RTC
    #define RTC_YEAR_REG 0x06
    #define RTC_MOTNh_REG 0x05
    #define RTC_DAY_REG 0x04
    #define RTC_WEEK_REG 0x03
    #define RTC_HOUR_REG 0x02
    #define RTC_MINUTE_REG 0x01
    #define RTC_SEC_REG 0x00

    #define RTC_CTRL_REG 0x07
//--------------------------------------------------------------------
//Erorrs
    //auth errors
        #define ERR_OK 0x00
        #define ERR_STR_OK "Ok"

    //query_find_user.sql
        #define ERR_NO_USER_FOUND 0x01
        #define ERR_STR_UER_FOUND "No User Found"

        #define ERR_USER_NOT_ACTIVE 0x02
        #define ERR_STR_USER_NOT_ACITVE "User Is Not Active"

        #define ERR_USER_NO_CREDIT_NOTSTART 0x03
        #define ERR_STR_USER_NO_CREDIT_NOTSTART "User Does Not Have Credit - Not Start Yet"

        #define ERR_USER_NO_CREDIT_ISSTOP 0x04
        #define ERR_STR_USER_NO_CREDIT_ISSTOP "User Does Not Have Credit - Is Stopped"

        #define ERR_USER_DEACIVE_BY_DATETIME 0x05
        #define ERR_STR_USER_DEACIVE_BY_DATETIME "User Is Deactivated By Datetime"

    //query_find_reader.sql
        #define ERR_NO_READER_FOUND 0x06
        #define ERR_STR_NO_READER_FOUND "No Reader Found"

        #define ERR_READER_NOT_ACTIVE 0x07
        #define ERR_STR_READER_NOT_ACTIVE "Reader Is Not Active"

        #define ERR_READER_DEACTIVE_BY_DATETIME 0x08
        #define ERR_STR_READER_DEACTIVE_BY_DATETIME "Reader Is Deactivated By Datetime"

    //query_find_holidays.sql

    //query_find_accesspairs.sql
        #define ERR_USER_NOACCESSPAIR 0x09
        #define ERR_STR_USER_NOACCESSPAIR "User Does Not Have Access Pair For Reader"

    //query_find_accesstimes.sql
        #define ERR_USER_NOACCESS_TIME 0x0A
        #define ERR_STR_USER_NOACCESS_TIME "User Does Not Have Access Time To Reader"

    //server
        #define ERR_CHECK_ANTIPASSBACK 0x0B
        #define EER_STR_CHECK_ANTIPASSBACK "No Anti Pass Back"

    //query error
        #define ERR_QUERY_USER_FIND 0x0C
        #define ERR_STR_QUERY_USER_FIND "Query Error of User Find"

        #define ERR_QUERY_READER_FIND 0x0D
        #define ERR_STR_QUERY_READER_FIND "Query Error of Reader Find"

        #define ERR_QUERY_CHECK_HOL 0x0E
        #define EER_STR_QUERY_CHECK_HOL "Query Error of Check Holidays"

        #define ERR_QUERY_CHECK_ACCESS 0x0F
        #define EER_STR_QUERY_CHECK_ACCESS "Query Error of Check Access Pair"

        #define ERR_QUERY_CHECK_TIME 0x10
        #define EER_STR_QUERY_CHECK_TIME "Query Error of Check Access Time"

        #define ERR_SERVER_TIMEOUT 0x11
        #define EER_STR_SERVER_TIMEOUT "Server Request Timeout"


        #define ERR_AND_2USER_NOTFOUND 0x12
        #define ERR_STR_AND_2USER_NOTFOUND "There is no User1 or User2"

//--------------------------------------------------------------------

//--------------------------------------------------------------------
//global variables

    //get from application and hardcode
        //directories
        extern QString gl_Prog_MainDirectory;
        extern QString gl_Prog_ConfigDirectory;
        extern QString gl_Prog_LogDirectory;
        extern QString gl_Prog_QueryDirectory;
        extern QString gl_Prog_ScriptDirectory;

        //files names
        extern QString gl_Prog_ConfigFileName;
        extern QString gl_Prog_WatchFileName;

        //query
            //in auth-thread
                extern QString gl_prog_QueryFindUser;
                extern QString gl_prog_QueryFindReader;
                extern QString gl_prog_QueryCheckHol;
                extern QString gl_prog_QueryCheckPair;
                extern QString gl_prog_QueryCheckAccess;

            //in table-thread
                extern QString gl_prog_QueryInsertLog;
                extern QString gl_prog_QueryInsertInp;

                extern QString gl_prog_QueryUpdateLog;
                extern QString gl_prog_QueryUpdateInp;

                extern QString gl_prog_QueryFindLog;
                extern QString gl_prog_QueryUpdatedAtLogs;

                extern QString gl_prog_QueryUpdatedAtInps;
                extern QString gl_prog_QueryFindInp;

                extern QString gl_prog_QueryDeleteLogs;
                extern QString gl_prog_QueryDeleteInps;

                //and
                    extern QString gl_prog_QueryInsertLogAnd;
                    extern QString gl_prog_QueryFindLogAnd;
                    extern QString gl_prog_QueryUpdatedAtLogsAnd;
                    extern QString gl_prog_QueryDeleteLogsAnd;
                    extern QString gl_prog_QueryUpdateLogAnd;

            //in job thread
                extern QString gl_prog_QuerySelectAllFromTimezonesTable;
                extern QString gl_prog_QueryUpdateAllFromTimezonesTable;

                extern QString gl_prog_QuerySelectAllFromAccessTimezonesTable;
                extern QString gl_prog_QueryUpdateAllFromAccessTimezonesTable;

                extern QString gl_prog_QuerySelectAllFromAccessShiftsTable;
                extern QString gl_prog_QueryUpdateAllFromAccessShiftsTable;

                extern QString gl_prog_QuerySelectAllFromTagUsersTable;
                extern QString gl_prog_QueryUpdateAllFromTagUsersTable;

                extern QString gl_prog_QuerySelectAllFromTagTerminalsTable;
                extern QString gl_prog_QueryUpdateAllFromTagTerminalsTable;

                extern QString gl_prog_QuerySelectAllFromHolidaysTable;
                extern QString gl_prog_QueryUpdateAllFromHolidaysTable;

                extern QString gl_prog_QuerySelectAllFromHolidaysSetsTable;
                extern QString gl_prog_QueryUpdateAllFromHolidaysSetsTable;

                extern QString gl_prog_QuerySelectAllFromHolidaysGroupsTable;
                extern QString gl_prog_QueryUpdateAllFromHolidaysGroupsTable;

                extern QString gl_prog_QuerySelectAllFromTagHolidaysTable;
                extern QString gl_prog_QueryUpdateAllFromTagHolidaysTable;

                extern QString gl_prog_QuerySelectAllFromTagAccessPairsTable;
                extern QString gl_prog_QueryUpdateAllFromTagAccessPairsTable;

                extern QString gl_prog_QuerySelectAllFromTagsTable;
                extern QString gl_prog_QueryUpdateAllFromTagsTable;

                extern QString gl_prog_QueryDeleteFromTimezonesTable;
                extern QString gl_prog_QueryUpdateToTimezonesTable;
                extern QString gl_prog_QueryInsertToTimezonesTable;

                extern QString gl_prog_QueryDeleteFromAccessTimezonesTable;
                extern QString gl_prog_QueryUpdateToAccessTimezonesTable;
                extern QString gl_prog_QueryInsertToAccessTimezonesTable;

                extern QString gl_prog_QueryDeleteFromAccessShiftsTable;
                extern QString gl_prog_QueryUpdateToAccessShiftsTable;
                extern QString gl_prog_QueryInsertToAccessShiftsTable;

                extern QString gl_prog_QueryDeleteFromTagUsersTable;
                extern QString gl_prog_QueryUpdateToTagUsersTable;
                extern QString gl_prog_QueryInsertToTagUsersTable;

                extern QString gl_prog_QueryDeleteFromTagTerminalsTable;
                extern QString gl_prog_QueryUpdateToTagTerminalsTable;
                extern QString gl_prog_QueryInsertToTagTerminalsTable;

                extern QString gl_prog_QueryDeleteFromHolidaysTable;
                extern QString gl_prog_QueryUpdateToHolidaysTable;
                extern QString gl_prog_QueryInsertToHolidaysTable;

                extern QString gl_prog_QueryDeleteFromHolidaysSetsTable;
                extern QString gl_prog_QueryUpdateToHolidaysSetsTable;
                extern QString gl_prog_QueryInsertToHolidaysSetsTable;

                extern QString gl_prog_QueryDeleteFromHolidaysGroupsTable;
                extern QString gl_prog_QueryUpdateToHolidaysGroupsTable;
                extern QString gl_prog_QueryInsertToHolidaysGroupsTable;

                extern QString gl_prog_QueryDeleteFromTagHolidaysTable;
                extern QString gl_prog_QueryUpdateToTagHolidaysTable;
                extern QString gl_prog_QueryInsertToTagHolidaysTable;

                extern QString gl_prog_QueryDeleteFromTagAccessPairsTable;
                extern QString gl_prog_QueryUpdateToTagAccessPairsTable;
                extern QString gl_prog_QueryInsertToTagAccessPairsTable;

                extern QString gl_prog_QueryDeleteFromTagsTable;
                extern QString gl_prog_QueryUpdateToTagsTable;
                extern QString gl_prog_QueryInsertToTagsTable;

    //get from ini
        //database connection info
            extern QString gl_Ini_DbDriver;
            extern QString gl_Ini_DbHostIp;
            extern QString gl_Ini_DbUsername;
            extern QString gl_Ini_DbPassword;
            extern QString gl_Ini_DbName;
            extern QString gl_Ini_DbOption;

        //ntp url
            extern QString gl_Ini_NtpUrl;

        //mqtt server
            extern QString gl_Ini_SrvIp;
            extern int gl_Ini_SrvPort;

        //sync time
            extern quint32 gl_Ini_SyncTmS;

        //peripheral ports (serial & i2c)
            extern QString gl_Ini_SerialCyPort;
            extern QString gl_Ini_I2cDevicePort;

        //i2c device addresses
            extern int gl_Ini_Ds1307DevAddr;

    //others
        //will
            //extern int gl_Ini_WillFg;
            extern int gl_Ini_WillQos;
            extern bool gl_Ini_WillRetain;
            extern QString gl_Ini_WillMsg_Dc;
            extern QString gl_Ini_WillMsg_Cc;
            extern QString gl_Ini_WillMsg_Fc;

        //mqtt
            extern int gl_Ini_MqttKeepAlive;
            extern int gl_Ini_MqttQos;
            extern bool gl_Ini_MqttRetain;


     //Ports
        //Port1
            extern int gl_Ini_Port1Bits;
            extern int gl_Ini_Port1Timeout;
            extern int gl_Ini_Port1ProcTimeout;

        //Port2
            extern int gl_Ini_Port2Bits;
            extern int gl_Ini_Port2Timeout;
            extern int gl_Ini_Port2ProcTimeout;

        //And
            extern bool gl_ini_And;
            extern int gl_ini_AndPort;
            extern int gl_ini_AndGetTimeout;

     //network
        //eth
            extern QString gl_Eth_MacAddr;

        //wlan
            extern QString gl_Wlan_MacAddr;


        //Thread and Thread Job boolean
            //Cy thread
                extern bool bCyThreadRun;
                extern bool bCyThreadJobRun;

            //Auth thread
                extern bool bAuthTreadRun;
                extern bool bAuthThreadJobRun;

            //Table thread
                extern bool bTableThreadRun;
                extern bool bTableThreadJobRun;

            //Job thread
                extern bool bJobThreadRun;
                extern bool bJobThreadJobRun;

            //Mqtt thread
                extern bool bMqttThreadRun;
                extern bool bMqttThreadJobRun;


            extern bool Mqtt_Status;

            extern bool db_in_sync;
//--------------------------------------------------------------------


//--------------------------------------------------------------------
//structs
struct Topic{
    int _kind = -1; //0->bool //1->int //2->double //3->QString
    QString _topic = "";
    bool _bvalue = false;
    int _ivalue = -1;
    double _dvalue = -1;
    QString _svalue = "";
};

struct queryResult
{
    bool success = false;
    int size = -1;
    QSqlQuery query;
};

struct AuthRes{
    QDateTime reqDateTime;

    int err = -2;
    QString errString = "";

    int userRowId = -1;
    QString userUUID = "";
    int userUniqueId = -1;
    bool isUser = false;
    int userTableType = -1;
    bool userActivation = false;
    QDateTime userCreditStartDateTime;
    QDateTime userCreditStopDateTime;
    bool userDeactivationByDateTime = false;
    QDateTime userDeactivationStartDateTime;
    QDateTime userDeactivationStopDateTime;

    int readerRowId = -1;
    QString readerUUID = "";
    int readerId = -1;
    int readerExpId = -1;
    int readerTableType = -1;
    bool readerActivation = false;
    bool readerDeactivationByDateTime = false;
    QDateTime readerDeactivationStartDateTime;
    QDateTime readerDeactivationStopDateTime;

    bool isHoliday = false;
    int holidayTableType = -1;
    QString holidayCode = "";
    int userDayOfWeek = -1;
    QString userDayOfWeekStr = "";

    bool Auth = false;
    bool Anti = false;
    bool Server = false;

    bool isSuccess = false;
};

struct Log{
    int userUniqueId = -1;
    int readerExpId = -1;
    int readerId = -1;
    QDateTime reqDateTime;
    bool isHoliday = false;
    bool isUser = false;
    int errorCode = -2;
    QString errorString = "";
    QString UUID = "";
    bool isSuccess = false;
};

struct Inp{
    int readerExpId = -1;
    int readerId = -1;
    QDateTime reqDateTime;
    QString UUID = "";
};

struct Apb{
    int userUniqueId = -1;
    int readerExpId = -1;
    int readerId = -1;
    QDateTime reqDateTime;
};

struct Auth{
    int userUniqueId = -1;
    int readerExpId = -1;
    int readerId = -1;
    QDateTime reqDateTime;
};

struct InpReq{
    quint8 readerExpId = 1;
    quint8 readerId = 0;
};

struct AuthReq{
    quint8 readerExpId = 1;
    quint8 readerId = 0;
    quint16 userUniqueId = 0;
};

struct Weigand{
    quint8 siteCode = 0;
    quint16 uinqueId = 0;
};

//Note: 1=Sunday, 2=Monday, 3=Tuesday, 4=Wednesday, 5=Thursday, 6=Friday, 7=Saturday.



struct AuthRes_And{
    AuthRes AuthResPort1;
    AuthRes AuthResPort2;
};

struct Apb_And{
    int userUniqueId_1 = -1;
    int userUniqueId_2 = -1;
    int readerExpId = 1;
    int readerId_1 = 1;
    int readerId_2 = 2;
    QDateTime reqDateTime;
};

struct Cy_AuthReq_And{
    int userUniqueId_1 = -1;
    int userUniqueId_2 = -1;
    int readerExpId = 1;
    int readerId_1 = 1;
    int readerId_2 = 2;
};

struct Auth_And{
    int userUniqueId_1 = -1;
    int userUniqueId_2 = -1;
    int readerExpId = 1;
    int readerId_1 = 1;
    int readerId_2 = 2;
    QDateTime reqDateTime;
};

struct LogAnd{
    int userUniqueId_1 = -1;
    int userUniqueId_2 = -1;
    int readerExpId = -1;
    int readerId_1 = -1;
    int readerId_2 = -1;
    QDateTime reqDateTime;
    bool isHoliday_1 = false;
    bool isHoliday_2 = false;
    bool isUser_1 = false;
    bool isUser_2 = false;
    int errorCode_1 = -2;
    int errorCode_2 = -2;
    QString errorString_1 = "";
    QString errorString_2 = "";
    QString UUID = "";
    bool isSuccess_1 = false;
    bool isSuccess_2 = false;
    bool isSuccess = false;
};
//--------------------------------------------------------------------
#endif // DEFINES_H

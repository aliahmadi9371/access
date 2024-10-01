#include "defines.h"

//--------------------------------------------------------------------
//global variables
    //get from application and hardcode
        //directories
        QString gl_Prog_MainDirectory = "";
        QString gl_Prog_ConfigDirectory = "";
        QString gl_Prog_LogDirectory = "";
        QString gl_Prog_QueryDirectory = "";
        QString gl_Prog_ScriptDirectory = "";

        //files names
        QString gl_Prog_ConfigFileName = "";
        QString gl_Prog_WatchFileName = "";

        //query
            //in auth-thread
                QString gl_prog_QueryFindUser = "";
                QString gl_prog_QueryFindReader = "";
                QString gl_prog_QueryCheckHol = "";
                QString gl_prog_QueryCheckPair = "";
                QString gl_prog_QueryCheckAccess = "";

            //in table-thread
                QString gl_prog_QueryInsertLog = "";
                QString gl_prog_QueryInsertInp = "";

                QString gl_prog_QueryUpdateLog = "";
                QString gl_prog_QueryUpdateInp = "";

                QString gl_prog_QueryFindLog = "";
                QString gl_prog_QueryUpdatedAtLogs = "";

                QString gl_prog_QueryFindInp = "";
                QString gl_prog_QueryUpdatedAtInps = "";

                QString gl_prog_QueryDeleteLogs = "";
                QString gl_prog_QueryDeleteInps = "";

                //and
                    QString gl_prog_QueryInsertLogAnd = "";
                    QString gl_prog_QueryFindLogAnd = "";
                    QString gl_prog_QueryUpdatedAtLogsAnd = "";
                    QString gl_prog_QueryDeleteLogsAnd = "";
                    QString gl_prog_QueryUpdateLogAnd = "";


            //in job thread
                QString gl_prog_QuerySelectAllFromTimezonesTable = "";
                QString gl_prog_QueryUpdateAllFromTimezonesTable = "";

                QString gl_prog_QuerySelectAllFromAccessTimezonesTable = "";
                QString gl_prog_QueryUpdateAllFromAccessTimezonesTable = "";

                QString gl_prog_QuerySelectAllFromAccessShiftsTable = "";
                QString gl_prog_QueryUpdateAllFromAccessShiftsTable = "";

                QString gl_prog_QuerySelectAllFromTagUsersTable = "";
                QString gl_prog_QueryUpdateAllFromTagUsersTable = "";

                QString gl_prog_QuerySelectAllFromTagTerminalsTable = "";
                QString gl_prog_QueryUpdateAllFromTagTerminalsTable = "";

                QString gl_prog_QuerySelectAllFromHolidaysTable = "";
                QString gl_prog_QueryUpdateAllFromHolidaysTable = "";

                QString gl_prog_QuerySelectAllFromHolidaysSetsTable = "";
                QString gl_prog_QueryUpdateAllFromHolidaysSetsTable = "";

                QString gl_prog_QuerySelectAllFromHolidaysGroupsTable = "";
                QString gl_prog_QueryUpdateAllFromHolidaysGroupsTable = "";

                QString gl_prog_QuerySelectAllFromTagHolidaysTable = "";
                QString gl_prog_QueryUpdateAllFromTagHolidaysTable = "";

                QString gl_prog_QuerySelectAllFromTagAccessPairsTable = "";
                QString gl_prog_QueryUpdateAllFromTagAccessPairsTable = "";

                QString gl_prog_QuerySelectAllFromTagsTable = "";
                QString gl_prog_QueryUpdateAllFromTagsTable = "";

                QString gl_prog_QueryDeleteFromTimezonesTable = "";
                QString gl_prog_QueryUpdateToTimezonesTable = "";
                QString gl_prog_QueryInsertToTimezonesTable = "";

                QString gl_prog_QueryDeleteFromAccessTimezonesTable = "";
                QString gl_prog_QueryUpdateToAccessTimezonesTable = "";
                QString gl_prog_QueryInsertToAccessTimezonesTable = "";

                QString gl_prog_QueryDeleteFromAccessShiftsTable = "";
                QString gl_prog_QueryUpdateToAccessShiftsTable = "";
                QString gl_prog_QueryInsertToAccessShiftsTable = "";

                QString gl_prog_QueryDeleteFromTagUsersTable = "";
                QString gl_prog_QueryUpdateToTagUsersTable = "";
                QString gl_prog_QueryInsertToTagUsersTable = "";

                QString gl_prog_QueryDeleteFromTagTerminalsTable = "";
                QString gl_prog_QueryUpdateToTagTerminalsTable = "";
                QString gl_prog_QueryInsertToTagTerminalsTable = "";

                QString gl_prog_QueryDeleteFromHolidaysTable = "";
                QString gl_prog_QueryUpdateToHolidaysTable = "";
                QString gl_prog_QueryInsertToHolidaysTable = "";

                QString gl_prog_QueryDeleteFromHolidaysSetsTable = "";
                QString gl_prog_QueryUpdateToHolidaysSetsTable = "";
                QString gl_prog_QueryInsertToHolidaysSetsTable = "";

                QString gl_prog_QueryDeleteFromHolidaysGroupsTable = "";
                QString gl_prog_QueryUpdateToHolidaysGroupsTable = "";
                QString gl_prog_QueryInsertToHolidaysGroupsTable = "";

                QString gl_prog_QueryDeleteFromTagHolidaysTable = "";
                QString gl_prog_QueryUpdateToTagHolidaysTable = "";
                QString gl_prog_QueryInsertToTagHolidaysTable = "";

                QString gl_prog_QueryDeleteFromTagAccessPairsTable = "";
                QString gl_prog_QueryUpdateToTagAccessPairsTable = "";
                QString gl_prog_QueryInsertToTagAccessPairsTable = "";

                QString gl_prog_QueryDeleteFromTagsTable = "";
                QString gl_prog_QueryUpdateToTagsTable = "";
                QString gl_prog_QueryInsertToTagsTable = "";


    //get from ini
        //database connection info
            QString gl_Ini_DbDriver = "";
            QString gl_Ini_DbHostIp = "";
            QString gl_Ini_DbUsername = "";
            QString gl_Ini_DbPassword = "";
            QString gl_Ini_DbName = "";
            QString gl_Ini_DbOption = "";

        //ntp url
            QString gl_Ini_NtpUrl = "";

        //mqtt server
            QString gl_Ini_SrvIp = "";
            int gl_Ini_SrvPort = 0;

        //sync time
            quint32 gl_Ini_SyncTmS = 0;

    //get from database
        //peripheral ports (serial & i2c)
            QString gl_Ini_SerialCyPort = "";
            QString gl_Ini_I2cDevicePort = "";

        //i2c device addresses
            int gl_Ini_Ds1307DevAddr = 0;

    //others
        //Will
            //int gl_Ini_WillFg = 1;
            int gl_Ini_WillQos = 2;
            bool gl_Ini_WillRetain = true;
            QString gl_Ini_WillMsg_Dc = "IroTeam is disconnected";
            QString gl_Ini_WillMsg_Cc = "IroTeam is connected";
            QString gl_Ini_WillMsg_Fc = "IroTeam is first connected";

        //mqtt sub config
            int gl_Ini_MqttKeepAlive = 120;
            int gl_Ini_MqttQos = 1;
            bool gl_Ini_MqttRetain = false;

    //Ports
       //Port1
           int gl_Ini_Port1Bits = 0;
           int gl_Ini_Port1Timeout = 0;
           int gl_Ini_Port1ProcTimeout = 0;

       //Port2
           int gl_Ini_Port2Bits = 0;
           int gl_Ini_Port2Timeout = 0;
           int gl_Ini_Port2ProcTimeout = 0;

        //And
           bool gl_ini_And = false;
           int gl_ini_AndPort = 1;
           int gl_ini_AndGetTimeout = 10000;

   //network
      //eth
          QString gl_Eth_MacAddr = "";
          QString gl_Eth_Ip = "";

      //wlan
          QString gl_Wlan_MacAddr = "";
          QString gl_Wlan_Ip = "";

      //Thread and Thread Job boolean
          //Cy thread
              bool bCyThreadRun = false;
              bool bCyThreadJobRun = false;

          //Auth thread
              bool bAuthTreadRun = false;
              bool bAuthThreadJobRun = false;

          //Table thread
              bool bTableThreadRun = false;
              bool bTableThreadJobRun = false;

          //Job thread
              bool bJobThreadRun = false;
              bool bJobThreadJobRun = false;

          //Mqtt thread
              bool bMqttThreadRun = false;
              bool bMqttThreadJobRun = false;

              bool Mqtt_Status = false;
               bool db_in_sync = false;
//--------------------------------------------------------------------

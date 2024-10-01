#include <QCoreApplication>
#include "defines.h"        //all defines and global variables
#include "mainclass.h"

#include "database.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   QHash<QtMsgType,QString> msgLevelHash;
   msgLevelHash[QtDebugMsg] = "Debug";
   msgLevelHash[QtInfoMsg] = "Info";
   msgLevelHash[QtWarningMsg] = "Warning";
   msgLevelHash[QtCriticalMsg] = "Critical";
   msgLevelHash[QtFatalMsg] = "Fatal";
   QByteArray localMsg = msg.toLocal8Bit();
   QDateTime time = QDateTime::currentDateTime();
   QString formattedTime = time.toString("yyyy-MM-dd hh:mm:ss:zzz");
   QString fileName =  QString(gl_Prog_LogDirectory + "/Log_%1_.txt").arg(QDate::currentDate().toString("yyyy_MM_dd"));
   QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
   QString logLevelName = msgLevelHash[type];
   QByteArray logLevelMsg = logLevelName.toLocal8Bit();
   QDir dir;
   if (!dir.exists(gl_Prog_LogDirectory))
       dir.mkpath(gl_Prog_LogDirectory);
   QFile outFileCheck(fileName);
   QString NewFileName = fileName;
   int size = outFileCheck.size();
   int counter = 0 ;
   while ( size > MAIN_LOGSIZE ){
       counter++;
       NewFileName = fileName+QString::number(counter);
       QFile outFileCheck(NewFileName);
       size = outFileCheck.size();
       QApplication::processEvents(QEventLoop::AllEvents);
   }
//   if(MAIN_LOG_TO_FILE){
   if(msg != ""){
       QString txt = QString("%1 %2: %3").arg(formattedTime, logLevelName, msg);
       QFile outFile(NewFileName);
       outFile.open(QIODevice::WriteOnly | QIODevice::Append);
       QTextStream ts(&outFile);
       ts << txt << endl;
       outFile.close();
   }
   else{
       QString txt = "";
       QFile outFile(NewFileName);
       outFile.open(QIODevice::WriteOnly | QIODevice::Append);
       QTextStream ts(&outFile);
       ts << txt << endl;
       outFile.close();
   }
//   } else {
//       fprintf(stderr, "%s %s: %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
//       fflush(stderr);
//   }
//   if (type == QtFatalMsg)
//       abort();

   return;
}

void CreateConfigFile()
{
    //this function is called if config file does not exists

    QSettings setting(gl_Prog_ConfigFileName, QSettings::IniFormat);

    //set log to file defualt
    setting.setValue(MAIN_LOG, MAIN_LOG_DEFAULT);

    //set defualt valuse for database in config .ini
    setting.setValue(DB_NAME, DB_NAME_DEFAULT);
    setting.setValue(DB_OPTION, DB_OPTION_DEFAULT);
    setting.setValue(DB_DRIVER, DB_DRIVER_DEFAULT);
    setting.setValue(DB_HOSTIP, DB_HOSTNAME_DEFAULT);
    setting.setValue(DB_PASS, DB_PASS_DEFAULT);
    setting.setValue(DB_USER, DB_USER_DEFAULT);

    //set defualt valuse for ntp in config .ini
    setting.setValue(NTP_URL, NTP_URL_DEFAULT);

    //set defualt valuse for mqtt server in config .ini
    setting.setValue(SRV_IP, SRV_IP_DEFAULT);
    setting.setValue(SRV_PORT, SRV_PORT_DEFAULT);

    //set default values for sync time second
    setting.setValue(SYNC_TM_S, SYNC_TM_S_DEFUALT);

    //set defualt valuse for peripherals in config .ini
    setting.setValue(P_CY_UART, P_CY_UART_DEFAULT);
    setting.setValue(P_RTC_I2C, P_RTC_I2C_DEFAULT);
    setting.setValue(P_RTC_ADDR, P_RTC_ADDR_DEFAULT);

    //set defualt valuse for Will in config .ini
    setting.setValue(WILL_FLAG, WILL_FLAG_DEFAULT);
    setting.setValue(WILL_QOS, WILL_QOS_DEFAULT);
    setting.setValue(WILL_RETAIN, WILL_RETAIN_DEFAULT);
    setting.setValue(WILL_MSG_CONNECT, WILL_MSG_CONNECT_DEFAULT);
    setting.setValue(WILL_MSG_DISCONNECT, WILL_MSG_DISCONNECT_DEFAULT);

    //set defualt valuse for mqtt in config .ini
    setting.setValue(MQTT_KEEP_ALIVE, MQTT_KEEP_ALIVE_DEFAULT);
    setting.setValue(MQTT_QOS, MQTT_QOS_DEFAULT);
    setting.setValue(MQTT_RETAIN, MQTT_RETAIN_DEFAULT);

    //set defualt valuse for port1 in config .ini
    setting.setValue(PORT1_BITS, PORT1_BITS_DEFAULT);
    setting.setValue(PORT1_TIMEOUT_MS, PORT1_TIMEOUT_MS_DEFAULT);
    setting.setValue(PORT1_PROC_TIMEOUT_MS, PORT1_PROC_TIMEOUT_MS_DEFAULT);

    //set defualt valuse for port2 in config .ini
    setting.setValue(PORT2_BITS, PORT2_BITS_DEFAULT);
    setting.setValue(PORT2_TIMEOUT_MS, PORT2_TIMEOUT_MS_DEFAULT);
    setting.setValue(PORT2_PROC_TIMEOUT_MS, PORT2_PROC_TIMEOUT_MS_DEFAULT);

    //set defualt valuse for And in config .ini
    setting.setValue(AND_AND, AND_AND_DEFAULT);
    setting.setValue(AND_PORT, AND_PORT_DEFAULT);
    setting.setValue(AND_GET_TIMEOUT, AND_GET_TIMEOUT_DEFAULT);

    return;
}

void CheckConfigDirFile()
{
    gl_Prog_ConfigDirectory = gl_Prog_MainDirectory + "/" + FOLDER_CONFIG;
    gl_Prog_LogDirectory = gl_Prog_MainDirectory + "/" + FOLDER_LOG;
    gl_Prog_QueryDirectory = gl_Prog_MainDirectory + "/" + FOLDER_QUERY;
    gl_Prog_ScriptDirectory = gl_Prog_MainDirectory + "/" + FOLDER_SCRIPT;



    QDir dir;
    if(!dir.exists(gl_Prog_ConfigDirectory))
        dir.mkdir(gl_Prog_ConfigDirectory);

    if(!dir.exists(gl_Prog_LogDirectory))
        dir.mkdir(gl_Prog_LogDirectory);

    if(!dir.exists(gl_Prog_QueryDirectory))
        dir.mkdir(gl_Prog_QueryDirectory);

    if(!dir.exists(gl_Prog_ScriptDirectory))
        dir.mkdir(gl_Prog_ScriptDirectory);

    gl_Prog_ConfigFileName = gl_Prog_ConfigDirectory + "/" + FILE_INI;
    gl_Prog_WatchFileName = gl_Prog_ScriptDirectory + "/" + FILE_WATCHDOG;

    //auth thread
    gl_prog_QueryFindUser = gl_Prog_QueryDirectory + "/" + QUERY_FIND_USER;
    gl_prog_QueryFindReader = gl_Prog_QueryDirectory + "/" + QUERY_FIND_READER;
    gl_prog_QueryCheckHol = gl_Prog_QueryDirectory + "/" + QUERY_CHECK_HOL;
    gl_prog_QueryCheckPair = gl_Prog_QueryDirectory + "/" + QUERY_CHECK_PAIR;
    gl_prog_QueryCheckAccess = gl_Prog_QueryDirectory + "/" + QUERY_CHECK_ACCESS;

    //table-thread
    gl_prog_QueryInsertLog = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_LOG;
    gl_prog_QueryInsertInp = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_INP;

    gl_prog_QueryFindLog = gl_Prog_QueryDirectory + "/" + QUERY_FIND_LOG;
    gl_prog_QueryUpdatedAtLogs = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEDAT_LOGS;

    gl_prog_QueryFindInp = gl_Prog_QueryDirectory + "/" + QUERY_FIND_INP;
    gl_prog_QueryUpdatedAtInps = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEDAT_INPS;

    gl_prog_QueryDeleteLogs = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_LOGS;
    gl_prog_QueryDeleteInps = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_INPS;

    gl_prog_QueryUpdateLog = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_LOG;
    gl_prog_QueryUpdateInp = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_INP;

    gl_prog_QueryInsertLogAnd = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_LOGAND;
    gl_prog_QueryFindLogAnd = gl_Prog_QueryDirectory + "/" + QUERY_FIND_LOGAND;
    gl_prog_QueryUpdatedAtLogsAnd = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEDAT_LOGSAND;
    gl_prog_QueryDeleteLogsAnd = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_LOGSAND;
    gl_prog_QueryUpdateLogAnd = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_LOGAND;


    //job-thread
    gl_prog_QuerySelectAllFromTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_TIMEZONES;
    gl_prog_QueryUpdateAllFromTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATETALL_TIMEZONES;

    gl_prog_QuerySelectAllFromAccessTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_ACCESSTIMEZONES;
    gl_prog_QueryUpdateAllFromAccessTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_ACCESSTIMEZONES;

    gl_prog_QuerySelectAllFromAccessShiftsTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_ACCESSHIFTS;
    gl_prog_QueryUpdateAllFromAccessShiftsTable =  gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_ACCESSHIFTS;

    gl_prog_QuerySelectAllFromTagUsersTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_TAGUSER;
    gl_prog_QueryUpdateAllFromTagUsersTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_TAGUSER;

    gl_prog_QuerySelectAllFromTagTerminalsTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_TAGTERMINAL;
    gl_prog_QueryUpdateAllFromTagTerminalsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_TAGTERMINAL;

    gl_prog_QuerySelectAllFromHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_HOLIDAYS;
    gl_prog_QueryUpdateAllFromHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_HOLIDAYS;

    gl_prog_QuerySelectAllFromHolidaysSetsTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_HOLIDAYSSETS;
    gl_prog_QueryUpdateAllFromHolidaysSetsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_HOLIDAYSSETS;

    gl_prog_QuerySelectAllFromHolidaysGroupsTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_HOLIDAYSGROUPS;
    gl_prog_QueryUpdateAllFromHolidaysGroupsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_HOLIDAYSGROUPS;

    gl_prog_QuerySelectAllFromTagHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_TAGHOLIDAY;
    gl_prog_QueryUpdateAllFromTagHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_TAGHOLIDAY;

    gl_prog_QuerySelectAllFromTagAccessPairsTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_TAGACCESSPAIR;
    gl_prog_QueryUpdateAllFromTagAccessPairsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_TAGACCESSPAIR;

    gl_prog_QuerySelectAllFromTagsTable = gl_Prog_QueryDirectory + "/" + QUERY_SELECTALL_TAG;
    gl_prog_QueryUpdateAllFromTagsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATEALL_TAG;


    gl_prog_QueryDeleteFromTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_TIMEZONES;
    gl_prog_QueryUpdateToTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_TIMEZONES;
    gl_prog_QueryInsertToTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_TIMEZONES;

    gl_prog_QueryDeleteFromAccessTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_ACCESSTIMEZONES;
    gl_prog_QueryUpdateToAccessTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_ACCESSTIMEZONES;
    gl_prog_QueryInsertToAccessTimezonesTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_ACCESSTIMEZONES;

    gl_prog_QueryDeleteFromAccessShiftsTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_ACCESSSHIFTS;
    gl_prog_QueryUpdateToAccessShiftsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_ACCESSSHIFTS;
    gl_prog_QueryInsertToAccessShiftsTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_ACCESSSHIFTS;

    gl_prog_QueryDeleteFromTagUsersTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_TAGUSERS;
    gl_prog_QueryUpdateToTagUsersTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_TAGUSERS;
    gl_prog_QueryInsertToTagUsersTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_TAGUSERS;

    gl_prog_QueryDeleteFromTagTerminalsTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_TAGTERMINALS;
    gl_prog_QueryUpdateToTagTerminalsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_TAGTERMINALS;
    gl_prog_QueryInsertToTagTerminalsTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_TAGTERMINALS;

    gl_prog_QueryDeleteFromHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_HOLIDAYS;
    gl_prog_QueryUpdateToHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_HOLIDAYS;
    gl_prog_QueryInsertToHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_HOLIDAYS;

    gl_prog_QueryDeleteFromHolidaysSetsTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_HOLIDAYSSETS;
    gl_prog_QueryUpdateToHolidaysSetsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_HOLIDAYSSETS;
    gl_prog_QueryInsertToHolidaysSetsTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_HOLIDAYSSETS;

    gl_prog_QueryDeleteFromHolidaysGroupsTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_HOLIDAYSGROUPS;
    gl_prog_QueryUpdateToHolidaysGroupsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_HOLIDAYSGROUPS;
    gl_prog_QueryInsertToHolidaysGroupsTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_HOLIDAYSGROUPS;

    gl_prog_QueryDeleteFromTagHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_TAGHOLIDAYS;
    gl_prog_QueryUpdateToTagHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_TAGHOLIDAYS;
    gl_prog_QueryInsertToTagHolidaysTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_TAGHOLIDAYS;

    gl_prog_QueryDeleteFromTagAccessPairsTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_TAGACCESSPAIRS;
    gl_prog_QueryUpdateToTagAccessPairsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_TAGACCESSPAIRS;
    gl_prog_QueryInsertToTagAccessPairsTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_TAGACCESSPAIRS;

    gl_prog_QueryDeleteFromTagsTable = gl_Prog_QueryDirectory + "/" + QUERY_DELETE_FROM_TAGS;
    gl_prog_QueryUpdateToTagsTable = gl_Prog_QueryDirectory + "/" + QUERY_UPDATE_TO_TAGS;
    gl_prog_QueryInsertToTagsTable = gl_Prog_QueryDirectory + "/" + QUERY_INSERT_TO_TAGS;


    if(!QFile::exists(gl_Prog_ConfigFileName)){
        QFile file(gl_Prog_ConfigFileName);
        CreateConfigFile();
    }

    return;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    gl_Prog_MainDirectory = QString("/opt/") + APP_NAME + QString("/bin");
    QDir dir;
    if(!dir.exists(gl_Prog_MainDirectory))
        dir.mkdir(gl_Prog_MainDirectory);

    CheckConfigDirFile();

    QSettings setting(gl_Prog_ConfigFileName,QSettings::IniFormat);
    bool bDebug = setting.value(MAIN_LOG, MAIN_LOG_DEFAULT).toBool();

    qInfo() << "config: Log to File: " << bDebug;
    qInfo() << "debug/release: Log to File: " << MAIN_LOG_TO_FILE;

    #ifndef QT_DEBUG
        if(bDebug){
            qInfo() << "debug to file";
            qInstallMessageHandler(myMessageOutput);
        }
        else{
            qInfo() << "debug to console";
        }
    #endif

    #ifdef QT_DEBUG
        qInfo() << "";
        qInfo() << "";

        qInfo() << MAIN << "main directory of the app = " << gl_Prog_MainDirectory;
        qInfo() << MAIN << "config sub-directory of the app  = " << gl_Prog_ConfigDirectory;
        qInfo() << MAIN << "log sub-directory of the app = " << gl_Prog_LogDirectory;
        qInfo() << MAIN << "query sub-directory name of the app = " << gl_Prog_QueryDirectory;

        qInfo() << "";
        qInfo() << "";
        qInfo() << MAIN << "config file name of the app  = " << gl_Prog_ConfigFileName;
        qInfo() << MAIN << "watchdog file name of the app  = " << gl_Prog_WatchFileName;

        qInfo() << "";
        qInfo() << "";
        qInfo() << MAIN << "query find user file name  = " << gl_prog_QueryFindUser;
        qInfo() << MAIN << "query find reader file name  = " << gl_prog_QueryFindReader;
        qInfo() << MAIN << "query check user holiday file name  = " << gl_prog_QueryCheckHol;
        qInfo() << MAIN << "query check user access pair file name  = " << gl_prog_QueryCheckPair;
        qInfo() << MAIN << "query check user access time file name  = " << gl_prog_QueryCheckAccess;

        qInfo() << "";
        qInfo() << "";
        qInfo() << MAIN << "query insert log = " << gl_prog_QueryInsertLog;
        qInfo() << MAIN << "query insert inp = " << gl_prog_QueryInsertInp;

        qInfo() << MAIN << "query find not sent log = " << gl_prog_QueryFindLog;
        qInfo() << MAIN << "query updated at logs = " << gl_prog_QueryUpdatedAtLogs;

        qInfo() << MAIN << "query find not sent inp = " << gl_prog_QueryFindInp;
        qInfo() << MAIN << "query updated at inps = " << gl_prog_QueryUpdatedAtInps;

        qInfo() << MAIN << "query delete logs from last 3 days ago before = " << gl_prog_QueryDeleteLogs;
        qInfo() << MAIN << "query delete inps from last 3 days ago before = " << gl_prog_QueryDeleteInps;

        qInfo() << MAIN << "query update log = " << gl_prog_QueryUpdateLog;
        qInfo() << MAIN << "query update inp = " << gl_prog_QueryUpdateInp;

        qInfo() << MAIN << "query insert logand = " << gl_prog_QueryInsertLogAnd;
        qInfo() << MAIN << "query find not sent logand = " << gl_prog_QueryFindLogAnd;
        qInfo() << MAIN << "query updated at logsand = " << gl_prog_QueryUpdatedAtLogsAnd;
        qInfo() << MAIN << "query delete logsand from last 3 days ago before = " << gl_prog_QueryDeleteLogsAnd;
        qInfo() << MAIN << "query update logand = " << gl_prog_QueryUpdateLogAnd;

        qInfo() << "";
        qInfo() << "";
        qInfo() << MAIN << "query select all from timezonestable = " << gl_prog_QuerySelectAllFromTimezonesTable;
        qInfo() << MAIN << "query update all from timezonestable = " << gl_prog_QueryUpdateAllFromTimezonesTable;

        qInfo() << MAIN << "query select all from accesstimezonestable = " << gl_prog_QuerySelectAllFromAccessTimezonesTable;
        qInfo() << MAIN << "query update all from accesstimezonestable = " << gl_prog_QueryUpdateAllFromAccessTimezonesTable;

        qInfo() << MAIN << "query select all from accessshiftstable = " << gl_prog_QuerySelectAllFromAccessShiftsTable;
        qInfo() << MAIN << "query update all from accessshiftstable = " << gl_prog_QueryUpdateAllFromAccessShiftsTable;

        qInfo() << MAIN << "query select all from tag_userstable = " << gl_prog_QuerySelectAllFromTagUsersTable;
        qInfo() << MAIN << "query update all from tag_userstable = " << gl_prog_QueryUpdateAllFromTagUsersTable;

        qInfo() << MAIN << "query select all from tag_terminalstable = " << gl_prog_QuerySelectAllFromTagTerminalsTable;
        qInfo() << MAIN << "query update all from tag_terminalstable = " << gl_prog_QueryUpdateAllFromTagTerminalsTable;

        qInfo() << MAIN << "query select all from holidays = " << gl_prog_QuerySelectAllFromHolidaysTable;
        qInfo() << MAIN << "query update all from holidays = " << gl_prog_QueryUpdateAllFromHolidaysTable;

        qInfo() << MAIN << "query select all from holidayssets = " << gl_prog_QuerySelectAllFromHolidaysSetsTable;
        qInfo() << MAIN << "query update all from holidayssets = " << gl_prog_QueryUpdateAllFromHolidaysSetsTable;

        qInfo() << MAIN << "query select all from holidaysgroups = " << gl_prog_QuerySelectAllFromHolidaysGroupsTable;
        qInfo() << MAIN << "query update all from holidaysgroups = " << gl_prog_QueryUpdateAllFromHolidaysGroupsTable;

        qInfo() << MAIN << "query select all from tag_holidays = " << gl_prog_QuerySelectAllFromTagHolidaysTable;
        qInfo() << MAIN << "query update all from tag_holidays = " << gl_prog_QueryUpdateAllFromTagHolidaysTable;

        qInfo() << MAIN << "query select all from tag_accesspairstable = " << gl_prog_QuerySelectAllFromTagAccessPairsTable;
        qInfo() << MAIN << "query update all from tag_accesspairstable  = " << gl_prog_QueryUpdateAllFromTagAccessPairsTable;

        qInfo() << MAIN << "query select all from tagstable = " << gl_prog_QuerySelectAllFromTagsTable;
        qInfo() << MAIN << "query update all from tagstable = " << gl_prog_QueryUpdateAllFromTagsTable;

        qInfo() << "";
        qInfo() << "";
        qInfo() << MAIN << "main--> " << "query delete from timezones table = " << gl_prog_QueryDeleteFromTimezonesTable;
        qInfo() << MAIN << "main--> " << "query update to timezones table = " << gl_prog_QueryUpdateToTimezonesTable;
        qInfo() << MAIN << "main--> " << "query insert to timezones table = " << gl_prog_QueryInsertToTimezonesTable;

        qInfo() << MAIN << "main--> " << "query delete from accesstimezones table = " << gl_prog_QueryDeleteFromAccessTimezonesTable;
        qInfo() << MAIN << "main--> " << "query update to accesstimezones table = " << gl_prog_QueryUpdateToAccessTimezonesTable;
        qInfo() << MAIN << "main--> " << "query insert to accesstimezones table = " << gl_prog_QueryInsertToAccessTimezonesTable;

        qInfo() << MAIN << "main--> " << "query delete from acccessshifts table = " << gl_prog_QueryDeleteFromAccessShiftsTable;
        qInfo() << MAIN << "main--> " << "query update to acccessshifts table = " << gl_prog_QueryUpdateToAccessShiftsTable;
        qInfo() << MAIN << "main--> " << "query insert to acccessshifts table = " << gl_prog_QueryInsertToAccessShiftsTable;

        qInfo() << MAIN << "main--> " << "query delete from tagusers table = " << gl_prog_QueryDeleteFromTagUsersTable;
        qInfo() << MAIN << "main--> " << "query update to tagusers table = " << gl_prog_QueryUpdateToTagUsersTable;
        qInfo() << MAIN << "main--> " << "query insert to tagusers table = " << gl_prog_QueryInsertToTagUsersTable;

        qInfo() << MAIN << "main--> " << "query delete from tagterminals table = " << gl_prog_QueryDeleteFromTagTerminalsTable;
        qInfo() << MAIN << "main--> " << "query update to tagterminals table = " << gl_prog_QueryUpdateToTagTerminalsTable;
        qInfo() << MAIN << "main--> " << "query insert to tagterminals table = " << gl_prog_QueryInsertToTagTerminalsTable;

        qInfo() << MAIN << "main--> " << "query delete from tagholidays table = " << gl_prog_QueryDeleteFromTagHolidaysTable;
        qInfo() << MAIN << "main--> " << "query update to tagholidays table = " << gl_prog_QueryUpdateToTagHolidaysTable;
        qInfo() << MAIN << "main--> " << "query insert to tagholidays table = " << gl_prog_QueryInsertToTagHolidaysTable;

        qInfo() << MAIN << "main--> " << "query delete from tagaccesspairs table = " << gl_prog_QueryDeleteFromTagAccessPairsTable;
        qInfo() << MAIN << "main--> " << "query update to tagaccesspairs table = " << gl_prog_QueryUpdateToTagAccessPairsTable;
        qInfo() << MAIN << "main--> " << "query insert to tagaccesspairs table = " << gl_prog_QueryInsertToTagAccessPairsTable;

        qInfo() << MAIN << "main--> " << "query delete from tags table = " << gl_prog_QueryDeleteFromTagsTable;
        qInfo() << MAIN << "main--> " << "query update to tags table = " << gl_prog_QueryUpdateToTagsTable;
        qInfo() << MAIN << "main--> " << "query insert to tags table = " << gl_prog_QueryInsertToTagsTable;

        qInfo() << "";
        qInfo() << "";
        qInfo() << MAIN << "************************                                                                                                *************************";
        qInfo() << MAIN << "*************************************************************************************************************************************************";
        qInfo().noquote() << MAIN << "**************************************************** START " << COMPANY_NAME + QString(" - ") + APP_PROFILE + " - " + QString(APP_VERSION) << "****************************************************";
        qInfo() << MAIN << "*************************************************************************************************************************************************";
        qInfo() << MAIN << "************************                                                                                                *************************";
    #else
        qInfo() << "";
        qInfo() << "";
        qInfo() << MAIN << "************************                                                                                                *************************";
        qInfo() << MAIN << "*************************************************************************************************************************************************";
        qInfo().noquote() << MAIN << "**************************************************** START " << COMPANY_NAME + QString(" - ") + APP_PROFILE + " - " + QString(APP_VERSION) << "****************************************************";
        qInfo() << MAIN << "*************************************************************************************************************************************************";
        qInfo() << MAIN << "************************                                                                                                *************************";
    #endif



    MainClass *M;
    M = new MainClass;
    QObject::connect(M, SIGNAL(StartWorker()), M, SLOT(on_StartWorker()), Qt::QueuedConnection);
    emit M->StartWorker();



    return a.exec();
}

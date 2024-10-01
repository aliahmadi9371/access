#include "mainclass.h"

mainClass::mainClass(QObject *parent) : QObject(parent)
{
    config();
    while(get_dir_size() > limit_size_B);
    qDebug() << "finish";
    QApplication::quit();
}


void mainClass::config()
{
    configDirectory = QString("/opt/") + APP_NAME + QString("/bin");
    configFileName = configDirectory + "/" + FILE_INI;
    QSettings setting(configFileName,QSettings::IniFormat);
    log_directory = setting.value("Main/dir", "/opt/Access-V1-0-0/bin/log").toString();
    limit_size_M = setting.value("Main/limitM", 1000).toInt();
    limit_size_B = (static_cast<qint64>(limit_size_M)) * 1000 * 1024;
//    limit_size_B = 135176;

    qDebug() << "log directory: "<< log_directory;
    qDebug() << "limit size M: "<< limit_size_M;
    qDebug() << "limit size B: "<< limit_size_B;
    qDebug() << "";
    qDebug() << "";

    return;
}

qint64 mainClass::get_dir_size()
{
    QDirIterator it(log_directory, QDirIterator::Subdirectories);
    qint64 total = 0;
    bool first = false;

    while (it.hasNext()) {
        // check if entry is file
        if(it.fileInfo().isFile()){
            total += it.fileInfo().size();
            qDebug() << "file path: " << it.fileInfo().filePath() <<
                        ", birth time: " << it.fileInfo().birthTime().toString("yyyy/MM/dd - hh:mm::ss") <<
                        ", size: " << it.fileInfo().size();
            if(!first){
                first = true;
                oldest_dataTime = it.fileInfo().birthTime();
                oldest_fileInfo = it.fileInfo();
            }
            else{
                if(oldest_dataTime > it.fileInfo().birthTime()){
                    oldest_dataTime =it.fileInfo().birthTime();
                    oldest_fileInfo = it.fileInfo();
                }
            }


        }
        it.next();
    }

    // if there is a file left "at the end" get it's size
    if(it.fileInfo().isFile()) {
        total+= it.fileInfo().size();
        qDebug() << "file path: " << it.fileInfo().filePath() <<
                    ", birth time: " << it.fileInfo().birthTime().toString("yyyy/MM/dd - hh:mm::ss") <<
                    ", size: " << it.fileInfo().size();
    }


    qDebug() << "total size = " << total;
    if(total > limit_size_B){

        qDebug() << "total > limit, this file sould be deleted";
        qDebug() << "oldest file info: file path: " << oldest_fileInfo.filePath() <<
                    ", birth time: " << oldest_fileInfo.birthTime().toString("yyyy/MM/dd - hh:mm::ss") <<
                    ", size: " << oldest_fileInfo.size();
        QDir current_dir(oldest_fileInfo.path());
        qDebug() << oldest_fileInfo.path();
        qDebug() << oldest_fileInfo.fileName();
        if(!current_dir.remove(oldest_fileInfo.fileName())){
            qDebug() << "can't delete file";
            QApplication::quit();
        }
    }

    qDebug() << "";
    qDebug() << "";

    return total;
}

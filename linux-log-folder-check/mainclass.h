#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include <QSettings>
#include <QDirIterator>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QApplication>

#define APP_NAME "log_file_checker"
#define FILE_INI "config.ini"

class mainClass : public QObject
{
    Q_OBJECT
public:
    explicit mainClass(QObject *parent = nullptr);
    void config();
    qint64 get_dir_size();

    QString configDirectory{};
    QString configFileName{};

    QString log_directory{};

    int limit_size_M{};
    qint64 limit_size_B{};

    QDateTime oldest_dataTime;
    QFileInfo oldest_fileInfo;

signals:

};

#endif // MAINCLASS_H

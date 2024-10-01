#include <QCoreApplication>
#include "mainclass.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    for(int i{0}; i < argc; ++i){
        qDebug().noquote() << "arg["  << QString::number(i) << "] = " << argv[i];
    }

    if(argc > 1){
        mainclass* m{};
        m = new mainclass(nullptr, argv[1]);
    }
    else{
//        mainclass* m{};
//        m = new mainclass(nullptr, "update.hex");
//        m->f_writeToSerial("test");
        qDebug() << "please provide file name as first option . . .";
        return 10;
    }

    return a.exec();
}

#include <QCoreApplication>
#include <mainclass.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    mainClass *M;
    M = new mainClass;

    return 0;
}

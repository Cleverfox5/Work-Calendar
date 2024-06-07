#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    setlocale(0, "");
    QCoreApplication a(argc, argv);
    server s;
    return a.exec();
}

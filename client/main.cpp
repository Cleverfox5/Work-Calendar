#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Авторизация");
    QIcon icon(":/resurses/images/logo.png");
    w.setWindowIcon(icon);
    w.show();
    return a.exec();
}

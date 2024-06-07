#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTableView>
#include <QLabel>
#include <QMessageBox>
#include <QTcpSocket>
#include "calendarlist.h"
#include "inputdatadialog.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void slotReadyRead();

private slots:
    void on_pushButton_clicked();
    void on_loginButton_clicked();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString loggedInUsername;
    calendarList *calendars;
    InputDataDialog *input_data_dialog;
    int Descriptor;
    QTcpSocket *socket;
    QByteArray Data;
    void SendToServer(QString str, quint16 curr_mode);
    quint16 nextBlockSize;
    quint16 mode;
    QString login;

signals:
    void senData(QString log);

};
#endif // MAINWINDOW_H

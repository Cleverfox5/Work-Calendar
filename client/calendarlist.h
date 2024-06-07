#ifndef CALENDARLIST_H
#define CALENDARLIST_H

#include <QDialog>
#include <QTcpSocket>
#include "addcalendar.h"
#include "calendarwindow.h"

namespace Ui {
class calendarList;
}

class calendarList : public QDialog
{
    Q_OBJECT

public:
    explicit calendarList(QWidget *parent = nullptr);
    ~calendarList();
    int Descriptor;
    void SendToServer(QString str, quint16 curr_mode);
    QString log;

private:
    Ui::calendarList *ui;
    QTcpSocket *socket;
    QByteArray Data;
    quint16 nextBlockSize;
    quint16 mode;
    AddCalendar *add_calendar;
    CalendarWindow *calendar_window;

signals:
    void sendDescriptor(QTcpSocket * soc, QString login);
    void sendData(QString str, QString login);
    void SendCalendarInformation(QTcpSocket * soc, QString calendar_information, QString login);
    void sendBrockerMembers(QString str);
    void sendSignal(QString str);
    void sendMessages(QString str);
    void sendNewMessage(QString str);

public slots:
    void getID(QString login);
    void slotReadyRead();
private slots:
    void on_pushButton_clicked();
    void on_listWidget_clicked(const QModelIndex &index);
};

#endif // CALENDARLIST_H

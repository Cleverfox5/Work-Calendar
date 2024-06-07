#ifndef CALENDARWINDOW_H
#define CALENDARWINDOW_H

#include <QDialog>
#include <QTcpSocket>
#include <QPointer>
#include <vector>

#include "weekwindow.h"
#include "creatingevents.h"

namespace Ui {
class CalendarWindow;
}

class CalendarWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CalendarWindow(QWidget *parent = nullptr);
    ~CalendarWindow();
    void SendToServer(QString str, quint16 curr_mode);
    QTcpSocket *socket;//при входе
    bool is_creator = false;
    QString calendar_name = "";
    int calendar_id = 0;
    QString login;
    WeekWindow * week_window;
    CreatingEvents * creating_events;
    QString days[42];

    int day;
    int month;
    int year;

private:
    Ui::CalendarWindow *ui;
    QByteArray Data;
    QVector<QString> drop_event;
    int first_day_in_month = 0;
    //void SendToServer(QString str, quint16 curr_mode);
    quint16 nextBlockSize;
    void cleaner();
    void next();
    void previous();
    void setEvents();
    void broker(int week_number);
    QString chooseColor(int count_events);
    QVector<QString> months = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    QVector<int> count_day_in_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    QVector<QString> color_range = {"#A6FB55", "#FFFE56", "#FFD151", "#FFA252", "#FC4D51"};
    void DrawData(int selected_year, int selecet_month);

signals:
    void SendCalendarInformation(QTcpSocket * socket, QString calendar_name, int calendar_id, bool is_creator, QString login, QString events_id[7], QVector<QString> numbers, int year, QString month, QString days[]);
    void sendStartInformatoinEvent(QTcpSocket * soc, QString login, int calendar_id, int selected_year, int selected_month, int selected_day);
    void sendMembers(QString str);
    void sendMessages(QString str);
    void sendNewMessage(QString str);

public slots:
    void GetCalendarInformation(QTcpSocket * soc, QString calendar_information, QString login);
    void getBrockerMembers(QString str);
    void getEvents(QString str);
    void getMessages(QString str);
    void getNewMessage(QString str);

private slots:
    void on_pushButton_sudema_clicked();
    void on_pushButton_tudema_clicked();
    void first_week();
    void second_week();
    void third_week();
    void fourth_week();
    void fifth_week();
    void sixth_week();
    void on_pushButton_calendarName_clicked();
    void on_pushButton_addEvent_clicked();
    void on_pushButton_Week_clicked();
};

#endif // CALENDARWINDOW_H

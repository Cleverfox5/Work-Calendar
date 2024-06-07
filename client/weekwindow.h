#ifndef WEEKWINDOW_H
#define WEEKWINDOW_H

#include "qlistwidget.h"
#include <QDialog>
#include <QTcpSocket>
#include "eventwindow.h"

namespace Ui {
class WeekWindow;
}

class WeekWindow : public QDialog
{
    Q_OBJECT

public:
    explicit WeekWindow(QWidget *parent = nullptr);
    ~WeekWindow();

private:
    Ui::WeekWindow *ui;
    EventWindow * event_window;
    void DrowEvents(QString days[42], int start_position, int end_position);
    void add_functional_to_events();
    QTcpSocket * socket;
    QString calendar_name;
    int calendar_id;
    QString login;
    bool is_creator;
    QString events_id[7];
    int year;
    QString month;
    int day;
    QString days[42];

signals:
    void sendEventLabel(QTcpSocket * socket, QString label, QString login, QString calendar_name, int calendar_id, int event_id);
    void sendMessages(QString str);
    void sendNewMessage(QString str);

public slots:
    void GetCalendarInformation(QTcpSocket * socket, QString calendar_name, int calendar_id, bool is_creator, QString login, QString events_id[7], QVector<QString> numbers, int year, QString month, QString days[]);
    void getMessages(QString str);
    void getNewMessage(QString str);
private slots:
    void on_pushButton_month_clicked();
    void on_event_clicked(QListWidgetItem *item);
};

#endif // WEEKWINDOW_H

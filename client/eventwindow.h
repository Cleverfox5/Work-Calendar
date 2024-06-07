#ifndef EVENTWINDOW_H
#define EVENTWINDOW_H

#include <QDialog>
#include <QTcpSocket>
#include <QTime>

namespace Ui {
class EventWindow;
}

class EventWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EventWindow(QWidget *parent = nullptr);
    ~EventWindow();
    void SendToServer(QString str, quint16 curr_mode);

public slots:
    void getEventLabel(QTcpSocket * socket, QString label, QString login, QString calendar_name, int calendar_id, int event_id);
    void getMessages(QString str);
    void getNewMessage(QString str);

private slots:
    void on_pushButton_send_clicked();

private:
    Ui::EventWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    quint16 nextBlockSize;

    QString login;
    int calendar_id;
    int event_id;
};

#endif // EVENTWINDOW_H

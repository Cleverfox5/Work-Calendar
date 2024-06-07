#ifndef ADDCALENDAR_H
#define ADDCALENDAR_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class AddCalendar;
}

class AddCalendar : public QDialog
{
    Q_OBJECT

public:
    explicit AddCalendar(QWidget *parent = nullptr);
    ~AddCalendar();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddCalendar *ui;
    QTcpSocket *socket;
    QByteArray Data;
    void SendToServer(QString str, quint16 curr_mode);
    quint16 nextBlockSize;
    quint16 mode;
    QString log;

public slots:
    void getDescriptor(QTcpSocket * soc, QString login);
    void getData(QString str);
    //void slotReadyRead();
};

#endif // ADDCALENDAR_H

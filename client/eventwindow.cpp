#include "eventwindow.h"
#include "ui_eventwindow.h"

EventWindow::EventWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventWindow)
{
    ui->setupUi(this);
}

EventWindow::~EventWindow()
{
    delete ui;
}

void EventWindow::getEventLabel(QTcpSocket *socket, QString label, QString login, QString calendar_name, int calendar_id, int event_id)
{
    ui->textBrowser->clear();

    this->socket = socket;
    ui->pushButton_event_name->setText(label);
    this->login = login;
    ui->pushButton_calendar_name->setText(calendar_name);
    this->calendar_id = calendar_id;
    this->event_id = event_id;
    //SendToServer(QString::number(event_id) + "|" + login, 8);
    SendToServer(QString::number(event_id), 8);
}

void EventWindow::getMessages(QString str)
{
    QVector<QString> massages = str.split("|");
    for (int i = 0; i < massages.size(); i++){
        ui->textBrowser->append(massages[i]);
    }
}

void EventWindow::getNewMessage(QString str)
{
    QVector<QString> info = str.split("|");
    if (info[1] == QString::number(event_id))
        ui->textBrowser->append(info[0]);
}

void EventWindow::on_pushButton_send_clicked()
{
    QString massage = ui->lineEdit->text();
    if (!massage.contains("|") && massage != "" && !massage.contains("\\")){
        SendToServer(login + "|" + QDate::currentDate().toString("dd.MM.yy") + "|" + QTime::currentTime().toString("hh:mm") + "|" + QString::number(event_id) + "|" + massage, 9);
        ui->lineEdit->clear();
    }

}

void EventWindow::SendToServer(QString str, quint16 curr_mode)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    if (curr_mode != 1){
        out << quint16(0) << curr_mode << str;
        out.device()->seek(0);
        out << quint16(Data.size() - 2*sizeof(quint16));
        socket->write(Data);
    }
}


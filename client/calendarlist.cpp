#include "calendarlist.h"
#include "ui_calendarlist.h"
#include <QString>
#include <QTime>
#include "calendarwindow.h"

calendarList::calendarList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::calendarList)
{
    ui->setupUi(this);
    add_calendar = new AddCalendar(this);
    calendar_window = new CalendarWindow(this);
}

void calendarList::getID(QString login)
{
    socket = new QTcpSocket(this);
    log = login;

    connect(socket, &QTcpSocket::readyRead, this, &calendarList::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;
    mode = 0;
    socket->connectToHost("127.0.0.1", 2323);
    SendToServer(login, 4);
}

void calendarList::SendToServer(QString str, quint16 curr_mode)
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

void calendarList::slotReadyRead()
{
    QDataStream in (socket);
    in.setVersion(QDataStream::Qt_6_6);
    if(in.status() == QDataStream::Ok){
        for (;;){
            if (nextBlockSize == 0){
                if (socket->bytesAvailable() < 2){
                    break;
                }
                in >> nextBlockSize;
                in >> mode;
            }
            if (socket->bytesAvailable() < nextBlockSize){
                break;
            }
            QString str;
            nextBlockSize = 0;
            in >> str;
            if (mode == 1){
                /*QString str;
                QTime time;
                in >> time >> str;
                nextBlockSize = 0;
                //ui->textBrowser->append(time.toString() + " " + str);
                mode = 0;*/
            }
            else if (mode == 2){
                //ui->listWidget->addItem(str);
            }
            else if (mode == 4){

                if (str == "False"){
                    //Не успех, не ура
                }
                else{
                    int len = str.size();
                    QString curr_str = "";
                    int counter = 0;
                    for (int i = 0; i < len; i++){
                        if (str[i] == ' '){
                            counter++;
                            if (counter == 2){
                                ui->listWidget->addItem(curr_str);
                                curr_str = "";
                                counter = 0;
                            }
                            else{
                                curr_str.push_back(str[i]);
                            }
                        }
                        else if (i == len - 1){
                            curr_str.push_back(str[i]);
                            ui->listWidget->addItem(curr_str);
                        }
                        else{
                            curr_str.push_back(str[i]);
                        }
                    }
                }
            }
            else if (mode == 5){
                connect(this, &calendarList::sendData, add_calendar, &AddCalendar::getData);

                emit sendData(str, log);
                /*int len = str.size();
                QString curr_str = "";
                for (int i = 0; i < len; i++){
                    if (str[i] == ' '){
                        ui->listWidget->addItem(curr_str);
                        curr_str = "";
                    }
                    else if (i == len - 1){
                        ui->listWidget->addItem(curr_str);
                    }
                    else{
                        curr_str.push_back(str[i]);
                    }
                }*/
            }
            else if (mode == 6){
                ui->listWidget->addItem(str);
            }
            else if (mode == 7){
                //CalendarWindow::getEvents
                disconnect(this, &calendarList::sendSignal, calendar_window, &CalendarWindow::getEvents);
                connect(this, &calendarList::sendSignal, calendar_window, &CalendarWindow::getEvents);
                emit sendSignal(str);
            }
            else if (mode == 8){
                if (str != ""){
                    disconnect(this, &calendarList::sendMessages, calendar_window, &CalendarWindow::getMessages);
                    connect(this, &calendarList::sendMessages, calendar_window, &CalendarWindow::getMessages);
                    emit sendMessages(str);
                }
            }
            else if (mode == 9){
                disconnect(this, &calendarList::sendNewMessage, calendar_window, &CalendarWindow::getNewMessage);
                connect(this, &calendarList::sendNewMessage, calendar_window, &CalendarWindow::getNewMessage);
                sendNewMessage(str);
            }
            else if (mode == 21){
                disconnect(this, &calendarList::sendBrockerMembers, calendar_window, &CalendarWindow::getBrockerMembers);
                connect(this, &calendarList::sendBrockerMembers, calendar_window, &CalendarWindow::getBrockerMembers);
                emit sendBrockerMembers(str);
            }
        }
    }
    else{
        // !!Сообщение: отсутствует соединение!!
    }
}

void calendarList::on_pushButton_clicked()//Нажали на добавить календарь
{
    delete add_calendar;
    add_calendar = new AddCalendar(this);
    add_calendar->show();

    connect(this, &calendarList::sendDescriptor, add_calendar, &AddCalendar::getDescriptor);

    emit sendDescriptor(socket, log);
}

void calendarList::on_listWidget_clicked(const QModelIndex &index)//index оставляем, ибо такова структура on_listWidget_clicked
{
    QString selected_item = ui->listWidget->currentItem()->text();

    delete calendar_window;
    calendar_window = new CalendarWindow(this);
    calendar_window->show();

    connect(this, &calendarList::SendCalendarInformation, calendar_window, &CalendarWindow::GetCalendarInformation);

    //Открытие календаря(передача сокета, имени календаря, автор каленадря или нет, id календаря)
    emit SendCalendarInformation(socket, selected_item, log);
}

calendarList::~calendarList()
{
    delete ui;
    delete add_calendar;
    delete calendar_window;
    delete socket;
}

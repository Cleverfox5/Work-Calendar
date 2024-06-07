#include "weekwindow.h"
#include "ui_weekwindow.h"

WeekWindow::WeekWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WeekWindow)
{
    ui->setupUi(this);
    add_functional_to_events();
    event_window = new EventWindow(this);
    connect(this, &WeekWindow::sendEventLabel, event_window, &EventWindow::getEventLabel);
}

void WeekWindow::on_event_clicked(QListWidgetItem *item){
    QVector<QString> event_parts = item->text().split(" ");
    QString event_label;
    for (int i = 0; i < event_parts.size() - 1; i++){
        event_label += event_parts[i] + " ";
    }
    event_label.removeLast();
    sendEventLabel(socket, event_label, login, calendar_name, calendar_id, event_parts[event_parts.size() - 1].toInt());
    event_window->show();
}

void WeekWindow::add_functional_to_events(){
    QListWidget * list;
    for (int i = 0; i < 7; i++){
        list = this->findChild<QListWidget *>("listWidget_" + QString::number(i + 1));
        connect(list, &QListWidget::itemClicked, this, &WeekWindow::on_event_clicked);
        if (!list){
            qDebug() << "Ошибка с ListWisget";
        }
    }
    for (int i = 0; i < 7; i++){
        list = this->findChild<QListWidget *>("listWidget_" + QString::number(i + 1) + "_" + QString::number(i + 1));
        connect(list, &QListWidget::itemClicked, this, &WeekWindow::on_event_clicked);
        if (!list){
            qDebug() << "Ошибка с ListWisget";
        }
    }
}

void WeekWindow::DrowEvents(QString days[], int start_position, int end_position){
    QPushButton *start_pushButton = this->findChild<QPushButton *>("pushButton_" + QString::number(start_position + 1));
    QPushButton *end_pushButton = this->findChild<QPushButton *>("pushButton_" + QString::number(end_position + 1));

    int counter = start_position + 1;

    for (int i = start_pushButton->text().toInt(); i <= end_pushButton->text().toInt(); i++){
        if (days[i] != ""){
            QVector<QString> day_arr;
            day_arr = days[i].split("|");
            for (int j = 0; j < day_arr.size(); j++){
                QVector<QString> event_arr = day_arr[j].split(" ");
                QListWidget * list_widget;
                QString adder_name;
                if (event_arr[event_arr.size() - 1] == "true"){
                    list_widget = this->findChild<QListWidget *>("listWidget_" + QString::number(counter));
                    for (int k = 0; k < event_arr.size() - 1; k++){
                        adder_name += event_arr[k] + " ";
                    }
                    adder_name.removeLast();
                    list_widget->addItem(adder_name);
                }
                else{
                    list_widget = this->findChild<QListWidget *>("listWidget_" + QString::number(counter) + "_" + QString::number(counter));
                    for (int k = 0; k < event_arr.size() - 1; k++){
                        adder_name += event_arr[k] + " ";
                    }
                    adder_name.removeLast();
                    list_widget->addItem(adder_name);
                }
            }
        }
        counter++;
    }
}

void WeekWindow::GetCalendarInformation(QTcpSocket * socket, QString calendar_name, int calendar_id, bool is_creator, QString login, QString events_id[7], QVector<QString> numbers, int year, QString month, QString days[]){
    this->socket = socket;
    this->calendar_name = calendar_name;
    ui->pushButton_calendarName->setText(calendar_name);
    this->calendar_id = calendar_id;
    this->is_creator = is_creator;
    this->login = login;
    this->month = month;
    this->year = year;
    int start_position = 0;
    int end_position = 6;

    for (int i = 0; i < 7; i++){
        if (numbers[0] == "*"){
            if (numbers[i] != "*"){
                QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(i + 1));
                button->setText(numbers[i]);
            }
            else{
                start_position++;
            }
        }
        else{
            if (numbers[i] != "*"){
                QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(i + 1));
                button->setText(numbers[i]);
            }
            else{
                end_position--;
            }
        }
    }
    ui->pushButton_month->setText(month + " " + QString::number(year));
    DrowEvents(days, start_position, end_position);
    //this->events_id = events_id;
}

void WeekWindow::getMessages(QString str)
{
    disconnect(this, &WeekWindow::sendMessages, event_window, &EventWindow::getMessages);
    connect(this, &WeekWindow::sendMessages, event_window, &EventWindow::getMessages);
    emit sendMessages(str);
}
void WeekWindow::getNewMessage(QString str)
{
    disconnect(this, &WeekWindow::sendNewMessage, event_window, &EventWindow::getNewMessage);
    connect(this, &WeekWindow::sendNewMessage, event_window, &EventWindow::getNewMessage);
    emit sendNewMessage(str);
}

WeekWindow::~WeekWindow()
{
    delete ui;
}

void WeekWindow::on_pushButton_month_clicked()
{
    reject();
}

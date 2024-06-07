#include "calendarwindow.h"
#include "ui_calendarwindow.h"
#include <QDate>

CalendarWindow::CalendarWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalendarWindow)
{
    ui->setupUi(this);

    QDate curr_data = QDate::currentDate();
    day = curr_data.day();
    month = curr_data.month();
    year = curr_data.year();

    int week_number = 0;

    //привязка кнопок к функциям
    for (int i = 0; i < 7; i++){
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(week_number*7 + i + 1));
        connect(button, SIGNAL(clicked()), this, SLOT(first_week()));
    }
    week_number++;
    for (int i = 0; i < 7; i++){
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(week_number*7 + i + 1));
        connect(button, SIGNAL(clicked()), this, SLOT(second_week()));
    }
    week_number++;
    for (int i = 0; i < 7; i++){
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(week_number*7 + i + 1));
        connect(button, SIGNAL(clicked()), this, SLOT(third_week()));
    }
    week_number++;
    for (int i = 0; i < 7; i++){
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(week_number*7 + i + 1));
        connect(button, SIGNAL(clicked()), this, SLOT(fourth_week()));
    }
    week_number++;
    for (int i = 0; i < 7; i++){
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(week_number*7 + i + 1));
        connect(button, SIGNAL(clicked()), this, SLOT(fifth_week()));
    }
    week_number++;
    for (int i = 0; i < 7; i++){
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(week_number*7 + i + 1));
        connect(button, SIGNAL(clicked()), this, SLOT(sixth_week()));
    }

    week_window = new WeekWindow(this);
    creating_events = new CreatingEvents(this);
}

void CalendarWindow::broker(int week_number){
    QString events_id[7];
    QVector<QString> numbers;
    for (int i = 0; i < 7; i++){
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(week_number*7 + i + 1));
        if (button->text() == ""){
            numbers.push_back("*");
        }
        else{
            numbers.push_back(button->text());
        }
    }
    for (int i = 0; i < 7; i++){
        events_id[i] = days[i + 1 + week_number*7];
    }

    week_window = new WeekWindow(this);
    week_window->show();

    connect(this, &CalendarWindow::SendCalendarInformation, week_window, &WeekWindow::GetCalendarInformation);

    emit SendCalendarInformation(socket, calendar_name, calendar_id, is_creator, login, events_id, numbers, year, months[month - 1], days);
}

void CalendarWindow::first_week(){
    broker(0);
}

void CalendarWindow::second_week(){
    broker(1);
}

void CalendarWindow::third_week(){
    broker(2);
}

void CalendarWindow::fourth_week(){
    broker(3);
}

void CalendarWindow::fifth_week(){
    if (ui->pushButton_29->text() != ""){
        broker(4);
    }
}

void CalendarWindow::sixth_week(){
    if (ui->pushButton_36->text() != ""){
        broker(5);
    }
}

void CalendarWindow::DrawData(int selected_year, int selecet_month){//вывод дат
    ui->label_2->setText(months[selecet_month - 1] + " " + QString::number(selected_year));
    if (selecet_month == 2){//Февраль
        if ((selected_year % 400 == 0) || ((selected_year % 4 == 0) && (selected_year % 100 != 0))){
            count_day_in_month[2 - 1] = 29;
        }
        else{
            count_day_in_month[2 - 1] = 28;
        }
    }
    QDate date(selected_year, selecet_month, 1);
    first_day_in_month = date.dayOfWeek();

    for (int i = first_day_in_month - 1; i < first_day_in_month - 1 + count_day_in_month[selecet_month - 1]; i++){
        QPushButton* button = this->findChild<QPushButton *>("pushButton_" + QString::number(i+1));
        if (button){
            button->setText(QString::number(i + 1 - first_day_in_month + 1));
        }
        else{
            ui->label_2->setText("Error");
        }
    }
    if (calendar_id != 0){
        SendToServer(QString::number(calendar_id) + " " + QString::number(year) + " " + QString::number(month) + " " + login, 7);
    }

    // !!Добавление рамки для текущего дня!!
}
//ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd
//слот получает информацию
void CalendarWindow::GetCalendarInformation(QTcpSocket * soc, QString calendar_information, QString login){
    socket = soc;

    if (calendar_information[0] == '*'){
        is_creator = true;
        calendar_information.remove(0, 1);
    }

    QVector<QString> information;

    information = calendar_information.split(" ");

    calendar_name = information[0];

    calendar_id = information[1].toInt();

    ui->pushButton_calendarName->setText(calendar_name);

    this->login = login;

    DrawData(year, month);
}

void CalendarWindow::getBrockerMembers(QString str)
{
    disconnect(this, &CalendarWindow::sendMembers, creating_events, &CreatingEvents::getMembers);
    connect(this, &CalendarWindow::sendMembers, creating_events, &CreatingEvents::getMembers);
    emit sendMembers(str);
}

QString CalendarWindow::chooseColor(int count_events){
    if (count_events < 4)
        return color_range[count_events];
    else
        return color_range[4];
}

void CalendarWindow::setEvents(){
    for (int i = 0; i < 42; i++){
        if (!(days[i] == "")){
            int count_events = days[i].count("|");
            count_events++;
            QPushButton* button = this->findChild<QPushButton *>("pushButton_" + QString::number(i - 1 + first_day_in_month));
            if (button) {
                QString color = chooseColor(count_events - 1);
                button->setStyleSheet(" background-color: " + color + ";");
            }
            else{
                ui->label_2->setText("Button error");
            }
        }
    }
}

void CalendarWindow::getEvents(QString str)
{
    if (str != ""){
        QVector<QString> data = str.split("|");
        for (int i = 0; i < data.size(); i += 4){
            drop_event.push_back(data[i] + "|" + data[i+1] + "|" + data[i+2] + "|" + data[i+3]);
            if (days[data[i].toInt()] == ""){
                days[data[i].toInt()] += data[i+2] + " " + data[i+1] + " " + data[i+3];
            }
            else{
                days[data[i].toInt()] += "|" + data[i+2] + " " + data[i+1] + " " + data[i+3];
            }
        }
        setEvents();
    }
}

void CalendarWindow::getMessages(QString str)
{
    disconnect(this, &CalendarWindow::sendMessages, week_window, &WeekWindow::getMessages);
    connect(this, &CalendarWindow::sendMessages, week_window, &WeekWindow::getMessages);
    emit sendMessages(str);
}

void CalendarWindow::getNewMessage(QString str)
{
    disconnect(this, &CalendarWindow::sendNewMessage, week_window, &WeekWindow::getNewMessage);
    connect(this, &CalendarWindow::sendNewMessage, week_window, &WeekWindow::getNewMessage);
    emit sendNewMessage(str);
}

CalendarWindow::~CalendarWindow()
{
    delete ui;
    delete week_window;
    delete creating_events;
}

void CalendarWindow::cleaner(){//стереть все даты
    for (int i = 0; i < 42; i++){
        days[i] = "";
        QPushButton * button = this->findChild<QPushButton *>("pushButton_" + QString::number(i+1));
        if (button){
            button->setText("");
            button->setStyleSheet(" background-color: white;");
        }
    }
}
//описание кнопок <-- и -->
void CalendarWindow::next(){
    month++;
    if (month == 13){
        month = 1;
        year++;
    }
    DrawData(year, month);
}

void CalendarWindow::previous(){
    month--;
    if (month == 0){
        month = 12;
        year--;
    }
    DrawData(year, month);
}

void CalendarWindow::on_pushButton_sudema_clicked()
{
    cleaner();
    next();
}

void CalendarWindow::on_pushButton_tudema_clicked()
{
    cleaner();
    previous();
}

void CalendarWindow::on_pushButton_calendarName_clicked()
{
    reject();
}

void CalendarWindow::on_pushButton_addEvent_clicked()
{
    delete creating_events;
    creating_events = new CreatingEvents(this);
    creating_events->show();

    disconnect(this, &CalendarWindow::sendStartInformatoinEvent, creating_events, &CreatingEvents::getStartInformatoin);

    connect(this, &CalendarWindow::sendStartInformatoinEvent, creating_events, &CreatingEvents::getStartInformatoin);

    emit sendStartInformatoinEvent(socket, login, calendar_id, year, month, 1);
}

void CalendarWindow::SendToServer(QString str, quint16 curr_mode)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    if (curr_mode == 7){//выгрузка из базы данных по столбцу
        out << quint16(0) << curr_mode << str;
        out.device()->seek(0);
        out << quint16(Data.size() - 2*sizeof(quint16));
        socket->write(Data);
    }
}

void CalendarWindow::on_pushButton_Week_clicked()
{

}


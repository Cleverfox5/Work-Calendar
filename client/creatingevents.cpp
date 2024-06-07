#include "creatingevents.h"
#include "ui_creatingevents.h"
//#include <QSqlQuery>
//#include <QtSql>
//#include <QSqlError>
//#include <QDebug>

CreatingEvents::CreatingEvents(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatingEvents)
{
    ui->setupUi(this);

    // Установка режима множественного выбора
    ui->ParticipantslistWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->InformedPersonslistWidget->setSelectionMode(QAbstractItemView::MultiSelection);

}

CreatingEvents::~CreatingEvents()
{
    delete ui;
}

void CreatingEvents::on_SaveButton_clicked()
{
    if (ui->TitleLineEdit->text() != "" && !ui->TitleLineEdit->text().contains("|")) {
        QList<QListWidgetItem *> selectedItems = ui->ParticipantslistWidget->selectedItems();
        QList<QListWidgetItem *> selectedItems_2 = ui->InformedPersonslistWidget->selectedItems();
        QString Title = ui->TitleLineEdit->text();
        QString Description = ui->DescriptionTextEdit->toPlainText();
        QString Location = ui->LocationLineEdit->text();
        QString Type = ui->TypeLineEdit->text();
        QString Priority = ui->PrioritycomboBox->currentText();
        QString StartDate = ui->StartDateEdit->text();
        QString StartTime = ui->StartTimeEdit->text();
        QString EndTime = ui->EndTimeEdit->text();
        QString FrequencyOfTheEvent = ui->comboBox->currentText();
        if(FrequencyOfTheEvent=="Без повторений"){
            FrequencyOfTheEvent="1";
        }
        else if(FrequencyOfTheEvent=="Каждый день"){
            FrequencyOfTheEvent="2";
        }
        else if(FrequencyOfTheEvent=="Раз в неделю"){
            FrequencyOfTheEvent="3";
        }
        else if(FrequencyOfTheEvent=="Раз в месяц"){
            FrequencyOfTheEvent="4";
        }
        else if(FrequencyOfTheEvent=="Раз в год"){
            FrequencyOfTheEvent="5";
        }
        QString Participants = "";
        QString InformedPersons = "";

        QString combinedData = Title + "/" + Description + "/" + Location + "/" + Type + "/" + Priority + "/" + StartDate + "/" + StartTime + "/" + EndTime +"/"+FrequencyOfTheEvent + "/" + QString::number(calendar_id) + "/";

        for (auto item : selectedItems) {
            combinedData += item->text() + " ";
        }
        combinedData.chop(1);

        combinedData += "/";

        for (auto item : selectedItems_2) {
            combinedData += item->text() + " ";
        }
        combinedData.chop(1);

        qDebug() << "Combined Data:" << combinedData;

        SendToServer(combinedData, 20);

        accept();
    }
    else {
        ui->label->setText("?");
    }
}

void CreatingEvents::getStartInformatoin(QTcpSocket * soc, QString login, int calendar_id, int selected_year, int selected_month, int selected_day = 1){//передача сокета с клиента в окно
    socket = soc;
    log = login;
    this->selected_year = selected_year;
    this->selected_month = selected_month;
    this->selected_day = selected_day;
    this->calendar_id = calendar_id;
    ui->StartDateEdit->setDate(QDate(selected_year, selected_month, selected_day));
    SendToServer(QString::number(calendar_id), 21);//запрос на получение списка участников
}

void CreatingEvents::getMembers(QString str){
    qDebug() << "Received data from server: " << str; // Выводим полученные данные в консоль

    int len = str.size();
    QString curr_str = "";
    /*ui->ParticipantslistWidget->clear();
    ui->InformedPersonslistWidget->clear();*/
    for (int i = 0; i < len; i++){
        if (str[i] == ' '){
            ui->ParticipantslistWidget->addItem(curr_str);
            ui->InformedPersonslistWidget->addItem(curr_str);
            curr_str = "";
        }
        else if (i == len - 1){
            curr_str.push_back(str[i]);
            ui->ParticipantslistWidget->addItem(curr_str);
            ui->InformedPersonslistWidget->addItem(curr_str);
        }
        else{
            curr_str.push_back(str[i]);
        }
    }
}
void CreatingEvents::SendToServer(QString str, quint16 curr_mode) //отправка на сервер
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    if (curr_mode == 21 || curr_mode == 20){
        out << quint16(0) << curr_mode << str;
        out.device()->seek(0);
        out << quint16(Data.size() - 2*sizeof(quint16));
        socket->write(Data);
    }
}

void CreatingEvents::on_CancelButton_clicked()
{
    reject();
}

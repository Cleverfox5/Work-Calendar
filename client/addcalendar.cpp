#include "addcalendar.h"
#include "ui_addcalendar.h"

AddCalendar::AddCalendar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCalendar)
{
    ui->setupUi(this);
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
}

AddCalendar::~AddCalendar()
{
    delete ui;
}

void AddCalendar::on_pushButton_clicked()
{
    if (ui->lineEditName->text() != "" && !ui->lineEditName->text().contains(" ") && !ui->lineEditName->text().contains("*") && !ui->lineEditName->text().contains("|") && !ui->lineEditName->text().contains("\\")&& !ui->lineEditName->text().contains("/")){//проверка на соотвествие
        QList<QListWidgetItem *> selectedItems = ui->listWidget->selectedItems();
        QString name_log_and_selected_users = ui->lineEditName->text() + " " + log;
        for (auto item : selectedItems) {
            name_log_and_selected_users += " " + item->text();
        }
        //name_log_and_selected_users.chop(1);
        //ui->label->setText(name_log_and_selected_users);

        SendToServer(name_log_and_selected_users, 6);
        accept();
    }
    else{//вывод предупреждения
        //Не все поля заполнены или в названии не должно быть пробелов, звёздочек

        //не предупреждение!!!
        ui->label->setText("Едирить твою налево");
        //
    }
}

void AddCalendar::getDescriptor(QTcpSocket * soc, QString login){//передача сокета с клиента в окно
    socket = soc;
    log = login;
    SendToServer("UserList Nickname", 5);
}

void AddCalendar::getData(QString str){
    int len = str.size();
    QString curr_str = "";
    for (int i = 0; i < len; i++){
        if (str[i] == ' '){
            ui->listWidget->addItem(curr_str);
            curr_str = "";
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

void AddCalendar::SendToServer(QString str, quint16 curr_mode)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    if (curr_mode == 5 || curr_mode == 6){//выгрузка из базы данных по столбцу
        out << quint16(0) << curr_mode << str;
        out.device()->seek(0);
        out << quint16(Data.size() - 2*sizeof(quint16));
        socket->write(Data);
    }
}

#include "inputdatadialog.h"
#include "ui_inputdatadialog.h"

#include <QDebug>

InputDataDialog::InputDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDataDialog)
{
    ui->setupUi(this);
}

InputDataDialog::~InputDataDialog()
{
    delete ui;
    delete socket;
}

void InputDataDialog::on_saveButton_clicked()
{
    if(ui->loginLineEdit->text() != "" && ui->passwordLineEdit->text() != "" &&
        ui->lastnameLineEdit->text() != "" && ui->firstnameLineEdit->text() != "" &&
        ui->middlenameLineEdit->text() != "" && ui->jobtitleLineEdit->text() != "" &&
        ui->departmentLineEdit->text() != "" && ui->nicknameLineEdit->text() != "" &&
        !(ui->loginLineEdit->text().contains(" ") || ui->loginLineEdit->text().contains("|") || ui->loginLineEdit->text().contains("\\") || ui->loginLineEdit->text().contains("/")) &&
        !(ui->passwordLineEdit->text().contains(" ") || ui->passwordLineEdit->text().contains("|") || ui->passwordLineEdit->text().contains("\\") || ui->passwordLineEdit->text().contains("/")) &&
        !(ui->lastnameLineEdit->text().contains(" ") || ui->lastnameLineEdit->text().contains("|") || ui->lastnameLineEdit->text().contains("\\") || ui->lastnameLineEdit->text().contains("/")) &&
        !(ui->firstnameLineEdit->text().contains(" ") || ui->firstnameLineEdit->text().contains("|") || ui->firstnameLineEdit->text().contains("\\") || ui->firstnameLineEdit->text().contains("/")) &&
        !(ui->middlenameLineEdit->text().contains(" ") || ui->middlenameLineEdit->text().contains("|") || ui->middlenameLineEdit->text().contains("\\") || ui->middlenameLineEdit->text().contains("/")) &&
        !(ui->jobtitleLineEdit->text().contains(" ") || ui->jobtitleLineEdit->text().contains("|") || ui->jobtitleLineEdit->text().contains("\\") || ui->jobtitleLineEdit->text().contains("/")) &&
        !(ui->departmentLineEdit->text().contains(" ") || ui->departmentLineEdit->text().contains("|") || ui->departmentLineEdit->text().contains("\\") || ui->departmentLineEdit->text().contains("/")) &&
        !(ui->nicknameLineEdit->text().contains(" ") || ui->nicknameLineEdit->text().contains("|") || ui->nicknameLineEdit->text().contains("\\") || ui->nicknameLineEdit->text().contains("/"))) //проверочка что всё введено + нет пробелов
    {
        user_data[0] = ui->loginLineEdit->text();
        user_data[1] = ui->passwordLineEdit->text();
        user_data[2] = ui->lastnameLineEdit->text();
        user_data[3] = ui->firstnameLineEdit->text();
        user_data[4] = ui->middlenameLineEdit->text();
        user_data[5] = ui->jobtitleLineEdit->text();
        user_data[6] = ui->departmentLineEdit->text();
        user_data[7] = ui->nicknameLineEdit->text();
        user_data[8] = "0";
        /*if(socket && socket->state() == QAbstractSocket::ConnectedState)
            socket->disconnectFromHost();
        else
            delete socket;*/
        socket = new QTcpSocket(this);
        connect(socket, &QTcpSocket::readyRead, this, &InputDataDialog::slotReadyRead);
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

        nextBlockSize = 0;
        mode = 0;
        socket->connectToHost("127.0.0.1", 2323);
        for (int i = 0; i < 9; i++){
            if (i == 8){
                str += user_data[i];
            }
            else{
                str += user_data[i] + " ";
            }
        }
        SendToServer(str, 2);
    }
    else
    {
        // Обработать ошибку при вставке данных
        qDebug("Чел срёшь...");
    }
}

void InputDataDialog::SendToServer(QString str, quint16 curr_mode)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    out << quint16(0) << curr_mode << str; //для коректной отправки по частям
    out.device()->seek(0);//сдвиг указателя в начало
    out << quint16(Data.size() - 2*sizeof(quint16));
    socket->write(Data);
}

void InputDataDialog::slotReadyRead()
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
            if (mode == 2){
                if (str == "True"){
                    mode = 0;
                    accept();
                }
                else{//ник занять или почта
                    ui->label->setText("Не true");
                }
            }
            else{
                ui->label->setText("mode != 2");
            }
        }
    }
    else{//ошибка!!!!!
        //ui->textBrowser->append("read error");
    }
}

void InputDataDialog::on_cancelButton_clicked()
{
    reject();
}

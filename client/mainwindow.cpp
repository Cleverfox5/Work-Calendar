#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "inputdatadialog.h"

#include <QSqlQueryModel>
#include <QTableView>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    input_data_dialog = new InputDataDialog(this);
    ui->lineEdit_pw->setEchoMode(QLineEdit::Password);
    ui->lineEdit_pw->setPlaceholderText("Введите пароль");
    ui->lineEdit_log->setPlaceholderText("Введите логин");
    QPixmap img_log(":/resurses/images/login.png");
    QPixmap img_pw(":/resurses/images/password.png");
    ui->label_login->setPixmap(img_log.scaled(ui->label_login->width(), ui->label_login->height(), Qt::KeepAspectRatio));
    ui->label_password->setPixmap(img_pw.scaled(ui->label_password->width(), ui->label_password->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_pushButton_clicked()
{
    input_data_dialog = new InputDataDialog(this);
    input_data_dialog->show();
}

void MainWindow::slotReadyRead()
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
            if (mode == 3){
                if (str == "True"){
                    mode = 0;
                    hide();
                    calendars = new calendarList(this);
                    calendars->show();

                    connect(this, &MainWindow::senData, calendars, &calendarList::getID);

                    emit senData(login);
                }
                else{
                    QMessageBox::warning(this, "Error", "Invalid login or password!");
                }
            }
        }
    }
    else{
        QMessageBox::warning(this, "Error", "The connection to the server is wrong");
    }
}

void MainWindow::on_loginButton_clicked()
{
    login = ui->lineEdit_log->text();
    QString password = ui->lineEdit_pw->text();

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;
    mode = 0;
    if (!ui->lineEdit_log->text().isEmpty() && !ui->lineEdit_log->text().contains(" ") && !ui->lineEdit_pw->text().contains(" ") && !ui->lineEdit_pw->text().isEmpty()){
        QString str = login + " " + password;
        socket->connectToHost("127.0.0.1", 2323);

        SendToServer(str, 3);
    }
    else{
        QMessageBox::warning(this, "Login", "The data was entered incorrectly!");
    }
}

void MainWindow::SendToServer(QString str, quint16 curr_mode)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    out << quint16(0) << curr_mode << str; //для коректной отправки по частям
    out.device()->seek(0);//сдвиг указателя в начало
    out << quint16(Data.size() - 2*sizeof(quint16));
    socket->write(Data);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete calendars;
    delete input_data_dialog;
}

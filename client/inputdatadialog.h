#ifndef INPUTDATADIALOG_H
#define INPUTDATADIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class InputDataDialog;
}

class InputDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDataDialog(QWidget *parent = nullptr);
    ~InputDataDialog();

private slots:
    void on_saveButton_clicked();
    void on_cancelButton_clicked();

public slots:
    void slotReadyRead();

private:
    Ui::InputDataDialog *ui;
    QByteArray Data;
    void SendToServer(QString str, quint16 curr_mode);
    quint16 nextBlockSize;
    quint16 mode;
    QTcpSocket *socket;
    /*QString login;
    QString password;
    QString lastname;
    QString firstname;
    QString middlename;
    QString  jobtitle;
    QString  department;
    QString  descriptor;
    QString nickname;*/

    QString user_data[9];
    QString str;
};

#endif // INPUTDATADIALOG_H

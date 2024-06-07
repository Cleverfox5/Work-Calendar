#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

class server : public QTcpServer
{
    Q_OBJECT

public:
    server();
    ~server();
    QTcpSocket *socket;
private:
    QVector <QTcpSocket *> Sockets;
    QByteArray Data;
    void SendToClient(QString str, quint16 curr_mode, QTcpSocket * curr_socket);
    quint16 nextBlockSize;
    quint16 mode;
    QSqlDatabase db;
    QSqlQuery *query;
    QString loggedInUsername;
    void send_members(QVector<QString> Descriptor_list, QString result);

public slots:
    void incomingConnection(qintptr SocketDescripor);
    void slotReadyRead();
};


#endif // SERVER_H

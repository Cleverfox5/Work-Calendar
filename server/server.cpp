#include "server.h"

#include <QSqlQueryModel>
#include <QVector>
#include <QPair>

server::server()
{
    if (this->listen(QHostAddress::Any, 2323))
        qDebug() << "Start";
    else
        qDebug() << "Error";

    nextBlockSize = 0;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./db_calendar.db");
    if(db.open())
        qDebug("open");
    else
        qDebug("no open");
    query = new QSqlQuery(db);

    query->exec("CREATE TABLE UserList(id INTEGER PRIMARY KEY AUTOINCREMENT , Login TEXT,  Password TEXT, LastName TEXT, FirstName TEXT, MiddleName TEXT, JobTitle TEXT, Department TEXT, Nickname TEXT, Descriptor INTEGER DEFAULT 0)");

    query->exec("CREATE TABLE GlobalCalendarslist (id INTEGER PRIMARY KEY AUTOINCREMENT, creator_id INTEGER,calendar_name TEXT, FOREIGN KEY(creator_id) REFERENCES UserList(id))");
    query->exec("CREATE TABLE CalendarMembers (calendar_id INTEGER,user_id INTEGER,FOREIGN KEY(calendar_id) REFERENCES GlobalCalendarslist(id),FOREIGN KEY(user_id) REFERENCES UserList(id))");

    query->exec("CREATE TABLE Events (id INTEGER PRIMARY KEY AUTOINCREMENT, calendar_id INTEGER NOT NULL,title TEXT NOT NULL, description TEXT,  location TEXT,  type TEXT, priority TEXT,  date DATE,  start_time TIME,  end_time TIME,FrequencyOfTheEvent TEXT, FOREIGN KEY (calendar_id) REFERENCES GlobalCalendarslist(id))");

    query->exec("CREATE TABLE EventParticipants (event_id INTEGER,user_id INTEGER,is_informed INTEGER DEFAULT 0,FOREIGN KEY (event_id) REFERENCES Events(id),FOREIGN KEY (user_id) REFERENCES UserList(id))");
    query->exec("CREATE TABLE EventInformants (event_id INTEGER,user_id INTEGER,FOREIGN KEY (event_id) REFERENCES Events(id),FOREIGN KEY (user_id) REFERENCES UserList(id))");

    query->exec("CREATE TABLE Notifications (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER, type TEXT, message TEXT, timestamp DATETIME, is_read INTEGER DEFAULT 0, FOREIGN KEY (user_id) REFERENCES UserList(id))");
    //query->exec("DROP TABLE IF EXISTS Messages"); //удаление таблицы
    query->exec("CREATE TABLE Messages (id INTEGER PRIMARY KEY AUTOINCREMENT, event_id INTEGER, user_id INTEGER, message_text TEXT, send_date DATE, send_time TIME, FOREIGN KEY (user_id) REFERENCES UserList(id), FOREIGN KEY (event_id) REFERENCES Events(id))");
    /*//Данные для ивента
    QString title = "Полить цветок";
    QString description = "This is a test event";
    QString location = "Somewhere";
    QString type = "Meeting";
    int priority = 3;
    QString date = "2024-05-15";
    QString start_time = "11:00:00";
    QString end_time = "12:00:00";
    int repeat_mode = 0;

    query->prepare("INSERT INTO Events (calendar_id, title, description, location, type, priority, date, start_time, end_time, repeat_mode) "
                  "VALUES (:calendar_id, :title, :description, :location, :type, :priority, :date, :start_time, :end_time, :repeat_mode)");

    // Задаем значения параметров запроса
    query->bindValue(":calendar_id", 3); // ID календаря, к которому относится событие
    query->bindValue(":title", title);
    query->bindValue(":description", description);
    query->bindValue(":location", location);
    query->bindValue(":type", type);
    query->bindValue(":priority", priority);
    query->bindValue(":date", date);
    query->bindValue(":start_time", start_time);
    query->bindValue(":end_time", end_time);
    query->bindValue(":repeat_mode", repeat_mode);

    if (query->exec()) {
        qDebug() << "Event added successfully.";
    } else {
        qDebug() << "Error adding event";
    }*/
}

server::~server()
{
    delete query;
    delete socket;
}

void server::incomingConnection(qintptr SocketDescripor){
    socket = new QTcpSocket;
    socket->setSocketDescriptor(SocketDescripor);
    connect(socket, &QTcpSocket::readyRead, this, &server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    qDebug() << "client connected" << SocketDescripor;

    //Получаю id и меняю его дескриптор


    /*for (int i = 0; i < Sockets.size(); i++){// -1
        QString str = QString::number(Sockets[i]->socketDescriptor());
        SendToClient(str, 2);
    }
    for (int i = 0; i < Sockets.size() - 1; i++){
        socket = Sockets[i];
        SendToClient(QString::number(SocketDescripor), 2);
    }*/
}

void server::slotReadyRead(){
    socket = (QTcpSocket*) sender();
    qDebug() << socket->socketDescriptor();
    QDataStream in (socket);
    in.setVersion(QDataStream::Qt_6_6);
    if(in.status() == QDataStream::Ok){
        qDebug() << "read...";
        for (;;){
            if (nextBlockSize == 0){
                qDebug() << "nextBlockSize = 0";
                if (socket->bytesAvailable() < 2){
                    qDebug() << "Data < 2";
                    break;
                }
                in >> nextBlockSize;//размер блока
                in >> mode;
                qDebug() << "next block size = " << nextBlockSize;
                qDebug() << "mode = " << mode;
            }
            if (socket->bytesAvailable() < nextBlockSize){
                qDebug() << "Data not full";
                break;
            }
            if (mode == 1){
                QString str;
                QTime time;
                in >> time >> str;
                nextBlockSize = 0;
                qDebug() << str;
                SendToClient(str, mode, socket);
            }
            else if (mode == 2){
                QString str;
                QVector<QString> client_data;
                qintptr SocketDescripor = 0;
                QString curr_str;
                int len = str.size();
                in >> str;
                nextBlockSize = 0;
                for (int i = 0; i < str.size(); i++){
                    if (str[i] == ' '){
                        client_data.append(curr_str);
                        curr_str = "";
                    }
                    else if (i == len - 1){
                        curr_str.push_back(str[i]);
                        client_data.append(curr_str);
                    }
                    else{
                        curr_str.push_back(str[i]);
                    }
                }
                query->prepare("INSERT INTO UserList (Login, Password, LastName, FirstName, MiddleName, JobTitle, Department, Nickname, Descriptor) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
                for (int i = 0; i < client_data.size(); i++){
                    query->addBindValue(client_data[i]);
                }
                query->addBindValue(SocketDescripor);
                if(query->exec())
                {
                    qDebug("Пользователь добавлен");
                    SendToClient("True", 2, socket);
                }
                else
                {
                    qDebug("Не удалось добавить пользователя");
                    SendToClient("False", 2, socket);
                }
            }
            else if (mode == 3){
                QSqlQuery query;
                QString str;
                QString login = "";
                QString password = "";
                in >> str;
                nextBlockSize = 0;
                QString curr_str;
                int len = str.size();
                for (int i = 0; i < len; i++){
                    if (str[i] == ' '){
                        login = curr_str;
                        curr_str = "";
                    }
                    else if (i == len - 1){
                        curr_str.push_back(str[i]);
                        password = curr_str;
                    }
                    else{
                        curr_str.push_back(str[i]);
                    }
                }
                qDebug() << login;
                qDebug() << password;
                query.prepare("SELECT * FROM UserList WHERE Login = :login AND Password = :password");
                query.bindValue(":login", login);
                query.bindValue(":password", password);
                if(query.exec() && query.next())
                {
                    qDebug() << "Верно";
                    SendToClient("True", 3, socket);
                }
                else
                {
                    SendToClient("False", 3, socket);
                }
            }
            else if (mode == 4){
                QString str;
                nextBlockSize = 0;
                in >> str;
                query->prepare("UPDATE UserList SET Descriptor = :descriptor WHERE Login = :login");
                query->bindValue(":descriptor", socket->socketDescriptor());
                query->bindValue(":login", str);
                if (query->exec()) {
                    QString Data;
                    query->prepare("SELECT id FROM UserList WHERE Login = :login");
                    query->bindValue(":login", str);

                    if(!query->exec())
                    {
                        qDebug("id creator не получен");
                    }

                    query->next();

                    int id_creator = query->value(0).toInt();

                    query->prepare("SELECT calendar_name, id FROM GlobalCalendarslist WHERE creator_id = :creator_id");
                    query->bindValue(":creator_id", id_creator);

                    if(!query->exec())
                    {
                        qDebug("не удалось добавить календарь");
                    }
                    else{
                        while(query->next()){
                            Data.push_back("*" + query->value(0).toString() + " "  + query->value(1).toString() + " ");
                        }
                        if (Data.size() != 0){
                            Data.chop(1);
                        }
                    }

                    QVector<QString> calendars_id;

                    query->prepare("SELECT calendar_id FROM CalendarMembers WHERE user_id = :creator_id");
                    query->bindValue(":creator_id", id_creator);

                    if(!query->exec())
                    {
                        qDebug("не удалось получить id календаря");
                    }
                    else{
                        while(query->next()){
                            calendars_id.push_back(query->value(0).toString());
                        }
                    }

                    for (int i = 0; i < calendars_id.size(); i++){
                        query->prepare("SELECT calendar_name FROM GlobalCalendarslist WHERE id = :calendar_id");
                        query->bindValue(":calendar_id", calendars_id[i]);

                        if(!query->exec())
                        {
                            qDebug("не удалось добавить календарь");
                        }
                        else{
                            query->next();
                            if (Data.isEmpty())
                                Data.push_back(query->value(0).toString() + " " + calendars_id[i]);
                            Data.push_back(" " + query->value(0).toString() + " " + calendars_id[i]);

                        }
                    }
                    Data.removeLast();

                    SendToClient(Data, 4, socket);

                }
                else{
                    qDebug() << "Ошибка выполнения запроса";
                    SendToClient("False", 4, socket);
                }
            }
            else if (mode == 5){
                QSqlQuery query;
                QString str;
                QString table;
                QString column;
                nextBlockSize = 0;//на этом всй держится забуду записать сервак рухнет
                QString curr_str;
                in >> str;
                int len = str.size();
                for (int i = 0; i < len; i++){
                    if (str[i] == ' '){
                        table = curr_str;
                        curr_str = "";
                    }
                    else if (i == len - 1){
                        curr_str.push_back(str[i]);
                        column = curr_str;
                    }
                    else{
                        curr_str.push_back(str[i]);
                    }
                }
                qDebug() << table;
                qDebug() << column;
                if (query.exec("SELECT " + column + " FROM " + table)){
                    QString Data;
                    while (query.next()) {
                        QString value = query.value(0).toString();
                        qDebug() << value;
                        Data.push_back(value);
                        Data.push_back(" ");
                    }
                    Data.chop(1);
                    SendToClient(Data, 5, socket);

                }
                else{
                    qDebug() << "неверный запрос";
                }
            }
            else if (mode == 6){//Добавление нового календаря
                QString str;
                QVector<QString> calendar_data;
                QString curr_str;
                in >> str;
                int len = str.size();
                nextBlockSize = 0;
                for (int i = 0; i < len; i++){
                    if (str[i] == ' '){
                        calendar_data.append(curr_str);
                        qDebug() << curr_str;
                        curr_str = "";
                    }
                    else if (i == len - 1){
                        curr_str.push_back(str[i]);
                        calendar_data.append(curr_str);
                        qDebug() << curr_str;

                    }
                    else{
                        curr_str.push_back(str[i]);
                    }
                }

                QVector<int> id_members;
                QString name = calendar_data[0];

                query->prepare("SELECT id FROM UserList WHERE Login = :login");
                query->bindValue(":login", calendar_data[1]);

                if(!query->exec())
                {
                    qDebug("id creator не получены");
                }

                query->next();

                int id_creator = query->value(0).toInt();

                for (int i = 2; i < calendar_data.size(); i++){
                    query->prepare("SELECT id FROM UserList WHERE Nickname = :nickname");
                    query->bindValue(":nickname", calendar_data[i]);
                    if(!query->exec())
                        qDebug("id пользователей не получены");
                    query->next();
                    id_members.append(query->value(0).toInt());
                }

                if(!query->exec())
                {
                    qDebug("id пользователей не получены");
                }

                query->prepare("INSERT INTO GlobalCalendarslist (creator_id, calendar_name) VALUES (?, ?)");

                query->addBindValue(id_creator);
                query->addBindValue(name);

                if(query->exec())
                {
                    qDebug("Календарь успешно добавлен");
                }
                else
                {
                    qDebug("Календарь не удалось добавить");
                }

                int calendar_id = query->lastInsertId().toInt();

                for (int i = 0; i < id_members.size(); i++){
                    query->prepare("INSERT INTO CalendarMembers (calendar_id, user_id) VALUES (?, ?)");
                    query->addBindValue(calendar_id);
                    query->addBindValue(id_members[i]);
                    if(!query->exec()){
                        qDebug("Не удалось удалить пользователя");
                    }

                }

                SendToClient("*" + name + " " + QString::number(calendar_id), 6, socket);
            }
            else if (mode == 7){//получаем id_calendar, месяц, год и логин пользователя
                //query->exec("CREATE TABLE Events (id INTEGER PRIMARY KEY AUTOINCREMENT, calendar_id INTEGER NOT NULL,title TEXT NOT NULL, description TEXT,  location TEXT,  type TEXT, priority TEXT,  date DATE,  start_time TIME,  end_time TIME,FrequencyOfTheEvent TEXT, FOREIGN KEY (calendar_id) REFERENCES GlobalCalendarslist(id))");
                QString str, result;
                QVector<QString> calendar_data;
                QString curr_str;
                QVector<QString> data;
                in >> str;
                data = str.split(" ");
                nextBlockSize = 0;
                QString id_calendar = data[0];
                QString year = data[1];
                QString month = data[2];
                if (month.size() == 1){
                    month = "0" + month;
                }
                QString login = data[3];

                query->prepare("SELECT id FROM UserList WHERE Login = :login");
                query->bindValue(":login", login);
                if (!query->exec() || !query->next()) {
                    qDebug() << "Не удалось получить id пользователя" << login;
                    return;
                }
                int user_id = query->value(0).toInt();

                query->prepare("SELECT id, title, date FROM Events WHERE calendar_id = :calendar_id "
                               "AND substr(date, 7, 4) = :year AND substr(date, 4, 2) = :month");
                query->bindValue(":calendar_id", id_calendar);
                query->bindValue(":year", year);
                query->bindValue(":month", month);
                if (!query->exec()) {
                    qDebug() << "Error: Could not retrieve events for calendar" << id_calendar << "in" << month << "/" << year;
                    return;
                }

                while (query->next()) {
                    int event_id = query->value(0).toInt();
                    QString event_title = query->value(1).toString();
                    QString day = query->value(2).toString().left(2);
                    if (day[0] == '0')
                        day = day[1];
                    // Проверяем входит ли пользотватель в событие
                    QSqlQuery query_in_query;
                    query_in_query.prepare("SELECT COUNT(*) FROM EventParticipants WHERE event_id = :event_id AND user_id = :user_id");
                    query_in_query.bindValue(":event_id", event_id);
                    query_in_query.bindValue(":user_id", user_id);
                    if (!query_in_query.exec() || !query_in_query.next()) {
                        qDebug() << "Не удалось проверить участие " << user_id << " в событии " << event_id;
                        continue;
                    }
                    QString isParticipant = "false";
                    if (query_in_query.value(0).toInt() > 0)
                        isParticipant = "true";

                    result.push_back(day + "|" + QString::number(event_id) + "|" + event_title + "|" + isParticipant);
                    result.push_back("|");
                    qDebug() << "|" << day << "|" << event_id << "|" << event_title << "|" << isParticipant;

                }
                if (!result.isEmpty())
                    result.removeLast();

                qDebug() << result;

                SendToClient(result, 7, socket);//отправляем списко id событий, их учатсников
            }
            else if (mode == 8){
                QString str, result;
                in >> str;
                nextBlockSize = 0;
                //query->exec("CREATE TABLE Messages (id INTEGER PRIMARY KEY AUTOINCREMENT, event_id INTEGER, user_id INTEGER, message_text TEXT, send_date DATE, send_time TIME, FOREIGN KEY (user_id) REFERENCES UserList(id), FOREIGN KEY (event_id) REFERENCES Event(id))");
                //query->exec("CREATE TABLE Messages (id INTEGER PRIMARY KEY AUTOINCREMENT, event_id INTEGER, user_id INTEGER, message_text TEXT, send_date DATE, send_time TIME, FOREIGN KEY (user_id) REFERENCES UserList(id), FOREIGN KEY (event_id) REFERENCES Events(id))");

                query->prepare("SELECT user_id, message_text, send_date, send_time, id FROM Messages WHERE event_id = :event_id");
                query->bindValue(":event_id", str);
                if (!query->exec()){
                    qDebug() << "Не получилось добавить пользователя";
                }
                QVector<QPair<int, QString>> Messages_list;
                while (query->next()){
                    QSqlQuery query_in_query;

                    //query->exec("CREATE TABLE UserList(id INTEGER PRIMARY KEY AUTOINCREMENT , Login TEXT,  Password TEXT, LastName TEXT, FirstName TEXT, MiddleName TEXT, JobTitle TEXT, Department TEXT, Nickname TEXT, Descriptor INTEGER DEFAULT 0)");

                    query_in_query.prepare("SELECT Nickname FROM UserList WHERE id = :user_id");
                    query_in_query.bindValue(":user_id", query->value(0));
                    if (!query_in_query.exec() || !query_in_query.next()) {
                        qDebug() << "Не удалось получить nickname пользотвателя";
                        return;
                    }
                    QString current_nickname = query_in_query.value(0).toString();

                    Messages_list.push_back(qMakePair(query->value(4).toInt(), current_nickname + " " + query->value(2).toString() + " " + query->value(3).toString() + " " + query->value(1).toString()));
                }

                int size_arr = Messages_list.size();

                for (int i = 0; i < size_arr - 1; i++){
                    for (int j = 0; j < size_arr - i - 1; j++){
                        if (Messages_list[j].first > Messages_list[j+1].first){
                            swap(Messages_list[j], Messages_list[j+1]);
                        }
                    }
                }

                for (int i = 0; i < size_arr; i++){
                    result += Messages_list[i].second + "|";
                }

                if (result != "")
                    result.removeLast();

                qDebug() << result;

                SendToClient(result, 8, socket);

            }
            else if (mode == 9){
                QString str, result;
                in >> str;
                QVector<QString> mes_parts = str.split("|");
                QString user = mes_parts[0];
                QString data = mes_parts[1];
                QString time = mes_parts[2];
                QString event_id = mes_parts[3];
                QString message = mes_parts[4];
                nextBlockSize = 0;
                query->prepare("SELECT Nickname, id FROM UserList WHERE Login = :login");
                query->bindValue(":login", user);
                if (!query->exec() || !query->next()) {
                    qDebug() << "Не удалось получить id пользователя";
                    return;
                }
                user = query->value(0).toString();
                QString user_id = query->value(1).toString();
                result = user + " " + time + " " + message + "|" + event_id;
                //query->exec("CREATE TABLE Messages (id INTEGER PRIMARY KEY AUTOINCREMENT, event_id INTEGER, user_id INTEGER, message_text TEXT, send_date DATE, send_time TIME, FOREIGN KEY (user_id) REFERENCES UserList(id), FOREIGN KEY (event_id) REFERENCES Event(id))");
                query->prepare("INSERT INTO Messages (event_id, user_id, message_text, send_date, send_time) VALUES (?, ?, ?, ?, ?)");

                query->addBindValue(event_id);
                query->addBindValue(user_id);
                query->addBindValue(message);
                query->addBindValue(data);
                query->addBindValue(time);

                if(!query->exec())
                    qDebug("Сообщение не удалось добавить");

                //query->exec("CREATE TABLE Messages (id INTEGER PRIMARY KEY AUTOINCREMENT, event_id INTEGER, user_id INTEGER, message_text TEXT, send_date DATE, send_time TIME, FOREIGN KEY (user_id) REFERENCES UserList(id), FOREIGN KEY (event_id) REFERENCES Event(id))");
                //query->exec("CREATE TABLE EventParticipants (event_id INTEGER,user_id INTEGER,is_informed INTEGER DEFAULT 0,FOREIGN KEY (event_id) REFERENCES Events(id),FOREIGN KEY (user_id) REFERENCES UserList(id))");

                QVector<QString> id_list;
                query->prepare("SELECT user_id FROM EventParticipants WHERE event_id = :event_id");
                query->bindValue(":event_id", event_id);
                if (!query->exec()) {
                    qDebug() << "Не удалось получить id пользователя";
                    return;
                }
                while (query->next()){
                    id_list.append(query->value(0).toString());
                }

                //query->exec("CREATE TABLE UserList(id INTEGER PRIMARY KEY AUTOINCREMENT , Login TEXT,  Password TEXT, LastName TEXT, FirstName TEXT, MiddleName TEXT, JobTitle TEXT, Department TEXT, Nickname TEXT, Descriptor INTEGER DEFAULT 0)");
                QVector<QString> Descriptor_list;
                for (int i = 0; i < id_list.size(); i++){
                    query->prepare("SELECT Descriptor FROM UserList WHERE id = :id");
                    query->bindValue(":id", id_list[i]);
                    if (!query->exec()) {
                        qDebug() << "Не удалось получить Descriptor пользователя";
                        return;
                    }
                    while (query->next()){
                        QString current_discriptor = query->value(0).toString();
                        if (current_discriptor != "0")
                            Descriptor_list.append(current_discriptor);
                    }
                }

                SendToClient(result, 9, socket);
            }
            else if (mode == 21){ //получение данных из таблицы
                QSqlQuery query;
                QString calendar_id;
                nextBlockSize = 0;//на этом всй держится забуду записать сервак рухнет
                QString curr_str;
                QVector<QString> members;
                in >> calendar_id;
                query.prepare("SELECT user_id FROM CalendarMembers WHERE calendar_id = :calendar_id");
                query.bindValue(":calendar_id", calendar_id);
                if (query.exec()){
                    while (query.next()){
                        QString value = query.value(0).toString();
                        qDebug() << value;
                        members.push_back(value);
                    }
                }
                else{
                    qDebug() << "Не удалось получить user_id from CalendarMembers";
                }

                //у меня есть id пользователя, мне нужно получить его ник, вот и всё
                QString data;

                //query->exec("CREATE TABLE GlobalCalendarslist (id INTEGER PRIMARY KEY AUTOINCREMENT, creator_id INTEGER,calendar_name TEXT, FOREIGN KEY(creator_id) REFERENCES UserList(id))");

                query.prepare("SELECT creator_id FROM GlobalCalendarslist WHERE id = :calendar_id");
                query.bindValue(":calendar_id", calendar_id.toInt());
                if (query.exec()){
                    if (query.next()){
                        int creator_id = query.value(0).toInt();
                        query.prepare("SELECT Nickname FROM UserList WHERE id = :user_id");
                        query.bindValue(":user_id", creator_id);
                        if (query.exec()){
                            if (query.next()){
                                data.push_back(query.value(0).toString());
                                data.push_back(" ");
                            }
                        }
                        else{
                            qDebug() << "Чики бомбони с creator_id";
                        }
                    }
                }
                else{
                    qDebug() << "Чики бомбони с creator_id";
                }

                for (int i = 0; i < members.size(); i++){
                    query.prepare("SELECT Nickname FROM UserList WHERE id = :user_id");
                    query.bindValue(":user_id", members[i]);
                    if (query.exec()){
                        query.next();
                        QString value = query.value(0).toString();
                        qDebug() << value;
                        data.push_back(value);
                        data.push_back(" ");
                    }
                    else{
                        qDebug() << "Не получилось получить ник пользователя";
                    }
                }
                if (!data.isEmpty()){
                    data.chop(1);
                    SendToClient(data, 21, socket);
                }
                /*
                if (query.exec("SELECT Nickname FROM UserList WHERE user_id = :user_id")){
                    QString Data;
                    while (query.next()) {
                        QString value = query.value(0).toString();
                        qDebug() << value;
                        Data.push_back(value);
                        Data.push_back(" ");
                    }
                    Data.chop(1);
                    SendToClient(Data, 21, socket);

                }
                else{
                    qDebug() << "неверный запрос";
                }*/
            }
            else if (mode == 20) { // Добавление данных в Events, EventParticipants и в EventInformants
                QString str;
                QVector<QString> events_data;
                QString curr_str;
                in >> str;

                events_data = str.split("/");
                qDebug() << "Received data:" << str;
                qDebug() << "Содержимое events_data:";
                for (const QString &data : events_data) {
                    qDebug() << data;
                }
                qDebug() << "Конец events_data:";
                int len = str.size();
                nextBlockSize = 0;
                /*for (int i = 0; i < len; i++) {
                    events_data.append(curr_str);
                    qDebug() << curr_str;
                    curr_str = "";
                }*/

                QString title = events_data[0];
                QString description = events_data[1];
                QString location = events_data[2];
                QString type = events_data[3];
                QString priority = events_data[4];
                QString start_date = events_data[5];
                QString start_time = events_data[6];
                QString end_time = events_data[7];
                QString frequency = events_data[8];
                QString calendar_id = events_data[9];
                QString participants = events_data[10];
                QString informants = events_data[11];


                QVector<QString> id_participants = participants.split(" ");

                for (int i = 0; i < id_participants.size(); i++){
                    query->prepare("SELECT id FROM UserList WHERE Nickname = :nickname");
                    query->bindValue(":nickname", id_participants[i]);
                    if (query->exec()){
                        if (query->next()){
                            id_participants[i] = query->value(0).toString();
                        }
                    }
                    else{
                        qDebug() << "Делать деньги, вот так! *щелчок пальцами* (ошибка запроса)";
                    }
                }


                QVector<QString> id_informants = informants.split(" ");

                for (int i = 0; i < id_informants.size(); i++){
                    query->prepare("SELECT id FROM UserList WHERE Nickname = :nickname");
                    query->bindValue(":nickname", id_informants[i]);
                    if (query->exec()){
                        if (query->next()){
                            id_informants[i] = query->value(0).toString();
                        }
                    }
                    else{
                        qDebug() << "Делать деньги, вот так! *щелчок пальцами* (ошибка запроса 2)";
                    }
                }

                /*for (int i = 10; i < events_data.size(); i++) {
                    if (!nickname.isEmpty()) { // Проверка на пустую строку
                        query->prepare("SELECT id FROM UserList WHERE Nickname = :nickname");
                        query->bindValue(":nickname", nickname);
                        if (!query->exec())
                            qDebug("id пользователей не получены");
                        if (query->next()) {
                            int userId = query->value(0).toInt();
                            if (i % 2 == 0) // Четные индексы добавляются в EventParticipants
                                id_participants.append(userId);
                            else // Нечетные индексы добавляются в EventInformants
                                id_informants.append(userId);
                        } else {
                            qDebug() << "Пользователь с никнеймом" << nickname << "не найден в базе данных";
                        }
                    }
                }*/

                // Добавление данных в таблицу Events
                //query->prepare("INSERT INTO GlobalCalendarslist (creator_id, calendar_name) VALUES (?, ?)");
                //query->exec("CREATE TABLE Events (id INTEGER PRIMARY KEY AUTOINCREMENT, calendar_id INTEGER NOT NULL,title TEXT NOT NULL, description TEXT, location TEXT, type TEXT, priority TEXT,  date DATE,  start_time TIME,  end_time TIME, FrequencyOfTheEvent TEXT, FOREIGN KEY (calendar_id) REFERENCES GlobalCalendarslist(id))");

                query->prepare("INSERT INTO Events (calendar_id, title, description, location, type, priority, date, start_time, end_time, FrequencyOfTheEvent) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                query->addBindValue(calendar_id); // Здесь нужно будет указать calendar_id, к которому относится событие
                query->addBindValue(title);
                query->addBindValue(description);
                query->addBindValue(location);
                query->addBindValue(type);
                query->addBindValue(priority);
                query->addBindValue(start_date);
                query->addBindValue(start_time);
                query->addBindValue(end_time);
                query->addBindValue(frequency);

                if(query->exec()) {
                    qDebug("Событие успешно добавлено");
                } else {
                    qDebug("Событие не удалось добавить");
                }

                int event_id = query->lastInsertId().toInt();

                // Добавление пользователей в таблицу EventParticipants
                for (int i = 0; i < id_participants.size(); i++) {
                    qDebug() << "User ID P:" << id_participants[i];
                    query->prepare("INSERT INTO EventParticipants (event_id, user_id) VALUES (?, ?)");
                    query->addBindValue(event_id);
                    query->addBindValue(id_participants[i]);
                    if (!query->exec()) {
                        qDebug("Не удалось добавить пользователя в EventParticipants");
                    }
                }

                // Добавление пользователей в таблицу EventInformants
                for (int i = 0; i < id_informants.size(); i++) {
                    qDebug() << "User ID I:" << id_informants[i];
                    query->prepare("INSERT INTO EventInformants (event_id, user_id) VALUES (?, ?)");
                    query->addBindValue(event_id);
                    query->addBindValue(id_informants[i]);
                    if (!query->exec()) {
                        qDebug("Не удалось добавить пользователя в EventInformants");
                    }
                }
            }
            else if (mode == 22){ //для извлечения данных из Events
                QSqlQuery query;
                QString str;
                QString table;
                QString columns;
                nextBlockSize = 0;
                QString curr_str;
                in >> str;
                int len = str.size();
                for (int i = 0; i < len; i++){
                    if (str[i] == ' '){
                        table = curr_str;
                        curr_str = "";
                    }
                    else if (i == len - 1){
                        curr_str.push_back(str[i]);
                        columns = curr_str;
                    }
                    else{
                        curr_str.push_back(str[i]);
                    }
                }
                qDebug() << table;//имя таблицы
                qDebug() << columns; //список столбцов, которые я хочу извлечь

                //Извлечение данных из таблицы Events по заданным столбцам
                /*if (query.exec("SELECT " + columns + " FROM " + table)){
                    QString Data;
                    while (query.next()) {
                        // Получаем значения из различных столбцов
                       // QString title = query.value(0).toString();
                        QString description = query.value(0).toString();
                        //QString location = query.value(2).toString();
                        //QString type = query.value(3).toString();
                        QString priority = query.value(4).toString();
                        QString start_date = query.value(5).toString();
                        QString start_time = query.value(6).toString();
                        QString end_date = query.value(7).toString();
                        QString end_time = query.value(8).toString();

                        // Формируем строку данных для отправки клиенту
                        //QString rowData = description + "/" + priority + "/" + start_date + "/" + start_time + "/" + end_date + "/" + end_time;
                        QString rowData = description;
                        qDebug() << rowData;
                        Data.push_back(rowData);
                        Data.push_back(" ");
                    }
                    Data.chop(1);
                    SendToClient(Data, 22, socket);
                }*/



                int targetId=5;
                if (query.exec("SELECT " + columns + " FROM " + table + " WHERE id = " + QString::number(targetId))) {
                    // Обработка результатов запроса
                    QString Data;
                    while (query.next()) {
                        QString description = query.value(0).toString();
                        // Продолжайте извлекать другие данные по мере необходимости
                        qDebug() << "Описание события:" << description;
                            Data += description;
                        Data += " ";
                    }
                    Data.chop(1);
                    SendToClient(Data, 22, socket);

                } else {
                    qDebug() << "Ошибка выполнения запроса";
                }

            }

            else if (mode == 23) { // для изменения данных в Events
                QString str;
                QVector<QString> events_data;
                QString curr_str;
                in >> str;

                events_data = str.split("/");
                int len = events_data.size(); // Исправлено на events_data.size(), чтобы получить размер вектора
                nextBlockSize = 0;

                // Заполнение вектора events_data данными из строки str
                for (int i = 0; i < len; i++) {
                    qDebug() << events_data[i];
                }

                // Извлечение данных из вектора events_data
                QString description = events_data[0];
                // Извлечение других данных, если необходимо, аналогичным образом

                // Подготовка запроса на обновление данных в таблице Events
                query->prepare("UPDATE Events SET description = ? WHERE id = ?");
                query->addBindValue(description);
                query->addBindValue("5");

                if (query->exec()) {
                    qDebug("Событие успешно изменено");
                } else {
                    qDebug("Событие не удалось изменить");
                }
            }
            mode = 0;
            break;
        }
    }
    else{
        qDebug() << "DataStream error";
    }
}

void server::SendToClient(QString str, quint16 curr_mode, QTcpSocket * curr_socket){
    if (curr_socket && curr_socket->isOpen() && curr_socket->isWritable() && curr_socket->isValid()) {
        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_6);
        if(curr_mode == 1){
            out << quint16(0) << curr_mode << QTime::currentTime() << str;//для коректной отправки по частям
            out.device()->seek(0);
            out << quint16(Data.size() - 2*sizeof(quint16));
        }
        else{
            out << quint16(0) << curr_mode << str;//для коректной отправки по частям
            out.device()->seek(0);
            out << quint16(Data.size() - 2*sizeof(quint16));
        }

        curr_socket->write(Data);
    }
}

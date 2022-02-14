#include "tcpserver.h"
#include <iostream>
TcpServer::TcpServer(QObject *parent)
    : QObject{parent}
{
    connect(&_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    connect(this, &TcpServer::newMessage, this, &TcpServer::onNewMessage);

    // listen on custom port 45000. if succeed
    if (_server.listen(QHostAddress::Any, 45000)) {
        qInfo() <<"Listening...";
    }
}

QString TcpServer::encrypt(const QString &message) {
    std::string text = message.toUtf8().constData();
    for (size_t i = 0; i != text.size(); i++) {
        text[i] += 2;
    }
    return QString::fromStdString(text);
}

void TcpServer::sendMessage(const QString &message)
{
    QString completeMsg = "Server: " + message;
    QString msg = encrypt(completeMsg);//encryption before send message
    emit newMessage(msg.toUtf8());
}

void TcpServer::onNewConnection()
{
    // get the next client that is waiting to connect to our server
    const auto client = _server.nextPendingConnection();
    if (client == nullptr) {
        return;
    }

    qInfo() << "New client connected.";

    // use this hashmap to access our clients
    _clients.insert(this ->getClientKey(client), client);

    // readyRead: when clients send message to us and we are ready to read
    connect(client, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);

    // when client is disconnect, need to remove that client from hash map
    connect(client, &QTcpSocket::disconnected, this, &TcpServer::onDisconnected);
}

void TcpServer::onReadyRead()
{
    // we can use sender() to get pointer to client object and read message from client object
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if (client == nullptr) {
        return;
    }

    const auto message = this -> getClientKey(client).toUtf8() + ": " + client -> readAll();

    // send message to other client and show in our user interface
    emit newMessage(message);
}

void TcpServer::onDisconnected()
{
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if (client == nullptr) {
        return;
    }

    _clients.remove(this -> getClientKey(client));
}

void TcpServer::onNewMessage(const QByteArray &ba)
{
    for (const auto &client : qAsConst(_clients)) {
        client -> write(ba);
        client -> flush();
    }
}

QString TcpServer::getClientKey(const QTcpSocket *client) const
{
    // identify our client using address and port
    return client -> peerAddress().toString().append(":").append(QString::number(client -> peerPort()));
}

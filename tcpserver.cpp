#include "tcpserver.h"
#include <iostream>
#include "QtWidgets/QMainWindow"
#include "chatbot.h"

TcpServer* TcpServer::instance = nullptr;

TcpServer::TcpServer(QObject *parent, quint16 port, int maxChatBots)
    : QObject{parent}
{
    connect(&_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    connect(this, &TcpServer::broadcast, this, &TcpServer::onBroadcast);
    connect(this, &TcpServer::unicast, this, &TcpServer::onUnicast);
    connect(this, &TcpServer::showMsg, this, &TcpServer::onShowMsg);

    // listen on custom port. if succeed
    if (_server.listen(QHostAddress::Any, port)) {
        qInfo() <<"Listening on port:" + QString::number(port);
    }

    _chatbots = new ChatBotGroup(maxChatBots);
}

void TcpServer::onNewConnection()
{
    qInfo() << "onNewConnection()";

    // get the next client that is waiting to connect to our server
    const auto client = _server.nextPendingConnection();
    if (client == nullptr) {
        return;
    }

    QString clientKey = this ->getClientKey(client);
    QString msg;

    if (_chatbots->addChatBot(clientKey) < 0) {
        qInfo() << "New client \"" + this->getClientUsername(client) + "\" wants to connect but server exceeds the maximum number of chatbots.";
        msg = "The server is busy, please wait and re-try";
        emit unicast(client, encrypt(msg).toUtf8());
        client->close();
        return;
    }

    qInfo() << "New client \"" + this->getClientUsername(client) + "\" is connected.";
    msg = "You are connected to the server";
    emit unicast(client, encrypt(msg).toUtf8());

    // use this hashmap to access our clients
    _clients.insert(clientKey, client);

    // readyRead: when clients send message to us and we are ready to read
    connect(client, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);

    // when client is disconnect, need to remove that client from hash map
    connect(client, &QTcpSocket::disconnected, this, &TcpServer::onDisconnected);
}

void TcpServer::onReadyRead()
{
    qInfo() << "onReadyRead()";
    // we can use sender() to get pointer to client object and read message from client object
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if (client == nullptr) {
        return;
    }
    const auto client_username = this->getClientUsername(client);
    const auto client_key = this -> getClientKey(client);
    const auto client_msg = client -> readAll();
    const auto message = client_username + ": " + client_msg;

    qInfo() << "(Receive)"+message;
    emit showMsg(message.toUtf8());
    // send message to the current client
    sendUnicast(client, client_msg);
}

void TcpServer::onDisconnected()
{
    qInfo() << "onDisconnected()";
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if (client == nullptr) {
        return;
    }
    QString clientKey = this -> getClientKey(client);
    _chatbots->rmChatBot(clientKey);
    _clients.remove(clientKey);
    qInfo() << "Removed client:" + this -> getClientKey(client);
}


QString TcpServer::encrypt(const QString &message) {
    std::string text = message.toUtf8().constData();
    for (size_t i = 0; i != text.size(); i++) {
        text[i] += 2;
    }
    return QString::fromStdString(text);
}

void TcpServer::sendUnicast(QTcpSocket *client, const QByteArray &message)
{
    QByteArray response = "Auto reply: " + _chatbots->replyClient(this -> getClientKey(client), message);
    emit showMsg(response);
    QString msg = encrypt(response);//encryption before send message
    qInfo() << "sendUnicast():" + msg;
    emit unicast(client, msg.toUtf8());
}

void TcpServer::onUnicast(QTcpSocket *client, const QByteArray &message)
{
    qInfo() << "onUnicast()";
    client -> write(message);
    client -> flush();
}

void TcpServer::sendBroadcast(const QByteArray &message)
{
    QByteArray completeMsg = "BROADCAST: " + message;
    emit showMsg(completeMsg);
    QString msg = encrypt(completeMsg);//encryption before send message
    qInfo() << "sendBroadcast():" + msg;
    emit broadcast(msg.toUtf8());
}

void TcpServer::onBroadcast(const QByteArray &ba)
{
    qInfo() << "onBroadcast()";

    for (const auto &client : qAsConst(_clients)) {
        client -> write(ba);
        client -> flush();
    }
}

void TcpServer::onShowMsg(const QByteArray &ba)
{

}

QString TcpServer::getClientKey(const QTcpSocket *client) const
{
    // identify our client using address and port
    return client -> peerAddress().toString().append(":").append(QString::number(client -> peerPort()));
}


QString TcpServer::getClientUsername(const QTcpSocket *client) const
{
    return "USER "+QString::number(client -> peerPort());
}

TcpServer* TcpServer::getInstance(QObject* parent, quint16 port, int maxChatBots) {
    if (!TcpServer::instance) {
         TcpServer::instance = new TcpServer(parent, port, maxChatBots);
    }
    return TcpServer::instance;
}

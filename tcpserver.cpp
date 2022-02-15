#include "tcpserver.h"

const QHash<QByteArray, QByteArray> TcpServer::QAs = {
    {"hi","Hello"},
    {"bye","GoodBye"},
    {"how are you", "Good, thank you!"}
};

TcpServer::TcpServer(QObject *parent, quint16 port)
    : QObject{parent}
{
    connect(&_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    connect(this, &TcpServer::broadcast, this, &TcpServer::onBroadcast);
    connect(this, &TcpServer::unicast, this, &TcpServer::onUnicast);

    // listen on custom port. if succeed
    if (_server.listen(QHostAddress::Any, port)) {
        qInfo() <<"Listening on port:" + QString::number(port);
    }
}

void TcpServer::onNewConnection()
{
    qInfo() << "onNewConnection()";

    // get the next client that is waiting to connect to our server
    const auto client = _server.nextPendingConnection();
    if (client == nullptr) {
        return;
    }

    qInfo() << "New client \"" + this->getClientUsername(client) + "\" is connected.";

    // use this hashmap to access our clients
    _clients.insert(this ->getClientKey(client), client);

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
    const auto message = client_key.toUtf8() + ": " + client_msg;

    qInfo() << "(Receive)"+client_username + ": " + client_msg;

    // send message to the current client
    emit unicast(client, client_msg);
}

void TcpServer::onDisconnected()
{
    qInfo() << "onDisconnected()";
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if (client == nullptr) {
        return;
    }

    _clients.remove(this -> getClientKey(client));
    qInfo() << "Removed client:" + this -> getClientKey(client);
}


void TcpServer::onUnicast(QTcpSocket *client, const QByteArray &message)
{
    qInfo() << "onUnicast()";
    QByteArray response = QAs.value(message.toLower(), "Sorry, I don't understand");
    client -> write("Auto reply: " + response);
    client -> flush();
}

void TcpServer::sendBroadcast(const QString &message)
{
    qInfo() << "sendBroadcast():" + message;
    emit broadcast("BROADCAST: " + message.toUtf8());
}

void TcpServer::onBroadcast(const QByteArray &ba)
{
    qInfo() << "onBroadcast()";

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

QString TcpServer::getClientUsername(const QTcpSocket *client) const
{
    return "USER "+QString::number(client -> peerPort());
}

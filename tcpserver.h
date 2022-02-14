#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
// for server
#include <QTcpServer>
// for client
#include <QTcpSocket>
#include <QHash>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);

signals:
    void newMessage(const QByteArray &message);

public slots:
    void sendMessage(const QString &message);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onNewMessage(const QByteArray &ba);
    void onDisconnected();

private:
    QString getClientKey(const QTcpSocket * client) const;

private:
    QString encrypt(const QString &message);

private:
    QTcpServer _server;
    QHash<QString, QTcpSocket*> _clients;
};

#endif // TCPSERVER_H

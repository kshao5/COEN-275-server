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
    static TcpServer* instance;
    static TcpServer* getInstance(QObject* parent = nullptr, quint16 port=45000);


signals:
    void broadcast(const QByteArray &message);
    void unicast(QTcpSocket * client, const QByteArray &message);
    void showMsg(const QByteArray &ba);

public slots:
    void sendUnicast(QTcpSocket *client, const QByteArray &message);
    void sendBroadcast(const QByteArray &message);


private slots:
    void onNewConnection();
    void onReadyRead();
    void onBroadcast(const QByteArray &ba);
    void onUnicast(QTcpSocket * client, const QByteArray &message);
    void onShowMsg(const QByteArray &ba);
    void onDisconnected();

private:
    QString getClientKey(const QTcpSocket * client) const;
    QString getClientUsername(const QTcpSocket * client) const;
    explicit TcpServer(QObject *parent = nullptr, quint16 port=45000);
private:
    QString encrypt(const QString &message);

private:
    QTcpServer _server;
    QHash<QString, QTcpSocket*> _clients;

    static const QHash<QByteArray, QByteArray> QAs;
};

#endif // TCPSERVER_H

#ifndef CHATBOT_H
#define CHATBOT_H
#include <QObject>
#include <QHash>

class ChatBot{
public:
    explicit ChatBot(QString clientKey);
    QByteArray reply(const QByteArray &question);

private:
    QString clientKey;
    static const QHash<QByteArray, QByteArray> QAs;
    QByteArray getTime();
    QByteArray getDate();
};

#endif // CHATBOT_H

#ifndef CHATBOT_H
#define CHATBOT_H
#include <QObject>
#include <QHash>

// ChatBot is a singleton
class ChatBot{
public:
    static ChatBot* getInstance();
    QByteArray reply(const QByteArray &question);

private:
    static ChatBot* instance;
    explicit ChatBot();
    static const QHash<QByteArray, QByteArray> QAs;
    QByteArray getTime();
    QByteArray getDate();
};

#endif // CHATBOT_H

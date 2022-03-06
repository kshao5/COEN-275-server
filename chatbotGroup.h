#ifndef CHATBOTGROUP_H
#define CHATBOTGROUP_H
#include <QObject>
#include <QHash>
#include "chatbot.h"

class ChatBotGroup{
public:
    explicit ChatBotGroup(int maxChatBots);
    QByteArray replyClient(QString clientKey, const QByteArray &question);
    int addChatBot(QString clientKey);
    int rmChatBot(QString clientKey);
private:
    int maxChatBots;
    QHash<QString, ChatBot*> chatbots;
};

#endif // CHATBOTGROUP_H

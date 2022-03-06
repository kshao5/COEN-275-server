#include "chatbotGroup.h"


ChatBotGroup::ChatBotGroup(int maxChatBots):maxChatBots(maxChatBots){}


QByteArray ChatBotGroup::replyClient(QString clientKey, const QByteArray &question){
    return chatbots[clientKey]->reply(question);
}


int ChatBotGroup::addChatBot(QString clientKey){
    if (chatbots.size() >= maxChatBots) {
        return -1;
    }

    chatbots.insert(clientKey, new ChatBot(clientKey));
    return chatbots.size();
}

int ChatBotGroup::rmChatBot(QString clientKey){
    delete chatbots[clientKey];
    chatbots.remove(clientKey);
    return chatbots.size();
}

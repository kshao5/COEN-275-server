#include "chatbot.h"

ChatBot* ChatBot::instance = nullptr;

const QHash<QByteArray, QByteArray> ChatBot::QAs = {
    {"hi","Hello"},
    {"bye","GoodBye"},
    {"how are you", "Good, thank you!"}
};

ChatBot* ChatBot::getInstance() {
    if (!ChatBot::instance) {
        ChatBot::instance = new ChatBot();
    }
    return ChatBot::instance;
}


QByteArray ChatBot::reply(const QByteArray &question){
    return QAs.value(question.toLower(), "Sorry, I don't understand");
}

ChatBot::ChatBot(){}

#include "chatbot.h"
#include <QDateTime>

const QHash<QByteArray, QByteArray> ChatBot::QAs = {
    {"hi","Hello"},
    {"bye","GoodBye"},
    {"how are you", "Good, thank you!"}
};


QByteArray ChatBot::reply(const QByteArray &question){
    QByteArray parsedQues = question.toLower().trimmed();
    if (parsedQues.compare("time") == 0){
        return getTime();
    } else if (parsedQues.compare("date") == 0) {
        return getDate();
    }
    return QAs.value(parsedQues, "Sorry, I don't understand");
}

ChatBot::ChatBot(QString clientKey):clientKey(clientKey){}

QByteArray ChatBot::getTime(){
    return QDateTime::currentDateTime().toString().toUtf8();
}

QByteArray ChatBot::getDate(){
    return QDateTime::currentDateTime().date().toString().toUtf8();
}

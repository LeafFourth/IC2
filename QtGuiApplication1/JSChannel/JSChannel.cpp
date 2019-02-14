#include "JSChannel.h"

#include <QDebug>

JSChannel::JSChannel(QObject *parent) : QObject(parent)
{
    
}

void JSChannel::callNative(const QString &json)
{
    qInfo() << json;
}
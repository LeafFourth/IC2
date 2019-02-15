#include "JSChannel.h"

#include <QDebug>

#include "bridge/register.h"

JSChannel::JSChannel(QObject *parent) : QObject(parent)
{
    
}

void JSChannel::init()
{
    JSRegisterCall("TestBind", JSChannel::TestBind, this, std::placeholders::_1);
}

//static
void JSChannel::TestBind(JSChannel *self, const std::string &msg)
{
    qInfo() << "invode TestBind, self:" << self << ", msg:" << msg.c_str();
}

void JSChannel::callNative(const QString &json)
{
    JSInvoke(json.toStdString());
}
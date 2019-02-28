#include "JSChannel.h"

#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>

#include "JSFunTranslator/translator/translator.h"

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
	QMessageBox::about(nullptr, "debug", QString("I'm function TestBind:%1").arg(msg.c_str()));
    qInfo() << "invode TestBind, self:" << self << ", msg:" << msg.c_str();
}

void JSChannel::callNative(const QString &json)
{
    bool ret = JSInvoke(json.toStdString());
	if (!ret) {
		QMessageBox::warning(nullptr, "debug", "not such call");
	}
}
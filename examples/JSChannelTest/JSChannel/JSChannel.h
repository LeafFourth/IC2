#ifndef JS_CHANNEL_H
#define JS_CHANNEL_H

#include <QtCore/QObject>
#include <QtCore/QString>

class JSChannel : public QObject {
    Q_OBJECT
public:
    JSChannel(QObject *parent);

signals:
    void callJS(const QString &method, const QString &argsJson);

public:
    void init();

    static void TestBind(JSChannel *self, const std::string &msg);

public slots:
    void callNative(const QString &json);
};

#endif //JS_CHANNEL_H
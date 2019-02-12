#ifndef JS_CHANNEL_H
#define JS_CHANNEL_H

#include <QtCore/QObject>
#include <QtCore/QString>

class JSChannel : public QObject {
    Q_OBJECT
public:
    JSChannel(QObject *parent);

signals:
    void CallJS(const QString &method, const QString &argsJson);

public slots:
    void CallNative(const QString &json);
};

#endif //JS_CHANNEL_H
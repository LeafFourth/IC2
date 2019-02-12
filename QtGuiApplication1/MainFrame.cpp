#include "MainFrame.h"

#include <QtCore/QUrl>

#include <QtWebEngine/QtWebEngine>
#include <QtWebEngineWidgets/QWebEngineSettings>
#include <QtWebChannel/QtWebChannel>

#include "JSChannel/JSChannel.h"

MainFrame::MainFrame(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

void MainFrame::SetupoJSChannel()
{
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

    QWebChannel *channel = new QWebChannel(this);
    JSChannel *js_api = new JSChannel(channel);
    channel->registerObject("JSChannel", js_api);
    ui.webView->page()->setWebChannel(channel);
    ui.webView->setUrl(QString("https://www.baidu.com"));
    //window->currentTab()->page()->setWebChannel(channel);
}
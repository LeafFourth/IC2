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

    connect(ui.testButton, &QPushButton::clicked, this, &MainFrame::onTestButtonClick);
}

JSChannel *js_api;
void MainFrame::SetupoJSChannel()
{
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

    QWebChannel *channel = new QWebChannel(this);
    js_api = new JSChannel(channel);
    channel->registerObject("JSChannel", js_api);
    ui.webView->page()->setWebChannel(channel);


    //ui.webView->setUrl(QString("https://www.baidu.com"));
    ui.webView->setUrl(QString("E:/code/IC2/QtGuiApplication1/Resources/js/index.html"));
}

void MainFrame::onTestButtonClick()
{
    js_api->callJS("none", "hello");
}
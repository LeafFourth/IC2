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
    js_api->init();

    //ui.webView->setUrl(QString("https://www.baidu.com"));

    ui.webView->setUrl(QApplication::applicationDirPath().append("/Resources/js/index.html"));
}

void MainFrame::onTestButtonClick()
{
    js_api->callJS("none", "hello");
}
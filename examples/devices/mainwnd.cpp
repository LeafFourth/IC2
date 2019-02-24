#include "mainwnd.h"

#include <Windows.h>
#include <cguid.h>
#include <shobjidl.h> 
#include <atlbase.h>
#include <dshow.h>
#include <coms/qedit.h>

#include <QtCore/QDebug>
#include <QtGui/QImage>
#include <QtWidgets/QLabel>

#include "common/log.h"

#include "device/camera.h"
#include "device/DeviceDetect.h"


QLabel *label = nullptr;



MainWnd::MainWnd(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);


    std::vector<DeviceInfo> cameras;
    std::vector<DeviceInfo> mics;
    GetCameras(cameras);
    GetMicphones(mics);

    for (auto it = cameras.begin(); it != cameras.end(); ++it) {
        ui.cameraListView->addItem(it->name);
    }

    for (auto it = mics.begin(); it != mics.end(); ++it) {
        ui.micphoneListView->addItem(it->name);
    }

    label = new QLabel();
    label->resize(800, 600);
    label->show();
    //OpenCamera(cameras);

    CameraThread *render = new CameraThread;
    render->init("");
    render->start();

    connect(render, &CameraThread::signalFrame, this, &MainWnd::OnReceiveMsg);

    qDebug() << 1;

    //LOG_TRACE0(qInfo); 
    //LOG_TRACE(qInfo, parent);
    //LOG_TRACE(qInfo, parent);
    
}

MainWnd::~MainWnd()
{

}

void MainWnd::OnReceiveMsg(const QImage &img)
{
    if (label) {
        label->setPixmap(QPixmap::fromImage(img));
    }
}

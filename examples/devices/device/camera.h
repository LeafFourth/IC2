#ifndef CAMERA_H
#define CAMERA_H

#include <Windows.h>
#include <cguid.h>
#include <shobjidl.h> 
#include <atlbase.h>
#include <dshow.h>
#include <coms/qedit.h>

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtGui/QImage>
#include <QtWidgets/QLabel>

#include "common/log/log.h"

#include "device/DeviceDetect.h"

//此对象必须声明在栈上
class CameraThread : public QThread {
    Q_OBJECT
private:
    class SampleReceiver : public ISampleGrabberCB  {
    public:
        SampleReceiver(int w, int h, const GUID &format, CameraThread *owner);

        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override;

        HRESULT STDMETHODCALLTYPE SampleCB(
            double SampleTime,
            IMediaSample *pSample) override;

        HRESULT STDMETHODCALLTYPE BufferCB(
            double /*SampleTime*/,
            BYTE */*pBuffer*/,
            long /*BufferLen*/) override;

    private:
        int w_;
        int h_;
        GUID format_;
        CameraThread *owner_;

    };

public:
    CameraThread() : inited_(false), stop_(false), graber(nullptr){}
    ~CameraThread();

    void init(const QString &deviceID);
signals:
    void signalOpenCameraCode(int code);
    void signalFrame(const QImage &image);

protected:
    void run() override;

private:
    bool openCamera();
    bool getFrame(QImage &image);

private:
    bool stop_;
    bool inited_;
    int wid_;
    int hei_;

    QString device_;

private:
    CComPtr<IGraphBuilder> pGraph2;
    CComPtr<ICaptureGraphBuilder2> pBuild2;

    CComPtr<IMediaControl> pControl2;
    CComPtr<IMediaEventEx> pEvent2;
    CComPtr<IBaseFilter> pGrabberF2;

    GUID imgFormat;

    SampleReceiver *graber;
};



#endif //CAMERA_H
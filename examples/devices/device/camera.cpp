#include "device/camera.h"

#include <QtCore/QThread>

#include "common/log/log.h"

static void *CameraImage2RGB24(const uchar *data, int len, const GUID &format);

static void imageRelease(void *p)
{
    if (p) {
        delete p;
    }
}


CameraThread::SampleReceiver::SampleReceiver(int w, int h, const GUID &format, CameraThread *owner)
{
    w_ = w;
    h_ = h;
    format_ = format;
    owner_ = owner;
}

ULONG CameraThread::SampleReceiver::AddRef()
{
    return 1;
}

ULONG CameraThread::SampleReceiver::Release()
{
    return 1;
}

HRESULT CameraThread::SampleReceiver::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown)
    {
        *ppv = (void *) static_cast<ISampleGrabberCB*> (this);
        return NOERROR;
    }

    return E_NOINTERFACE;
}

HRESULT CameraThread::SampleReceiver::SampleCB(
    double SampleTime,
    IMediaSample *pSample)
{
    BYTE *buffer = nullptr;
    pSample->GetPointer(&buffer);

    

    if (!buffer) {
        return S_OK;
    }

    uchar *data = (uchar*)CameraImage2RGB24((uchar*)buffer, pSample->GetSize(), format_);
    if (!data) {
        return S_OK;
    }

    QImage image = QImage(data, w_, h_, QImage::Format_RGB888, imageRelease, data);
    emit owner_->signalFrame(image);

    return S_OK;
}

HRESULT CameraThread::SampleReceiver::BufferCB(
    double /*SampleTime*/,
    BYTE */*pBuffer*/,
    long /*BufferLen*/)
{
    return S_OK;
}

static uchar *YUY2ToRGB24(const uchar *in, const long size)
{
    if (!in) {
        return nullptr;
    }

    if (size % 4) {
        //size invalid;
        return nullptr;
    }
    long pixels = size >> 1;
    long sizeAfter = (pixels << 1) + pixels;  //pixels * 3;
    long groups = pixels >> 1;

    uchar *ret = (uchar*)malloc(sizeAfter);
    if (!ret) {
        return nullptr;
    }

    

    for (int i = 0; i < groups; ++i) {
        int baseBefore = i << 2;  // i * 4;
        int baseAfter = i * 6;  //i * 6

        int y0 = in[baseBefore + 0];
        int u = in[baseBefore + 1] - 128;
        int y1 = in[baseBefore + 2];
        int v = in[baseBefore + 3] - 128;

        int r;
        int g;
        int b;

        r = y0 + 1.28033 * v;
        g = y0 - 0.21482 * u - 0.38059 * v;
        b = y0 + 0.212798 * u;

#define ADJUST(v) \
        if (v < 0) { \
            v = 0; \
        } else if (v > 255) { \
            v = 255; \
        }

        r = y0 + 1.28033 * v;
        g = y0 - 0.21482 * u - 0.38059 * v;
        b = y0 + 2.12798 * u;

        ADJUST(r);
        ADJUST(g);
        ADJUST(b);

        ret[baseAfter + 0] = r;
        ret[baseAfter + 1] = g;
        ret[baseAfter + 2] = b;

        r = y1 + 1.28033 * v;
        g = y1 - 0.21482 * u - 0.38059 * v;
        b = y1 + 2.12798 * u;

        ADJUST(r);
        ADJUST(g);
        ADJUST(b);

        ret[baseAfter + 3] = r;
        ret[baseAfter + 4] = g;
        ret[baseAfter + 5] = b;
    }

    return ret;
}

static uchar *AYUVToRGB24(const uchar *data, int len)
{


    return nullptr;
}

void *CameraImage2RGB24(const uchar *data, int len, const GUID &format)
{
    uchar *ret = nullptr;
    if (format == MEDIASUBTYPE_YUY2) {
        ret = YUY2ToRGB24((uchar*)data, len);
    } else if (format == MEDIASUBTYPE_AYUV) {
        ret = AYUVToRGB24(data, len);
    }
    
    return ret;
}

CameraThread::~CameraThread()
{
    if (graber) {
        delete graber;
        graber = nullptr;
    }
}

void CameraThread::init(const QString &deviceID)
{
    if (inited_) {
        return;
    }

    inited_ = true;
    device_ = deviceID;

    //device = "@device:pnp:\\\\?\\usb#vid_2232&pid_1041&mi_00#7&81a613b&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global";
    device_ = "@device:pnp:\\\\?\\usb#vid_18ec&pid_3399&mi_00#6&14fb4671&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global";
}

void CameraThread::run()
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    if (!openCamera()) {
        emit signalOpenCameraCode(-1);

        return;
    }

    HRESULT hr;
    hr = pControl2->Run();
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return;
    }

    QThread::run();

    //while (!stop_) {
    //    QImage img;
    //    bool getted = getFrame(img);
    //    if (getted) {
    //        emit signalFrame(img);
    //    }
    //}
}

bool CameraThread::openCamera()
{
    CComPtr<IGraphBuilder> pGraph;
    CComPtr<ICaptureGraphBuilder2> pBuild;

    // Create the Capture Graph Builder.
    HRESULT hr = pBuild.CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
        CLSCTX_INPROC_SERVER);
    if (!SUCCEEDED(hr))
    {
        LOG_TRACE(qCritical, hr);
        return false;
    }


    hr = pGraph.CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER);
    if (!SUCCEEDED(hr))
    {
        // Initialize the Capture Graph Builder.
        LOG_TRACE(qCritical, hr);
        return false;

    }

    hr = pBuild->SetFiltergraph(pGraph);
    if (!SUCCEEDED(hr))
    {
        LOG_TRACE(qCritical, hr);
        return false;
    }

    CComPtr<IBindCtx> pbc;
    CreateBindCtx(0, &pbc);

    CComPtr<IMoniker> mon;
    ULONG parseLen = 0;
    hr = MkParseDisplayName(
        pbc,
        L"@device:pnp:\\\\?\\usb#vid_2232&pid_1041&mi_00#7&81a613b&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global",            //LPCOLESTR szUserName,
        //L"@device:pnp:\\\\?\\root#image#0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global",
        //L"@device:pnp:\\\\?\\usb#vid_18ec&pid_3399&mi_00#6&14fb4671&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global",
        &parseLen,
        &mon
        );


    if (!SUCCEEDED(hr))
    {
        LOG_TRACE(qCritical, hr);
        return false;
    }

    CComPtr<IBaseFilter> pCap;
    hr = mon->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap);
    if (!SUCCEEDED(hr))
    {
        LOG_TRACE(qCritical, hr);
        return false;
    }

    hr = pGraph->AddFilter(pCap, L"Capture Filter");
    if (!SUCCEEDED(hr))
    {
        LOG_TRACE(qCritical, hr);
        return false;
    }



    CComPtr<IMediaControl> pControl;
    CComPtr<IMediaEventEx> pEvent;
    CComPtr<IBaseFilter> pGrabberF;
    CComPtr<ISampleGrabber> pGrabber;
    CComPtr<IBaseFilter> pSourceF;
    CComPtr<IEnumPins> pEnum;
    CComPtr<IPin> pPin;
    CComPtr<IBaseFilter> pNullF;


  
    hr = pGraph->QueryInterface(IID_PPV_ARGS(&pControl));
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    hr = pGraph->QueryInterface(IID_PPV_ARGS(&pEvent));
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    // Create the Sample Grabber filter.
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pGrabberF));
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    hr = pGrabberF->QueryInterface(IID_PPV_ARGS(&pGrabber));
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }



    hr = pGrabber->SetOneShot(FALSE);
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    hr = pGrabber->SetBufferSamples(FALSE);
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }


    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pNullF));
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    hr = pGraph->AddFilter(pNullF, L"Null Filter");
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
        pCap, pGrabberF, pNullF);
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(mt));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;

    //hr = pGrabber->SetMediaType(&mt);
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    CComPtr<IAMStreamConfig> pam;
    hr = pBuild->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
        pCap, IID_IAMStreamConfig, reinterpret_cast<void**>(&pam)); // 得到媒体控制接口

    int nCount = 0;
    int nSize = 0;
    int nwid = 0;
    int nhei = 0;

    //AM_MEDIA_TYPE* pmmt2;
    //hr = pam->GetFormat(&pmmt2);
    //if (!pmmt2) {
    //    return false;
    //}

    //if (pmmt2->formattype == FORMAT_VideoInfo) {
    //    VIDEOINFOHEADER* pvih = reinterpret_cast<VIDEOINFOHEADER*>(pmmt2->pbFormat);

    //    nwid = pvih->bmiHeader.biWidth; // 得到采集的宽
    //    nhei = pvih->bmiHeader.biHeight; //　得到采集的高
    //}
    //todo
    //DeleteMediaType(pmmt2);

    hr = pGrabber->GetConnectedMediaType(&mt);
    if (mt.formattype == FORMAT_VideoInfo) {
        VIDEOINFOHEADER* pvih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);

        nwid = pvih->bmiHeader.biWidth; // 得到采集的宽
        nhei = pvih->bmiHeader.biHeight; //　得到采集的高

        imgFormat = mt.subtype;
    }
    else {
        return false;
    }

    
    

    pGraph2 = pGraph;
    pBuild2 = pBuild;

    pControl2 = pControl;
    pEvent2 = pEvent;
    pGrabberF2 = pGrabberF;
    wid_ = nwid;
    hei_ = nhei;

    graber = new SampleReceiver(wid_, hei_, imgFormat, this);
    pGrabber->SetCallback(graber, 0);

    return true;
    //获取摄像头分辨率的代码
    //hr = pam->GetNumberOfCapabilities(&nCount, &nSize);

    //// 判断是否为视频信息
    //if (sizeof(VIDEO_STREAM_CONFIG_CAPS) != nSize) {
    //    LOG_TRACE(qCritical, holder, hr);
    //    return false;
    //}
    //if (nCount <= 0)  {
    //    return false;
    //}
    //CComPtr<IAMStreamConfig> pam;
    //VIDEO_STREAM_CONFIG_CAPS scc;
    //AM_MEDIA_TYPE* pmmt;
    //hr = pBuild->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
    //    pCap, IID_IAMStreamConfig, reinterpret_cast<void**>(&pam)); // 得到媒体控制接口
    //hr = pam->GetStreamCaps(0, &pmmt, reinterpret_cast<BYTE*>(&scc));

    //if (pmmt->formattype == FORMAT_VideoInfo) {
    //    VIDEOINFOHEADER* pvih = reinterpret_cast<VIDEOINFOHEADER*>(pmmt->pbFormat);

    //    nwid = pvih->bmiHeader.biWidth; // 得到采集的宽
    //    nhei = pvih->bmiHeader.biHeight; //　得到采集的高
    //}

    
}

bool CameraThread::getFrame(QImage &image)
{
    HRESULT hr;
    BYTE *pBuffer = NULL;

    long cbBuffer = 0;

    CComPtr<ISampleGrabber> pGrabber;
    hr = pGrabberF2->QueryInterface(IID_PPV_ARGS(&pGrabber));
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    //hr = pGrabber->SetOneShot(TRUE);
    //if (FAILED(hr))
    //{
    //    LOG_TRACE(qCritical, holder, hr);
    //    return false;
    //}

    //hr = pGrabber->SetBufferSamples(TRUE);
    //if (FAILED(hr))
    //{
    //    LOG_TRACE(qCritical, holder, hr);
    //    return false;
    //}

    //hr = pControl2->Run();
    //if (FAILED(hr))
    //{
    //    LOG_TRACE(qCritical, holder, hr);
    //    return false;
    //}

    long evCode;
    E_ABORT;
    EC_COMPLETE;
    //hr = pEvent2->WaitForCompletion(INFINITE, &evCode);
    hr = pEvent2->WaitForCompletion(INFINITE, &evCode);
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    // Find the required buffer size.
    VFW_E_WRONG_STATE;
    hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    pBuffer = (BYTE*)CoTaskMemAlloc(cbBuffer);
    if (!pBuffer)
    {
        hr = E_OUTOFMEMORY;
        return false;
    }

    hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
    if (FAILED(hr))
    {
        LOG_TRACE(qCritical, holder, hr);
        return false;
    }

    uchar *data = (uchar*)CameraImage2RGB24((uchar*)pBuffer, cbBuffer, imgFormat);
    
    if (data) {
        image = QImage(data, wid_, hei_, QImage::Format_RGB888);
    }


    CoTaskMemFree(pBuffer);
    return data != nullptr;
}
#include "device/DeviceDetect.h"

#include <Windows.h>
#include <cguid.h>
#include <shobjidl.h> 
#include <atlbase.h>
#include <dshow.h>

#include <QtCore/QDebug>


void InitCOM()
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}

bool GetCameras(std::vector<DeviceInfo> &list)
{
    HRESULT hr;

    CComPtr<ICreateDevEnum> devEnum;
    hr = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER);

    if (!SUCCEEDED(hr)) {
        return false;
    }


    CComPtr<IEnumMoniker> pEnum;
    list.clear();

    // Create an enumerator for the category.
    hr = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_FALSE)
    {
        hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        printf("no cameras\n");
    }
    else {

        IMoniker *pMoniker;

        while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
        {

            CComPtr<IPropertyBag> pPropBag;
            HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
            if (FAILED(hr))
            {
                continue;
            }

            VARIANT var;
            VariantInit(&var);

            DeviceInfo device;

            // Get description or friendly name.
            hr = pPropBag->Read(L"Description", &var, 0);
            if (FAILED(hr))
            {
                hr = pPropBag->Read(L"FriendlyName", &var, 0);
            }
            if (SUCCEEDED(hr))
            {
                printf("%S\n", var.bstrVal);
                device.name = QString::fromUtf16((const ushort*)var.bstrVal);
                VariantClear(&var);
            }

            hr = pPropBag->Read(L"DevicePath", &var, 0);
            if (SUCCEEDED(hr))
            {
                // The device path is not intended for display.
                device.id = QString::fromUtf16((const ushort*)var.bstrVal);
                VariantClear(&var);
            }

            CComPtr<IBindCtx> pcb;
            hr = CreateBindCtx(0, &pcb);
            if (SUCCEEDED(hr)) {
                LPOLESTR name = NULL;
                pMoniker->GetDisplayName(pcb, NULL, &name);
                if (name) {
                    device.id = QString::fromUtf16((const ushort*)name);

                    CoTaskMemFree(name);

                }
            }

           

            list.push_back(device);

            pMoniker->Release();
        }

    }

    return true;
}

bool GetMicphones(std::vector<DeviceInfo> &list)
{
    CComPtr<ICreateDevEnum> pDevEnum;
    HRESULT hr = pDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER);

    if (!SUCCEEDED(hr))
    {
        return false;
    }

    CComPtr<IEnumMoniker> pEnum;
    list.clear();

    // Create an enumerator for the category.
    hr = pDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEnum, 0);
    if (hr == S_FALSE)
    {
        hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        printf("no micphones\n");
    }
    else {

        IMoniker *pMoniker = NULL;

        while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
        {
            CComPtr<IPropertyBag> pPropBag;
            HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
            if (FAILED(hr))
            {
                pMoniker->Release();
                continue;
            }

            DeviceInfo device;

            VARIANT var;
            VariantInit(&var);

            // Get description or friendly name.
            hr = pPropBag->Read(L"Description", &var, 0);
            if (FAILED(hr))
            {
                hr = pPropBag->Read(L"FriendlyName", &var, 0);
            }
            if (SUCCEEDED(hr))
            {
                device.name = QString::fromUtf16((const ushort*)var.bstrVal);
                VariantClear(&var);
            }

            hr = pPropBag->Write(L"FriendlyName", &var);

            // WaveInID applies only to audio capture devices.
            hr = pPropBag->Read(L"WaveInID", &var, 0);
            if (SUCCEEDED(hr))
            {
                VariantClear(&var);
            }

            CComPtr<IBindCtx> pcb;
            hr = CreateBindCtx(0, &pcb);
            if (SUCCEEDED(hr)) {
                LPOLESTR name = NULL;
                pMoniker->GetDisplayName(pcb, NULL, &name);
                if (name) {
                    device.id = QString::fromUtf16((const ushort*)name);

                    CoTaskMemFree(name);

                }
            }


            list.push_back(device);

            pMoniker->Release();
        }

    }
    return true;
}
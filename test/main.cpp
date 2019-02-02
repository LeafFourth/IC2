#include <Windows.h>

#include <dshow.h>

void Init()
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}

void ShowCameras()
{
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        IEnumMoniker *pEnum;

        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
            printf("no cameras\n");
        }
        else {


            IMoniker *pMoniker = NULL;

            while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
            {
                IPropertyBag *pPropBag;
                HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
                if (FAILED(hr))
                {
                    pMoniker->Release();
                    continue;
                }

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
                    printf("%S\n", var.bstrVal);
                    VariantClear(&var);
                }

                hr = pPropBag->Write(L"FriendlyName", &var);

                hr = pPropBag->Read(L"DevicePath", &var, 0);
                if (SUCCEEDED(hr))
                {
                    // The device path is not intended for display.
                    printf("Device path: %S\n", var.bstrVal);
                    VariantClear(&var);
                }

                pPropBag->Release();
                pMoniker->Release();
            }

        }

        pDevEnum->Release();
    }
}

void ShowMicphones()
{
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        IEnumMoniker *pEnum;

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
                IPropertyBag *pPropBag;
                HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
                if (FAILED(hr))
                {
                    pMoniker->Release();
                    continue;
                }

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
                    wprintf(L"%s\n", var.bstrVal);
                    VariantClear(&var);
                }

                hr = pPropBag->Write(L"FriendlyName", &var);

                // WaveInID applies only to audio capture devices.
                hr = pPropBag->Read(L"WaveInID", &var, 0);
                if (SUCCEEDED(hr))
                {
                    printf("WaveIn ID: %d\n", var.lVal);
                    VariantClear(&var);
                }


                pPropBag->Release();
                pMoniker->Release();
            }

        }

        pDevEnum->Release();
    }
}

void ShowSpeakers()
{

}


int main()
{
    
    Init();
    ShowCameras();
    ShowMicphones();

    getchar();
    return 0;

}
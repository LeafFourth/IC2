#include "mainwnd.h"
#include <QtWidgets/QApplication>

#include "device/DeviceDetect.h"

int main(int argc, char *argv[])
{
    InitCOM();

    QApplication a(argc, argv);
    MainWnd w;
    w.show();
    return a.exec();
}

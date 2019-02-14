#include "MainFrame.h"

#include <QtCore/QProcessEnvironment>
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "12345");

    MainFrame w;
    w.SetupoJSChannel();
    w.show();
    return a.exec();
}

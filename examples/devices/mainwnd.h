#ifndef MAINWND_H
#define MAINWND_H

#include <QtWidgets/QWidget>
#include "ui_mainwnd.h"

class MainWnd : public QWidget
{
    Q_OBJECT

public:
    MainWnd(QWidget *parent = 0);
    ~MainWnd();

private slots:
    void OnReceiveMsg(const QImage &img);

private:
    Ui::MainWndClass ui;
};

#endif // MAINWND_H

#pragma once

#include <QtWidgets/QWidget>
#include "ui_MainFrame.h"

class MainFrame : public QWidget
{
    Q_OBJECT

public:
    MainFrame(QWidget *parent = Q_NULLPTR);

    void SetupoJSChannel();

private:
    Ui::MainFrameClass ui;
};

/********************************************************************************
** Form generated from reading UI file 'MainFrame.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFRAME_H
#define UI_MAINFRAME_H

#include <QtCore/QVariant>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainFrameClass
{
public:
    QHBoxLayout *horizontalLayout;
    QWebEngineView *webView;

    void setupUi(QWidget *MainFrameClass)
    {
        if (MainFrameClass->objectName().isEmpty())
            MainFrameClass->setObjectName(QStringLiteral("MainFrameClass"));
        MainFrameClass->resize(600, 400);
        horizontalLayout = new QHBoxLayout(MainFrameClass);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        webView = new QWebEngineView(MainFrameClass);
        webView->setObjectName(QStringLiteral("webView"));

        horizontalLayout->addWidget(webView);


        retranslateUi(MainFrameClass);

        QMetaObject::connectSlotsByName(MainFrameClass);
    } // setupUi

    void retranslateUi(QWidget *MainFrameClass)
    {
        MainFrameClass->setWindowTitle(QApplication::translate("MainFrameClass", "MainFrame", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainFrameClass: public Ui_MainFrameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFRAME_H

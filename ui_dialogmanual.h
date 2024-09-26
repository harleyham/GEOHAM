/********************************************************************************
** Form generated from reading UI file 'dialogmanual.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGMANUAL_H
#define UI_DIALOGMANUAL_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_DialogManual
{
public:
    QTextBrowser *textBrowser;

    void setupUi(QDialog *DialogManual)
    {
        if (DialogManual->objectName().isEmpty())
            DialogManual->setObjectName(QString::fromUtf8("DialogManual"));
        DialogManual->resize(951, 590);
        textBrowser = new QTextBrowser(DialogManual);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(10, 10, 931, 571));
        QFont font;
        font.setPointSize(12);
        textBrowser->setFont(font);
        textBrowser->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        retranslateUi(DialogManual);

        QMetaObject::connectSlotsByName(DialogManual);
    } // setupUi

    void retranslateUi(QDialog *DialogManual)
    {
        DialogManual->setWindowTitle(QCoreApplication::translate("DialogManual", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogManual: public Ui_DialogManual {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGMANUAL_H

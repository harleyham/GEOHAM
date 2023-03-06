/********************************************************************************
** Form generated from reading UI file 'dialogexif.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGEXIF_H
#define UI_DIALOGEXIF_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_DialogEXIF
{
public:
    QTextBrowser *textBrowser;

    void setupUi(QDialog *DialogEXIF)
    {
        if (DialogEXIF->objectName().isEmpty())
            DialogEXIF->setObjectName(QString::fromUtf8("DialogEXIF"));
        DialogEXIF->resize(748, 617);
        DialogEXIF->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        textBrowser = new QTextBrowser(DialogEXIF);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(20, 30, 701, 561));
        QFont font;
        font.setPointSize(10);
        textBrowser->setFont(font);
        textBrowser->setFrameShape(QFrame::NoFrame);

        retranslateUi(DialogEXIF);

        QMetaObject::connectSlotsByName(DialogEXIF);
    } // setupUi

    void retranslateUi(QDialog *DialogEXIF)
    {
        DialogEXIF->setWindowTitle(QCoreApplication::translate("DialogEXIF", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogEXIF: public Ui_DialogEXIF {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGEXIF_H

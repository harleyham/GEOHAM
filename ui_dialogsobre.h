/********************************************************************************
** Form generated from reading UI file 'dialogsobre.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGSOBRE_H
#define UI_DIALOGSOBRE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_DialogSobre
{
public:
    QTextBrowser *textBrowser;

    void setupUi(QDialog *DialogSobre)
    {
        if (DialogSobre->objectName().isEmpty())
            DialogSobre->setObjectName(QString::fromUtf8("DialogSobre"));
        DialogSobre->resize(544, 464);
        textBrowser = new QTextBrowser(DialogSobre);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(10, 10, 521, 441));
        QFont font;
        font.setPointSize(12);
        textBrowser->setFont(font);

        retranslateUi(DialogSobre);

        QMetaObject::connectSlotsByName(DialogSobre);
    } // setupUi

    void retranslateUi(QDialog *DialogSobre)
    {
        DialogSobre->setWindowTitle(QCoreApplication::translate("DialogSobre", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogSobre: public Ui_DialogSobre {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGSOBRE_H

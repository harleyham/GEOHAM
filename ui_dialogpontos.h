/********************************************************************************
** Form generated from reading UI file 'dialogpontos.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGPONTOS_H
#define UI_DIALOGPONTOS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_DialogPontos
{
public:
    QDialogButtonBox *buttonBox;
    QTextBrowser *textBrowser;

    void setupUi(QDialog *DialogPontos)
    {
        if (DialogPontos->objectName().isEmpty())
            DialogPontos->setObjectName(QString::fromUtf8("DialogPontos"));
        DialogPontos->setWindowModality(Qt::NonModal);
        DialogPontos->resize(644, 506);
        DialogPontos->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        buttonBox = new QDialogButtonBox(DialogPontos);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(550, 20, 81, 241));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textBrowser = new QTextBrowser(DialogPontos);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(10, 10, 521, 481));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(10);
        textBrowser->setFont(font);

        retranslateUi(DialogPontos);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogPontos, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogPontos, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogPontos);
    } // setupUi

    void retranslateUi(QDialog *DialogPontos)
    {
        DialogPontos->setWindowTitle(QCoreApplication::translate("DialogPontos", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogPontos: public Ui_DialogPontos {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGPONTOS_H

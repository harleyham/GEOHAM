/********************************************************************************
** Form generated from reading UI file 'dialog_rename.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_RENAME_H
#define UI_DIALOG_RENAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Rename
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit;

    void setupUi(QDialog *Dialog_Rename)
    {
        if (Dialog_Rename->objectName().isEmpty())
            Dialog_Rename->setObjectName(QString::fromUtf8("Dialog_Rename"));
        Dialog_Rename->resize(252, 81);
        buttonBox = new QDialogButtonBox(Dialog_Rename);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(60, 40, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit = new QLineEdit(Dialog_Rename);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(20, 10, 211, 22));

        retranslateUi(Dialog_Rename);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog_Rename, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog_Rename, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog_Rename);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Rename)
    {
        Dialog_Rename->setWindowTitle(QCoreApplication::translate("Dialog_Rename", "Renomear arquivo", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Rename: public Ui_Dialog_Rename {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_RENAME_H

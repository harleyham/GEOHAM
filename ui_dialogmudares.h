/********************************************************************************
** Form generated from reading UI file 'dialogmudares.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGMUDARES_H
#define UI_DIALOGMUDARES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_DialogMudaRes
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *XRes_O;
    QLineEdit *YRes_O;
    QLineEdit *CM_PX_O;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *XRes_M;
    QLineEdit *YRes_M;
    QLineEdit *CM_PX_M;
    QLabel *label_5;

    void setupUi(QDialog *DialogMudaRes)
    {
        if (DialogMudaRes->objectName().isEmpty())
            DialogMudaRes->setObjectName("DialogMudaRes");
        DialogMudaRes->resize(420, 300);
        DialogMudaRes->setLayoutDirection(Qt::LeftToRight);
        buttonBox = new QDialogButtonBox(DialogMudaRes);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(40, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        XRes_O = new QLineEdit(DialogMudaRes);
        XRes_O->setObjectName("XRes_O");
        XRes_O->setGeometry(QRect(212, 50, 61, 20));
        XRes_O->setLayoutDirection(Qt::LeftToRight);
        XRes_O->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        XRes_O->setReadOnly(true);
        YRes_O = new QLineEdit(DialogMudaRes);
        YRes_O->setObjectName("YRes_O");
        YRes_O->setGeometry(QRect(212, 90, 61, 20));
        YRes_O->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        YRes_O->setReadOnly(true);
        CM_PX_O = new QLineEdit(DialogMudaRes);
        CM_PX_O->setObjectName("CM_PX_O");
        CM_PX_O->setGeometry(QRect(202, 130, 71, 20));
        CM_PX_O->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        CM_PX_O->setReadOnly(true);
        label = new QLabel(DialogMudaRes);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 50, 141, 16));
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        label_2 = new QLabel(DialogMudaRes);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 90, 111, 16));
        label_2->setFont(font);
        label_3 = new QLabel(DialogMudaRes);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(30, 130, 161, 16));
        label_3->setFont(font);
        label_4 = new QLabel(DialogMudaRes);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(220, 20, 61, 16));
        XRes_M = new QLineEdit(DialogMudaRes);
        XRes_M->setObjectName("XRes_M");
        XRes_M->setGeometry(QRect(320, 50, 61, 20));
        XRes_M->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        YRes_M = new QLineEdit(DialogMudaRes);
        YRes_M->setObjectName("YRes_M");
        YRes_M->setGeometry(QRect(320, 90, 61, 20));
        YRes_M->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        CM_PX_M = new QLineEdit(DialogMudaRes);
        CM_PX_M->setObjectName("CM_PX_M");
        CM_PX_M->setGeometry(QRect(310, 130, 71, 20));
        CM_PX_M->setInputMethodHints(Qt::ImhDigitsOnly|Qt::ImhPreferNumbers);
        CM_PX_M->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5 = new QLabel(DialogMudaRes);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(320, 20, 81, 16));

        retranslateUi(DialogMudaRes);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, DialogMudaRes, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, DialogMudaRes, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(DialogMudaRes);
    } // setupUi

    void retranslateUi(QDialog *DialogMudaRes)
    {
        DialogMudaRes->setWindowTitle(QCoreApplication::translate("DialogMudaRes", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("DialogMudaRes", "Dimens\303\243o Horizontal:", nullptr));
        label_2->setText(QCoreApplication::translate("DialogMudaRes", "Dimens\303\243o Vertical:", nullptr));
        label_3->setText(QCoreApplication::translate("DialogMudaRes", "Resolu\303\247\303\243o espacial: (cm/px)", nullptr));
        label_4->setText(QCoreApplication::translate("DialogMudaRes", "ORIGINAL", nullptr));
        label_5->setText(QCoreApplication::translate("DialogMudaRes", "MODIFICADO", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogMudaRes: public Ui_DialogMudaRes {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGMUDARES_H

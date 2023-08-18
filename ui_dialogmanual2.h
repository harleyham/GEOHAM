/********************************************************************************
** Form generated from reading UI file 'dialogmanual2.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGMANUAL2_H
#define UI_DIALOGMANUAL2_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_DialogManual2
{
public:

    void setupUi(QDialog *DialogManual2)
    {
        if (DialogManual2->objectName().isEmpty())
            DialogManual2->setObjectName(QString::fromUtf8("DialogManual2"));
        DialogManual2->resize(1500, 900);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DialogManual2->sizePolicy().hasHeightForWidth());
        DialogManual2->setSizePolicy(sizePolicy);
        DialogManual2->setMinimumSize(QSize(1500, 900));
        DialogManual2->setMaximumSize(QSize(1500, 900));

        retranslateUi(DialogManual2);

        QMetaObject::connectSlotsByName(DialogManual2);
    } // setupUi

    void retranslateUi(QDialog *DialogManual2)
    {
        DialogManual2->setWindowTitle(QCoreApplication::translate("DialogManual2", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogManual2: public Ui_DialogManual2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGMANUAL2_H

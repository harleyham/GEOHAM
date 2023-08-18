/********************************************************************************
** Form generated from reading UI file 'dialog_mescla_deslocando.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_MESCLA_DESLOCANDO_H
#define UI_DIALOG_MESCLA_DESLOCANDO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Mescla_Deslocando
{
public:
    QLineEdit *Num_Imagens;
    QLineEdit *Imagem1;
    QLineEdit *Imagem2;
    QPushButton *bt_Proximo;
    QLineEdit *Angulo;

    void setupUi(QDialog *Dialog_Mescla_Deslocando)
    {
        if (Dialog_Mescla_Deslocando->objectName().isEmpty())
            Dialog_Mescla_Deslocando->setObjectName(QString::fromUtf8("Dialog_Mescla_Deslocando"));
        Dialog_Mescla_Deslocando->resize(341, 255);
        Num_Imagens = new QLineEdit(Dialog_Mescla_Deslocando);
        Num_Imagens->setObjectName(QString::fromUtf8("Num_Imagens"));
        Num_Imagens->setGeometry(QRect(170, 60, 113, 22));
        Imagem1 = new QLineEdit(Dialog_Mescla_Deslocando);
        Imagem1->setObjectName(QString::fromUtf8("Imagem1"));
        Imagem1->setGeometry(QRect(40, 110, 113, 22));
        Imagem2 = new QLineEdit(Dialog_Mescla_Deslocando);
        Imagem2->setObjectName(QString::fromUtf8("Imagem2"));
        Imagem2->setGeometry(QRect(180, 110, 113, 22));
        bt_Proximo = new QPushButton(Dialog_Mescla_Deslocando);
        bt_Proximo->setObjectName(QString::fromUtf8("bt_Proximo"));
        bt_Proximo->setGeometry(QRect(130, 200, 75, 24));
        Angulo = new QLineEdit(Dialog_Mescla_Deslocando);
        Angulo->setObjectName(QString::fromUtf8("Angulo"));
        Angulo->setGeometry(QRect(120, 160, 113, 22));

        retranslateUi(Dialog_Mescla_Deslocando);

        QMetaObject::connectSlotsByName(Dialog_Mescla_Deslocando);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Mescla_Deslocando)
    {
        Dialog_Mescla_Deslocando->setWindowTitle(QCoreApplication::translate("Dialog_Mescla_Deslocando", "Dialog", nullptr));
        bt_Proximo->setText(QCoreApplication::translate("Dialog_Mescla_Deslocando", "Pr\303\263ximo", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Mescla_Deslocando: public Ui_Dialog_Mescla_Deslocando {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_MESCLA_DESLOCANDO_H

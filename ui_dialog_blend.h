/********************************************************************************
** Form generated from reading UI file 'dialog_blend.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_BLEND_H
#define UI_DIALOG_BLEND_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Blend
{
public:
    QLineEdit *Image_Fundo;
    QLineEdit *Image_Processada;
    QSlider *horizontalSlider;
    QTextBrowser *Output_txt;
    QLineEdit *Valor_Slider;
    QRadioButton *Botao_Blend;
    QRadioButton *Botao_Sobrepor;
    QPushButton *Botao_Usar;
    QPushButton *Botao_Cancel;
    QLabel *label;
    QLabel *label_2;

    void setupUi(QDialog *Dialog_Blend)
    {
        if (Dialog_Blend->objectName().isEmpty())
            Dialog_Blend->setObjectName(QString::fromUtf8("Dialog_Blend"));
        Dialog_Blend->setEnabled(true);
        Dialog_Blend->resize(1142, 259);
        Image_Fundo = new QLineEdit(Dialog_Blend);
        Image_Fundo->setObjectName(QString::fromUtf8("Image_Fundo"));
        Image_Fundo->setGeometry(QRect(20, 40, 541, 20));
        Image_Fundo->setReadOnly(true);
        Image_Processada = new QLineEdit(Dialog_Blend);
        Image_Processada->setObjectName(QString::fromUtf8("Image_Processada"));
        Image_Processada->setGeometry(QRect(20, 110, 541, 20));
        Image_Processada->setReadOnly(true);
        horizontalSlider = new QSlider(Dialog_Blend);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(20, 150, 541, 22));
        horizontalSlider->setOrientation(Qt::Horizontal);
        Output_txt = new QTextBrowser(Dialog_Blend);
        Output_txt->setObjectName(QString::fromUtf8("Output_txt"));
        Output_txt->setGeometry(QRect(580, 10, 541, 231));
        Valor_Slider = new QLineEdit(Dialog_Blend);
        Valor_Slider->setObjectName(QString::fromUtf8("Valor_Slider"));
        Valor_Slider->setEnabled(true);
        Valor_Slider->setGeometry(QRect(520, 210, 51, 20));
        Valor_Slider->setReadOnly(true);
        Botao_Blend = new QRadioButton(Dialog_Blend);
        Botao_Blend->setObjectName(QString::fromUtf8("Botao_Blend"));
        Botao_Blend->setGeometry(QRect(170, 190, 83, 18));
        Botao_Sobrepor = new QRadioButton(Dialog_Blend);
        Botao_Sobrepor->setObjectName(QString::fromUtf8("Botao_Sobrepor"));
        Botao_Sobrepor->setGeometry(QRect(70, 190, 83, 18));
        Botao_Sobrepor->setChecked(true);
        Botao_Usar = new QPushButton(Dialog_Blend);
        Botao_Usar->setObjectName(QString::fromUtf8("Botao_Usar"));
        Botao_Usar->setGeometry(QRect(320, 190, 75, 23));
        Botao_Cancel = new QPushButton(Dialog_Blend);
        Botao_Cancel->setObjectName(QString::fromUtf8("Botao_Cancel"));
        Botao_Cancel->setGeometry(QRect(420, 190, 75, 23));
        label = new QLabel(Dialog_Blend);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(260, 20, 171, 16));
        label_2 = new QLabel(Dialog_Blend);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(260, 90, 161, 16));

        retranslateUi(Dialog_Blend);

        QMetaObject::connectSlotsByName(Dialog_Blend);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Blend)
    {
        Dialog_Blend->setWindowTitle(QCoreApplication::translate("Dialog_Blend", "Dialog", nullptr));
        Botao_Blend->setText(QCoreApplication::translate("Dialog_Blend", "Misturar", nullptr));
        Botao_Sobrepor->setText(QCoreApplication::translate("Dialog_Blend", "Soprepor", nullptr));
        Botao_Usar->setText(QCoreApplication::translate("Dialog_Blend", "Usar", nullptr));
        Botao_Cancel->setText(QCoreApplication::translate("Dialog_Blend", "Cancel", nullptr));
        label->setText(QCoreApplication::translate("Dialog_Blend", "Imagem de Refer\303\252ncia:", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog_Blend", "Imagem de Trabalho:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Blend: public Ui_Dialog_Blend {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_BLEND_H

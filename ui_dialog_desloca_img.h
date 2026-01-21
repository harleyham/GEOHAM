/********************************************************************************
** Form generated from reading UI file 'dialog_desloca_img.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_DESLOCA_IMG_H
#define UI_DIALOG_DESLOCA_IMG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Desloca_Img
{
public:
    QLineEdit *Nome_Tif;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_4;
    QFrame *line_3;
    QFrame *line_5;
    QFrame *line_6;
    QFrame *line_7;
    QFrame *line_8;
    QLineEdit *LatOriginal;
    QLineEdit *LonOriginal;
    QDoubleSpinBox *BoxLatNew;
    QDoubleSpinBox *BoxLonNew;
    QPushButton *Bt_Ok;
    QPushButton *Bt_Cancel;
    QLabel *label;
    QRadioButton *Bt_Digita;
    QRadioButton *Bt_Img;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *Nome_Tif_Ref;
    QLabel *label_6;
    QRadioButton *bBt_2Pontos;
    QLineEdit *LatOriginal_2;
    QLineEdit *LonOriginal_2;
    QLabel *label_7;
    QLabel *label_8;

    void setupUi(QDialog *Dialog_Desloca_Img)
    {
        if (Dialog_Desloca_Img->objectName().isEmpty())
            Dialog_Desloca_Img->setObjectName(QString::fromUtf8("Dialog_Desloca_Img"));
        Dialog_Desloca_Img->resize(579, 373);
        Nome_Tif = new QLineEdit(Dialog_Desloca_Img);
        Nome_Tif->setObjectName(QString::fromUtf8("Nome_Tif"));
        Nome_Tif->setGeometry(QRect(90, 20, 451, 22));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(10);
        Nome_Tif->setFont(font);
        line = new QFrame(Dialog_Desloca_Img);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(20, 80, 230, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(Dialog_Desloca_Img);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(20, 80, 20, 210));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(Dialog_Desloca_Img);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(230, 80, 20, 210));
        line_4->setFrameShape(QFrame::VLine);
        line_4->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(Dialog_Desloca_Img);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(20, 270, 230, 16));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_5 = new QFrame(Dialog_Desloca_Img);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(320, 80, 230, 16));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        line_6 = new QFrame(Dialog_Desloca_Img);
        line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setGeometry(QRect(320, 80, 20, 210));
        line_6->setFrameShape(QFrame::VLine);
        line_6->setFrameShadow(QFrame::Sunken);
        line_7 = new QFrame(Dialog_Desloca_Img);
        line_7->setObjectName(QString::fromUtf8("line_7"));
        line_7->setGeometry(QRect(530, 80, 20, 210));
        line_7->setFrameShape(QFrame::VLine);
        line_7->setFrameShadow(QFrame::Sunken);
        line_8 = new QFrame(Dialog_Desloca_Img);
        line_8->setObjectName(QString::fromUtf8("line_8"));
        line_8->setGeometry(QRect(320, 270, 230, 16));
        line_8->setFrameShape(QFrame::HLine);
        line_8->setFrameShadow(QFrame::Sunken);
        LatOriginal = new QLineEdit(Dialog_Desloca_Img);
        LatOriginal->setObjectName(QString::fromUtf8("LatOriginal"));
        LatOriginal->setGeometry(QRect(110, 150, 80, 22));
        LatOriginal->setReadOnly(true);
        LonOriginal = new QLineEdit(Dialog_Desloca_Img);
        LonOriginal->setObjectName(QString::fromUtf8("LonOriginal"));
        LonOriginal->setGeometry(QRect(110, 180, 80, 22));
        LonOriginal->setReadOnly(true);
        BoxLatNew = new QDoubleSpinBox(Dialog_Desloca_Img);
        BoxLatNew->setObjectName(QString::fromUtf8("BoxLatNew"));
        BoxLatNew->setGeometry(QRect(420, 150, 80, 22));
        BoxLatNew->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        BoxLatNew->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        BoxLatNew->setButtonSymbols(QAbstractSpinBox::NoButtons);
        BoxLatNew->setDecimals(6);
        BoxLonNew = new QDoubleSpinBox(Dialog_Desloca_Img);
        BoxLonNew->setObjectName(QString::fromUtf8("BoxLonNew"));
        BoxLonNew->setGeometry(QRect(420, 180, 80, 22));
        BoxLonNew->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        BoxLonNew->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        BoxLonNew->setButtonSymbols(QAbstractSpinBox::NoButtons);
        BoxLonNew->setDecimals(6);
        Bt_Ok = new QPushButton(Dialog_Desloca_Img);
        Bt_Ok->setObjectName(QString::fromUtf8("Bt_Ok"));
        Bt_Ok->setGeometry(QRect(370, 320, 75, 24));
        Bt_Cancel = new QPushButton(Dialog_Desloca_Img);
        Bt_Cancel->setObjectName(QString::fromUtf8("Bt_Cancel"));
        Bt_Cancel->setGeometry(QRect(460, 320, 75, 24));
        label = new QLabel(Dialog_Desloca_Img);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 61, 16));
        label->setFont(font);
        Bt_Digita = new QRadioButton(Dialog_Desloca_Img);
        Bt_Digita->setObjectName(QString::fromUtf8("Bt_Digita"));
        Bt_Digita->setGeometry(QRect(70, 300, 89, 20));
        Bt_Digita->setFont(font);
        Bt_Img = new QRadioButton(Dialog_Desloca_Img);
        Bt_Img->setObjectName(QString::fromUtf8("Bt_Img"));
        Bt_Img->setGeometry(QRect(70, 320, 89, 20));
        Bt_Img->setFont(font);
        label_2 = new QLabel(Dialog_Desloca_Img);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(70, 152, 49, 16));
        label_2->setFont(font);
        label_3 = new QLabel(Dialog_Desloca_Img);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(380, 152, 49, 16));
        label_3->setFont(font);
        label_4 = new QLabel(Dialog_Desloca_Img);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(70, 182, 49, 16));
        label_4->setFont(font);
        label_5 = new QLabel(Dialog_Desloca_Img);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(380, 182, 49, 16));
        label_5->setFont(font);
        Nome_Tif_Ref = new QLineEdit(Dialog_Desloca_Img);
        Nome_Tif_Ref->setObjectName(QString::fromUtf8("Nome_Tif_Ref"));
        Nome_Tif_Ref->setGeometry(QRect(90, 50, 451, 22));
        Nome_Tif_Ref->setFont(font);
        label_6 = new QLabel(Dialog_Desloca_Img);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 50, 71, 16));
        label_6->setFont(font);
        bBt_2Pontos = new QRadioButton(Dialog_Desloca_Img);
        bBt_2Pontos->setObjectName(QString::fromUtf8("bBt_2Pontos"));
        bBt_2Pontos->setGeometry(QRect(70, 340, 89, 20));
        bBt_2Pontos->setFont(font);
        LatOriginal_2 = new QLineEdit(Dialog_Desloca_Img);
        LatOriginal_2->setObjectName(QString::fromUtf8("LatOriginal_2"));
        LatOriginal_2->setGeometry(QRect(110, 210, 80, 22));
        LatOriginal_2->setReadOnly(true);
        LonOriginal_2 = new QLineEdit(Dialog_Desloca_Img);
        LonOriginal_2->setObjectName(QString::fromUtf8("LonOriginal_2"));
        LonOriginal_2->setGeometry(QRect(110, 240, 80, 22));
        LonOriginal_2->setReadOnly(true);
        label_7 = new QLabel(Dialog_Desloca_Img);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(70, 210, 49, 16));
        label_7->setFont(font);
        label_8 = new QLabel(Dialog_Desloca_Img);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(70, 240, 49, 16));
        label_8->setFont(font);

        retranslateUi(Dialog_Desloca_Img);

        QMetaObject::connectSlotsByName(Dialog_Desloca_Img);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Desloca_Img)
    {
        Dialog_Desloca_Img->setWindowTitle(QCoreApplication::translate("Dialog_Desloca_Img", "Dialog", nullptr));
        Bt_Ok->setText(QCoreApplication::translate("Dialog_Desloca_Img", "Ok", nullptr));
        Bt_Cancel->setText(QCoreApplication::translate("Dialog_Desloca_Img", "Cancel", nullptr));
        label->setText(QCoreApplication::translate("Dialog_Desloca_Img", "Arquivo:", nullptr));
        Bt_Digita->setText(QCoreApplication::translate("Dialog_Desloca_Img", "Manual", nullptr));
        Bt_Img->setText(QCoreApplication::translate("Dialog_Desloca_Img", "Imagem", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog_Desloca_Img", "LAT:", nullptr));
        label_3->setText(QCoreApplication::translate("Dialog_Desloca_Img", "LAT:", nullptr));
        label_4->setText(QCoreApplication::translate("Dialog_Desloca_Img", "LON:", nullptr));
        label_5->setText(QCoreApplication::translate("Dialog_Desloca_Img", "LON:", nullptr));
        label_6->setText(QCoreApplication::translate("Dialog_Desloca_Img", "Arquivo Ref:", nullptr));
        bBt_2Pontos->setText(QCoreApplication::translate("Dialog_Desloca_Img", "2 Pontos", nullptr));
        label_7->setText(QCoreApplication::translate("Dialog_Desloca_Img", "LAT:", nullptr));
        label_8->setText(QCoreApplication::translate("Dialog_Desloca_Img", "LON:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Desloca_Img: public Ui_Dialog_Desloca_Img {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_DESLOCA_IMG_H

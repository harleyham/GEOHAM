/********************************************************************************
** Form generated from reading UI file 'dialog_conv.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_CONV_H
#define UI_DIALOG_CONV_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Conv
{
public:
    QLineEdit *txt_Longitude;
    QLineEdit *txt_Easting_4;
    QRadioButton *radioButton_GMS;
    QLabel *label_7;
    QLineEdit *txt_Norting_3;
    QLineEdit *txt_Hemisferio;
    QLabel *label_2;
    QFrame *line;
    QLabel *label_3;
    QLineEdit *txt_Norting_4;
    QLineEdit *txt_Easting_3;
    QLineEdit *txt_LatLon;
    QLineEdit *txt_Zona_4;
    QLabel *label_6;
    QLabel *label_5;
    QLineEdit *txt_Latidude;
    QLineEdit *txt_Zona_3;
    QLabel *label;
    QLabel *label_9;
    QLabel *label_4;
    QRadioButton *radioButton_Dec;
    QFrame *line_2;
    QLabel *label_8;
    QPushButton *bt_Ok;
    QLineEdit *txt_MC1;
    QLineEdit *txt_MC2;
    QLabel *label_10;
    QLabel *label_11;

    void setupUi(QDialog *Dialog_Conv)
    {
        if (Dialog_Conv->objectName().isEmpty())
            Dialog_Conv->setObjectName(QString::fromUtf8("Dialog_Conv"));
        Dialog_Conv->resize(629, 313);
        txt_Longitude = new QLineEdit(Dialog_Conv);
        txt_Longitude->setObjectName(QString::fromUtf8("txt_Longitude"));
        txt_Longitude->setGeometry(QRect(90, 160, 130, 20));
        txt_Easting_4 = new QLineEdit(Dialog_Conv);
        txt_Easting_4->setObjectName(QString::fromUtf8("txt_Easting_4"));
        txt_Easting_4->setGeometry(QRect(530, 160, 80, 20));
        radioButton_GMS = new QRadioButton(Dialog_Conv);
        radioButton_GMS->setObjectName(QString::fromUtf8("radioButton_GMS"));
        radioButton_GMS->setGeometry(QRect(130, 270, 82, 17));
        label_7 = new QLabel(Dialog_Conv);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(460, 160, 51, 16));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(10);
        label_7->setFont(font);
        txt_Norting_3 = new QLineEdit(Dialog_Conv);
        txt_Norting_3->setObjectName(QString::fromUtf8("txt_Norting_3"));
        txt_Norting_3->setGeometry(QRect(320, 210, 80, 20));
        txt_Hemisferio = new QLineEdit(Dialog_Conv);
        txt_Hemisferio->setObjectName(QString::fromUtf8("txt_Hemisferio"));
        txt_Hemisferio->setGeometry(QRect(530, 110, 80, 20));
        label_2 = new QLabel(Dialog_Conv);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 160, 61, 16));
        label_2->setFont(font);
        line = new QFrame(Dialog_Conv);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(230, 39, 20, 251));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        label_3 = new QLabel(Dialog_Conv);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(260, 160, 51, 16));
        label_3->setFont(font);
        txt_Norting_4 = new QLineEdit(Dialog_Conv);
        txt_Norting_4->setObjectName(QString::fromUtf8("txt_Norting_4"));
        txt_Norting_4->setGeometry(QRect(530, 210, 80, 20));
        txt_Easting_3 = new QLineEdit(Dialog_Conv);
        txt_Easting_3->setObjectName(QString::fromUtf8("txt_Easting_3"));
        txt_Easting_3->setGeometry(QRect(320, 160, 80, 20));
        txt_LatLon = new QLineEdit(Dialog_Conv);
        txt_LatLon->setObjectName(QString::fromUtf8("txt_LatLon"));
        txt_LatLon->setGeometry(QRect(20, 210, 201, 20));
        txt_Zona_4 = new QLineEdit(Dialog_Conv);
        txt_Zona_4->setObjectName(QString::fromUtf8("txt_Zona_4"));
        txt_Zona_4->setGeometry(QRect(530, 60, 80, 20));
        label_6 = new QLabel(Dialog_Conv);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(460, 60, 47, 13));
        label_6->setFont(font);
        label_5 = new QLabel(Dialog_Conv);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(260, 110, 47, 13));
        label_5->setFont(font);
        txt_Latidude = new QLineEdit(Dialog_Conv);
        txt_Latidude->setObjectName(QString::fromUtf8("txt_Latidude"));
        txt_Latidude->setGeometry(QRect(90, 110, 130, 20));
        txt_Zona_3 = new QLineEdit(Dialog_Conv);
        txt_Zona_3->setObjectName(QString::fromUtf8("txt_Zona_3"));
        txt_Zona_3->setGeometry(QRect(320, 110, 80, 20));
        label = new QLabel(Dialog_Conv);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 110, 51, 16));
        label->setFont(font);
        label_9 = new QLabel(Dialog_Conv);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(460, 110, 71, 16));
        label_9->setFont(font);
        label_4 = new QLabel(Dialog_Conv);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(260, 210, 51, 16));
        label_4->setFont(font);
        radioButton_Dec = new QRadioButton(Dialog_Conv);
        radioButton_Dec->setObjectName(QString::fromUtf8("radioButton_Dec"));
        radioButton_Dec->setGeometry(QRect(130, 250, 82, 17));
        line_2 = new QFrame(Dialog_Conv);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(420, 39, 20, 251));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);
        label_8 = new QLabel(Dialog_Conv);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(460, 210, 61, 16));
        label_8->setFont(font);
        bt_Ok = new QPushButton(Dialog_Conv);
        bt_Ok->setObjectName(QString::fromUtf8("bt_Ok"));
        bt_Ok->setGeometry(QRect(540, 280, 75, 23));
        bt_Ok->setFont(font);
        txt_MC1 = new QLineEdit(Dialog_Conv);
        txt_MC1->setObjectName(QString::fromUtf8("txt_MC1"));
        txt_MC1->setGeometry(QRect(320, 60, 80, 20));
        txt_MC2 = new QLineEdit(Dialog_Conv);
        txt_MC2->setObjectName(QString::fromUtf8("txt_MC2"));
        txt_MC2->setGeometry(QRect(530, 10, 80, 20));
        label_10 = new QLabel(Dialog_Conv);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(260, 60, 49, 16));
        label_10->setFont(font);
        label_11 = new QLabel(Dialog_Conv);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(460, 10, 49, 16));
        label_11->setFont(font);

        retranslateUi(Dialog_Conv);

        QMetaObject::connectSlotsByName(Dialog_Conv);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Conv)
    {
        Dialog_Conv->setWindowTitle(QCoreApplication::translate("Dialog_Conv", "Dialog", nullptr));
        radioButton_GMS->setText(QCoreApplication::translate("Dialog_Conv", "G M S", nullptr));
        label_7->setText(QCoreApplication::translate("Dialog_Conv", "Easting:", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog_Conv", "Longitude:", nullptr));
        label_3->setText(QCoreApplication::translate("Dialog_Conv", "Easting:", nullptr));
        label_6->setText(QCoreApplication::translate("Dialog_Conv", "Zona:", nullptr));
        label_5->setText(QCoreApplication::translate("Dialog_Conv", "Zona:", nullptr));
        label->setText(QCoreApplication::translate("Dialog_Conv", "Latitude:", nullptr));
        label_9->setText(QCoreApplication::translate("Dialog_Conv", "Hemisf\303\251rio:", nullptr));
        label_4->setText(QCoreApplication::translate("Dialog_Conv", "Northing:", nullptr));
        radioButton_Dec->setText(QCoreApplication::translate("Dialog_Conv", "Decimal", nullptr));
        label_8->setText(QCoreApplication::translate("Dialog_Conv", "Northing:", nullptr));
        bt_Ok->setText(QCoreApplication::translate("Dialog_Conv", "Ok", nullptr));
        label_10->setText(QCoreApplication::translate("Dialog_Conv", "MC:", nullptr));
        label_11->setText(QCoreApplication::translate("Dialog_Conv", "MC:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Conv: public Ui_Dialog_Conv {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_CONV_H

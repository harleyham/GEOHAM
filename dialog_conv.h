#ifndef DIALOG_CONV_H
#define DIALOG_CONV_H

#include <QDialog>

#include <GeographicLib/GeoCoords.hpp>
#include "LatLong-UTM.h"

namespace Ui {
class Dialog_Conv;
}

class Dialog_Conv : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Conv(QWidget *parent = nullptr);
    ~Dialog_Conv();


    UTM_GEO                     Valores;
    GeographicLib::GeoCoords    Valores_C;

    void Inicializa(UTM_GEO &Val,GeographicLib::GeoCoords &Val_C);
    void LatLonEdited();
    void Zona3Edited();
    void Zona4Edited();

private slots:
    void on_txt_Latidude_editingFinished();

    void on_txt_Longitude_editingFinished();

    void on_txt_Zona_3_editingFinished();

    void on_txt_Easting_3_editingFinished();

    void on_txt_Norting_3_editingFinished();

    void on_txt_Zona_4_editingFinished();

    void on_txt_Hemisferio_editingFinished();

    void on_txt_Easting_4_editingFinished();

    void on_txt_Norting_4_editingFinished();

    void on_radioButton_GMS_clicked();

    void on_radioButton_Dec_clicked();

    void on_txt_LatLon_editingFinished();

    void on_bt_Ok_clicked();

    void on_txt_MC1_editingFinished();

    void on_txt_MC2_editingFinished();

private:

    Ui::Dialog_Conv *ui;
};

#endif // DIALOG_CONV_H

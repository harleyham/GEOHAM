#ifndef DIALOG_DESLOCA_IMG_H
#define DIALOG_DESLOCA_IMG_H

#include <QDialog>

#include "geo_exif_xmp.h"

namespace Ui {
class Dialog_Desloca_Img;
}

class Dialog_Desloca_Img : public QDialog
{
    Q_OBJECT

public:

    double  NewLat,NewLon;

    explicit Dialog_Desloca_Img(QWidget *parent = nullptr);
    ~Dialog_Desloca_Img();

    void printFile(QString FileName);
    void printFile_Ref(QString FileName);
    void printOriginalCoords(Geometria_Imagem &GeoProc);
    void printDeslocaCoords(Geometria_Imagem &GeoProc);
    void setLocaleBr();
    void getLatLon(double &Lat, double &Lon);
    void setBoxReadOnly();
    void setBoxWrite();
    void setBoxValues(double Lat, double Lon);
    void setDigita(void);
    void setImagem(void);
    void Posiciona_LATLON(int Pts);

private slots:

    void on_BoxLatNew_editingFinished();

    void on_Bt_Cancel_clicked();

    void on_Bt_Ok_clicked();

    void on_BoxLonNew_editingFinished();

    void on_Bt_Digita_clicked();

    void on_Bt_Img_clicked();

    void on_bBt_2Pontos_clicked();

private:
    Ui::Dialog_Desloca_Img *ui;

signals:
        bool    Cancel();
        bool    Ok();
        bool    Bt_Digita_Signal();
        bool    Bt_Img_Signal();
        bool    Bt_2Pontos_Signal();

};

#endif // DIALOG_DESLOCA_IMG_H

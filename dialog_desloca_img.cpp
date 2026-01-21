#include <QDoubleValidator>

#include <opencv2/highgui.hpp>

#include "dialog_desloca_img.h"
#include "ui_dialog_desloca_img.h"

extern bool Brazil;
extern Geometria_Imagem GeoProc,Geo_Tiff;

Dialog_Desloca_Img::Dialog_Desloca_Img(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Desloca_Img)
{
    ui->setupUi(this);

    QLocale     LocaleBr(QLocale::Portuguese,QLocale::Brazil);
    QLocale     LocaleEn(QLocale::English,QLocale::UnitedStates);

    ui->LatOriginal->setAlignment(Qt::AlignRight);
    ui->LonOriginal->setAlignment(Qt::AlignRight);
    ui->Nome_Tif->setAlignment(Qt::AlignRight);
    ui->Nome_Tif_Ref->setAlignment(Qt::AlignRight);
    ui->BoxLatNew->setAlignment(Qt::AlignRight);
    ui->BoxLonNew->setAlignment(Qt::AlignRight);

    ui->LatOriginal->setReadOnly(true);
    ui->LonOriginal->setReadOnly(true);
    ui->Nome_Tif->setReadOnly(true);
    ui->BoxLatNew->setReadOnly(true);
    ui->BoxLonNew->setReadOnly(true);

    if (Brazil) {
        ui->BoxLatNew->setLocale(LocaleBr);
        ui->BoxLonNew->setLocale(LocaleBr);
    } else {
        ui->BoxLatNew->setLocale(LocaleEn);
        ui->BoxLonNew->setLocale(LocaleEn);
    }

    ui->BoxLatNew->setValue(0.0);
    ui->BoxLonNew->setValue(0.0);
    NewLat = 0.0;
    NewLon = 0.0;

    ui->BoxLatNew->setMinimum(-90.0);
    ui->BoxLatNew->setMaximum(90.0);
    ui->BoxLonNew->setMinimum(-180.0);
    ui->BoxLonNew->setMaximum(180.0);

    ui->BoxLatNew->setEnabled(false);
    ui->BoxLonNew->setEnabled(false);

    ui->Bt_Digita->setEnabled(true);
    ui->Bt_Img->setEnabled(true);
    ui->Bt_Digita->setChecked(false);
    ui->Bt_Img->setChecked(false);
}

Dialog_Desloca_Img::~Dialog_Desloca_Img()
{
    GeoProc.DeslocaImagem = false;
    Geo_Tiff.DeslocaImagem = false;
    delete ui;
}

void Dialog_Desloca_Img::printFile(QString FileName) {

    ui->Nome_Tif->setText(FileName);
}

void Dialog_Desloca_Img::printFile_Ref(QString FileName) {

    ui->Nome_Tif_Ref->setText(FileName);
}


void Dialog_Desloca_Img::printOriginalCoords(Geometria_Imagem &GeoProc) {

    if (GeoProc.Pontos.length()) {
        QLocale Br;
        char    intStr[30];
        sprintf(intStr,"%s",Br.toString(GeoProc.Pontos.at(0).real(),'f',6).toStdString().c_str());
        ui->LatOriginal->setText(intStr);
        sprintf(intStr,"%s",Br.toString(GeoProc.Pontos.at(0).imag(),'f',6).toStdString().c_str());
        ui->LonOriginal->setText(intStr);

        ui->BoxLatNew->setEnabled(true);
        ui->BoxLonNew->setEnabled(true);
    } else {
        ui->LatOriginal->setText("Não definido");
        ui->LonOriginal->setText("Não definido");

        ui->BoxLatNew->setValue(0.0);
        ui->BoxLonNew->setValue(0.0);

        ui->BoxLatNew->setEnabled(false);
        ui->BoxLonNew->setEnabled(false);
    }
}

void Dialog_Desloca_Img::printDeslocaCoords(Geometria_Imagem &GeoProc) {
    if (GeoProc.Pontos.length()) {
        ui->BoxLatNew->setEnabled(true);
        ui->BoxLonNew->setEnabled(true);

        ui->BoxLatNew->setValue(GeoProc.Pontos.at(0).real());
        ui->BoxLonNew->setValue(GeoProc.Pontos.at(0).imag());
    } else {
        ui->BoxLatNew->setValue(0.0);
        ui->BoxLonNew->setValue(0.0);

        ui->BoxLatNew->setEnabled(false);
        ui->BoxLonNew->setEnabled(false);
    }

    NewLat = ui->BoxLatNew->value();
    NewLon = ui->BoxLonNew->value();
}

void Dialog_Desloca_Img::getLatLon(double &Lat, double &Lon){
    Lat = NewLat;
    Lon = NewLon;
}

void Dialog_Desloca_Img::setLocaleBr() {

}

void Dialog_Desloca_Img::on_BoxLatNew_editingFinished()
{
    NewLat = ui->BoxLatNew->value();
}


void Dialog_Desloca_Img::on_BoxLonNew_editingFinished()
{
    NewLon = ui->BoxLonNew->value();
}


void Dialog_Desloca_Img::on_Bt_Cancel_clicked()
{
    cv::destroyAllWindows();
    close();
    emit    Cancel();
}


void Dialog_Desloca_Img::on_Bt_Ok_clicked()
{
//    cv::destroyAllWindows();
    emit    Ok();
}


void Dialog_Desloca_Img::on_Bt_Digita_clicked()
{
    ui->Bt_Img->setChecked(false);
    ui->bBt_2Pontos->setChecked(false);
    emit Bt_Digita_Signal();
}



void Dialog_Desloca_Img::on_Bt_Img_clicked()
{
    ui->Bt_Digita->setChecked(false);
    ui->bBt_2Pontos->setChecked(false);
    emit Bt_Img_Signal();
}


void Dialog_Desloca_Img::on_bBt_2Pontos_clicked()
{
    ui->Bt_Digita->setChecked(false);
    ui->Bt_Img->setChecked(false);
    emit Bt_2Pontos_Signal();
}

void Dialog_Desloca_Img::setBoxReadOnly()
{
    ui->BoxLatNew->setReadOnly(true);
    ui->BoxLonNew->setReadOnly(true);
}

void Dialog_Desloca_Img::setBoxWrite()
{
    ui->BoxLatNew->setReadOnly(false);
    ui->BoxLonNew->setReadOnly(false);
}

void Dialog_Desloca_Img::setBoxValues(double Lat, double Lon) {
    ui->BoxLatNew->setValue(Lat);
    ui->BoxLonNew->setValue(Lon);
    NewLat = Lat;
    NewLon = Lon;
}

void Dialog_Desloca_Img::setDigita(void) {
    ui->Nome_Tif_Ref->setVisible(false);
    ui->label_6->setVisible(false);
}

void Dialog_Desloca_Img::setImagem(void) {
    ui->label_6->setVisible(true);
    ui->Nome_Tif_Ref->setVisible(true);
}

void Dialog_Desloca_Img::Posiciona_LATLON(int Pts) {
    switch (Pts)   {
        case 1:
            ui->LatOriginal->setGeometry(110,150,80,22);
            ui->LonOriginal->setGeometry(110,180,80,22);
            ui->LatOriginal_2->setVisible(false);
            ui->LonOriginal_2->setVisible(false);
            ui->label_7->setVisible(false);
            ui->label_8 ->setVisible(false);
            break;

        case 2:
            ui->LatOriginal->setGeometry(110,120,80,22);
            ui->LatOriginal->setGeometry(110,150,80,22);
            ui->LatOriginal_2->setGeometry(110,180,80,22);
            ui->LatOriginal_2->setGeometry(110,210,80,22);

            ui->LatOriginal_2->setVisible(true);
            ui->LonOriginal_2->setVisible(true);
            ui->label_7->setVisible(true);
            ui->label_8 ->setVisible(true);
    }
}

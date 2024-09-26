#include <QFileDialog>

#include <stdio.h>

#include "dialogpontos.h"
#include "ui_dialogpontos.h"

#include "geo_exif_xmp.h"

extern  Geometria_Imagem  GeoProc;

DialogPontos::DialogPontos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPontos)
{
    ui->setupUi(this);
}

DialogPontos::~DialogPontos()
{
    delete ui;
}

void DialogPontos::on_buttonBox_rejected()
{
    close();
//    destroy();
}

void DialogPontos::Print(QString Valor){
    ui->textBrowser->append(Valor);
}

void DialogPontos::clear(void){
    ui->textBrowser->clear();
}

void DialogPontos::on_buttonBox_accepted()
{
    QString FileName;

    if (GeoProc.Pontos.length() == 0) return;

    FileName = QFileDialog::getSaveFileName(this, tr("Arquivo CSV"),"",tr("CSV (*.csv)"));
    if (FileName.isNull()) return;

    // Create a new file
    FILE *fp;
    fp = fopen(FileName.toStdString().c_str(), "w");  // Try changing to "a"

    fprintf(fp,"id;Name;Lon;Lat;\n");

    int j;
    for (int i = 0 ; i < GeoProc.Pontos.length() ; i++) {
        j = i + 1;
        fprintf(fp,"%d;%d;%f;%f;\n",j,j,GeoProc.Pontos.at(i).imag(),GeoProc.Pontos.at(i).real());
    }

    fclose(fp);
}

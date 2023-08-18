#include "dialog_mescla_deslocando.h"
#include "ui_dialog_mescla_deslocando.h"

#include "mainwindow.h"

extern  Geometria_Imagem        GeoProc,Geo_Img,Geo_Tiff,Geo_Referencia;


Dialog_Mescla_Deslocando::Dialog_Mescla_Deslocando(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Mescla_Deslocando)
{
    ui->setupUi(this);

    Num_Files = 0;
    Imagem1 = 0;
    Imagem2 = 0;
    Processa = true;
    Angulo = 0.0;


    set_Angulo(Angulo);
}


Dialog_Mescla_Deslocando::~Dialog_Mescla_Deslocando()
{
    delete ui;
}

void Dialog_Mescla_Deslocando::print_NumFile(int Num) {
    char Str[30];

    sprintf(Str,"%d/%d",Num,Num_Files);
    ui->Num_Imagens->setText(Str);
}


void Dialog_Mescla_Deslocando::on_bt_Proximo_clicked()
{
  Imagem1++;
  Imagem2++;
  Angulo = ui->Angulo->text().toDouble();
  if (Imagem1 > Num_Files)  Processa = false;

  emit (Next());
}

int Dialog_Mescla_Deslocando::get_Posicao() {
    return(Imagem1);
}

void Dialog_Mescla_Deslocando::setNumFiles(int Num) {
    Num_Files = Num;
}

void Dialog_Mescla_Deslocando::set_Angulo(double Ang) {
    Angulo = Ang;
    ui->Angulo->setText(QString::number(Angulo));
}

double Dialog_Mescla_Deslocando::get_Angulo() {
    return (Angulo);
}

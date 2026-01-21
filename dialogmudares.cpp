#include <QIntValidator>
#include <QDoubleValidator>


#include "dialogmudares.h"
#include "ui_dialogmudares.h"

#include <geo_exif_xmp.h>

extern Geometria_Imagem Geo_Img;

DialogMudaRes::DialogMudaRes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMudaRes)
{
    ui->setupUi(this);

    ui->XRes_M->setValidator(new QIntValidator(100,100000,this));
    ui->YRes_M->setValidator(new QIntValidator(100,100000,this));
    ui->CM_PX_M->setValidator(new QDoubleValidator(1,500,2,this));

    print();

}

DialogMudaRes::~DialogMudaRes()
{
    delete ui;
}


void DialogMudaRes::print(void)
{
   QString TMP;
   TMP.setNum(Geo_Img.DimX);
   ui->XRes_O->setText(TMP);
   TMP.setNum(Geo_Img.DimY);
   ui->YRes_O->setText(TMP);
   TMP.setNum(Geo_Img.cmppx);
   ui->CM_PX_O->setText(TMP);
}


void DialogMudaRes::on_XRes_M_editingFinished()
{
    double Razao,Razao2,XX,ZZ,YY,A,B;
    int Y;
    QString TMP;

    XX = Geo_Img.DimX;
    YY = Geo_Img.DimY;
    Razao = XX / YY;

    ZZ = ui->XRes_M->text().toDouble();

    Razao2 = ZZ / XX;

    A = round(ZZ / Razao);

    Y = A;
    TMP.setNum(Y);
    ui->YRes_M->setText(TMP);

    ZZ = ui->CM_PX_O->text().toDouble();

    B = ZZ / Razao2;
    TMP.setNum(B);
    ui->CM_PX_M->setText(TMP);

}

void DialogMudaRes::on_YRes_M_editingFinished()
{
    double Razao,Razao2,XX,ZZ,YY,A,B;
    int Y;
    QString TMP;

    XX = Geo_Img.DimX;
    YY = Geo_Img.DimY;
    Razao = XX / YY;

    ZZ = ui->YRes_M->text().toDouble();

    Razao2 = ZZ / YY;

    A = round(ZZ * Razao);

    Y = A;
    TMP.setNum(Y);
    ui->XRes_M->setText(TMP);

    ZZ = ui->CM_PX_O->text().toDouble();

    B = ZZ / Razao2;
    TMP.setNum(B);
    ui->CM_PX_M->setText(TMP);

}

void DialogMudaRes::on_CM_PX_M_editingFinished()
{
    double Razao,XX,ZZ,YY,A,B;
    int Y;
    QString TMP;

    XX = ui->CM_PX_O->text().toDouble();
    YY = ui->CM_PX_M->text().toDouble();
    Razao = XX / YY;

    A = round(Razao * Geo_Img.DimX);
    B = round(Razao * Geo_Img.DimY);

    Y = A;
    TMP.setNum(Y);
    ui->XRes_M->setText(TMP);

    Y = B;
    TMP.setNum(Y);
    ui->YRes_M->setText(TMP);
}

void DialogMudaRes::on_buttonBox_accepted()
{
    this->setResult(ui->XRes_M->text().toInt());
}

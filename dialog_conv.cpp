#include <string.h>
#include <QString>
#include <QStringList>
//#include <QRegExpValidator>
#include <QRegularExpressionValidator>
#include <QLocale>

#include "dialog_conv.h"
#include "ui_dialog_conv.h"

#include "LatLong-UTM.h"
#include <GeographicLib/GeoCoords.hpp>

#define PRECISAO    6


Dialog_Conv::Dialog_Conv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Conv)
{
    ui->setupUi(this);


    ui->txt_LatLon->setWhatsThis("Coordenadas no formato Grau:Minuto:Segundo (D:M:S)");
    ui->txt_Zona_3->setWhatsThis("Zona UTM. Varia entre 1 e 60");
    ui->txt_Zona_4->setWhatsThis("Zona UTM. Varia entre 1 e 60");
    ui->txt_Latidude->setWhatsThis("Distância ao Equador medida em graus, podendo variar entre 0º e 90º para Norte(N) ou para Sul(S)");
    ui->txt_Longitude->setWhatsThis("Distância ao meridiano de Greenwich medida em graus, podendo variar entre 0º e 180º para Leste(E) ou para Oeste(W).");
    ui->txt_Easting_3->setWhatsThis("Distância em metros");
    ui->txt_Easting_4->setWhatsThis("Distância em metros");
    ui->txt_Norting_3->setWhatsThis("Distância em metros");
    ui->txt_Norting_4->setWhatsThis("Distância em metros");
    ui->txt_Hemisferio->setWhatsThis("Norte ou Sul");
    ui->radioButton_Dec->setWhatsThis("Seleciona edição de Lat e Lon decimais");
    ui->radioButton_GMS->setWhatsThis("Seleciona edição de Lat e Lon em Graus, minutos e segundos");

    ui->txt_MC1->setWhatsThis("Falta escrever");
    ui->txt_MC2->setWhatsThis("Falta escrever");

    QLocale loc = QLocale::system();
    QLocale brasil(QLocale::Portuguese);
    loc.setNumberOptions(brasil.numberOptions());
    QLocale::setDefault(loc);

//    QLocale curLocale(QLocale(QLocale::Portuguese,QLocale::Brazil));
//    QLocale::setDefault(curLocale);
//    QLocale::setNumberOptions(QLocale::DefaultNumberOptions);

}

Dialog_Conv::~Dialog_Conv()
{
    delete ui;
}

void Dialog_Conv::Inicializa(UTM_GEO &Val,GeographicLib::GeoCoords &Val_C)
{
    ui->radioButton_Dec->setChecked(true);
    ui->radioButton_GMS->setChecked(false);
    ui->txt_LatLon->setReadOnly(true);


    ui->txt_Latidude->setAlignment(Qt::AlignRight);
//    ui->txt_Latidude->setText(QString::number(Val.Lat));
    ui->txt_Longitude->setAlignment(Qt::AlignRight);
//    ui->txt_Longitude->setText(QString::number(Val.Lon));
    ui->txt_Zona_3->setAlignment(Qt::AlignRight);
//    ui->txt_Zona_3->setText(QString::number(Val.Zone));
    ui->txt_Easting_3->setAlignment(Qt::AlignRight);
//    ui->txt_Easting_3->setText(QString::number(Val.Easting));
    ui->txt_Norting_3->setAlignment(Qt::AlignRight);
//    ui->txt_Norting_3->setText(QString::number(Val.Northing));
    ui->txt_Hemisferio->setAlignment(Qt::AlignRight);
//    ui->txt_Hemisferio->setText(Val.Hemisferio);
    ui->txt_Zona_4->setAlignment(Qt::AlignRight);
//    ui->txt_Zona_4->setText(QString::number(Val.Zone));
    ui->txt_Easting_4->setAlignment(Qt::AlignRight);
//    ui->txt_Easting_4->setText(QString::number(Val.Easting));
    ui->txt_Norting_4->setAlignment(Qt::AlignRight);
//    ui->txt_Norting_4->setText(QString::number(Val.Northing));
    ui->txt_LatLon->setAlignment(Qt::AlignRight);
//    ui->txt_LatLon->setText(QString::fromStdString(Val_C.DMSRepresentation()));


    ui->txt_MC1->setAlignment(Qt::AlignRight);
    ui->txt_MC2->setAlignment(Qt::AlignRight);


//    QRegExpValidator*       Pos_Int_Val = new QRegExpValidator(QRegExp("[+]?\\d*"),this);
    QValidator*       Pos_Int_Val = new QRegularExpressionValidator(QRegularExpression("[+]?\\d*"),this);

    ui->txt_Zona_3->setValidator(Pos_Int_Val);
    ui->txt_Zona_4->setValidator(Pos_Int_Val);

    QValidator*       Int_Val = new QRegularExpressionValidator(QRegularExpression("[+-]?\\d*"),this);
    ui->txt_MC1->setValidator(Int_Val);
    ui->txt_MC2->setValidator(Int_Val);

    Valores = Val;
    Valores_C = Val_C;

    LatLonEdited();
}

void Dialog_Conv::LatLonEdited() {
    int     argc;
    char    **argv;
    int     Num_Elementos = 3;
    int     Tamanho = Num_Elementos + 1;

    argv = static_cast<char **>(malloc(Tamanho * sizeof(char *)));
    int i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
    }
    argv[Num_Elementos] = nullptr;


    strcpy(argv[0],"conv");
    strcpy(argv[1],ui->txt_Latidude->text().toStdString().c_str());
    strcpy(argv[2],ui->txt_Longitude->text().toStdString().c_str());


    Valores.Out = "";
    conv(Num_Elementos,argv,Valores);

    ui->txt_LatLon->setText(Valores.Out);
    ui->txt_Latidude->setText(QString::number(Valores.Lat,'f',PRECISAO));
    ui->txt_Longitude->setText(QString::number(Valores.Lon,'f',PRECISAO));
    ui->txt_Zona_3->setText(QString::number(Valores.Zone));
    ui->txt_Easting_3->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_3->setText(QString::number(Valores.Northing_Signed,'f',3));
    ui->txt_Hemisferio->setText(Valores.Hemisferio);
    ui->txt_Zona_4->setText(QString::number(Valores.Zone));
    ui->txt_Easting_4->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_4->setText(QString::number(Valores.Northing,'f',3));

    Valores.MC = (6 * Valores.Zone - 183);
    ui->txt_MC1->setText(QString::number(Valores.MC));
    ui->txt_MC2->setText(QString::number(Valores.MC));

}

void Dialog_Conv::on_txt_Latidude_editingFinished()
{
    double Val = ui->txt_Latidude->text().toDouble();

    /////////////////// Golpe
//    Val += 1e-6;
//    ui->txt_Latidude->setText(QString::number(Val,'f',PRECISAO));


    if ((Val > 90.0) || (Val < -90.0)) {
        Val = 0.0;
        ui->txt_Latidude->setText(QString::number(Val,'f',9));
        return;
    }

    LatLonEdited();
}

void Dialog_Conv::on_txt_Longitude_editingFinished()
{
    double Val = ui->txt_Longitude->text().toDouble();
    if ((Val > 180.0) || (Val < -180.0)) {
        Val = 0.0;
        ui->txt_Longitude->setText(QString::number(Val,'f',PRECISAO));
        return;
    }

    LatLonEdited();
}


void Dialog_Conv::Zona3Edited() {
    int     argc;
    char    **argv;
    int     Num_Elementos = 4;
    int     Tamanho = Num_Elementos + 1;

    argv = static_cast<char **>(malloc(Tamanho * sizeof(char *)));
    int i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
    }
    argv[Num_Elementos] = nullptr;


    strcpy(argv[0],"conv");
    strcpy(argv[1],ui->txt_Zona_3->text().toStdString().c_str());
    strcpy(argv[2],ui->txt_Easting_3->text().toStdString().c_str());
    strcpy(argv[3],ui->txt_Norting_3->text().toStdString().c_str());


    Valores.Out = "";
    conv(Num_Elementos,argv,Valores);

    ui->txt_LatLon->setText(Valores.Out);
    ui->txt_Latidude->setText(QString::number(Valores.Lat,'f',PRECISAO));
    ui->txt_Longitude->setText(QString::number(Valores.Lon,'f',PRECISAO));
    ui->txt_Zona_3->setText(QString::number(Valores.Zone));
    ui->txt_Easting_3->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_3->setText(QString::number(Valores.Northing_Signed,'f',3));
    ui->txt_Hemisferio->setText(Valores.Hemisferio);
    ui->txt_Zona_4->setText(QString::number(Valores.Zone));
    ui->txt_Easting_4->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_4->setText(QString::number(Valores.Northing,'f',3));

    Valores.MC = 6 * Valores.Zone - 183;

    ui->txt_MC1->setText(QString::number(Valores.MC));
    ui->txt_MC2->setText(QString::number(Valores.MC));
}

void Dialog_Conv::on_txt_Zona_3_editingFinished()
{
    int Val = ui->txt_Zona_3->text().toInt();
    if ((Val > 60) || (Val < 1)) {
        Val = 31;
        ui->txt_Zona_3->setText(QString::number(Val));
        return;
    }

    Zona3Edited();
}

void Dialog_Conv::on_txt_Easting_3_editingFinished()
{
    Zona3Edited();
}

void Dialog_Conv::on_txt_Norting_3_editingFinished()
{
    Zona3Edited();
}

void Dialog_Conv::Zona4Edited() {
    int     argc;
    char    **argv;
    int     Num_Elementos = 5;
    int     Tamanho = Num_Elementos + 1;

    argv = static_cast<char **>(malloc(Tamanho * sizeof(char *)));
    int i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
    }
    argv[Num_Elementos] = nullptr;


    strcpy(argv[0],"conv");
    strcpy(argv[1],ui->txt_Zona_4->text().toStdString().c_str());
    strcpy(argv[2],ui->txt_Hemisferio->text().toStdString().c_str());
    strcpy(argv[3],ui->txt_Easting_4->text().toStdString().c_str());
    strcpy(argv[4],ui->txt_Norting_4->text().toStdString().c_str());


    Valores.Out = "";
    conv(Num_Elementos,argv,Valores);

    ui->txt_LatLon->setText(Valores.Out);
    ui->txt_Latidude->setText(QString::number(Valores.Lat,'f',PRECISAO));
    ui->txt_Longitude->setText(QString::number(Valores.Lon,'f',PRECISAO));
    ui->txt_Zona_3->setText(QString::number(Valores.Zone));
    ui->txt_Easting_3->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_3->setText(QString::number(Valores.Northing_Signed,'f',3));
    ui->txt_Hemisferio->setText(Valores.Hemisferio);
    ui->txt_Zona_4->setText(QString::number(Valores.Zone));
    ui->txt_Easting_4->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_4->setText(QString::number(Valores.Northing,'f',3));

    Valores.MC = 6 * Valores.Zone - 183;

    ui->txt_MC1->setText(QString::number(Valores.MC));
    ui->txt_MC2->setText(QString::number(Valores.MC));
}


void Dialog_Conv::on_txt_Zona_4_editingFinished()
{
    int Val = ui->txt_Zona_4->text().toInt();
    if ((Val > 60) || (Val < 1)) {
        Val = 31;
        ui->txt_Zona_4->setText(QString::number(Val));
        return;
    }

    Zona4Edited();
}

void Dialog_Conv::on_txt_Hemisferio_editingFinished()
{
    Zona4Edited();
}

void Dialog_Conv::on_txt_Easting_4_editingFinished()
{
    Zona4Edited();
}

void Dialog_Conv::on_txt_Norting_4_editingFinished()
{
    Zona4Edited();
}

void Dialog_Conv::on_radioButton_GMS_clicked()
{
    ui->txt_Latidude->setReadOnly(true);
    ui->txt_Longitude->setReadOnly(true);
    ui->txt_LatLon->setReadOnly(false);
}

void Dialog_Conv::on_radioButton_Dec_clicked()
{
    ui->txt_LatLon->setReadOnly(true);
    ui->txt_Longitude->setReadOnly(false);
    ui->txt_Latidude->setReadOnly(false);
}

void Dialog_Conv::on_txt_LatLon_editingFinished()
{
    int     argc;
    char    **argv;
    int     Num_Elementos = 3;
    int     Tamanho = Num_Elementos + 1;

    argv = static_cast<char **>(malloc(Tamanho * sizeof(char *)));
    int i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
    }
    argv[Num_Elementos] = nullptr;

    QStringList TMP;
    TMP = ui->txt_LatLon->text().split(' ');

    strcpy(argv[0],"conv");
    strcpy(argv[1],TMP.at(0).toStdString().c_str());
    strcpy(argv[2],TMP.at(1).toStdString().c_str());


    Valores.Out = "";
    conv(Num_Elementos,argv,Valores);

    ui->txt_LatLon->setText(Valores.Out);
    ui->txt_Latidude->setText(QString::number(Valores.Lat,'f',PRECISAO));
    ui->txt_Longitude->setText(QString::number(Valores.Lon,'f',PRECISAO));
    ui->txt_Zona_3->setText(QString::number(Valores.Zone));
    ui->txt_Easting_3->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_3->setText(QString::number(Valores.Northing_Signed,'f',3));
    ui->txt_Hemisferio->setText(Valores.Hemisferio);
    ui->txt_Zona_4->setText(QString::number(Valores.Zone));
    ui->txt_Easting_4->setText(QString::number(Valores.Easting,'f',3));
    ui->txt_Norting_4->setText(QString::number(Valores.Northing,'f',3));
}

void Dialog_Conv::on_bt_Ok_clicked()
{
    destroy();
}

void Dialog_Conv::on_txt_MC1_editingFinished()
{
    int Val = ui->txt_MC1->text().toInt();
    if ((Val > 180) || (Val < -180)) {
        Val = 3;

        ui->txt_MC1->setText(QString::number(Val));
        return;
    }

    Valores.MC = Val;
    Valores.Zone = (183 + Val) / 6;
    ui->txt_Zona_3->setText(QString::number(Valores.Zone));

    Zona3Edited();
}


void Dialog_Conv::on_txt_MC2_editingFinished()
{
    int Val = ui->txt_MC2->text().toInt();
    if ((Val > 180) || (Val < -180)) {
        Val = 3;

        ui->txt_MC2->setText(QString::number(Val));
        return;
    }

    Valores.MC = Val;
    Valores.Zone = (183 - abs(Val)) / 6;
    ui->txt_Zona_4->setText(QString::number(Valores.Zone));

    Zona4Edited();
}


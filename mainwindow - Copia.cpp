#define _CRT_SECURE_NO_WARNINGS
// #define __MSW32__

#define USE_STD_NAMESPACE
#define CCUTIL_IMPORTS

#if __WORDSIZE == 64
   #define PRT_U64 "lu"
#else
   #define PRT_U64 "llu"
#endif

#define INSERE_THUMB

//#define GDAL

// #include <omp.h>

#include "Windows.h"
#include "WinUser.h"
#include "cstdlib"
#include "conio.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogexif.h"
#include "dialogpontos.h"
#include "dialogmanual.h"
#include "dialogmudares.h"

#include "exif.h"

#include "geo_exif_xmp.h"

#include "mythread.h"

#include <exception>
#include <GeographicLib/GeoCoords.hpp>

#include <QInputDialog>
#include <QProcess>
#include <QByteArray>
#include <QDir>
#include <QTableWidget>
#include <QtGui>
#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTime>
#include <QObject>
#include <QLocale>

#include <QCloseEvent>

#include <QAbstractItemView>

#include <QAction>
#include <QMenu>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/stitching.hpp>

#include <opencv2/highgui/highgui_c.h>

#include <opencv2/imgcodecs/legacy/constants_c.h>

#include <QTimer>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>

#include <QCoreApplication>

#include <stdio.h>
#include <conio.h>
#include "locale.h"
#include "stdlib.h"

#include <locale>
#include <sstream>

#include "cpl_string.h"
#include "gdal_version.h"
#include "gdal_priv.h"
#include "ogr_spatialref.h"
#include "commonutils.h"
#include "gdal_utils_priv.h"


#include "GDAL_HAM.h"
#include "translate.h"
#include "warp.h"

//#define USAR_IPL

using namespace std;
using namespace cv;

bool                ICE_Pressed,TIF_Pressed;

char                intStr[1000];

Geometria_Imagem    Geo,Geometria_ImgProc,GeoProc,Geo_ICE,Geo_Tiff,Geo_Img,Geo_Referencia;
std::string         Name_Imagem1 = "";
Plot_Coords         Coords;

#ifdef USAR_IPL
IplImage            *imageRead;
CvCapture           *capture;
#else
Mat                 imageRead;
VideoCapture        capture;
#endif

////////////////////////////////////////////////////////////////////
Ui::MainWindow* windowPrincipal;
DialogPontos*   uidialogPontos;

cv::Mat         ImgWindowPontos;

TiposDrones     Drones;
TiposCameras    Cameras;

int             Out_Y;


message_function MeFu;

/*
class message_function
{
public:
    void Print(Ui::MainWindow *ui,std::string Valor);
};
*/


char* th_sep_u64(uint64_t val, char* buf) {
   char tmpbuf[32]; //18'446'744'073'709'551'615 -> 26 chars
   int  nch, toffs, pos;
   pos   = 1;
   toffs = 31;
   nch   = snprintf(tmpbuf, 32, "%llu", val);
   nch  -- ;
   buf[toffs] = 0;

   for (; nch>=0; --nch) {
      toffs -- ;
      buf[toffs] = tmpbuf[nch];
      if ((0 == (pos % 3)) && (nch > 0)) {
         toffs -- ;
         buf[toffs] = '.'; //inject the separator
      }
      pos ++ ;
   }
   buf += toffs;
   return buf;
}


void MainWindow::Draw_Cel2()
{

    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B;
    std::vector<cv::Point>      fillContSingle;


    ImgWindowPontos.copyTo(GeoProc.image);
    if ((ui->checkBox_Linhas->isChecked()) || (ui->CheckBox_Poligono->isChecked()) ) {
        for (int k = 1 ; k < GeoProc.PImg.length() ; k++ ) {
            PosLat_A = GeoProc.PImg.at(k - 1).real();
            PosLon_A = GeoProc.PImg.at(k - 1).imag();
            PosLat_B = GeoProc.PImg.at(k).real();
            PosLon_B = GeoProc.PImg.at(k).imag();


            if (ui->CheckBox_Poligono->isChecked()) {
                fillContSingle.push_back(cv::Point(PosLat_A,PosLon_A));
                cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 4 );
            } else {
                cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 0, 255, 0), 4 );
            }
        }
    }


    if (GeoProc.Poligono && (GeoProc.PImg.length() > 2)) {
        PosLat_A = GeoProc.PImg.at(GeoProc.PImg.length() - 1).real();
        PosLon_A = GeoProc.PImg.at(GeoProc.PImg.length() - 1).imag();
        PosLat_B = GeoProc.PImg.at(0).real();
        PosLon_B = GeoProc.PImg.at(0).imag();
        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 4 );

        if (GeoProc.Preenche) {
            fillContSingle.push_back(cv::Point(PosLat_A,PosLon_A));
            std::vector<std::vector<cv::Point> > fillContAll;
            fillContAll.push_back(fillContSingle);
            cv::Mat overlay;
            double alpha = 0.2;
            GeoProc.image.copyTo(overlay);
            cv::fillPoly(overlay,fillContAll,cv::Scalar( 28, 255, 222));
            cv::addWeighted(overlay,alpha,GeoProc.image,1 - alpha,0,GeoProc.image);
        }
    }


    if (GeoProc.Circle) {
        for (int j = 0 ; j < GeoProc.Pontos.length() ; j++) {
            GeoProc.CcUTM.Reset(GeoProc.Pontos.at(j).real(),GeoProc.Pontos.at(j).imag());

            if ( (GeoProc.Proximidade == true) && (GeoProc.Proximidade_Num == j)) {
                cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(255, 0, 0), 3, cv::LINE_8, 0);
            } else {
                cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
            }

//            cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
        }
    }

    if (GeoProc.Pontos.length()) imshow(Name_Imagem1,GeoProc.image);

}

void message_function::Draw_Cel() {
    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B;
    bool                        Plot;

    std::vector<cv::Point>      fillContSingle;

    ImgWindowPontos.copyTo(GeoProc.image);
    if (GeoProc.Poligono || GeoProc.Line) {
        for (int k = 1 ; k < GeoProc.PImg.length() ; k++ ) {
            PosLat_A = GeoProc.PImg.at(k - 1).real();
            PosLon_A = GeoProc.PImg.at(k - 1).imag();
            PosLat_B = GeoProc.PImg.at(k).real();
            PosLon_B = GeoProc.PImg.at(k).imag();

            if (GeoProc.Poligono) {
                fillContSingle.push_back(cv::Point(PosLat_A,PosLon_A));
                cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 4 );
            } else {
                cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 0, 255, 0), 4 );
            }
        }
    }

    if (GeoProc.Poligono && (GeoProc.PImg.length() > 0)) {
        PosLat_A = GeoProc.PImg.at(GeoProc.PImg.length() - 1).real();
        PosLon_A = GeoProc.PImg.at(GeoProc.PImg.length() - 1).imag();
        PosLat_B = GeoProc.PImg.at(0).real();
        PosLon_B = GeoProc.PImg.at(0).imag();
        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 4 );

        if (GeoProc.Preenche) {
            fillContSingle.push_back(cv::Point(PosLat_A,PosLon_A));
            std::vector<std::vector<cv::Point> > fillContAll;
            fillContAll.push_back(fillContSingle);
            cv::Mat overlay;
            double alpha = 0.2;
            GeoProc.image.copyTo(overlay);
            cv::fillPoly(overlay,fillContAll,cv::Scalar( 28, 255, 222));
            cv::addWeighted(overlay,alpha,GeoProc.image,1 - alpha,0,GeoProc.image);
        }
    }


    if (GeoProc.Circle) {
        for (int j = 0 ; j < GeoProc.Pontos.length() ; j++) {
            GeoProc.CcUTM.Reset(GeoProc.Pontos.at(j).real(),GeoProc.Pontos.at(j).imag());
            if ((GeoProc.Selecionado == true) && (GeoProc.Proximidade_Num == j)) {
                cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(255, 0, 0), -1, cv::LINE_8, 0);
            }
            if ( (GeoProc.Proximidade == true) && (GeoProc.Proximidade_Num == j)) {
                    cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(255, 0, 0), 3, cv::LINE_8, 0);
            }
            else {
                cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
            }
        }
    }

    if (Coords.Tipo) {
        double      PassoX,PassoY;
        int         Num = 0,Num_Selected = 0;

        PassoX = fabs(GeoProc.AUTM.Longitude() - GeoProc.DUTM.Longitude()) / GeoProc.Iw;
        PassoY = fabs(GeoProc.AUTM.Latitude() - GeoProc.DUTM.Latitude()) / GeoProc.Ih;


        switch (Coords.Tipo) {
        case 1:
        case 3:
            Num = Coords.IMGS.Lat.length();
            Num_Selected = windowPrincipal->list_Files->selectedItems().length();
            if (Num_Selected == 0) {
                Coords.Tipo = 1;
            } else {
                Coords.Tipo = 3;
            }
            break;

        case 2:
        case 4:
            Num = Coords.PROC.Lat.length();
            Num_Selected = windowPrincipal->list_Files_Proc->selectedItems().length();
            if (Num_Selected == 0) {
                Coords.Tipo = 2;
            } else {
                Coords.Tipo = 4;
            }
            break;

        }

        for (int i = 0 ; i < Num ; i++) {
            std::complex<double> P;
            double               Cx,Cy;

            Plot = false;
            switch (Coords.Tipo) {
            case 1:
                P.real(Coords.IMGS.Lon.at(i));
                P.imag(Coords.IMGS.Lat.at(i));
                Plot = true;
                break;

            case 2:
                P.real(Coords.PROC.Lon.at(i));
                P.imag(Coords.PROC.Lat.at(i));
                Plot = true;
                break;

            case 3:
                for (int j = 0 ; j < Num_Selected ; j++) {
                    QString S1,S2;
                    S1 = Coords.IMGS.FileName.at(i);
                    S2 = windowPrincipal->list_Files->selectedItems().at(j)->text();
                    if (S1.contains(S2)) {
                        Plot = true;
                        P.real(Coords.IMGS.Lon.at(i));
                        P.imag(Coords.IMGS.Lat.at(i));
                    }
                }
                break;

            case 4:
                for (int j = 0 ; j < Num_Selected ; j++) {
                    QString S1,S2;
                    S1 = Coords.PROC.FileName.at(i);
                    S2 = windowPrincipal->list_Files_Proc->selectedItems().at(j)->text();
                    if (S1.contains(S2)) {
                        Plot = true;
                        P.real(Coords.PROC.Lon.at(i));
                        P.imag(Coords.PROC.Lat.at(i));
                    }
                }
                break;
            }


            Cx = (P.real() - GeoProc.AUTM.Longitude()) / PassoX;
            Cy = (GeoProc.AUTM.Latitude() - P.imag()) / PassoY;

            if ( (Cx < GeoProc.image.cols) && (Cy < GeoProc.image.rows) && (Cx > 0) && (Cy > 0) && (Plot == true) ) {

                if (i == 0) {// Primeiro ponto -> Verde
                    cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo,cv::Scalar(0, 255, 0), -1, cv::LINE_8, 0);
                } else if (i == (Num - 1)) {// Último ponto -> Vermelho
                    cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo,cv::Scalar(0, 0, 255), -1, cv::LINE_8, 0);
                } else {// Pontos intermediários -> Laranja
                    cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo,cv::Scalar(0, 165, 255), -1, cv::LINE_8, 0);
                }
            }
        }

    }

    if (Name_Imagem1 != "") imshow(Name_Imagem1,GeoProc.image);
}


void message_function::Print(Ui::MainWindow* ui,std::string Valor)
{
    ui->txt_Output->append(QString::fromUtf8(Valor.c_str()));
}

void message_function::Atualiza_Raio_Marcacao_Coordenadas(int Raio) {
    Raio_Circulo = Raio;
}

int message_function::Valor_Raio_Marcacao_Coordenadas(void) {
    return (Raio_Circulo);
}

void message_function::Table1(Ui::MainWindow* ui, int Row, bool Insert)
{
    if (Insert) {
        QLocale Br;
        QString S1,S2,S3;
        S1 = Br.toString(GeoProc.CcUTM.Latitude(),'f',6);
        S2 = Br.toString(GeoProc.CcUTM.Longitude(),'f',6);

        ui->tablePontos->insertRow( ui->tablePontos->rowCount() );

        ui->tablePontos->setItem(Row - 1,0,new QTableWidgetItem());
        ui->tablePontos->setItem(Row - 1,1,new QTableWidgetItem());
        ui->tablePontos->setItem(Row - 1,2,new QTableWidgetItem());

        QTableWidgetItem* Item;

        // Latitude
        Item = ui->tablePontos->item(Row - 1,0);
        Item->setTextAlignment(4);
        Item->setText(S1);

        //Longitude
        Item = ui->tablePontos->item(Row - 1,1);
        Item->setTextAlignment(4);
        Item->setText(S2);
        ui->tablePontos->setItem(Row - 1,0,Item);

        Draw_Cel();

        if (Row > 1) {
            float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B,Dist;
            GeographicLib::GeoCoords    A,B;


            PosLat_A = GeoProc.Pontos.at(GeoProc.PImg.length() - 2).real();
            PosLon_A = GeoProc.Pontos.at(GeoProc.PImg.length() - 2).imag();
            PosLat_B = GeoProc.Pontos.at(GeoProc.PImg.length() - 1).real();
            PosLon_B = GeoProc.Pontos.at(GeoProc.PImg.length() - 1).imag();

            A.Reset(PosLat_A,PosLon_A);
            B.Reset(PosLat_B,PosLon_B);

            Dist = sqrt(pow((A.Northing()-B.Northing()),2) + pow(A.Easting()-B.Easting(),2));

            S3 = Br.toString(Dist,'f',2);
            Item = ui->tablePontos->item(Row - 1,2);
            Item->setTextAlignment(4);
            Item->setText(S3);
            ui->tablePontos->setItem(Row - 1,2,Item);
        }
    } else {
        ui->tablePontos->removeRow(Row);
        Draw_Cel();
    }

    ui->tablePontos->horizontalHeader()->resizeSections(QHeaderView::Stretch);

    // Calcula distancia
    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B,Dist;
    GeographicLib::GeoCoords    A,B;
    double                      Area,x0,y0,x1,y1;


    QLocale Br;
    QString S1;

    Dist = 0;

    // Apenas um ponto
    if (GeoProc.PImg.length() < 2) {
//        S1 = Br.toString(Dist,'f',2);
//        ui->txt_Dist->setText(S1 + " metros");
        ui->txt_Dist->setText("");
        ui->lineEdit_Area->setText("");
        return;
    }


    int i = 2;
    for ( ; i <= (GeoProc.PImg.length()) ; i ++) {
        PosLat_A = GeoProc.Pontos.at(i - 2).real();
        PosLon_A = GeoProc.Pontos.at(i - 2).imag();
        PosLat_B = GeoProc.Pontos.at(i - 1).real();
        PosLon_B = GeoProc.Pontos.at(i - 1).imag();

        A.Reset(PosLat_A,PosLon_A);
        B.Reset(PosLat_B,PosLon_B);

        x0 = A.Easting();
        y0 = A.Northing();
        x1 = B.Easting();
        y1 = B.Northing();
        Dist = Dist + sqrt(pow(y1 - y0,2) + pow(x1-x0,2));
        Area = Area + (x0*y1 - y0*x1);


        PosLat_A = GeoProc.PImg.at(i - 2).real();
        PosLon_A = GeoProc.PImg.at(i - 2).imag();
        PosLat_B = GeoProc.PImg.at(i - 1).real();
        PosLon_B = GeoProc.PImg.at(i - 1).imag();

        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 1 );


        /* // Na tabela está armazenando o número no formato BR(com virgula)
        Item = ui->tablePontos->item(i - 1,2);
        S1 = Item->text();
        Dist = Dist +  S1.toFloat();
        */
    }

    if (GeoProc.Poligono && (GeoProc.PImg.length() > 2)) {
        PosLat_A = GeoProc.Pontos.at(i - 2).real();
        PosLon_A = GeoProc.Pontos.at(i - 2).imag();
        PosLat_B = GeoProc.Pontos.at(0).real();
        PosLon_B = GeoProc.Pontos.at(0).imag();

        A.Reset(PosLat_A,PosLon_A);
        B.Reset(PosLat_B,PosLon_B);

        x0 = A.Easting();
        y0 = A.Northing();
        x1 = B.Easting();
        y1 = B.Northing();
        Dist = Dist + sqrt(pow(y1 - y0,2) + pow(x1-x0,2));
        Area = Area + (x0*y1 - y0*x1);

        Area = fabs(Area) / 2;
   }

    S1 = Br.toString(Dist,'f',2);
    ui->txt_Dist->setText(S1 + " m");

    // Menos de três pontos não tem área
    if (GeoProc.PImg.length() < 3) {
        ui->lineEdit_Area->setText("");
    } else {
        S1 = Br.toString(Area,'f',1);
        ui->lineEdit_Area->setText(S1 + " m2");
    }
}

void message_function::Gera_Retangulo(Ui::MainWindow* ui) {
    int                     Xmin,Ymin,Xmax,Ymax;
    std::complex<double>    Ponto,P;
    double                  PosLat,PosLon;
    QList< std::complex<double> >   Pontos_TMP;
    QList< std::complex<int> >      PImg_TMP;


    if (GeoProc.PImg.length() != 2) return;

    Xmin = 1e8;
    Ymin = 1e8;
    Xmax = -1e8;
    Ymax = -1e8;

    for (int i = 0 ; i < GeoProc.PImg.length() ; i ++ ) {
        if (Xmin > GeoProc.PImg.at(i).real()) Xmin = GeoProc.PImg.at(i).real();
        if (Xmax < GeoProc.PImg.at(i).real()) Xmax = GeoProc.PImg.at(i).real();
        if (Ymin > GeoProc.PImg.at(i).imag()) Ymin = GeoProc.PImg.at(i).imag();
        if (Ymax < GeoProc.PImg.at(i).imag()) Ymax = GeoProc.PImg.at(i).imag();
    }

    Ponto.imag(Ymin);
    Ponto.real(Xmax);
    GeoProc.PImg.insert(1,Ponto);
    PosLat = GeoProc.AUTM.Northing() + Ymin * GeoProc.mppy;
    PosLon = GeoProc.AUTM.Easting() + Xmax * GeoProc.mppx;  // Eixo x é longitude
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);
    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());
    GeoProc.Pontos.insert(1,P);

    Ponto.imag(Ymax);
    Ponto.real(Xmin);
    GeoProc.PImg.push_back(Ponto);
    PosLat = GeoProc.AUTM.Northing() + Ymax * GeoProc.mppy;
    PosLon = GeoProc.AUTM.Easting() + Xmin * GeoProc.mppx;  // Eixo x é longitude
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);
    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());
    GeoProc.Pontos.push_back(P);

    PreencheTable(ui);
}


void message_function::Atualiza_Lista_Pontos_Apos_Move(Ui::MainWindow *ui) {
    double                  PosLat,PosLon;
    int                     x,y;
    std::complex<double>    P;
    std::complex<int>       PImg;
    QList< std::complex<double> >   Pontos_TMP;
    QList< std::complex<int> >      PImg_TMP;


////////////////////////////
// Calcula nova Lat e Lon do ponto movido
    x = GeoProc.PImg.at(GeoProc.Proximidade_Num).real();
    y = GeoProc.PImg.at(GeoProc.Proximidade_Num).imag();
    PosLat = GeoProc.AUTM.Northing() + y * GeoProc.mppy;
    PosLon = GeoProc.AUTM.Easting() + x * GeoProc.mppx;  // Eixo x é longitude
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);

/////////////////////////////
// Substitui a nova coordenada no lugar da anterior
    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());
    PImg.real(x);
    PImg.imag(y);
    GeoProc.Pontos.replace(GeoProc.Proximidade_Num,P);
//    GeoProc.PImg.replace(GeoProc.Proximidade_Num,PImg);

    PreencheTable(ui);
}


void message_function::PreencheTable(Ui::MainWindow* ui) {
    double                  PosLat,PosLon;
    int                     x,y;
    std::complex<double>    P;
    std::complex<int>       PImg;
    QList< std::complex<double> >   Pontos_TMP;
    QList< std::complex<int> >      PImg_TMP;


/////////////////////////////
// Copia pontos para lista temporária
    for ( int i = 0 ; i < GeoProc.Pontos.length(); i++) {
        Pontos_TMP.push_back(GeoProc.Pontos.at(i));
        PImg_TMP.push_back(GeoProc.PImg.at(i));
    }

/////////////////////////////
// Limpa a tabela
    GeoProc.Pontos.clear();
    GeoProc.PImg.clear();
    uidialogPontos->clear();
    ui->tablePontos->setRowCount(0);
//    Calc_Dist();
    ui->lineEdit_Area->setText("");

//////////////////////////////
// Para o Dialog Pontos
    QString S1,S2;
    QLocale Br;

    S1 = Br.toString(GeoProc.CcUTM.Latitude(),'f',6);
    S2 = Br.toString(GeoProc.CcUTM.Longitude(),'f',6);
    sprintf(intStr,"Coordenada (%s ; %s) (%s)",S1.toStdString().c_str(),S2.toStdString().c_str(),GeoProc.CcUTM.DMSRepresentation().c_str());


/////////////////////////////
// Reescreve a tabela
    for ( int i = 0 ; i < Pontos_TMP.length() ; i++) {
        GeoProc.Pontos.push_back(Pontos_TMP.at(i));
        GeoProc.PImg.push_back(PImg_TMP.at(i));
        x = GeoProc.PImg.at(i).real();
        y = GeoProc.PImg.at(i).imag();
        PosLat = GeoProc.AUTM.Northing() + y * GeoProc.mppy;
        PosLon = GeoProc.AUTM.Easting() + x * GeoProc.mppx;  // Eixo x é longitude
        GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);

        S1 = Br.toString(GeoProc.CcUTM.Latitude(),'f',6);
        S2 = Br.toString(GeoProc.CcUTM.Longitude(),'f',6);
        sprintf(intStr,"Coordenada (%s ; %s) (%s)",S1.toStdString().c_str(),S2.toStdString().c_str(),GeoProc.CcUTM.DMSRepresentation().c_str());
        uidialogPontos->Print(intStr);

        Table1(windowPrincipal,GeoProc.Pontos.count(),true);
     }
}


void MainWindow::imprimeFimKMZ(void)
{
    qint64  Elapsed;

    Elapsed = timer2.elapsed();
    sprintf(intStr," KMZ gerado em %3.3f segundos.",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


//    QThread::msleep(5000);

    AcochambracaoKMZ = true;
    Lista_Files_Result();
    AcochambracaoKMZ = false;

    QString Output;
    Size_GeoTiff(mThread1.NameKMZ,false,Output);
    ui->txt_Output->append(Output);

}

void MainWindow::imprimeFimMerge(void)
{
    qint64  Elapsed;

    Elapsed = timer2.elapsed();
    sprintf(intStr," Merge gerado em %3.3f segundos.",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    if (ui->checkBox_KMZ->isChecked()) {
        QString OutTxt;
        Sleep(2000);
        timer2.restart();
        QString NameKMZ;

        NameKMZ = Imagem_Composta.Complete_Name.path() + "/" + Imagem_Composta.Complete_Name.completeBaseName() + ".kmz";

        OutTxt = "\n Iniciando geração de [" + NameKMZ + "]";
        ui->txt_Output->append(OutTxt);

        mThread1.NameTIF = mThread_Merge.NameTIF;
        mThread1.NameKMZ = NameKMZ;

        mThread1.start();
    }

    Lista_Files_Imgs();
    Lista_Files_Proc();
    Lista_Files_Result();
}

void MainWindow::updateTxt(void)
{
    QApplication::processEvents();
//    QByteArray data = myProcess->readAllStandardOutput();
//    ui->txt_Output->append(QString(data));
//    QApplication::processEvents();
}


void MainWindow::Monta_Tabela_Inicial(void){
    Out_Y = 500; //845
    ui->tablePontos->setColumnCount(3);

    const QStringList Labels = {"Latitude","Longitude","Distância"};
    ui->tablePontos->setHorizontalHeaderLabels(Labels);
    ui->tablePontos->setStyleSheet("QHeaderView::section {background-color:lightgray}");

    ui->tablePontos->horizontalHeader()->resizeSections(QHeaderView::Stretch);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    windowPrincipal = ui;

    ICE_Pressed = false;
    TIF_Pressed = false;

    ui->list_Files_Proc->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->list_Files_Result->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->list_Files->setSelectionMode(QAbstractItemView::MultiSelection);

    myProcess = new QProcess(this);


    MeFu.Atualiza_Raio_Marcacao_Coordenadas(30);

    QObject::connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateTxt()),Qt::DirectConnection);
//    QObject::connect(myProcess, SIGNAL(readyRead()), this, SLOT(updateTxt()),Qt::DirectConnection);

    connect(&mThread1,SIGNAL(finished()),this,SLOT(imprimeFimKMZ()),Qt::DirectConnection);
    connect(&mThread_Merge,SIGNAL(finished()),this,SLOT(imprimeFimMerge()),Qt::DirectConnection);

//    connect(&mThread1,SIGNAL(FimKMZ()),this,SLOT(Lista_Files_Result()),Qt::DirectConnection);

    //    iconCheckpix.load(":/imgH/checkicon3.png");
    //    iconCheck.addPixmap(iconCheckpix);
        iconCheckpix.load(":/imgH/checkicon3.png");
        iconCheckpixOff.load(":/imgH/checkicon2.png");
        iconCheck.addPixmap(QPixmap(":/imgH/checkicon3.png"),QIcon::Normal);
        iconCheck.addPixmap(QPixmap(":/imgH/checkicon2.png"),QIcon::Disabled);

    READCOUNT = 0;

    QStringList Labels;
    Labels << "Arquivo" << "Size";

    this->setWindowTitle("Geo 0.652");

    QLocale::setDefault(QLocale::Portuguese);
//    QLocale::setDefault(QLocale::English);

    // Ajusta parâmetos da tabela de pontos

    Monta_Tabela_Inicial();


    //////////////////////////////////// Esconde os comandos para a versão de produção
    ui->bt_Mostar_Imagem->setEnabled(false);
    ui->Selected_IMG->setReadOnly(true);
    ui->Selected_IMG->setAlignment(Qt::AlignRight);
    ui->Selected_IMG->setVisible(false);
    ui->Selected_PROC->setReadOnly(true);
    ui->Selected_PROC->setAlignment(Qt::AlignRight);
    ui->Selected_PROC->setVisible(false);
    ui->Selected_RESULT->setReadOnly(true);
    ui->Selected_RESULT->setAlignment(Qt::AlignRight);
    ui->Selected_RESULT->setVisible(false);

    ui->actionGDAL->setChecked(false);
    ui->actionInterno->setChecked(true);


    ui->progressBar_Mesclar->setMinimum(0);
    ui->progressBar_Mesclar->setMaximum(100);
    ui->progressBar_Mesclar->setValue(0);
    ui->progressBar_Mesclar->setEnabled(false);
    ui->progressBar_Mesclar->setVisible(true);

    ///////////////Processamento
    ui->progressBar1->setMinimum(0);
    ui->progressBar1->setMaximum(100);
    ui->progressBar1->setValue(0);
    ui->progressBar1->setEnabled(false);

    /////////////// Listas de arquivo
    ui->progressBar_Files->setMinimum(0);
    ui->progressBar_Files->setMaximum(100);
    ui->progressBar_Files->setValue(0);
    ui->progressBar_Files->setVisible(false);

    ui->progressBar_Proc->setMinimum(0);
    ui->progressBar_Proc->setMaximum(100);
    ui->progressBar_Proc->setValue(0);
    ui->progressBar_Proc->setVisible(false);

    ui->progressBar_Result->setMinimum(0);
    ui->progressBar_Result->setMaximum(100);
    ui->progressBar_Result->setValue(0);
    ui->progressBar_Result->setVisible(false);

    ui->actionHarley_2->setVisible(false);
    ui->actionMiranda_2->setVisible(false);
    ui->menuAlgoritmo->setHidden(true);

    // ui->menuBar->setHidden(true);
    ui->btOpenFile->setHidden(true);
    ui->bt_Teste->setHidden(false);
    ui->btCmd->setHidden(true);

    ui->checkBox_JPG->setVisible(false);
    ui->checkBox_GTiff->setVisible(false);
    ui->checkBox_KMZ->setVisible(false);
    ui->bt_EXIF_XMP_List->setVisible(false);
    ui->bt_DialogPontos->setVisible(false);

    ui->bt_Lista_Arquivos->setVisible(false);
    ui->txt_Lista_Arquivos->setVisible(false);
    ui->radioButton_Merge->setVisible(false);
    ui->radioButton_Stitch->setVisible(false);
    ui->bt_MudaResolucao->setVisible(false);

    ui->pushButton->setEnabled(false);
//    ui->checkBox_Linhas->setEnabled(false);

    ui->txt_Mesclar_Name->setVisible(false);

//    ui->txt_Nome_Imagem->setEnabled(false);

    ui->actionThread_2->setEnabled(true);
    ui->actionThread_2->setCheckable(true);
    ui->actionThread_2->setChecked(false);

    ui->bt_DialogPontos->setEnabled(false);
    ui->actionPontos->setEnabled(false);
    ui->bt_limpaProc->setEnabled(false);
    ui->bt_EXIF_XMP_List->setEnabled(false);
    ui->actionEXIF_XMP->setEnabled(false);

    ui->txt_Reducao->setAlignment(Qt::AlignRight);
    Informacao.Reducao = 3;
    ui->txt_Reducao->setText(QString::number(Informacao.Reducao));

    ui->txt_Escala->setAlignment(Qt::AlignRight);
    Informacao.Escala1 = 0.25;
    ui->txt_Escala->setText(QString::number(Informacao.Escala1 * 100));

    ui->bt_Reduzir->setEnabled(false);

    ui->bt_ProcessarImgs->setEnabled(false);

    ui->txt_Path->setVisible(false);

    ui->bt_Cancelar_Proc->setEnabled(false);
    CancelarProc = false;

    ui->bt_Dir_Proc->setEnabled(false);

    ui->checkBox_GTiff->setChecked(true);
    ui->checkBox_JPG->setChecked(false);
    ui->checkBox_KMZ->setChecked(false);
    ui->checkBox_GTiff->setEnabled(true);
    ui->checkBox_JPG->setEnabled(false);
    ui->checkBox_KMZ->setEnabled(true);

    Informacao.Diretorio_EXE = QDir::currentPath();
    Informacao2.Diretorio_EXE = QDir::currentPath();
    Informacao.Diretorio_Img_Original = "";
    Informacao2.Diretorio_Img_Original = "";

    ui->bt_Lista_Arquivos->setEnabled(false);
    ui->bt_Mesclar->setEnabled(false);

    ui->radioButton_Merge->setEnabled(true);
    ui->radioButton_Stitch->setEnabled(true);
    ui->radioButton_Merge->setChecked(true);

    ui->menuMadidas->setEnabled(false);

    ui->txt_Alt_Voo->setAlignment(Qt::AlignRight);
    ui->txt_Dist->setAlignment(Qt::AlignRight);
    ui->lineEdit_Area->setAlignment(Qt::AlignRight);

    uidialogPontos = NULL;

    ui->actionEspecificar_2->setChecked(false);
    ui->actionValor->setEnabled(false);
    Geo.Alt_Default = 30.0;
    ui->actionValor->setText("Valor: 30.0 m");

    ui->label_Perimetro->setText("Caminho:");
    ui->label->setVisible(false);
    ui->lineEdit_Area->setVisible(false);

    ui->actionCortarTIFF->setEnabled(false);

    on_actionThread_2_triggered();
    on_actionThumbnail_triggered();
    on_actionPreenche_Pol_gono_triggered();
    on_actionBranco_triggered();

    on_actionDrone_triggered();

    on_actionInterno_triggered();

    on_actionBordas_na_imagens_processadas_triggered();

    on_actionDesativado_triggered();


    ui->actionUsar->setChecked(false);
    ui->actionUsar->setEnabled(false);

    if (ui->actionPortugu_s->isChecked()) on_actionPortugu_s_triggered();
    else on_actionIngl_s_triggered();

    if (ui->actionMiranda_2->isChecked()) {
        QProcess process;
        QString out,err;
        QDir("c:\\ProgramData\\Anaconda3\\Library\\bin");

        process.start("gdalwarp");
        process.waitForFinished(-1); // will wait forever until finished
        out = process.readAllStandardOutput();
        err = process.readAllStandardError();
        process.close();
        if ((out == "") && (err == "")) {
            ui->txt_Output->append("Não encontrado gdalwarp.exe !");
        } else {
    //        ui->txt_Output->append("\n");
        }

        process.start("gdal_translate");
        process.waitForFinished(-1); // will wait forever until finished
        out = process.readAllStandardOutput();
        err = process.readAllStandardError();
        process.close();
        if ((out == "") && (err == "")) {
            ui->txt_Output->append("Não encontrado gdal_translate.exe ! \n");
        } else {
    //        ui->txt_Output->append("\n");
        }

        QFile   PyGeoreff("georref.py");
        if (!PyGeoreff.exists()) {
            ui->txt_Output->append("Não encontrado georref.py !");
                Informacao.PyGeoRef = "";
        } else {
            Informacao.PyGeoRef = Informacao2.Diretorio_EXE + "/georref.py";
        }
    }

    ui->txt_Output->append("\n " + Informacao.Diretorio_EXE);

    thumb_Earth.load(":/imgH/thumb_Google_Earth.png");


}


MainWindow::~MainWindow()
{
    cv::destroyAllWindows();

//    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    delete ui;
}


void MainWindow::on_bt_Lista_Arquivos_clicked()
{
    QString  Name,Complete;
    bool     Existente;


    Complete = QFileDialog::getSaveFileName(this,"TXT file",Informacao2.DiretorioProc,tr("Arquivo (*.txt)"),nullptr,QFileDialog::DontConfirmOverwrite);
    QFileInfo CompleteName(Complete);

    if (Complete.isNull()) {
        return;
    }

    Name = CompleteName.fileName();
//    Informacao2.DiretorioProc = CompleteName.path();
    Existente = CompleteName.exists();

    Informacao2.TXT_File = Name;

    if (!Existente) {
        ui->txt_Output->append("Arquivo txt sendo criado.");

        QDir directory(Informacao2.DiretorioProc);
        QStringList Filters;
        Filters << "*.vrt" << "*.tif";
        QFileInfoList infoList = directory.entryInfoList(Filters,QDir::Files | QDir::NoDotAndDotDot);
        Informacao2.ListadeArquivos = infoList;

        ui->txt_Dir_Proc->setText(Informacao2.DiretorioProc);
        sprintf(intStr,"%i imagens", Informacao2.ListadeArquivos.length());
        ui->txt_NumEntries_Proc->setAlignment(Qt::AlignRight);
        ui->txt_NumEntries_Proc->setText(intStr);

        QFile file(Complete);
              if(file.open(QIODevice::WriteOnly | QIODevice::Text))
              {
                  // We're going to streaming text to the file
                  QTextStream stream(&file);

                  for (int i = 0 ; i < infoList.length() ; i++){
                      QFileInfo Info = infoList.at(i);
                      QString Arquivo = Info.fileName();
                      ui->txt_Output->append(Arquivo);
                      stream << Arquivo  <<'\n';
                   }
                  file.close();
                  ui->txt_Output->append("Writing finished");
              }
    }

    ui->txt_Lista_Arquivos ->setText(Complete);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_mouse_OpenFile(int event, int x, int y, int, void *) {
    sprintf(intStr,"Posiçao ( %d , %d )",x,y);
    displayStatusBar("Imagem Original",intStr,0);
}

void MainWindow::on_mouse_Original_JPG(int event, int x, int y, int flags, void *) {
    std::complex<double>    P,PImg;
    char                    Str[100];

//    win32api.SetCursor(win32api.LoadCursor(0, win32con.IDC_SIZEALL));


    sprintf(intStr,"Posiçao ( %d , %d )",x,y);
    displayStatusBar("Imagem ICE",intStr,0);

    PImg.real(x);
    PImg.imag(y);

    if ((event == EVENT_LBUTTONDOWN) && (ICE_Pressed == false)) {
        switch (flags) {
        case  (EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON): // Botão esquerdo Mouse clic
            Geo_ICE.PImg.push_back(PImg);
            ICE_Pressed = true;
            MeFu.Print(windowPrincipal,intStr);
//            uidialogPontos->Print(intStr);
            cv::circle(Geo_ICE.image,cv::Point(x,y),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
            imshow("Imagem ICE",Geo_ICE.image);
            break;

        case (EVENT_FLAG_ALTKEY + EVENT_FLAG_LBUTTON):  // Botão esquerdo + ALT
            if (!Geo_ICE.PImg.isEmpty()) {
                MeFu.Print(windowPrincipal," Ponto retirado\n");
                Geo_ICE.PImg.removeLast();
//                uidialogPontos->clear();
                imageRead.copyTo(Geo_ICE.image);
                for (int j = 0 ; j < Geo_ICE.PImg.length() ; j++) {
/*
                    Geo_ICE.CcUTM.Reset(Geo_ICE.Pontos.at(j).real(),Geo_ICE.Pontos.at(j).imag());
                    sprintf(Str,"Coordenada (%4.4f,%4.4f) (%s)",Geo_ICE.Pontos.at(j).real(),
                                                                Geo_ICE.Pontos.at(j).imag(),
                                                                Geo_ICE.CcUTM.DMSRepresentation().c_str());
*/
//                    uidialogPontos->Print(Str);
                    cv::circle(Geo_ICE.image,cv::Point(Geo_ICE.PImg.at(j).real(),Geo_ICE.PImg.at(j).imag()),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
                }
                imshow("Imagem ICE",Geo_ICE.image);
            } else {
                MeFu.Print(windowPrincipal," Lista vazia.\n");
            }
            break;
/*
        case (EVENT_FLAG_ALTKEY + EVENT_FLAG_LBUTTON):
            MeFu.Print(windowPrincipal," ALT pressionado.\n");
            break;
*/
        }
    }
}

void MainWindow::on_mouse_Original_TIF(int event, int x, int y, int flags, void *) {
    double                  PosLat,PosLon;
    std::complex<double>    P,PImg;
    char                    Str[100];

    PosLat = Geo_Tiff.AUTM.Northing() + y * Geo_Tiff.mppy ;
    PosLon = Geo_Tiff.AUTM.Easting() + x * Geo_Tiff.mppx;  // Eixo x é longitude
    Geo_Tiff.CcUTM.Reset(Geo_Tiff.AUTM.Zone(),Geo_Tiff.AUTM.Northp(),PosLon,PosLat);

    P.real(Geo_Tiff.CcUTM.Latitude());
    P.imag(Geo_Tiff.CcUTM.Longitude());
    PImg.real(x);
    PImg.imag(y);

    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%8.4f,%8.4f) (%s)\n",x,y,P.real(),P.imag(),Geo_Tiff.CcUTM.DMSRepresentation().c_str());
    displayStatusBar("Imagem TIF",intStr,0);

    if ((event == EVENT_LBUTTONDOWN) && (ICE_Pressed == true)) {
        switch (flags) {
        case  (EVENT_FLAG_LBUTTON + EVENT_FLAG_CTRLKEY): // Botão esquerdo Mouse clic
            Geo_Tiff.Pontos.push_back(P);
            ICE_Pressed = false;
            TIF_Pressed = true;
            MeFu.Print(windowPrincipal,intStr);
//            uidialogPontos->Print(intStr);
            cv::circle(Geo_Tiff.image,cv::Point(x,y),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
            imshow("Imagem TIF",Geo_Tiff.image);
            break;

        }
    }
}

void MainWindow::on_mouse_Referencia_TIF(int event, int x, int y, int flags, void *) {
    double                  PosLat,PosLon;
    std::complex<double>    P,PImg;
    char                    Str[100];

    PosLat = Geo_Referencia.AUTM.Northing() + y * Geo_Referencia.mppy ;
    PosLon = Geo_Referencia.AUTM.Easting() + x * Geo_Referencia.mppx;  // Eixo x é longitude
    Geo_Referencia.CcUTM.Reset(Geo_Referencia.AUTM.Zone(),Geo_Referencia.AUTM.Northp(),PosLon,PosLat);

    P.real(Geo_Referencia.CcUTM.Latitude());
    P.imag(Geo_Referencia.CcUTM.Longitude());
    PImg.real(x);
    PImg.imag(y);

    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%8.4f,%8.4f) (%s)\n",x,y,P.real(),P.imag(),Geo_Referencia.CcUTM.DMSRepresentation().c_str());
    displayStatusBar("TIF Referencia",intStr,0);

}


void MainWindow::on_mouse_ImagemRotacionada(int event, int x, int y, int flags, void *) {
    double                  PosLat,PosLon;
    int                     PosLat_A,PosLat_B,PosLon_A,PosLon_B;
    std::complex<double>    P,PImg;
    char                    Str[100];

    double corX;
    double corY;

    corX = GeoProc.mppx;
    corY = GeoProc.mppy;

    corX = GeoProc.mppx / 0.00158228;
    corY = - GeoProc.mppy / 0.00158228;

    corX = 0;
    corY = 0;
//    x = x + (0.5 / GeoProc.mppx);
//    y = y + (0.5 / GeoProc.mppy);

    PosLat = GeoProc.AUTM.Northing() + y * GeoProc.mppy + corY;
    PosLon = GeoProc.AUTM.Easting() + x * GeoProc.mppx + corX;  // Eixo x é longitude
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);

    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());
    PImg.real(x);
    PImg.imag(y);

    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%9.6f,%9.6f) (%s)",x,y,P.real(),P.imag(),GeoProc.CcUTM.DMSRepresentation().c_str());
    displayStatusBar(Name_Imagem1,intStr,0);

//    sprintf(intStr,"Coordenada (%9.6f,%9.6f) (%s)",GeoProc.CcUTM.Latitude(),GeoProc.CcUTM.Longitude(),GeoProc.CcUTM.DMSRepresentation().c_str());
    QString S1,S2;

    QLocale Br;
    S1 = Br.toString(GeoProc.CcUTM.Latitude(),'f',6);
    S2 = Br.toString(GeoProc.CcUTM.Longitude(),'f',6);
    sprintf(intStr,"Coordenada (%s ; %s) (%s)",S1.toStdString().c_str(),S2.toStdString().c_str(),GeoProc.CcUTM.DMSRepresentation().c_str());

    if (event == EVENT_MOUSEMOVE) {
        std::complex<double>    P_D;
        std::complex<int>       P_Atual,P_Img_D;
        double                  Dist;
        P_Atual.real(x);
        P_Atual.imag(y);

        if (GeoProc.Proximidade) {
            GeoProc.Era_Proximo = true;
        }

        if (GeoProc.Pontos.length() > 0) {
            Dist = 0;
        }

        for (int j = 0 ; j < GeoProc.Pontos.length() ; j++) {
            P_Img_D = GeoProc.PImg.at(j) - P_Atual;
            Dist = sqrt(pow(P_Img_D.real(),2)+pow(P_Img_D.imag(),2));
            if (Dist > 10) {
                GeoProc.Proximidade = false;
                if (GeoProc.Era_Proximo) {
                    MeFu.Draw_Cel();
                }
                GeoProc.Era_Proximo = false;
            } else {
                GeoProc.Proximidade = true;
                GeoProc.Proximidade_Num = j;
                j = 1000;
            }
        }

        if (GeoProc.Proximidade) {
            MeFu.Draw_Cel();
        }

//        cv::c/ircle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(255, 0, 0), 3, cv::LINE_8, 0);
//        imshow(Name_Imagem1,GeoProc.image);


        switch  (flags) {
            case (EVENT_FLAG_LBUTTON):
                S1 = "hh";

            break;

        }
    }

    if (event == EVENT_FLAG_RBUTTON) {
            S1 = "hh";

    }

    if (event == EVENT_LBUTTONDOWN) {
        switch (flags) {
        case (EVENT_FLAG_SHIFTKEY + EVENT_FLAG_LBUTTON):
            MeFu.Gera_Retangulo(windowPrincipal);
            break;

        case  (EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON): // Botão esquerdo Mouse clic + CTRL
            GeoProc.Pontos.push_back(P);
            GeoProc.PImg.push_back(PImg);

            MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),true);

            uidialogPontos->Print(intStr);

            break;

        case (EVENT_FLAG_ALTKEY + EVENT_FLAG_LBUTTON):  // Botão esquerdo + ALT
            if (!GeoProc.Pontos.isEmpty()) {

//                MeFu.Print(windowPrincipal," Ponto retirado\n");
//                MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),false);

                GeoProc.Pontos.removeLast();
                GeoProc.PImg.removeLast();
                uidialogPontos->clear();

                ImgWindowPontos.copyTo(GeoProc.image);
                for (int j = 0 ; j < GeoProc.Pontos.length() ; j++) {
                    GeoProc.CcUTM.Reset(GeoProc.Pontos.at(j).real(),GeoProc.Pontos.at(j).imag());
                    sprintf(Str,"Coordenada (%9.6f,%9.6f) (%s)",GeoProc.Pontos.at(j).real(),
                                                                GeoProc.Pontos.at(j).imag(),
                                                                GeoProc.CcUTM.DMSRepresentation().c_str());
                    uidialogPontos->Print(Str);
                    if (GeoProc.Circle) cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
                }


                if (GeoProc.Line) {
                    for (int k = 1 ; k < GeoProc.PImg.length() ; k++ ) {
                        PosLat_A = GeoProc.PImg.at(k - 1).real();
                        PosLon_A = GeoProc.PImg.at(k - 1).imag();
                        PosLat_B = GeoProc.PImg.at(k).real();
                        PosLon_B = GeoProc.PImg.at(k).imag();
                        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),Scalar( 0, 255, 0), 4 );
                    }
                }


                // imshow("Imagem",GeoProc.image);

                MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),false);

            } else {
//                MeFu.Print(windowPrincipal," Lista vazia.\n");
            }
            break;

         case (EVENT_FLAG_ALTKEY + EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON):
            if (GeoProc.Proximidade) {
                GeoProc.Selecionado = !GeoProc.Selecionado;
                MeFu.Draw_Cel();
             }


/*
*/
            break;

        default:
            if (GeoProc.Selecionado) {
                GeoProc.PImg.replace(GeoProc.Proximidade_Num,PImg);
                GeoProc.Pontos.replace(GeoProc.Proximidade_Num,P);
//                MeFu.Draw_Cel();
                MeFu.Atualiza_Lista_Pontos_Apos_Move(windowPrincipal);
                GeoProc.Selecionado = false;
            }

        }
    }
}

void MainWindow::on_mouse_Teste(int event, int x, int y, int, void *) {
    double PosLat,PosLon;
    PosLat = Geo.AUTM.Northing() + y * Geo.mppy;
    PosLon = Geo.AUTM.Easting() + x * Geo.mppx;  // Eixo x é longitude
    Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),PosLon,PosLat);

    sprintf(intStr,"Posiçao ( %d , %d ) Coordenada (%4.4f,%4.4f) (%s)",x,y,Geo.CcUTM.Latitude(),Geo.CcUTM.Longitude(),Geo.CcUTM.DMSRepresentation().c_str());
    displayStatusBar("Imagem Teste",intStr,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Le_Geo_from_file(QFileInfo CompleteName)
{
    qint64      Elapsed;
    bool        GeoReff = false;
    double      Rel1;
    QString     Name;

    cv::destroyAllWindows();

    Name = CompleteName.absoluteFilePath();

    GDALAllRegister();

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(Name.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

    GDAL2MAT(poSrcDS,imageRead);

    QStringList stringList;

    DecodeXMP(stringList,poSrcDS);

    double Transform[6];
    pOldDS->GetGeoTransform(Transform);
    Geo.Name = Name;
    char ValorStr[100],InputStr[100];

    // Verifica se possui informação geográfica
    if ((Transform[0] == 0) && (Transform[1] == 1) &&
        (Transform[2] == 0) && (Transform[3] == 0) &&
        (Transform[4] == 0) && (Transform[5] == 1) ) {

        // Erro -> Não possui informação geográfica

        return;

    } else { // Possui
        GeoReff = true;

        bool    E_UTM;
        int     Zona = 0;
        int     Hemisferio = 10;
        if (pOldDS->GetSpatialRef()->IsProjected()) {
            E_UTM = true;
            Zona = pOldDS->GetSpatialRef()->GetUTMZone(&Hemisferio); // Hemisferio (= 0 Sul), (= 1 Norte)
        } else {
            E_UTM = false;
        }
        if (!E_UTM) {  // LAT LON
            Geo.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
            Geo.Iw = pOldDS->GetRasterXSize();
            Geo.Ih = pOldDS->GetRasterYSize();
            Geo.DUTM.Reset( Transform[3] + Transform[5] * Geo.Ih,Transform[0] + Transform[1] * Geo.Iw );
            Geo.mppx = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting()) / Geo.Iw; // LON
            Geo.mppy = -abs(Geo.DUTM.Northing() - Geo.AUTM.Northing()) / Geo.Ih; // LAT
            Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
        } else { // UTM
            Geo.AUTM.Reset(Zona,Hemisferio,Transform[0],Transform[3]);
            Geo.Iw = pOldDS->GetRasterXSize();
            Geo.Ih = pOldDS->GetRasterYSize();
            Geo.mppx = Transform[1]; // LON
            Geo.mppy = Transform[5]; // LAT
            Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
        }
    }

    Geometria_Img = Geo;
    Geometria_Img.FileDS = poSrcDS;  // ---------------> Aqui tem que trocar para copia

    if (!GeoReff) {
        // Erro -> Não possui informação geográfica
        return;
    } else {
        Geometria_ImgProc = Geometria_Img;
        Destino = imageRead;
    }

    Geometria_ImgProc.Pontos.clear();
    GeoProc = Geometria_ImgProc;

    // EXIF
    char ** papszIter;
    papszIter = papszMetadata;

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    GDALClose(pOldDS);
}


void MainWindow::AbreArquivo(QFileInfo CompleteName, bool Clear_txt_Output, bool UsaEscala, bool Mesmo_Dir)
{
    qint64      Elapsed;
    bool        GeoReff = false;
    double      Rel1;
    QString     Name;

    bool        E_JPG;
    QString     Extensao;

    // cv::destroyAllWindows();
    if (Name_Imagem1 != "") cv::destroyWindow(Name_Imagem1);

//    QLocale curLocale(QLocale("en_US"));
//    QLocale::setDefault(curLocale);

    Name = CompleteName.absoluteFilePath();
    Extensao = CompleteName.suffix().toUpper();

    if (Extensao == "JPG") E_JPG = true; else E_JPG = false;

    GeoProc.Pontos.clear();
    GeoProc.PImg.clear();
    // Limpa tabela
    ui->tablePontos->setRowCount(0);

    timer2.start();
    ui->txt_Path->setText(Name);

    if (Clear_txt_Output)
        ui->txt_Output->setText(" Imagem [" + Name + "]");

    else
        ui->txt_Output->append(" Imagem [" + Name + "]");

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


    GDALAllRegister();

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(Name.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());


    //GDAL2MAT(poSrcDS,imageRead);
    if (Extensao == "KMZ") {
        GDAL2MAT(pOldDS,imageRead);
    } else {
        imageRead = cv::imread(Name.toStdString(),CV_LOAD_IMAGE_COLOR);
    }


    // Faz a redução no tamanho da imagem
    if (UsaEscala) {
        if (Informacao.Escala1 != 1.0) {
            cv::resize(imageRead,imageRead,cv::Size(imageRead.cols * Informacao.Escala1,imageRead.rows * Informacao.Escala1));
        }
    }

    Elapsed = timer2.elapsed();
    sprintf(intStr,"\n Imagem lida em %3.3f segundos.\n",(Elapsed / 1000.0));
    ui->txt_Output->append(intStr);

    if (ui->actionEspecificar_2->isChecked()) {
        sprintf(intStr,"\n Processando com altura definida pelo usuário em %6.2f metros.\n",Geo.Alt_Default);
        ui->txt_Output->append(intStr);
    }

    timer2.restart();

    QStringList stringList;

    DecodeXMP(stringList,poSrcDS);

    double Transform[6];
    pOldDS->GetGeoTransform(Transform);
    Geo.Name = Name;
    char ValorStr[100],InputStr[100];


    // Busca qual modelo do drone
/*
    strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_Model"));
    Geo.TipoCamera = InputStr;
    Geo.TipoCamera = "";

    if (Geo.TipoCamera == Cameras.myOptions.at(0)) {
        Geo.TipoDrone = Drones.myOptions.at(0);
    } else if (Geo.TipoCamera == Cameras.myOptions.at(1)) {
        Geo.TipoDrone = Drones.myOptions.at(1);
    } else if (Geo.TipoCamera == Cameras.myOptions.at(2)) {
        Geo.TipoDrone = Drones.myOptions.at(2);
    } else if (Geo.TipoCamera == Cameras.myOptions.at(3)) {
        Geo.TipoDrone = Drones.myOptions.at(3);
    } else if (Geo.TipoCamera == Cameras.myOptions.at(4)) {
        Geo.TipoDrone = Drones.myOptions.at(4);
    } else if (Geo.TipoCamera == Cameras.myOptions.at(5)) {
        Geo.TipoDrone = Drones.myOptions.at(5);
    }

    ui->txt_Output->append(" Tipo do Drone: " + Geo.TipoDrone + "\n");
*/

    // Verifica se possui informação geográfica
    if ((Transform[0] == 0) && (Transform[1] == 1) &&
        (Transform[2] == 0) && (Transform[3] == 0) &&
        (Transform[4] == 0) && (Transform[5] == 1) ) {


        // Se fez escala, estes numeros podem estar errados, deveriam ser o tamanho de ImageRead após escala

        Geo.Iw = poSrcDS->GetRasterXSize();
        Geo.Ih = poSrcDS->GetRasterYSize();
/*
        Geo.Iw = imageRead.cols;
        Geo.Ih = imageRead.rows;
*/
        strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLength"));
        RetiraParenteses(ValorStr,InputStr);
        Geo.DistFocal = atof(ValorStr);
        strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLengthIn35mmFilm"));
//        RetiraParenteses(ValorStr,InputStr);
        Geo.DistFocal35mm = atof(InputStr);

        if (ui->actionEspecificar_2->isChecked()) {
            Geo.Alt = Geo.Alt_Default;
        } else {
            Geo.Alt = GetXMPItem(stringList,"RelativeAltitude");
        }

        if (ui->actionGimbal->isChecked()) {
            Geo.Gimbal_Angle = GetXMPItem(stringList,"GimbalYawDegree");
        } else {
            Geo.Gimbal_Angle = GetXMPItem(stringList,"FlightYawDegree");
        }

        // Aqui vai a correção manual de ângulo
//        Geo.Gimbal_Angle = Geo.Gimbal_Angle - 6.0;


        Geo.Cc.real(getLATfromEXIF(poSrcDS));
        Geo.Cc.imag(getLONfromEXIF(poSrcDS));

        if (UsaEscala) {
            Geo.Escala = Informacao.Escala1;
        } else {
            Geo.Escala = 1;
        }

        Geo.Calculos();

    } else { // Possui
        GeoReff = true;
        Geo_Tiff_Name = Name;

        bool    E_UTM;
        int     Zona = 0;
        int     Hemisferio = 10;
        if (pOldDS->GetSpatialRef()->IsProjected()) {
            E_UTM = true;
            Zona = pOldDS->GetSpatialRef()->GetUTMZone(&Hemisferio); // Hemisferio (= 0 Sul), (= 1 Norte)
        } else {
            E_UTM = false;
        }
        if (!E_UTM) {  // LAT LON
            Geo.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
            Geo.Iw = pOldDS->GetRasterXSize();
            Geo.Ih = pOldDS->GetRasterYSize();
            Geo.DUTM.Reset( Transform[3] + Transform[5] * Geo.Ih,Transform[0] + Transform[1] * Geo.Iw );
            Geo.Dw = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting());
            Geo.Dh = abs(Geo.DUTM.Northing() - Geo.AUTM.Northing());
            Geo.mppx = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting()) / Geo.Iw; // LON
            Geo.mppy = -abs(Geo.DUTM.Northing() - Geo.AUTM.Northing()) / Geo.Ih; // LAT
            Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
            Geo.A.real(Geo.AUTM.Latitude());
            Geo.A.imag(Geo.AUTM.Longitude());
            Geo.D.real(Geo.DUTM.Latitude());
            Geo.D.imag(Geo.DUTM.Longitude());
        } else { // UTM
            Geo.AUTM.Reset(Zona,Hemisferio,Transform[0],Transform[3]);
            Geo.Iw = pOldDS->GetRasterXSize();
            Geo.Ih = pOldDS->GetRasterYSize();
            Geo.mppx = Transform[1]; // LON
            Geo.mppy = Transform[5]; // LAT
            Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
        }
    }

    Geo.Cor_Nodata = pOldDS->GetRasterBand(1)->GetNoDataValue();

    Geometria_Img = Geo;
    Geometria_Img.FileDS = poSrcDS;  // ---------------> Aqui tem que trocar para copia

    if (!GeoReff) {
        Rotaciona_Imagem(imageRead,Destino);

        ////////////////////////////////////////////////////////////
        int Cor_DATA;
        if (ui->actionBranco->isChecked()) {
            Cor_DATA = 0;
        } else {
            Cor_DATA = 255;
        }

        if (ui->actionBordas_na_imagens_processadas->isChecked()) cv::rectangle(Destino,cv::Point2f(0,0),cv::Point2f(Destino.cols - 1,Destino.rows - 1),cv::Scalar(Cor_DATA,Cor_DATA,Cor_DATA),3);
        /////////////////////////////////////////////////////////////


        Elapsed = timer2.elapsed();
        sprintf(intStr," Imagem rotacionada e georeferenciada em %3.3f segundos.\n",(Elapsed / 1000.0));
        ui->txt_Output->append(intStr);

        frameW = imageRead.cols;
        frameH = imageRead.rows;

        // 700 pontos em Y

        if (frameW > frameH) {
            Rel1 = frameH / 700.0;
        } else {
            Rel1 = frameW / 600.0;
        }
        namedWindow("Imagem Original", WINDOW_GUI_EXPANDED);

        moveWindow("Imagem Original", 20,20);
        resizeWindow("Imagem Original",static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
        imshow("Imagem Original",imageRead);

        setMouseCallback("Imagem Original",on_mouse_OpenFile,NULL);
    } else {
        Geometria_ImgProc = Geometria_Img;
        Destino = imageRead;
    }

/*
    sprintf(intStr," Bordas: (%4.4f,%3.4f) - (%4.4f,%4.4f) \n",Geometria_ImgProc.AUTM.Latitude(),Geometria_ImgProc.AUTM.Longitude(),Geometria_ImgProc.DUTM.Latitude(),Geometria_ImgProc.DUTM.Longitude());
    ui->txt_Output->append(intStr);
*/
    // Calcula tamanho da imagem em metros quadrados
    double Area;
    Area = fabs(Geometria_ImgProc.AUTM.Easting() - Geometria_ImgProc.DUTM.Easting()) *
           fabs(Geometria_ImgProc.AUTM.Northing() - Geometria_ImgProc.DUTM.Northing());
//    puts(setlocale(LC_ALL,"it_IT"));
/*
    sprintf(intStr," Área %d metros quadrados.\n",static_cast<int> (Area));
    ui->txt_Output->append(intStr);
*/

//    namedWindow("Imagem",WINDOW_GUI_EXPANDED);

    if (Mesmo_Dir) {
        Geo_Tiff_Name = CompleteName.absolutePath() + "/" + CompleteName.completeBaseName() + ".tif";
    } else {
        Geo_Tiff_Name = Informacao.DiretorioProc + "/" + CompleteName.completeBaseName() + ".tif";
    }
    Name_Imagem1 = Geo_Tiff_Name.toStdString();
    namedWindow(Name_Imagem1,WINDOW_GUI_EXPANDED);




    if (E_JPG) {
        moveWindow(Name_Imagem1, 700,80);
    } else {
        moveWindow(Name_Imagem1, 20,20);
    }

    frameW = Destino.cols;
    frameH = Destino.rows;

    if (E_JPG) {
        if (frameW > frameH) {
            Rel1 = frameH / 700.0;
        } else {
            Rel1 = frameW / 550.0;
            moveWindow(Name_Imagem1, 700,10);
        }
    } else {
        if (frameW > frameH) {
            Rel1 = frameH / 920.0;
        } else {
            Rel1 = frameW / 550.0;
        }
    }


    ui->txt_Nome_Imagem->setText(QString::fromStdString(Name_Imagem1));
    ui->bt_Mostar_Imagem->setEnabled(true);
    resizeWindow(Name_Imagem1,Destino.cols / Rel1,Destino.rows / Rel1);

    // Se tiver TIF de Referencia não mostrar imagem agora, vai fazer o Blend
    if (!ui->actionUsar->isChecked()) imshow(Name_Imagem1,Destino);

    Geometria_ImgProc.Pontos.clear();
    Destino.copyTo(Geometria_ImgProc.image);
    Destino.copyTo(ImgWindowPontos);
    GeoProc = Geometria_ImgProc;
    setMouseCallback(Name_Imagem1,on_mouse_ImagemRotacionada,NULL);
    Get_Info_Window(QString::fromStdString(Name_Imagem1),GeoProc);

    if (!uidialogPontos) {
        dialogPontos = new DialogPontos(this);
     }
     dialogPontos->clear();
     uidialogPontos = dialogPontos;
    /*
    dialogPontos->show();
    dialogPontos->setWindowTitle("Pontos");
    */

    QString Result;
    Result = Name;
    Result = Result + "\n\n\nDados EXIF:\n";
    // EXIF
    char ** papszIter;
    papszIter = papszMetadata;
    while(papszIter && *papszIter)
    {
        Result = Result + *papszIter + "\n";
        papszIter++;
    }
    Result = Result + "\n Dados XMP:\n";
    // XMP
    for (int i = 0 ; i <stringList.length() ; i++) {
        Result = Result + stringList.at(i) + "\n";// + "--->>> " + QString::number(stringList.at(i).length()) + "\n";
//        ui->txt_Output->append(stringList.at(i));
    }

    Geometria_Img.ExifXMPInfo = Result;

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    if (Informacao.Diretorio == "") {
        if (Name.contains("/proc",Qt::CaseInsensitive)) {
              Informacao.DiretorioProc = CompleteName.path();
              Informacao.Diretorio = Informacao.DiretorioProc.mid(0,(Informacao.DiretorioProc.length()-5));
              Informacao.DiretorioResult = Informacao.Diretorio + "/Result";
        }
        if (Name.contains("/result",Qt::CaseInsensitive)) {
              Informacao.DiretorioResult = CompleteName.path();
              Informacao.Diretorio = Informacao.DiretorioResult.mid(0,(Informacao.DiretorioResult.length()-7));
              Informacao.DiretorioProc = Informacao.Diretorio + "/Proc";
        }
    }


    if (!GeoReff) {
        /// Grava arquivo
        timer2.restart();
        GDALDataset* pNewDS = GDALDataset::FromHandle(pOldDS);
        char** papszMetadata2 = CSLDuplicate(pNewDS->GetMetadata());

              //////////////////////////////////////////////////////
              // Altura do voo

              sprintf(intStr,"%6.2f metros",Geo.Alt);
              ui->txt_Alt_Voo->setText(intStr);
        if (Informacao.Diretorio_Img_Original == "") {
            Informacao.Diretorio_Img_Original = CompleteName.absoluteFilePath();
        }

        ui->bt_Reduzir->setEnabled(true);
        ui->bt_Dir_Proc->setEnabled(true);
        ui->bt_ProcessarImgs->setEnabled(true);
        ui->bt_limpaProc->setEnabled(true);


        ///////////////////////////////////////////////////////////////////////////////////////


        QFile Old(Geo_Tiff_Name);
        if (Old.exists()) {
            Old.remove(Geo_Tiff_Name);
        }

#ifdef GDAL
        MAT2GDAL(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc);
#else
        std::vector<int> params;
//            params.push_back(IMWRITE_JPEG_QUALITY);
//            params.push_back(50); // Mais valor -> menor compressao. Default 95
//            params.push_back(IMWRITE_PNG_COMPRESSION);
//            params.push_back(3); // Maior valor -> mais compressão. Default 3
        params.push_back(IMWRITE_TIFF_COMPRESSION);

        bool check = cv::imwrite(Geo_Tiff_Name.toStdString(),Destino);
        if (!check) {
            QString Path;
            Path = "Não foi possível gravar arquivo " + Geo_Tiff_Name;
            QMessageBox::information(this,tr("Erro"),Path);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            return;
         }

         writeGeoInfo(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc);

         if (!Mesmo_Dir) {
            Grava_Thumb(Geo_Tiff_Name);
         }

#endif

        Elapsed = timer2.elapsed();
        sprintf(intStr,"\n Imagem %s gravada em %3.3f segundos.\n",Geo_Tiff_Name.toStdString().c_str(),(Elapsed / 1000.0));
        ui->txt_Output->append(intStr);
    }


    ui->bt_limpaProc->setEnabled(true);


    ui->bt_DialogPontos->setEnabled(true);
    ui->actionPontos->setEnabled(true);

    ui->pushButton->setEnabled(true);
    ui->checkBox_Linhas->setEnabled(true);

    ui->bt_EXIF_XMP_List->setEnabled(true);
    ui->actionEXIF_XMP->setEnabled(true);
    ui->menuMadidas->setEnabled(true);
    ui->actionCortarTIFF->setEnabled(true);

    imshow(Name_Imagem1,GeoProc.image);


    GDALClose(pOldDS);


    // Se tiver uma imagem de fundo, fazer a mixagem aqui!
    if (ui->actionUsar->isChecked()) {
        on_actionUsar_triggered();

/*
        Geo_ICE = GeoProc;
        dialogBlend = new Dialog_Blend(this);
        dialogBlend->raise();
        dialogBlend->setWindowTitle("Blend");
        dialogBlend->Ajusta_Parametros();
        dialogBlend->Print_Slider();

        connect(dialogBlend, SIGNAL(Alterado()),    this,   SLOT(Valor_Slider_Changed()));
        connect(dialogBlend, SIGNAL(Sobrepor()),    this,   SLOT(Calc_Sobrepor_Images()));
        connect(dialogBlend, SIGNAL(Blend()),       this,   SLOT(Calc_Blend_Images()));
        connect(dialogBlend, SIGNAL(Cancel_Blend()),this,   SLOT(on_bt_Teste_Cancel()));
        connect(dialogBlend, SIGNAL(Usar_Blend()),  this,   SLOT(Blend_Ok()));


        dialogBlend->show();
        QString Output;
        Size_GeoTiff(Geo_Referencia.Name,false,Output);
        dialogBlend->Print(Output);

        Output = "\n";
        Size_GeoTiff(Geo_ICE.Name,false,Output);
        dialogBlend->Print(Output);
        dialogBlend->Print_File1(Geo_Referencia.Name);
        dialogBlend->Print_File2(Geo_ICE.Name);

        Calculou_Sobrepor = false;
        Calculou_Blend = false;
        if (dialogBlend->Modo_de_Operacao()) Calc_Blend_Images(); else Calc_Sobrepor_Images();
*/
    }

    MeFu.Draw_Cel();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_btCmd_clicked()
{
    QDir::setCurrent(Informacao2.DiretorioProc);
    system("cmd");
}


void Espera(int n){
    QTime dieTime= QTime::currentTime().addSecs(n);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void MainWindow::on_actionHarley_2_changed()
{
  //  ui->actionMiranda_2->setChecked(false);
}


void MainWindow::on_actionMiranda_2_changed()
{
  //  ui->actionHarley_2->setChecked(false);
}


void MainWindow::on_actionMiranda_2_triggered()
{
    ui->actionHarley_2->setChecked(false);
}


void MainWindow::on_actionHarley_2_triggered()
{
    ui->actionMiranda_2->setChecked(false);
}




void MainWindow::on_actionPontos_triggered()
{
    on_bt_DialogPontos_clicked();
}


void MainWindow::on_bt_DialogPontos_clicked()
{
    dialogPontos->show();
    dialogPontos->setWindowTitle("Pontos");
}


void MainWindow::on_actionEXIF_XMP_triggered()
{
    dialogEXIF  = new DialogEXIF(this);
    dialogEXIF->show();
    dialogEXIF->setWindowTitle("EXIF-XMP information");
    dialogEXIF->print(Geometria_Img.ExifXMPInfo);
}





/*
QImage PIX2QImage(PIX *pixImage) {
  int width = pixGetWidth(pixImage);
  int height = pixGetHeight(pixImage);
  int depth = pixGetDepth(pixImage);
  int bytesPerLine = pixGetWpl(pixImage) * 4;
  l_uint32 * s_data = pixGetData(pixEndianByteSwapNew(pixImage));

  QImage::Format format;
  if (depth == 1)
    format = QImage::Format_Mono;
  else if (depth == 8)
    format = QImage::Format_Indexed8;
  else
    format = QImage::Format_RGB32;

  QImage result((uchar*)s_data, width, height, bytesPerLine, format);

  // Handle pallete
  QVector<QRgb> _bwCT;
  _bwCT.append(qRgb(255,255,255));
  _bwCT.append(qRgb(0,0,0));

  QVector<QRgb> _grayscaleCT(256);
  for (int i = 0; i < 256; i++)  {
    _grayscaleCT.append(qRgb(i, i, i));
  }
  if (depth == 1) {
    result.setColorTable(_bwCT);
  }  else if (depth == 8)  {
    result.setColorTable(_grayscaleCT);

  } else {
    result.setColorTable(_grayscaleCT);
  }

  if (result.isNull()) {
    static QImage none(0,0,QImage::Format_Invalid);
    qDebug() << "***Invalid format!!!";
    return none;
  }

  return result.rgbSwapped();
}


IplImage* QImage2IplImage(QImage *qimg)
{

IplImage *imgHeader = cvCreateImageHeader( cvSize(qimg->width(), qimg->height()), IPL_DEPTH_8U, 4);
imgHeader->imageData = (char*) qimg->bits();

uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg->byteCount());
memcpy(newdata, qimg->bits(), qimg->byteCount());
imgHeader->imageData = (char*) newdata;
//cvClo
return imgHeader;
}

QImage*  IplImage2QImage(IplImage *iplImg)
{
int h = iplImg->height;
int w = iplImg->width;
int channels = iplImg->nChannels;
QImage *qimg = new QImage(w, h, QImage::Format_ARGB32);
char *data = iplImg->imageData;

for (int y = 0; y < h; y++, data += iplImg->widthStep)
{
for (int x = 0; x < w; x++)
{
char r, g, b, a = 0;
if (channels == 1)
{
r = data[x * channels];
g = data[x * channels];
b = data[x * channels];
}
else if (channels == 3 || channels == 4)
{
r = data[x * channels + 2];
g = data[x * channels + 1];
b = data[x * channels];
}

if (channels == 4)
{
a = data[x * channels + 3];
qimg->setPixel(x, y, qRgba(r, g, b, a));
}
else
{
qimg->setPixel(x, y, qRgb(r, g, b));
}
}
}
return qimg;

}
*/



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionEspecificar_2_triggered()
{
   ui->actionValor->setEnabled(ui->actionEspecificar_2->isChecked());
}

void MainWindow::on_actionValor_triggered()
{
    bool    ok;
    float   Valor_A;
    QString text = QInputDialog::getText(0, "Altura do voo (em metros)",
                                             "Altura:", QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            Valor_A = text.toFloat();
        }

        if (Valor_A >= 10.0) {
            Geo.Alt_Default = Valor_A;
            text = "Valor: " + text + " m";
            ui->actionValor->setText(text);
            sprintf(intStr,"\n Altura do voo definida para %6.2f metros\n",Valor_A);
            ui->txt_Output->append(intStr);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
}



void MainWindow::on_actionTIFF_JPG_triggered()
{
    /////////////////////////////////////////////////////////// Escolhe arquivo Geo TIFF
     QString    InputTif,OutTxt;
     QString    NameJPG;
     int        No_Data;

     InputTif = QFileDialog::getOpenFileName(this, tr("TIFF Image"),Informacao.Diretorio,tr("Image (*.tif)"));
     QFileInfo CompleteNameTif(InputTif);

     if (InputTif == "") {
         return;
      }

      timer2.restart();

    /// Confere se é um arquivo Geo TIFF
      if (!Eh_GeoTiff(InputTif,No_Data)) { // Não é GeoTiff, informa erro e retorna
          OutTxt = "\n Erro!   Arquivo [" + InputTif + "] não é um GEOTIFF.\n";

          ui->txt_Output->append(OutTxt);
          return;
      }

      ui->txt_Output->clear();
      QString Output;
      Size_GeoTiff(InputTif,true,Output);

      ////// Faz a tranformação e salva JPG

        NameJPG = CompleteNameTif.path() + "/" + CompleteNameTif.completeBaseName() + ".jpg";

         QFile CompleteNameJPG(NameJPG);
         if (CompleteNameJPG.exists()) {

             QMessageBox messageBox(QMessageBox::Question,
                         "Sobrescrever",
                         "Arquivo JPG existe ! Sobrescrever ?",
                         QMessageBox::Yes | QMessageBox::No,
                         this);
                 messageBox.setButtonText(QMessageBox::Yes, "Gravar");
                 messageBox.setButtonText(QMessageBox::No, "Cancelar");
                 int reply = messageBox.exec();

             if (reply == QMessageBox::Yes) {
                 CompleteNameJPG.remove();
             }

             if(reply == QMessageBox::No)
             {
                 OutTxt = "\n Arquivo [" + NameJPG + "] existe e não será sobrescrito.\n";

                 ui->txt_Output->append(OutTxt);
                 return;
             }
         }

         OutTxt = "\n Iniciando geração [" + NameJPG + "]";

         ui->txt_Output->append(OutTxt);
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

         // gdal_translate -of JPEG Name.tif Name.jpg
         int Num_Elementos = 5;
         int Tamanho = Num_Elementos + 1;

         char **argv3;
         argv3 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

         int i = 0;
         for (i = 0 ; i < Num_Elementos ; i ++) {
             argv3[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
         }
         argv3[Num_Elementos] = nullptr;

         strcpy(argv3[0],"gdal_translate.exe");
         strcpy(argv3[1],"-of");
         strcpy(argv3[2],"JPEG");
         strcpy(argv3[3],InputTif.toStdString().c_str());
         strcpy(argv3[4],NameJPG.toStdString().c_str());

         translate_HAM(Num_Elementos,static_cast<char **>(argv3));

         for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
             free(argv3[i]);
         }

         qint64 Elapsed = timer2.elapsed();
         sprintf(intStr," Processado em %3.3f segundos.\n",(Elapsed / 1000.0));
         ui->txt_Output->append(intStr);
}



void MainWindow::on_actionICE_JPG_TIF_triggered()
{
    QString InputJPG,InputTif,OutputTif,Name,Dir;

    InputJPG = QFileDialog::getOpenFileName(this, tr("ICE Image"),"./",tr("Image (*.jpg)"));
    QFileInfo CompleteNameJPG(InputJPG);

    if (InputJPG == "") {
        return;
     }

    // Limpa lista de arquivos e campos de texto
    ui->list_Files->clear();
    ui->list_Files_Proc->clear();
    ui->list_Files_Result->clear();
    ui->txt_Dir_Proc->clear();
    ui->txt_Directory->clear();
    ui->txt_NumEntries->clear();
    ui->txt_NumEntries_Proc->clear();
    ui->txt_Path->clear();

    ui->checkBox_KMZ->setChecked(false);

    sprintf(intStr,"    Georeferencia automaticamente arquivo ICE (.jpg) tendo como referência arquivo GeoTiff.\n");
    ui->txt_Output->setText(intStr);


    sprintf(intStr,"\nArquivo ICE: [%s]\n",InputJPG.toStdString().c_str());
    ui->txt_Output->append(intStr);

    Dir = CompleteNameJPG.path();

    InputTif = QFileDialog::getOpenFileName(this, tr("TIF de referência"),Dir,tr("Image (*.tif)"));
    QFileInfo CompleteNameInputTif(InputTif);

    if (InputTif == "") {
        ui->txt_Output->clear();
        return;
     }
    sprintf(intStr,"Arquivo TIF de referência: [%s]\n",InputTif.toStdString().c_str());
    ui->txt_Output->append(intStr);

    OutputTif = QFileDialog::getSaveFileName(this,"Output",Dir,"Arquivo (*.tif)",nullptr,QFileDialog::DontConfirmOverwrite);
    QFileInfo CompleteNameTif(OutputTif);

    if (OutputTif.isNull()) {
        ui->txt_Output->clear();
        return;
    }

/*
    sprintf(intStr,"Arquivo TIF de resultado: [%s]\n",OutputTif.toStdString().c_str());
    ui->txt_Output->append(intStr);
*/
    Name = CompleteNameTif.fileName();

   Le_Geo_from_file(CompleteNameInputTif);

    cv::Mat outputimage;
    outputimage = cv::imread(InputJPG.toStdString());

    MAT2GDAL(OutputTif,outputimage,NULL,Geometria_ImgProc);

    AbreArquivo(OutputTif,false,true); // precisa não limpar o txt_Output

    QString Output;
    Size_GeoTiff(OutputTif,true,Output);
}

void MainWindow::on_actionTIFs_ICE_JPG_triggered()
{
    QString InputJPG,InputTif,OutputTif,Name,Dir;

    if (Informacao.Diretorio == "") {
        on_btOpenDirectory_clicked();
    }

    InputJPG = QFileDialog::getOpenFileName(this, tr("ICE Image"),Informacao.Diretorio,tr("Image (*.jpg)"));
    QFileInfo CompleteNameJPG(InputJPG);

    if (InputJPG == "") {
        return;
     }

    ui->checkBox_KMZ->setChecked(false);

    sprintf(intStr,"Arquivo ICE: [%s]\n",InputJPG.toStdString().c_str());
    ui->txt_Output->setText(intStr);

    Dir = CompleteNameJPG.path();
    QDir::setCurrent(Informacao.DiretorioProc);

    timer2.restart();

    // Acha centro geográfico das imagens
    double Sminx, Smaxx, Sminy, Smaxy, Scx, Scy;
    Sminx = 1e20;
    Sminy = 1e20;
    Smaxx = -1e20;
    Smaxy = -1e20;

    Imagem_Composta.Geo_File_List.clear();
    QDir directory(Informacao.DiretorioProc);
    QStringList Filters;
    Filters << "*.tif";
    Imagem_Composta.Geo_File_List = directory.entryInfoList(Filters,QDir::Files | QDir::NoDotAndDotDot);

    if (Imagem_Composta.Geo_File_List.length() < 2) { // Erro - menos de duas imagens
        ui->txt_Output->setText("    Erro. São necessárias pelo menos duas imagens.\n");
        QCoreApplication::processEvents();
        return;
    }

    ui->txt_Output->setText(" Georreferenciando imagem JPG [" + InputJPG + "]\n");
    QCoreApplication::processEvents();

    GDALAllRegister();
    for (int k = 0 ; k < Imagem_Composta.Geo_File_List.length() ; k++) {
        QString Temp = Imagem_Composta.Geo_File_List.at(k).filePath();
        GDALDataset *pNewDS;
        pNewDS = (GDALDataset *) GDALOpen(static_cast <const char *> (Temp.toStdString().c_str()),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pNewDS);

        double transform[6];
        double minx,miny,maxx,maxy,width,height,cx,cy;
        width = pNewDS->GetRasterXSize();
        height = pNewDS->GetRasterYSize();
        pNewDS->GetGeoTransform(transform);
        minx = transform[0];
        miny = transform[3] + width * transform[4] + height * transform[5];
        maxx = transform[0] + width * transform[1] + height * transform[2];
        maxy = transform[3];
        cx = minx + (maxx - minx)/2;
        cy = miny + (maxy - miny)/2;

        ///// Modifica maximos e minimos
        if (minx < Sminx) Sminx = minx;
        if (miny < Sminy) Sminy = miny;
        if (maxy > Smaxy) Smaxy = maxy;
        if (maxx > Smaxx) Smaxx = maxx;

        char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

        sprintf(intStr,"[%d/%d] %s (%f,%f)",(k+1),Imagem_Composta.Geo_File_List.length(),Temp.toStdString().c_str(),cy,cx);
        ui->txt_Output->append(intStr);

        sprintf(intStr,"--------------------------------------------------------------------------------------------------");
        ui->txt_Output->append(intStr);

        GDALClose(pNewDS);
    }

    Scx = (Smaxx - Sminx) /2 + Sminx;
    Scy = (Smaxy - Sminy) /2 + Sminy;

    // Calcula tamanho da imagem em metros quadrados
    GeographicLib::GeoCoords UL,RD;
    UL.Reset(Smaxy,Sminx);
    RD.Reset(Sminy,Smaxx);

    double Area;
    Area = fabs(UL.Easting() - RD.Easting()) * fabs(UL.Northing() - RD.Northing());

    sprintf(intStr,"(%f, %f) - (%f, %f). Centro(%f,%f).\nÁrea %d metros quadrados\n",Smaxy,Sminx,Sminy,Smaxx,Scy,Scx,static_cast<int> (Area));
    ui->txt_Output->append(intStr);

// Grava arquivo
    OutputTif = Dir + "/" + CompleteNameJPG.completeBaseName() + ".tif";
    ui->txt_Output->append(" Gravando imagem TIF [" + OutputTif + "]\n");
    QCoreApplication::processEvents();
    GDALDataset *pNewDS;
    pNewDS = (GDALDataset *) GDALOpen(static_cast <const char *> (InputJPG.toStdString().c_str()),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pNewDS);
    char** papszMetadata2 = CSLDuplicate(pNewDS->GetMetadata());

    imageRead = cv::imread(InputJPG.toStdString());
    std::vector<int> params;
//    params.push_back(IMWRITE_JPEG_QUALITY);
//    params.push_back(80);
    params.push_back(IMWRITE_TIFF_COMPRESSION);
//    params.push_back(6);
    cv::imwrite(OutputTif.toStdString(),imageRead,params);

    Geometria_ImgProc.AUTM.Reset(Smaxy,Sminx);
    Geometria_ImgProc.DUTM.Reset(Sminy,Smaxx);

    writeGeoInfo(OutputTif,imageRead,papszMetadata2,Geometria_ImgProc);

    GDALClose(pNewDS);
    qint64 Elapsed = timer2.elapsed();
    sprintf(intStr," Processado em %3.3f segundos.\n",(Elapsed / 1000.0));
    ui->txt_Output->append(intStr);

    Lista_Files_Imgs();
    Lista_Files_Result();

}



void MainWindow::on_actionGerar_GeoTiff_triggered()
{
    QString         Txt,Output_VRT;


    QFileInfo       CompleteNameTiff(GeoProc.Name);

    Output_VRT = CompleteNameTiff.path() + "/" +CompleteNameTiff.baseName() + ".vrt";

    QFileInfo       CompleteNameVRT(Output_VRT);

    if (CompleteNameVRT.exists()) {
        QFile Del_VRT(Output_VRT);
        if (Del_VRT.remove()) {
            ui->txt_Output->append("\n Apagando arquivo " + Output_VRT + "\n");
        }
    }

    sprintf(intStr,"\n -----------------------------------------------------------------------------------");
    ui->txt_Output->append(intStr);

    sprintf(intStr,"\n Gerando imagem georeferenciada.");
    ui->txt_Output->append(intStr);

    // Create a new file CSV
    FILE        *fp;
    QString     FileName;

    FileName = CompleteNameTiff.path() + "/" +CompleteNameTiff.baseName() + ".csv";

    sprintf(intStr,"\n Arquivo CSV com pontos e coordenadas: [%s]\n",FileName.toStdString().c_str());
    ui->txt_Output->append(intStr);

    fp = fopen(FileName.toStdString().c_str(), "w");  // Try changing to "a"

    fprintf(fp,"X;Y;Lon;Lat;\n");

    for (int i = 0 ; i < Geo_Tiff.Pontos.length() ; i++) {
        fprintf(fp,"%d;%d;%f;%f\n",Geo_ICE.PImg.at(i).real(),Geo_ICE.PImg.at(i).imag(),Geo_Tiff.Pontos.at(i).imag(),Geo_Tiff.Pontos.at(i).real());
    }

    fclose(fp);

    timer2.restart();

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    ////////////////////////////// VRT
         int Num_Elementos = 9 + (Geo_Tiff.Pontos.length() * 5);
         int Tamanho = Num_Elementos + 1;

         char **argv3;
         argv3 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

         int i = 0;
         for (i = 0 ; i < Num_Elementos ; i ++) {
             argv3[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
         }
         argv3[Num_Elementos] = nullptr;

         strcpy(argv3[0],"gdal_translate.exe");
         strcpy(argv3[1],"-of");
         strcpy(argv3[2],"vrt");
         strcpy(argv3[3],"-a_nodata");
         strcpy(argv3[4],"0");
         strcpy(argv3[5],"-a_srs");
         strcpy(argv3[6],"EPSG:4326");

         for (int i = 0 ; i < Geo_Tiff.Pontos.length() ; i++) {
            fprintf(fp,"%d;%d;%f;%f\n",Geo_ICE.PImg.at(i).real(),Geo_ICE.PImg.at(i).imag(),Geo_Tiff.Pontos.at(i).imag(),Geo_Tiff.Pontos.at(i).real());

            strcpy(argv3[((i * 5) + 7)],"-gcp");
            sprintf(intStr,"%d",Geo_ICE.PImg.at(i).real());
            strcpy(argv3[((i * 5) + 8)],intStr);
            sprintf(intStr,"%d",Geo_ICE.PImg.at(i).imag());
            strcpy(argv3[((i * 5) + 9)],intStr);
            sprintf(intStr,"%f",Geo_Tiff.Pontos.at(i).imag());
            strcpy(argv3[((i * 5) + 10)],intStr);
            sprintf(intStr,"%f",Geo_Tiff.Pontos.at(i).real());
            strcpy(argv3[((i * 5) + 11)],intStr);
         }

         strcpy(argv3[Num_Elementos - 2],Geo_ICE.Name.toStdString().c_str());
         strcpy(argv3[Num_Elementos - 1],Output_VRT.toStdString().c_str());

         translate_HAM(Num_Elementos,static_cast<char **>(argv3));

         for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
             free(argv3[i]);
         }

         Txt = "\n Comandos GDAL equivalente:\ngdal_translate -of vrt -a_srs EPSG:4326 -a_nodata 0 ";
         for (int j = 0 ; j < Geo_Tiff.Pontos.length() ; j++) {
     //        sprintf(intStr,"Coordenada (%f,%f) -> (%d,%d) ",Geo_Tiff.Pontos.at(j).real(),Geo_Tiff.Pontos.at(j).imag(),Geo_ICE.PImg.at(j).real(),Geo_ICE.PImg.at(j).imag());
               sprintf(intStr,"-gcp %d %d %f %f ",Geo_ICE.PImg.at(j).real(),Geo_ICE.PImg.at(j).imag(),Geo_Tiff.Pontos.at(j).imag(),Geo_Tiff.Pontos.at(j).real());
             Txt = Txt + intStr;
         }

         Txt = Txt + Geo_ICE.Name + " " + Output_VRT;

         ui->txt_Output->append(Txt + "\n");

         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

         ////////////////////////////////// WARP

         Num_Elementos = 10;
         Tamanho = Num_Elementos + 1;

         char **argv4;
         argv4 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

         i = 0;
         for (i = 0 ; i < Num_Elementos ; i ++) {
             argv4[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
         }
         argv4[Num_Elementos] = nullptr;

         strcpy(argv4[0],"gdalwarp.exe");
         strcpy(argv4[1],"-s_srs");
         strcpy(argv4[2],"EPSG:4326");
         strcpy(argv4[3],"-t_srs");
         strcpy(argv4[4],"EPSG:4326");
         strcpy(argv4[5],"-overwrite");
         strcpy(argv4[6],"-co");
         strcpy(argv4[7],"COMPRESS=JPEG");
         strcpy(argv4[8],Output_VRT.toStdString().c_str());
         strcpy(argv4[9],GeoProc.Name.toStdString().c_str());

         warp_H(Num_Elementos,static_cast<char **>(argv4));

         ////////////////////////////////////////
         ///
         /*///
         QApplication::processEvents();
         QByteArray data =    ->readAllStandardOutput();
         ui->txt_Output->append(QString(data));
         QApplication::processEvents();
         */

         ////////////////////////////////////////

         for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
             free(argv4[i]);
         }

         Txt = "gdalwarp -s_srs EPSG:4326 -t_srs EPSG:4326 -overwrite -co COMPRESS=JPEG " + Output_VRT + " " + GeoProc.Name + "\n";
         ui->txt_Output->append(Txt);

         qint64  Elapsed;

         Elapsed = timer2.elapsed();
         sprintf(intStr," Transformação em %3.3f segundos.\n",(Elapsed/1000.0));
         ui->txt_Output->append(intStr);

         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

         QString Output;
         Size_GeoTiff(GeoProc.Name, true, Output);

         Lista_Files_Result();
}

void MainWindow::on_actionCapturar_Pontos_triggered()
{
    QString InputJPG,InputTif,OutputTif,Name,Dir;
    int     No_Data;

    InputJPG = QFileDialog::getOpenFileName(this, tr("ICE Image"),"./",tr("Image (*.jpg)"));
    QFileInfo CompleteNameJPG(InputJPG);

    if (InputJPG == "") {
        return;
     }

    ui->checkBox_KMZ->setChecked(false);

    sprintf(intStr,"Arquivo ICE: [%s]\n",InputJPG.toStdString().c_str());
    ui->txt_Output->setText(intStr);

    Geo_ICE.Name = InputJPG;

    Dir = CompleteNameJPG.path();

    InputTif = QFileDialog::getOpenFileName(this, tr("TIF de referência"),Dir,tr("Image (*.tif)"));
    QFileInfo CompleteNameInputTif(InputTif);

    if (InputTif == "") {
        return;
     }

    if (!Eh_GeoTiff(InputTif,No_Data)) {
        sprintf(intStr,"\n Erro! [%s] não é um arquivo GeoTiff.",InputTif.toStdString().c_str());
        ui->txt_Output->append(intStr);
        return;
    }


    sprintf(intStr,"Arquivo TIF de referência: [%s]\n",InputTif.toStdString().c_str());
    ui->txt_Output->append(intStr);

    Geo_Img.Name = InputTif;

    OutputTif = QFileDialog::getSaveFileName(this,"Output",Dir,"Arquivo (*.tif)",nullptr,QFileDialog::DontConfirmOverwrite);
    QFileInfo CompleteNameTif(OutputTif);

    if (OutputTif.isNull()) {
        return;
    }

    sprintf(intStr,"Arquivo TIF de saída: [%s]\n",OutputTif.toStdString().c_str());
    ui->txt_Output->append(intStr);

    GeoProc.Name = OutputTif;


    cv::destroyAllWindows();

    imageRead = cv::imread(InputJPG.toStdString());
    frameW = imageRead.cols;
    frameH = imageRead.rows;

    // 600 pontos em Y
    double Rel1;
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
    } else {
        Rel1 = frameW / 600.0;
    }
    namedWindow("Imagem ICE", WINDOW_GUI_EXPANDED);
    moveWindow("Imagem ICE", 20,20);
    resizeWindow("Imagem ICE",static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow("Imagem ICE",imageRead);
    imageRead.copyTo(Geo_ICE.image);
    setMouseCallback("Imagem ICE",on_mouse_Original_JPG,NULL);


// Imagem TIF georeferenciada

    GDALAllRegister();

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(InputTif.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

    GDAL2MAT(poSrcDS,Destino);

    double Transform[6];
    pOldDS->GetGeoTransform(Transform);
    Geo.Name = InputTif;
    bool    E_UTM;
    int     Zona = 0;
    int     Hemisferio = 10;
    if (pOldDS->GetSpatialRef()->IsProjected()) {
        E_UTM = true;
        Zona = pOldDS->GetSpatialRef()->GetUTMZone(&Hemisferio); // Hemisferio (= 0 Sul), (= 1 Norte)
    } else {
        E_UTM = false;
    }
    if (!E_UTM) {  // LAT LON
        Geo.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
        Geo.Iw = pOldDS->GetRasterXSize();
        Geo.Ih = pOldDS->GetRasterYSize();
        Geo.DUTM.Reset( Transform[3] + Transform[5] * Geo.Ih,Transform[0] + Transform[1] * Geo.Iw );
        Geo.mppx = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting()) / Geo.Iw; // LON
        Geo.mppy = -abs(Geo.DUTM.Northing() - Geo.AUTM.Northing()) / Geo.Ih; // LAT
        Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
    } else { // UTM
        Geo.AUTM.Reset(Zona,Hemisferio,Transform[0],Transform[3]);
        Geo.Iw = pOldDS->GetRasterXSize();
        Geo.Ih = pOldDS->GetRasterYSize();
        Geo.mppx = Transform[1]; // LON
        Geo.mppy = Transform[5]; // LAT
        Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
    }

    Geo_Tiff = Geo;

    frameW = Destino.cols;
    frameH = Destino.rows;

    // 600 pontos em Y
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
    } else {
        Rel1 = frameW / 600.0;
    }
    namedWindow("Imagem TIF", WINDOW_GUI_EXPANDED);
    moveWindow("Imagem TIF", 700,20);
    resizeWindow("Imagem TIF",static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow("Imagem TIF",Destino);
    Destino.copyTo(Geo_Tiff.image);
    setMouseCallback("Imagem TIF",on_mouse_Original_TIF,NULL);
}


void MainWindow::on_actionThread_2_triggered()
{
    if (ui->actionThread_2->isChecked()) {
        sprintf(intStr,"\n Threads ativadas.");
    } else {
       sprintf(intStr,"\n Threads desativadas.");
    }
    ui->txt_Output->append(intStr);
}



void MainWindow::on_checkBox_KMZ_clicked()
{
    if (ui->checkBox_KMZ->isChecked()) {
        if (!ui->checkBox_GTiff->isChecked()) {
            ui->checkBox_GTiff->setChecked(true);
        }
    }
}

void MainWindow::on_checkBox_GTiff_clicked()
{
    if (ui->checkBox_KMZ->isChecked()) {
        ui->checkBox_GTiff->setChecked(true);
    }
}


void MainWindow::on_actionAbrir_triggered()
{

    dirOpen = QFileDialog::getOpenFileName(this, tr("Open Image"),"./",tr("Image (*.*)"));
    QFileInfo CompleteName(dirOpen);

    if (dirOpen == "") {
        return;
     }

    AbreArquivo(CompleteName,true,false,true);
    QString Output;
    Size_GeoTiff(dirOpen,true,Output);
}


void MainWindow::on_actionLimpar_triggered()
{
    GeoProc.Pontos.clear();
    GeoProc.PImg.clear();
    uidialogPontos->clear();

    // Limpa tabela
    ui->tablePontos->setRowCount(0);
    Calc_Dist();

    ui->lineEdit_Area->setText("");


    Destino.copyTo(GeoProc.image);
    imshow(Name_Imagem1,GeoProc.image);
}


void MainWindow::on_actionThumbnail_triggered()
{
    if (ui->actionThumbnail->isChecked()) {
        sprintf(intStr,"\n Thumbnails ativados.");
    } else {
       sprintf(intStr,"\n Thumbnails desativados.");
    }
    ui->txt_Output->append(intStr);
}


void MainWindow::on_Limpa_Quadro_clicked()
{
    ui->txt_Output->clear();
}

void MainWindow::on_actionPortugu_s_triggered()
{
    QLocale::setDefault(QLocale::Portuguese);
    ui->actionPortugu_s->setChecked(true);
    ui->actionIngl_s->setChecked(false);
    ui->txt_Output->append("\n Separação milhar configurado para Português.");
}

void MainWindow::on_actionIngl_s_triggered()
{
    QLocale::setDefault(QLocale::English);
    ui->actionPortugu_s->setChecked(false);
    ui->actionIngl_s->setChecked(true);
    ui->txt_Output->append("\n Separação milhar configurado para Inglês.");
}


void MainWindow::on_pushButton_clicked()
{
  on_actionLimpar_triggered();
}


void MainWindow::on_actionPreenche_Pol_gono_triggered()
{
    if (ui->actionPreenche_Pol_gono->isChecked()) {
        Geo.Preenche = true;
        GeoProc.Preenche = true;
        sprintf(intStr,"\n Preenchimento de Polígono ativado.");
    } else {
        Geo.Preenche = false;
        GeoProc.Preenche = false;
        sprintf(intStr,"\n Preenchimento de Polígono desativado.");
    }
    MeFu.Draw_Cel();
    ui->txt_Output->append(intStr);
}


void MainWindow::on_actionBranco_triggered()
{
    ui->actionBranco->setChecked(true);
    ui->actionPreto->setChecked(false);
    ui->txt_Output->append("\n Cor do NODATA -> BRANCO.");
}


void MainWindow::on_actionPreto_triggered()
{
    ui->actionBranco->setChecked(false);
    ui->actionPreto->setChecked(true);
    ui->txt_Output->append("\n Cor do NODATA -> PRETO.");
}

void MainWindow::on_actionSair_triggered()
{
    qApp->quit();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    qApp->quit();
}

void MainWindow::on_actionDrone_triggered()
{
    ui->actionDrone->setChecked(true);
    ui->actionGimbal->setChecked(false);
    ui->txt_Output->append("\n Referência de ângulo -> DRONE.");
}

void MainWindow::on_actionGimbal_triggered()
{
    ui->actionDrone->setChecked(false);
    ui->actionGimbal->setChecked(true);
    ui->txt_Output->append("\n Referência de ângulo -> GIMBAL.");

}


///////////////////////////////////////////////////////////////////////////////////////
/// Esta rotina é de teste. Chamada por um botão que em produção fica oculto
///
void MainWindow::on_btOpenFile_clicked()
{
    GDALAllRegister();

    int X,Y;


    GeographicLib::GeoCoords UL_Geral,RD_Geral;
    double  MinX = 1e10,MinY = 1e10,MaxX = -1e10,MaxY = -1e10;
    double Cx = 0,Cy = 0;


    bool Print = true;

    Informacao.Merge.clear();
    Info_Merge  Inf;
 //////////////////////
    for (int i = 0; i < Informacao.ListadeArquivosProc.length() ; i++) {
        GDALDataset *pOldDS;
        QString InputFile = Informacao.ListadeArquivosProc.at(i).absoluteFilePath()   ;
        Inf.FileName = InputFile;
        pOldDS = (GDALDataset *) GDALOpen(InputFile.toStdString().c_str(),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);

        double      Transform[6];
        double minx,miny,maxx,maxy,width,height,cx,cy,TamX,TamY,Area;
        GeographicLib::GeoCoords UL,RD;

        pOldDS->GetGeoTransform(Transform);

        X = poSrcDS->GetRasterXSize();
        Y = poSrcDS->GetRasterYSize();

        // Verifica se possui informação geográfica
        if ((Transform[0] == 0) && (Transform[1] == 1) &&
            (Transform[2] == 0) && (Transform[3] == 0) &&
            (Transform[4] == 0) && (Transform[5] == 1) ) {
            Geo_Img.DimX = X;                           // Dimensão em X
            Geo_Img.DimY = Y;                           // Dimensão em Y
            Geo_Img.Area = 0;
            GDALClose(pOldDS);
            return;
        }

        Geo_Img.DimX = X;                           // Dimensão em X
        Geo_Img.DimY = Y;                           // Dimensão em Y
        Geo_Img.Iw = X;
        Geo_Img.Ih = Y;


        bool    E_UTM;
        int     Zona = 0;
        int     Hemisferio = 10;
        if (pOldDS->GetSpatialRef()->IsProjected()) {
            E_UTM = true;
            Zona = pOldDS->GetSpatialRef()->GetUTMZone(&Hemisferio); // Hemisferio (= 0 Sul), (= 1 Norte)
        } else {
            E_UTM = false;
        }
        if (!E_UTM) {  // LAT LON
            width = X;
            height = Y;
            minx = Transform[0];
            miny = Transform[3] + width * Transform[4] + height * Transform[5];
            maxx = Transform[0] + width * Transform[1] + height * Transform[2];
            maxy = Transform[3];
            cx = minx + (maxx - minx)/2;
            cy = miny + (maxy - miny)/2;
            UL.Reset(maxy,minx);
            RD.Reset(miny,maxx);
            TamX = (fabs(UL.Easting() - RD.Easting()) * 100.0) / X;
            TamY = (fabs(UL.Northing() - RD.Northing()) * 100.0) / Y;
            Geo_Img.AUTM.Reset(maxy,minx);
            Geo_Img.DUTM.Reset(miny,maxx);
            Geo_Img.mppx = abs(Geo_Img.DUTM.Easting() - Geo_Img.AUTM.Easting()) / Geo_Img.Iw; // LON
            Geo_Img.mppy = -abs(Geo_Img.DUTM.Northing() - Geo_Img.AUTM.Northing()) / Geo_Img.Ih; // LAT
        } else { // UTM
            Geo_Img.AUTM.Reset(Zona,Hemisferio,Transform[0],Transform[3]);
            Geo_Img.mppx = Transform[1]; // LON
            Geo_Img.mppy = Transform[5]; // LAT

            Geo_Img.CcUTM.Reset(Geo_Img.AUTM.Zone(),Geo_Img.AUTM.Northp(),Geo_Img.AUTM.Easting() + Geo_Img.mppx * (Geo_Img.Iw /2),Geo_Img.AUTM.Northing() + Geo_Img.mppy * (Geo_Img.Ih /2));
            Geo_Img.DUTM.Reset(Geo_Img.AUTM.Zone(),Geo_Img.AUTM.Northp(),Geo_Img.AUTM.Easting() + Geo_Img.mppx * Geo_Img.Iw,Geo.AUTM.Northing() + Geo_Img.mppy * Geo_Img.Ih);
            TamX = (fabs(Geo_Img.AUTM.Easting() - Geo_Img.DUTM.Easting()) * 100.0) / X;
            TamY = (fabs(Geo_Img.AUTM.Northing() - Geo_Img.DUTM.Northing()) * 100.0) / Y;
            cy = Geo_Img.CcUTM.Latitude();
            cx = Geo_Img.CcUTM.Longitude();
            maxy = Geo_Img.AUTM.Latitude();
            minx = Geo_Img.AUTM.Longitude();
            miny = Geo_Img.DUTM.Latitude();
            maxx = Geo_Img.DUTM.Longitude();
        }

        Area = Geo_Img.Dw * Geo_Img.Dh;

        Geo_Img.Name = InputFile;                   // Nome do Arquivo
        Geo_Img.Area = Area;                        // Área em metros quadrados
        Geo_Img.cmppx = TamX;                       // em cm/px
        Geo_Img.cmppy = TamY;


        Geo_Img.Cc.real(cy);                        // Coordenada LAT do centro da imagem
        Geo_Img.Cc.imag(cx);                        // Coordenada LON do centro da imagem

        if (minx < MinX) MinX = minx;
        if (miny < MinY) MinY = miny;
        if (maxx > MaxX) MaxX = maxx;
        if (maxy > MaxY) MaxY = maxy;

        GDALClose(pOldDS);

        Inf.Lat = maxy;
        Inf.Lon = minx;
        Informacao.Merge.append(Inf);
    }

    Geo_Img.A.real(MaxY);                       // Coordenada LAT do canto superior esquerdo
    Geo_Img.A.imag(MinX);
    Geo_Img.D.real(MinY);                       // Coordenada LAT do canto inferior direito
    Geo_Img.D.imag(MaxX);
    Geo_Img.AUTM.Reset(MaxY,MinX);
    Geo_Img.DUTM.Reset(MinY,MaxX);
    Geo_Img.Dw = fabs(Geo_Img.AUTM.Easting() - Geo_Img.DUTM.Easting());                  // Tamanho horizontal no terreno em metros
    Geo_Img.Dh = fabs(Geo_Img.AUTM.Northing() - Geo_Img.DUTM.Northing());                // Tamanho vertical no terreno em metros

    X = (fabs(Geo_Img.AUTM.Easting() - Geo_Img.DUTM.Easting()) * 100.0) / Geo_Img.cmppx;
    Y = (fabs(Geo_Img.AUTM.Northing() - Geo_Img.DUTM.Northing()) * 100.0) / Geo_Img.cmppy;


    double PassoX,PassoY;
    PassoX = X;
    PassoX = PassoX / fabs(MaxX - MinX);
    PassoY = Y;
    PassoY = PassoY / fabs(MaxY - MinY);
            // Faz o Merge aqui
    cv::Mat Img_Total(Y,X,CV_8UC3,Scalar(255,255,255));


    ui->progressBar_Mesclar->setMaximum(Informacao.Merge.length() - 1);
    ui->progressBar_Mesclar->setValue(0);

    for (int i = 0; i < Informacao.Merge.length() ; i++) {
        cv::Mat img = cv::imread(Informacao.Merge.at(i).FileName.toStdString());
        cv::Mat MaskImage;
        cvtColor(img,MaskImage,COLOR_BGR2GRAY);
        int PosX,PosY;
        double Lat,Lon;

        Lat = Informacao.Merge.at(i).Lat;
        Lon = Informacao.Merge.at(i).Lon;

        PosY = PassoY * fabs(Lat - MaxY);
        PosX = PassoX * fabs(Lon - MinX);

        if ((PosY + img.rows) > Img_Total.rows) {
            PosY = Img_Total.rows - img.rows;
        }
        if ((PosX + img.cols) > Img_Total.cols) {
            PosX = Img_Total.cols - img.cols;
        }

        img.copyTo(Img_Total(Rect(PosX,PosY,img.cols ,img.rows)),MaskImage != 255);

        ui->progressBar_Mesclar->setValue(i);
    }

    imshow("T",Img_Total);

    if (Print) {
        QLocale Br;
        QString S1,S2;

        ui->txt_Output->append("-------------------\n Informações sobre Merge ") ;
/*
        sprintf(intStr,"     Dimensão da imagem: %d x %d",X,Y);
        ui->txt_Output->append(intStr);

        S1 = Br.toString(TamX,'f',2);
        S2 = Br.toString(TamY,'f',2);
        sprintf(intStr,"     Resolução espacial: %s cm/px",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);

        S1 = Br.toString(TamX,'f',2);
        S2 = Br.toString(TamY,'f',2);
*/
        sprintf(intStr,"     Bordas: (%s ; %s) - (%s ; %s). Centro: (%s ; %s).",(Br.toString(MaxY,'f',5)).toStdString().c_str(),
                                                                          (Br.toString(MinX,'f',5)).toStdString().c_str(),
                                                                          (Br.toString(MinY,'f',5)).toStdString().c_str(),
                                                                          (Br.toString(MaxX,'f',5)).toStdString().c_str(),
                                                                          (Br.toString(Cy,'f',5)).toStdString().c_str(),
                                                                          (Br.toString(Cx,'f',5)).toStdString().c_str());

        ui->txt_Output->append(intStr);
/*
        S1 = Br.toString(static_cast<float> (Area),'f',0);
        sprintf(intStr,"     Área: %s metros quadrados\n",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);
*/
    }

}

void MainWindow::on_actionGDAL_triggered()
{
    ui->actionGDAL->setChecked(true);
    ui->actionInterno->setChecked(false);
    ui->txt_Output->append("\n Algoritmo de mesclagem -> GDAL.");
    ui->progressBar_Mesclar->setVisible(false);
}

void MainWindow::on_actionInterno_triggered()
{
    ui->actionGDAL->setChecked(false);
    ui->actionInterno->setChecked(true);
    ui->txt_Output->append("\n Algoritmo de mesclagem -> INTERNO.");
    ui->progressBar_Mesclar->setVisible(true);

}

void MainWindow::on_actionBordas_na_imagens_processadas_triggered()
{
    if (ui->actionBordas_na_imagens_processadas->isChecked()) {
        ui->txt_Output->append("\n Gerar bordas nas imagens processadas -> ATIVADO.");
    } else {
        ui->txt_Output->append("\n Gerar bordas nas imagens processadas -> DESATIVADO.");
    }
}

void MainWindow::getEXIF_XML_Info(QString File)
{

//    GDALAllRegister();

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(File.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

    QString Result;
    Result = File;
    Result = Result + "\n\n\nDados EXIF:\n";
    // EXIF
    char ** papszIter;
    papszIter = papszMetadata;
    while(papszIter && *papszIter)
    {
        Result = Result + *papszIter + "\n";
        papszIter++;
    }

    QStringList stringList;

    DecodeXMP(stringList,poSrcDS);

    Result = Result + "\n Dados XMP:\n";
    // XMP
    for (int i = 0 ; i <stringList.length() ; i++) {
        Result = Result + stringList.at(i) + "\n";// + "--->>> " + QString::number(stringList.at(i).length()) + "\n";
//        ui->txt_Output->append(stringList.at(i));
    }
        GDALClose(pOldDS);
//        ui->txt_Output-> append("\n " + Result);
        Geometria_Img.ExifXMPInfo = Result;
}

void MainWindow::on_bt_EXIF_XMP_List_clicked()
{

    if (ui->list_Files->selectedItems().length() != 1) {
        return;
    }

    QListWidgetItem *item = ui->list_Files->selectedItems().at(0);
    getEXIF_XML_Info(Informacao.Diretorio + "/" + item->text());

    dialogEXIF  = new DialogEXIF(this);
    dialogEXIF->show();
    dialogEXIF->setWindowTitle("EXIF-XMP information");
    dialogEXIF->print(Geometria_Img.ExifXMPInfo);
}

void MainWindow::get_Info_Img() {

    if (ui->list_Files->selectedItems().length() != 1) {
        return;
    }

    QListWidgetItem *item = ui->list_Files->selectedItems().at(0);
    getEXIF_XML_Info(Informacao.Diretorio + "/" + item->text());

    dialogEXIF  = new DialogEXIF(this);
    dialogEXIF->show();
    dialogEXIF->setWindowTitle("EXIF-XMP information");
    dialogEXIF->print(Geometria_Img.ExifXMPInfo);

    ui->actionEXIF_XMP->setEnabled(true);
}

void MainWindow::get_Info_Proc() {

    if (ui->list_Files_Proc->selectedItems().length() != 1) {
        return;
    }

    QListWidgetItem *item = ui->list_Files_Proc->selectedItems().at(0);
    getEXIF_XML_Info(Informacao.DiretorioProc + "/" + item->text());

    QString Output;
    Size_GeoTiff(Informacao.DiretorioProc + "/" + item->text(),false,Output);
    Geometria_Img.ExifXMPInfo = Geometria_Img.ExifXMPInfo + Output;
    dialogEXIF  = new DialogEXIF(this);
    dialogEXIF->show();
    dialogEXIF->setWindowTitle("EXIF-XMP information");
    dialogEXIF->print(Geometria_Img.ExifXMPInfo);

    ui->actionEXIF_XMP->setEnabled(true);
}



void MainWindow::Grava_Thumb(QString FileName)
{
    QFileInfo  Tiff_Writed(FileName);
    QString    Dir_Tiff;

    Dir_Tiff = Tiff_Writed.path();

#ifdef INSERE_THUMB
           QString pathFileImageTemp = Dir_Tiff + "/" + Tiff_Writed.baseName() + "-thumb.jpg";
#else
           QString pathFileImageTemp = Informacao.DiretorioProc + "/Thumb" + "/" + Tiff_Writed.baseName() + "-thumb.jpg";
#endif
           QPixmap pix;
           int TSizex = 130;

           pix = MAT2PIX(FileName);

           pix = pix.scaled(TSizex,TSizex,Qt::KeepAspectRatio);
           pix.save(pathFileImageTemp,"JPG");
#ifdef INSERE_THUMB
           //Inserir Thumbnail
               QProcess processThumbnails;
#ifdef QT_DEBUG
               QString command2 = "c:/Dev/Exiv2/bin/Release/exiv.exe"; //exiv2.exe              // Para Desenvolvimento
#else
               QString command2 = Informacao.Diretorio_EXE + "/exiv2.exe"; //exiv2.exe                // Para Produção
#endif
               QString args2;
               QString pathThumbnailsTemp;
               args2 = '"'+ command2 + '"';
               args2 = args2 + " -it ";
               args2 = args2 + '"' + FileName + '"';


               processThumbnails.start(args2);
               processThumbnails.waitForFinished(-1);

               QString out, err;
               out = processThumbnails.readAllStandardOutput();
               err = processThumbnails.readAllStandardError();

               if ((out != "")) {
                   ui->txt_Output->append(" Exif -> " + out);
               }
               if ((err != "")) {
                   ui->txt_Output->append(" Erro Exif -> " + err);
               }



               QFile Old_t(Dir_Tiff + "/thumb/" + Tiff_Writed.baseName() + "-thumb.jpg");
               if (Old_t.exists()) {
                   Old_t.remove(Dir_Tiff + "/thumb/" + Tiff_Writed.baseName() + "-thumb.jpg");
               }

               QDir f(Dir_Tiff);
               f.rename(pathFileImageTemp,"thumb/" + Tiff_Writed.baseName() + "-thumb.jpg" );

#endif

}


void MainWindow::Rotaciona_Imagem(cv::Mat imageRead, cv::Mat &Destino)
{
    Geometria_ImgProc = Geo;
    double angle = -Geometria_ImgProc.Gimbal_Angle; //-Info_Img.GeoLocation.Direction_Gimbal;

    Rotaciona(imageRead,Destino,angle);

    double xl,yl, GSDm, Iw2, Ih2;
//    double sin_alfa,cos_alfa;

    Geometria_ImgProc.Iw = Destino.cols;
    Geometria_ImgProc.Ih = Destino.rows;
    Iw2 = Destino.cols / 2;
    Ih2 = Destino.rows / 2;

    GSDm = Geometria_ImgProc.GSD / 100.0;

    xl = Geometria_ImgProc.CcUTM.Easting() - GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() - GSDm * Ih2;
    Geometria_ImgProc.CUTM.Reset(Geometria_ImgProc.CcUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);

    xl = Geometria_ImgProc.CcUTM.Easting() + GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() + GSDm * Ih2;
    Geometria_ImgProc.BUTM.Reset(Geometria_ImgProc.BUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);

    xl = Geometria_ImgProc.CcUTM.Easting() - GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() + GSDm * Ih2;
    Geometria_ImgProc.AUTM.Reset(Geometria_ImgProc.CcUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);

    xl = Geometria_ImgProc.CcUTM.Easting() + GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() - GSDm * Ih2;
    Geometria_ImgProc.DUTM.Reset(Geometria_ImgProc.BUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);
}


void MainWindow::Gera_Retangulo() {
    int                     Xmin,Ymin,Xmax,Ymax;
    std::complex<double>    Ponto,P;
    double                  PosLat,PosLon;
    QList< std::complex<double> >   Pontos_TMP;
    QList< std::complex<int> >      PImg_TMP;


    if (GeoProc.PImg.length() != 2) return;

    Xmin = 1e8;
    Ymin = 1e8;
    Xmax = -1e8;
    Ymax = -1e8;

    for (int i = 0 ; i < GeoProc.PImg.length() ; i ++ ) {
        if (Xmin > GeoProc.PImg.at(i).real()) Xmin = GeoProc.PImg.at(i).real();
        if (Xmax < GeoProc.PImg.at(i).real()) Xmax = GeoProc.PImg.at(i).real();
        if (Ymin > GeoProc.PImg.at(i).imag()) Ymin = GeoProc.PImg.at(i).imag();
        if (Ymax < GeoProc.PImg.at(i).imag()) Ymax = GeoProc.PImg.at(i).imag();
    }

    Ponto.imag(Ymin);
    Ponto.real(Xmax);
    GeoProc.PImg.insert(1,Ponto);
    PosLat = GeoProc.AUTM.Northing() + Ymin * GeoProc.mppy;
    PosLon = GeoProc.AUTM.Easting() + Xmax * GeoProc.mppx;  // Eixo x é longitude
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);
    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());
    GeoProc.Pontos.insert(1,P);

    Ponto.imag(Ymax);
    Ponto.real(Xmin);
    GeoProc.PImg.push_back(Ponto);
    PosLat = GeoProc.AUTM.Northing() + Ymax * GeoProc.mppy;
    PosLon = GeoProc.AUTM.Easting() + Xmin * GeoProc.mppx;  // Eixo x é longitude
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);
    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());
    GeoProc.Pontos.push_back(P);

    for ( int i = 0 ; i < 4 ; i++) {
        Pontos_TMP.push_back(GeoProc.Pontos.at(i));
        PImg_TMP.push_back(GeoProc.PImg.at(i));
    }

    GeoProc.Pontos.clear();
    GeoProc.PImg.clear();
    uidialogPontos->clear();

    for ( int i = 0 ; i < 4 ; i++) {
        GeoProc.Pontos.push_back(Pontos_TMP.at(i));
        GeoProc.PImg.push_back(PImg_TMP.at(i));
        MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),true);
     }
}

void MainWindow::on_bt_Mostar_Imagem_clicked()
{
    ::ShowWindow(GeoProc.hParent,SW_RESTORE);
    imshow(Name_Imagem1,GeoProc.image);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_actionDesativado_triggered()
{
        ui->actionDesativado->setChecked(true);
        ui->txt_Output->append("\n Mostrar coodenadas centrais -> DESATIVADO.");
        ui->actionOriginais->setChecked(false);
        ui->actionProcessadas->setChecked(false);

        if (actShowCoordProc != NULL) actShowCoordProc->setIcon(iconCheckpixOff);
        if (actShowCoord != NULL) actShowCoord->setIcon(iconCheckpixOff);

        Coords.Tipo = 0;
        MeFu.Draw_Cel();
}


void MainWindow::on_actionOriginais_triggered()
{
    ui->txt_Output->append("\n Mostrar coodenadas centrais -> Imagens da lista ORIGINAIS.");
    ui->actionDesativado->setChecked(false);
    ui->actionProcessadas->setChecked(false);

    actShowCoord->setIcon(iconCheckpix);
    actShowCoordProc->setIcon(iconCheckpixOff);

    Coords.Tipo = 1;
    MeFu.Draw_Cel();
}

void MainWindow::on_actionProcessadas_triggered()
{
    ui->txt_Output->append("\n Mostrar coodenadas centrais -> Imagens da lista PROCESSSADOS.");
    ui->actionDesativado->setChecked(false);
    ui->actionOriginais->setChecked(false);

    actShowCoordProc->setIcon(iconCheckpix);
    actShowCoord->setIcon(iconCheckpixOff);


    Coords.Tipo = 2;
    MeFu.Draw_Cel();
}

Plot_Coords::Plot_Coords() {
    Tipo = 0; // Sem Mostrar as coordenadas centrais na imagem
}

void MainWindow::ShowCoord() {
    if (ui->actionOriginais->isChecked()) {
        ui->actionDesativado->setChecked(true);
        on_actionDesativado_triggered();
    } else {
        ui->actionOriginais->setChecked(true);
        on_actionOriginais_triggered();
    }
}

void MainWindow::ShowCoordProc() {
    if (ui->actionProcessadas->isChecked()) {
        ui->actionDesativado->setChecked(true);
        on_actionDesativado_triggered();
    } else {
        ui->actionProcessadas->setChecked(true);
        on_actionProcessadas_triggered();
    }
}

void MainWindow::on_actionRaio_30_pts_triggered()
{
    bool    ok;
    int    Valor_A;
    Valor_A = QInputDialog::getInt(NULL,"Raio (em pts)",
                                        "Valor:", MeFu.Valor_Raio_Marcacao_Coordenadas(),10,100,1,&ok);

    if (!ok) return;


    QString text = "Raio: " + QString::number(Valor_A) + " pts";
    ui->actionRaio_30_pts->setText(text);

    MeFu.Atualiza_Raio_Marcacao_Coordenadas(Valor_A);

    sprintf(intStr,"\n Raio do círculo definido para %d pontos",Valor_A);
    ui->txt_Output->append(intStr);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    MeFu.Draw_Cel();
}


void MainWindow::on_bt_Teste_Cancel(void)
{
    ::ShowWindow(Geo_Referencia.hParent,SW_MINIMIZE);
    ui->actionMostrar->setChecked(false);
    ui->actionUsar->setChecked(false);
}

void MainWindow::on_bt_Teste_clicked()
{
///// Lê imagem de referência -> primeiro deve ser maior (em termos de coordenadas) que a mesclada, depois implementar o contrário

/// Abre Dialog
    dialogBlend = new Dialog_Blend(this);
    dialogBlend->raise();
    dialogBlend->setWindowTitle("Blend");
    dialogBlend->Ajusta_Parametros();
    dialogBlend->Print_Slider();

    connect(dialogBlend, SIGNAL(Alterado()), this, SLOT(Valor_Slider_Changed()));
    connect(dialogBlend, SIGNAL(Sobrepor()),this,SLOT(Calc_Sobrepor_Images()));
    connect(dialogBlend, SIGNAL(Blend()),this,SLOT(Calc_Blend_Images()));
    connect(dialogBlend, SIGNAL(Cancel_Blend()),this,SLOT(on_bt_Teste_Cancel()));
    connect(dialogBlend,SIGNAL(Usar_Blend()),this,SLOT())
;
//   Referencia.Name = Informacao.DiretorioResult + "/" + "Google1.tif";
    Geo_Referencia.Name = Informacao.DiretorioResult + "/" + "SPS_Google1.tif";
    Geo_Referencia.Complete_Name = QFileInfo(Geo_Referencia.Name);
    ui->txt_Output->append("\n Arquivo: " + Geo_Referencia.Name);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    Le_Geo_from_file2(Geo_Referencia);
    ui->txt_Output->append(" -> Lido.");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

   Geo_ICE.Name = Informacao.DiretorioResult + "/" + "GEO_SPS_2.tif";
   Geo_ICE.Complete_Name = QFileInfo(Geo_ICE.Name);
   ui->txt_Output->append("\n Arquivo: " + Geo_ICE.Name);
   QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
   Le_Geo_from_file2(Geo_ICE);
   ui->txt_Output->append(" -> Lido.");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

   dialogBlend->show();
   QString Output;
   Size_GeoTiff(Geo_Referencia.Name,false,Output);
   dialogBlend->Print(Output);

   Output = "\n";
   Size_GeoTiff(Geo_ICE.Name,false,Output);
   dialogBlend->Print(Output);
   dialogBlend->Print_File1(Geo_Referencia.Name);
   dialogBlend->Print_File2(Geo_ICE.Name);

   namedWindow("TIF Referencia", WINDOW_GUI_EXPANDED);
   moveWindow("TIF Referencia", 20,20);
   resizeWindow("TIF Referencia",900,700);
   setMouseCallback("TIF Referencia",on_mouse_Referencia_TIF,NULL);


   if (dialogBlend->Modo_de_Operacao()) Calc_Blend_Images(); else Calc_Sobrepor_Images();
}


void MainWindow::Le_Geo_from_file2(Geometria_Imagem &Ref)
{
    bool        GeoReff = false;

    GDALAllRegister();

//    GDALDataset *pOldDS;
    Ref.FileDS = (GDALDataset *) GDALOpen(Ref.Name.toStdString().c_str(),GA_ReadOnly);
//    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);

    Ref.image = imread(Ref.Name.toStdString(),IMREAD_COLOR);

    double Transform[6];
    Ref.FileDS->GetGeoTransform(Transform);
//    char ValorStr[100],InputStr[100];

    // Verifica se possui informação geográfica
    if ((Transform[0] == 0) && (Transform[1] == 1) &&
        (Transform[2] == 0) && (Transform[3] == 0) &&
        (Transform[4] == 0) && (Transform[5] == 1) ) {

        // Erro -> Não possui informação geográfica

        return;

    } else { // Possui
        GeoReff = true;

        bool    E_UTM;
        int     Zona = 0;
        int     Hemisferio = 10;
        if (Ref.FileDS->GetSpatialRef()->IsProjected()) {
            E_UTM = true;
            Zona = Ref.FileDS->GetSpatialRef()->GetUTMZone(&Hemisferio); // Hemisferio (= 0 Sul), (= 1 Norte)
        } else {
            E_UTM = false;
        }
        if (!E_UTM) {  // LAT LON
            Ref.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
            Ref.Iw = Ref.FileDS->GetRasterXSize();
            Ref.Ih = Ref.FileDS->GetRasterYSize();
            Ref.DUTM.Reset( Transform[3] + Transform[5] * Ref.Ih,Transform[0] + Transform[1] * Ref.Iw );
            Ref.Dw = abs(Ref.DUTM.Easting() - Ref.AUTM.Easting());
            Ref.Dh = abs(Ref.DUTM.Northing() - Ref.AUTM.Northing());
            Ref.mppx = abs(Ref.DUTM.Easting() - Ref.AUTM.Easting()) / Ref.Iw; // LON
            Ref.mppy = -abs(Ref.DUTM.Northing() - Ref.AUTM.Northing()) / Ref.Ih; // LAT
            Ref.CcUTM.Reset(Ref.AUTM.Zone(),Ref.AUTM.Northp(),Ref.AUTM.Easting() + Ref.mppx * (Ref.Iw /2),Ref.AUTM.Northing() + Ref.mppy * (Ref.Ih /2));
            Ref.A.real(Ref.AUTM.Latitude());
            Ref.A.imag(Ref.AUTM.Longitude());
            Ref.D.real(Ref.DUTM.Latitude());
            Ref.D.imag(Ref.DUTM.Longitude());
        } else { // UTM
            Ref.AUTM.Reset(Zona,Hemisferio,Transform[0],Transform[3]);
            Ref.Iw = Ref.FileDS->GetRasterXSize();
            Ref.Ih = Ref.FileDS->GetRasterYSize();
            Ref.mppx = Transform[1]; // LON
            Ref.mppy = Transform[5]; // LAT
            Ref.CcUTM.Reset(Ref.AUTM.Zone(),Ref.AUTM.Northp(),Ref.AUTM.Easting() + Ref.mppx * (Ref.Iw /2),Ref.AUTM.Northing() + Ref.mppy * (Ref.Ih /2));
        }
    }

    Ref.Cor_Nodata = Ref.FileDS->GetRasterBand(1)->GetNoDataValue();
    if (!GeoReff) {
        // Erro -> Não possui informação geográfica
        return;
     }


    GDALClose(Ref.FileDS);
}


void MainWindow::Get_Info_Window(QString NameWindow, Geometria_Imagem &Geometria) {
    std::string s;

    s = NameWindow.toStdString();
    std::wstring stemp = std::wstring(s.begin(),s.end());
    LPCWSTR sw = stemp.c_str();
    Geometria.hParent = (HWND)FindWindow(NULL,sw);
    Geometria.hWnd = (HWND)FindWindowEx(Geometria.hParent,NULL,L"HightGUI class",NULL);
}

bool MainWindow::Contido_Referencia(Geometria_Imagem &Geometria_Ref,Geometria_Imagem Geometria) {
    double    MaxX,MaxY,MinX,MinY;

    MinX = Geometria_Ref.AUTM.Longitude();
    MinY = Geometria_Ref.DUTM.Latitude();
    MaxX = Geometria_Ref.DUTM.Longitude();
    MaxY = Geometria_Ref.AUTM.Latitude();

    if (!((Geometria.A.imag() > MinX) && (Geometria.A.real() < MaxY) && (Geometria.D.imag() < MaxX) && (Geometria.D.real() > MinY))) {
        sprintf(intStr,"\n Erro! [%s] não está contida na imagem de referência.",Geometria.Name.toStdString().c_str());
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append(intStr);
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        QMessageBox::information(this,tr("Erro"),"Não está contida na imagem de referência");

//        Get_Info_Window("TIF Referencia",Geometria_Ref);

        ::ShowWindow(Geometria_Ref.hParent,SW_MINIMIZE);
        dialogBlend->close();
        ui->actionUsar->setChecked(false);
        return false; // Não está contido
    }

    return true;
}


void MainWindow::Valor_Slider_Changed() {
// Mistura com diferente gau de transparência
   double alpha,beta;
   Mat A;

   alpha = 0.01 * dialogBlend->Get_Value();

   beta = ( 1.0 - alpha );
   addWeighted( Blend_Calculada.image, alpha, Geo_Referencia.image, beta, 0.0, A);
   imshow("TIF Referencia",A);
}


void MainWindow::Valor_Slider_Changed(cv::Mat &Image) {
    double alpha,beta;
    Mat A;

    alpha = 0.01 * dialogBlend->Get_Value();

    beta = ( 1.0 - alpha );
    addWeighted( Blend_Calculada.image, alpha, Geo_Referencia.image, beta, 0.0, A);
    imshow("TIF Referencia",A);
    A.copyTo(Image);
}

void MainWindow::Calc_Blend_Images(void) { // Tem que racalcular e colocar no Geo_ICE os novos valores devido a mudanca de resolução
    double    X,Y,Z,Kx,Ky,MaxX,MaxY,MinX,MinY,PassoX,PassoY;

    if (Calculou_Blend) {
        Valor_Slider_Changed();
        return;
    }

    if (!Contido_Referencia(Geo_Referencia,Geo_ICE)) return;

    Blend_Calculada.image = Geo_ICE.image.clone();

    X = Geo_Referencia.Dw;
    Y = Geo_ICE.Dw;
    Z = Geo_ICE.Iw / Geo_ICE.Ih;
    Kx = Geo_ICE.Dw * Geo_Referencia.Iw / Geo_Referencia.Dw;
    Ky = Geo_ICE.Dh * Geo_Referencia.Ih / Geo_Referencia.Dh;

    cv::resize(Blend_Calculada.image,Blend_Calculada.image,Size(Kx,Ky));

    X = (fabs(Geo_Referencia.AUTM.Easting() - Geo_Referencia.DUTM.Easting())) / Geo_Referencia.mppx;
    Y =  - (fabs(Geo_Referencia.AUTM.Northing() - Geo_Referencia.DUTM.Northing())) / Geo_Referencia.mppy;
    MinX = Geo_Referencia.AUTM.Longitude();
    MinY = Geo_Referencia.DUTM.Latitude();
    MaxX = Geo_Referencia.DUTM.Longitude();
    MaxY = Geo_Referencia.AUTM.Latitude();

    PassoX = X;
    PassoX = PassoX / fabs(MaxX - MinX);
    PassoY = Y;
    PassoY = PassoY / fabs(MaxY - MinY);

    int  Cor_NoDATA = Geo_ICE.Cor_Nodata;
    cv::Mat Img_Total(Y,X,CV_8UC3,Scalar(0,0,0));

    cv::Mat MaskImage;
    cvtColor(Blend_Calculada.image,MaskImage,COLOR_BGR2GRAY);
    int PosX,PosY;
    double Lat,Lon;

    Lat = Geo_ICE.AUTM.Latitude();
    Lon = Geo_ICE.AUTM.Longitude();

    PosY = PassoY * fabs(Lat - MaxY);
    PosX = PassoX * fabs(Lon - MinX);

    if ((PosY + Geo_ICE.image.rows) > Geo_Referencia.image.rows) {
        PosY = Geo_Referencia.image.rows - Geo_ICE.image.rows;
    }
    if ((PosX + Geo_ICE.image.cols) > Geo_Referencia.image.cols) {
        PosX = Geo_Referencia.image.cols - Geo_ICE.image.cols;
    }

    Blend_Calculada.image.copyTo(Img_Total(Rect(PosX,PosY,Geo_ICE.image.cols ,Geo_ICE.image.rows)),MaskImage != Geo_ICE.Cor_Nodata);
    Blend_Calculada.image = Img_Total.clone();
    Calculou_Blend = true;
    Valor_Slider_Changed();
}


void MainWindow::Show_Sobrepor_Images(void) {
    imshow("TIF Referencia",Sobrepor_Calculada.image);
}


void MainWindow::Calc_Sobrepor_Images(void) {
    double    X,Y,Z,Kx,Ky,MaxX,MaxY,MinX,MinY,PassoX,PassoY;

    if (Calculou_Sobrepor) {
        Show_Sobrepor_Images();
        return;
    }

    // Testa de a imagem processada está contida na imagem de referência. Se não estiver -> erro    

    if (!Contido_Referencia(Geo_Referencia,Geo_ICE)) return;

    MinX = Geo_Referencia.AUTM.Longitude();
    MinY = Geo_Referencia.DUTM.Latitude();
    MaxX = Geo_Referencia.DUTM.Longitude();
    MaxY = Geo_Referencia.AUTM.Latitude();


    X = Geo_Referencia.Dw;
    Y = Geo_ICE.Dw;
    Z = Geo_ICE.Iw / Geo_ICE.Ih;
    Kx = Geo_ICE.Dw * Geo_Referencia.Iw / Geo_Referencia.Dw;
    Ky = Geo_ICE.Dh * Geo_Referencia.Ih / Geo_Referencia.Dh;

    cv::resize(Geo_ICE.image,Geo_ICE.image,Size(Kx,Ky));

    X = (fabs(Geo_Referencia.AUTM.Easting() - Geo_Referencia.DUTM.Easting())) / Geo_Referencia.mppx;
    Y =  - (fabs(Geo_Referencia.AUTM.Northing() - Geo_Referencia.DUTM.Northing())) / Geo_Referencia.mppy;

    PassoX = X;
    PassoX = PassoX / fabs(MaxX - MinX);
    PassoY = Y;
    PassoY = PassoY / fabs(MaxY - MinY);

    cv::Mat MaskImage;
    cvtColor(Geo_ICE.image,MaskImage,COLOR_BGR2GRAY);
    int PosX,PosY;
    double Lat,Lon;

    Lat = Geo_ICE.AUTM.Latitude();
    Lon = Geo_ICE.AUTM.Longitude();

    PosY = PassoY * fabs(Lat - MaxY);
    PosX = PassoX * fabs(Lon - MinX);

    if ((PosY + Geo_ICE.image.rows) > Geo_Referencia.image.rows) {
        PosY = Geo_Referencia.image.rows - Geo_ICE.image.rows;
    }
    if ((PosX + Geo_ICE.image.cols) > Geo_Referencia.image.cols) {
        PosX = Geo_Referencia.image.cols - Geo_ICE.image.cols;
    }

    Mat A = Geo_Referencia.image.clone();

    Geo_ICE.image.copyTo(A(Rect(PosX,PosY,Geo_ICE.image.cols ,Geo_ICE.image.rows)),MaskImage != Geo_ICE.Cor_Nodata);

    Calculou_Sobrepor = true;
    Sobrepor_Calculada.image = A;
    Show_Sobrepor_Images();
}

void MainWindow::Blend_Ok(void) {
    if (dialogBlend->Modo_de_Operacao()) {
       Geo_Referencia.CopyTo(GeoProc,Blend_Calculada.image);
       Valor_Slider_Changed(ImgWindowPontos);
    } else {
        Geo_Referencia.CopyTo(GeoProc, Sobrepor_Calculada.image);
        Sobrepor_Calculada.image.copyTo(ImgWindowPontos);
    }
    cv::destroyWindow("TIF Referencia");
//    cv::destroyWindow(Name_Imagem1);
    imshow(Name_Imagem1 ,GeoProc.image);
    MeFu.Draw_Cel();
}

Mat MainWindow::equalizeIntensity(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat ycrcb;

        cvtColor(inputImage,ycrcb,COLOR_BGR2YCrCb);

        vector<Mat> channels;
        split(ycrcb,channels);

        equalizeHist(channels[0], channels[0]);

        Mat result;
        merge(channels,ycrcb);

        cvtColor(ycrcb,result,COLOR_YCrCb2BGR);

        return result;
    }
    return Mat();
}


void MainWindow::on_actionCarregar_triggered()
{
    QString         InputTif;
    int             No_Data;

    InputTif = QFileDialog::getOpenFileName(this, tr("Imagem TIF de fundo"),Informacao.DiretorioResult,tr("Image (*.tif)"));
    if (InputTif == "") {
        ui->actionUsar->setChecked(false);
        ui->actionUsar->setEnabled(false);
        return;
     }


    if (!Eh_GeoTiff(InputTif,No_Data)) {
        sprintf(intStr,"\n Erro! [%s] não é um arquivo GeoTiff.",InputTif.toStdString().c_str());
//        ui->txt_Output->setStyleSheet("QTextBrowser { color : red }");
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append(intStr);
        ui->txt_Output->setTextColor(tc);
        ui->actionUsar->setChecked(false);
        ui->actionUsar->setEnabled(false);
        ui->actionMostrar->setEnabled(false);
        return;
    }

    Geo_Referencia.Name = InputTif;
    ui->actionUsar->setEnabled(true);
    ui->actionUsar->setChecked(true);
    ui->actionMostrar->setEnabled(true);

    //   Referencia.Name = Informacao.DiretorioResult + "/" + "Google1.tif";
    //    Geo_Referencia.Name = Informacao.DiretorioResult + "/" + "SPS_Google1.tif";


        Geo_Referencia.Complete_Name = QFileInfo(Geo_Referencia.Name);
        ui->txt_Output->append("\n Arquivo: " + Geo_Referencia.Name);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        Le_Geo_from_file2(Geo_Referencia);
        ui->txt_Output->append(" -> Lido.");
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        namedWindow("TIF Referencia", WINDOW_GUI_EXPANDED);
        moveWindow("TIF Referencia", 20,20);
        resizeWindow("TIF Referencia",900,700);
        setMouseCallback("TIF Referencia",on_mouse_Referencia_TIF,NULL);
        imshow("TIF Referencia",Geo_Referencia.image);
        Get_Info_Window("TIF Referencia",Geo_Referencia);


        if (Name_Imagem1 == "") return;

        on_actionUsar_triggered();
}

void MainWindow::on_actionMostrar_triggered()
{
    ::ShowWindow(Geo_Referencia.hParent,SW_RESTORE);
    imshow("TIF Referencia",Geo_Referencia.image);
}


void MainWindow::on_actionUsar_triggered()
{
    Geo_ICE = GeoProc;
    dialogBlend = new Dialog_Blend(this);
    dialogBlend->raise();
    dialogBlend->setWindowTitle("Blend");
    dialogBlend->Ajusta_Parametros();
    dialogBlend->Print_Slider();

    connect(dialogBlend, SIGNAL(Alterado()),    this,   SLOT(Valor_Slider_Changed()));
    connect(dialogBlend, SIGNAL(Sobrepor()),    this,   SLOT(Calc_Sobrepor_Images()));
    connect(dialogBlend, SIGNAL(Blend()),       this,   SLOT(Calc_Blend_Images()));
    connect(dialogBlend, SIGNAL(Cancel_Blend()),this,   SLOT(on_bt_Teste_Cancel()));
    connect(dialogBlend, SIGNAL(Usar_Blend()),  this,   SLOT(Blend_Ok()));


    dialogBlend->show();
    QString Output;
    Size_GeoTiff(Geo_Referencia.Name,false,Output);
    dialogBlend->Print(Output);

    Output = "\n";
    Size_GeoTiff(Geo_ICE.Name,false,Output);
    dialogBlend->Print(Output);
    dialogBlend->Print_File1(Geo_Referencia.Name);
    dialogBlend->Print_File2(Geo_ICE.Name);

    Calculou_Sobrepor = false;
    Calculou_Blend = false;
    if (dialogBlend->Modo_de_Operacao()) Calc_Blend_Images(); else Calc_Sobrepor_Images();
}

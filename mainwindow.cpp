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

#include "widget.h"
#include "stdoutredirector.h"

#include "Windows.h"
#include "WinUser.h"
#include "cstdlib"
#include "conio.h"
#include "math.h"

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
#include <QTranslator>


#include <QCloseEvent>

#include <QAbstractItemView>

#include <QAction>
#include <QMenu>

#include <QtCore>
#include <QtWidgets>

#include <QRegularExpressionValidator>>

#include <QXmlStreamReader>

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
#include "info_H.h"

//#define USAR_IPL

using namespace std;
using namespace cv;

bool                ICE_Pressed,TIF_Pressed;

char                intStr[1000];

Geometria_Imagem    Geo,Geometria_ImgProc,GeoProc,Geo_ICE,Geo_Tiff,Geo_Img,Geo_Referencia;
std::string         Name_Imagem1 = "";
Plot_Coords         Coords;
bool                Brazil;
// bool                DeslocaImagem = false;

#ifdef USAR_IPL
IplImage            *imageRead;
CvCapture           *capture;
#else
Mat                 imageRead;
VideoCapture        capture;
#endif

////////////////////////////////////////////////////////////////////
Ui::MainWindow*     windowPrincipal;
DialogPontos*       uidialogPontos = NULL;
Dialog_Desloca_Img  *dialogDeslocaImg;

cv::Mat             ImgWindowPontos;
cv::Mat             ImgWindowDesloca;

TiposDrones     Drones;
TiposCameras    Cameras;

int             Out_Y;


message_function MeFu;

QTranslator translator;

/*
class message_function
{
public:
    void Print(Ui::MainWindow *ui,std::string Valor);
};
*/

Plot_Coords_Tipo::Plot_Coords_Tipo() {
    clear();
}

void Plot_Coords_Tipo::clear(void) {
    Media_Ida = 0.0;
    Media_Volta = 0.0;
    Num_Ida = 0;
    Num_Volta = 0;
    Direcao.clear();
    FileName.clear();
    Lat.clear();
    Lon.clear();
    Angle.clear();
    Lat_A.clear();
    Lon_A.clear();
    mx.clear();
    my.clear();
    Iw.clear();
    Ih.clear();
    Alt.clear();
    Gimbal.clear();
    Model = "";
    Make = "";
}


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


    // Colocar o GRID

    if (GeoProc.Pontos.length()) imshow(Name_Imagem1,GeoProc.image);

}

void message_function::Draw_Cel() {
    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B;
    bool                        Plot;

    std::vector<cv::Point>      fillContSingle;

    if (ImgWindowPontos.empty()) return;

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
        int         Num = 0,Num_Selected = 0;

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

            GeoProc.CcUTM.Reset(P.imag(),P.real());

/*
            Cy = (GeoProc.CcUTM.Northing() - GeoProc.AUTM.Northing()) / ( GeoProc.mppy ) ;
            Cx = (GeoProc.CcUTM.Easting() - GeoProc.AUTM.Easting()) / ( GeoProc.cmppx );
*/
            Cy =   ( GeoProc.CcUTM.Latitude() - GeoProc.AUTM.Latitude() ) / GeoProc.y_Size;
            Cx =   ( GeoProc.CcUTM.Longitude() - GeoProc.AUTM.Longitude() ) / GeoProc.x_Size;


            if ( (Cx < GeoProc.image.cols) && (Cy < GeoProc.image.rows) && (Cx > 0) && (Cy > 0) && (Plot == true) ) {

                if (i == 0) {// Primeiro ponto -> Verde
                    cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo,cv::Scalar(0, 255, 0), -1, cv::LINE_8, 0);
                } else if (i == (Num - 1)) {// Último ponto -> Vermelho
                    cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo,cv::Scalar(0, 0, 255), -1, cv::LINE_8, 0);
                } else {// Pontos intermediários -> Laranja ou azul (se curva)
                    cv::Scalar Cor = cv::Scalar(0,165,255);

                    if (Coords.IMGS.Direcao.length()) {
                        if ((Coords.Tipo == 2) || (Coords.Tipo == 4)) {
                            if (Coords.PROC.Direcao.at(i) == 0) Cor = cv::Scalar(255,0,0);
                        }
                        if ((Coords.Tipo == 1) || (Coords.Tipo == 3)) {
                            if (Coords.IMGS.Direcao.at(i) == 0) Cor = cv::Scalar(255,0,0);
                        }
                    }


//                    cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo,cv::Scalar(0, 165, 255), -1, cv::LINE_8, 0);
                        cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo,Cor, -1, cv::LINE_8, 0);
                }


                if ( (GeoProc.Proximidade == true) && (GeoProc.Proximidade_Num == i)) {
                        cv::circle(GeoProc.image,cv::Point(Cx,Cy),Raio_Circulo + 10,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
//                        PrintStr(windowPrincipal,Coords.IMGS.FileName.at(i));
                }
/*                else {
                    cv::circle(GeoProc.image,cv::Point(GeoProc.PImg.at(j).real(),GeoProc.PImg.at(j).imag()),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
                }
*/

                // Desenha uma linha indicando o ângulo
                if (Coords.ShowAngle) {

                    double      R = Raio_Circulo * 6;
                    double      A,Ax,Ay;

                    if ((Coords.Tipo == 1) || (Coords.Tipo == 3)) {

                    ////// Linha da média
                        if (Coords.IMGS.Direcao.at(i) == 1) {
                            A = 90.0 - Coords.IMGS.Media_Ida;
                        } else if (Coords.IMGS.Direcao.at(i) == -1) {
                            A = 90.0 - Coords.IMGS.Media_Volta;
                        } else {
                            A = 90.0 - Coords.IMGS.Angle.at(i);
                        }

                        Ax = Cx + R * std::cos(A * PI_180);
                        Ay = Cy - R * std::sin(A * PI_180);

                        cv::line(GeoProc.image,cv::Point(Cx,Cy),cv::Point(Ax,Ay),cv::Scalar( 0, 255, 0), GROSSURA );

                    ////// Linha do ângulo
                        if (Coords.IMGS.Direcao.at(i) == 1) {
                            A = 90.0 - Coords.IMGS.Angle.at(i);
                        } else if (Coords.IMGS.Direcao.at(i) == -1) {
                            A = 90.0 - Coords.IMGS.Angle.at(i);
                        } else {
                            A = 90.0 - Coords.IMGS.Angle.at(i);
                        }

                    } else {
                        ////// Linha da média
                            if (Coords.PROC.Direcao.at(i) == 1) {
                                A = 90.0 - Coords.PROC.Media_Ida;
                            } else if (Coords.PROC.Direcao.at(i) == -1) {
                                A = 90.0 - Coords.PROC.Media_Volta;
                            } else {
                                A = 90.0 - Coords.PROC.Angle.at(i);
                            }

                            Ax = Cx + R * std::cos(A * PI_180);
                            Ay = Cy - R * std::sin(A * PI_180);

                            cv::line(GeoProc.image,cv::Point(Cx,Cy),cv::Point(Ax,Ay),cv::Scalar( 0, 255, 0), GROSSURA );

                        ////// Linha do ângulo
                            if (Coords.PROC.Direcao.at(i) == 1) {
                                A = 90.0 - Coords.PROC.Angle.at(i);
                            } else if (Coords.PROC.Direcao.at(i) == -1) {
                                A = 90.0 - Coords.PROC.Angle.at(i);
                            } else {
                                A = 90.0 - Coords.PROC.Angle.at(i);
                            }
                    }

                    Ax = Cx + R * std::cos(A * PI_180);
                    Ay = Cy - R * std::sin(A * PI_180);

                    cv::line(GeoProc.image,cv::Point(Cx,Cy),cv::Point(Ax,Ay),cv::Scalar( 0, 0, 255), GROSSURA );

                }
            }
        }
    }


    ////////////////////////////////////////////////////////////       Coloca o GRID
//    cv::line(GeoProc.image,cv::Point(GeoProc.Iw/2,0),cv::Point(GeoProc.Iw/2,GeoProc.Ih-1),cv::Scalar(0, 255, 255),5);
//    cv::line(GeoProc.image,cv::Point(0,GeoProc.Ih/2),cv::Point(GeoProc.Iw,GeoProc.Ih/2),cv::Scalar(0, 255, 255),5);

//    QtFont font = fontQt("Times",10,Scalar(0,255,255));
//    cv::addText(GeoProc.image,"Teste",cv::Point(0,GeoProc.Ih/2-20),font);
    ///////////////////////////////////////////////////////////////////////////////

    if (Name_Imagem1 != "") imshow(Name_Imagem1,GeoProc.image);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void message_function::PrintStr(Ui::MainWindow* ui,QString Valor) {
    ui->txt_Output->append(Valor);
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

/*
            PosLat_A = GeoProc.Pontos.at(GeoProc.PImg.length() - 2).real();
            PosLon_A = GeoProc.Pontos.at(GeoProc.PImg.length() - 2).imag();
            PosLat_B = GeoProc.Pontos.at(GeoProc.PImg.length() - 1).real();
            PosLon_B = GeoProc.Pontos.at(GeoProc.PImg.length() - 1).imag();
*/
            PosLat_A = GeoProc.Pontos.at(Row - 2).real();
            PosLon_A = GeoProc.Pontos.at(Row - 2).imag();
            PosLat_B = GeoProc.Pontos.at(Row - 1).real();
            PosLon_B = GeoProc.Pontos.at(Row - 1).imag();

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

    if (GeoProc.Pontos.length() > 0) {
        ui->actionSave_KML->setEnabled(true);
    } else {
        ui->actionSave_KML->setEnabled(false);
    }

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

//        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 1 );


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

//        PosLat = GeoProc.AUTM.Northing() + y * GeoProc.mppy;
//        PosLon = GeoProc.AUTM.Easting() + x * GeoProc.mppx;  // Eixo x é longitude
//        GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);

        PosLat = GeoProc.AUTM.Latitude() + y * GeoProc.y_Size;
        PosLon = GeoProc.AUTM.Longitude() + x * GeoProc.x_Size;

        S1 = Br.toString(GeoProc.CcUTM.Latitude(),'f',6);
        S2 = Br.toString(GeoProc.CcUTM.Longitude(),'f',6);
        sprintf(intStr,"Coordenada (%s ; %s) (%s)",S1.toStdString().c_str(),S2.toStdString().c_str(),GeoProc.CcUTM.DMSRepresentation().c_str());
        uidialogPontos->Print(intStr);

        Table1(windowPrincipal,GeoProc.Pontos.count(),true);
     }
}



void MainWindow::Monta_Tabela_Inicial(void){
    Out_Y = 500; //845
    ui->tablePontos->setColumnCount(3);

    QStringList Labels = {"Latitude","Longitude",tr("Distância")};
    ui->tablePontos->setHorizontalHeaderLabels(Labels);
    ui->tablePontos->setStyleSheet("QHeaderView::section {background-color:lightgray}");

    ui->tablePontos->horizontalHeader()->resizeSections(QHeaderView::Stretch);
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
        sprintf(intStr,"%i imagens", (int) Informacao2.ListadeArquivos.length());
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
//    std::complex<double>    P;
    std::complex<double>    PImg;
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
//    double                  PosLat,PosLon;
    std::complex<double>    P,PImg;
//    char                    Str[100];



//    PosLat = Geo_Tiff.AUTM.Northing() + y * Geo_Tiff.mppy ;
//    PosLon = Geo_Tiff.AUTM.Easting() + x * Geo_Tiff.mppx;  // Eixo x é longitude

    Geo_Tiff.CcUTM.Reset(Geo_Tiff.AUTM.Latitude()+Geo_Tiff.y_Size * y,
                        Geo_Tiff.AUTM.Longitude()+Geo_Tiff.x_Size * x);
//    Geo_Tiff.CcUTM.Reset(Geo_Tiff.AUTM.Zone(),Geo_Tiff.AUTM.Northp(),PosLon,PosLat);

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


///////////////////////////////////////////////////////////////////////////////////////////////////////
/// Rotinas de Mouse para deslocamento de imagens



void MainWindow::on_mouse_Desloca_TIF_1(int event, int x, int y, int flags, void *) {
//    double                  PosLat,PosLon;
    std::complex<double>    P,PImg;
//    char                    Str[100];

//    PosLat = Geo_Referencia.AUTM.Northing() + y * Geo_Referencia.mppy ;
//    PosLon = Geo_Referencia.AUTM.Easting() + x * Geo_Referencia.mppx;  // Eixo x é longitude


    Geo_Referencia.CcUTM.Reset(Geo_Referencia.AUTM.Latitude()+Geo_Referencia.y_Size * y,
                        Geo_Referencia.AUTM.Longitude()+Geo_Referencia.x_Size * x);

//    Geo_Referencia.CcUTM.Reset(Geo_Referencia.AUTM.Zone(),Geo_Referencia.AUTM.Northp(),PosLon,PosLat);

    P.real(Geo_Referencia.CcUTM.Latitude());
    P.imag(Geo_Referencia.CcUTM.Longitude());
    PImg.real(x);
    PImg.imag(y);

    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%8.4f,%8.4f) (%s)\n",x,y,P.real(),P.imag(),Geo_Referencia.CcUTM.DMSRepresentation().c_str());
    displayStatusBar(Geo_Referencia.Name.toStdString(),intStr,0);

    if (event == EVENT_LBUTTONDOWN ) {
        switch (flags) {
        case  (EVENT_FLAG_LBUTTON + EVENT_FLAG_CTRLKEY): // Botão esquerdo Mouse clic
            if (Geo_Referencia.Pontos.length() > 0) {
                Geo_Referencia.Pontos.removeLast();
                Geo_Referencia.PImg.removeLast();
                Geo_Referencia.image_Lida.copyTo(Geo_Referencia.image);
            }

            Geo_Referencia.Pontos.push_back(P);
            Geo_Referencia.PImg.push_back(PImg);
            ICE_Pressed = false;
            TIF_Pressed = true;
            cv::circle(Geo_Referencia.image,cv::Point(x,y),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
            imshow(Geo_Referencia.Name.toStdString(),Geo_Referencia.image);
            if (Geo_Referencia.DeslocaImagem) dialogDeslocaImg->printOriginalCoords(Geo_Referencia);
            break;

        case (EVENT_FLAG_ALTKEY + EVENT_FLAG_LBUTTON):  // Botão esquerdo + ALT
            if (!Geo_Referencia.Pontos.isEmpty()) {
                Geo_Referencia.Pontos.removeLast();
                Geo_Referencia.PImg.removeLast();
                Geo_Referencia.image_Lida.copyTo(Geo_Referencia.image);
                imshow(Geo_Referencia.Name.toStdString(),Geo_Referencia.image);
                if (Geo_Referencia.DeslocaImagem) dialogDeslocaImg->printOriginalCoords(Geo_Referencia);
            }
        }
    }
}


void MainWindow::on_mouse_Desloca_TIF_2(int event, int x, int y, int flags, void *) {
//    double                  PosLat,PosLon;
    std::complex<double>    P,PImg;
//    char                    Str[100];

//    PosLat = Geo_Tiff.AUTM.Northing() + y * Geo_Tiff.mppy ;
//    PosLon = Geo_Tiff.AUTM.Easting() + x * Geo_Tiff.mppx;  // Eixo x é longitude
//    Geo_Tiff.CcUTM.Reset(Geo_Tiff.AUTM.Zone(),Geo_Tiff.AUTM.Northp(),PosLon,PosLat);
    Geo_Tiff.CcUTM.Reset(Geo_Tiff.AUTM.Latitude()+Geo_Tiff.y_Size * y,
                        Geo_Tiff.AUTM.Longitude()+Geo_Tiff.x_Size * x);


    P.real(Geo_Tiff.CcUTM.Latitude());
    P.imag(Geo_Tiff.CcUTM.Longitude());
    PImg.real(x);
    PImg.imag(y);

    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%8.4f,%8.4f) (%s)\n",x,y,P.real(),P.imag(),Geo_Tiff.CcUTM.DMSRepresentation().c_str());
    displayStatusBar(Geo_Tiff.Name.toStdString(),intStr,0);

    if (event == EVENT_LBUTTONDOWN ) {
        switch (flags) {
        case  (EVENT_FLAG_LBUTTON + EVENT_FLAG_CTRLKEY): // Botão esquerdo Mouse clic
            if (Geo_Tiff.Pontos.length() > 0) {
                Geo_Tiff.Pontos.removeLast();
                Geo_Tiff.PImg.removeLast();
                Geo_Tiff.image_Lida.copyTo(Geo_Tiff.image);
            }

            Geo_Tiff.Pontos.push_back(P);
            Geo_Tiff.PImg.push_back(PImg);
            ICE_Pressed = false;
            TIF_Pressed = true;
            cv::circle(Geo_Tiff.image,cv::Point(x,y),30,cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
            imshow(Geo_Tiff.Name.toStdString(),Geo_Tiff.image);
            if (Geo_Tiff.DeslocaImagem) dialogDeslocaImg->printDeslocaCoords(Geo_Tiff);
            break;

        case (EVENT_FLAG_ALTKEY + EVENT_FLAG_LBUTTON):  // Botão esquerdo + ALT
            if (!Geo_Tiff.Pontos.isEmpty()) {
                Geo_Tiff.Pontos.removeLast();
                Geo_Tiff.PImg.removeLast();
                Geo_Tiff.image_Lida.copyTo(Geo_Tiff.image);
                imshow(Geo_Tiff.Name.toStdString(),Geo_Tiff.image);
                if (Geo_Tiff.DeslocaImagem) dialogDeslocaImg->printDeslocaCoords(Geo_Tiff);
            }
        }
    }
}

void MainWindow::on_mouse_Desloca_TIF_N(int event, int x, int y, int flags, void *) {

}

void MainWindow::on_mouse_Referencia_TIF(int event, int x, int y, int flags, void *) {
//    double                  PosLat,PosLon;
    std::complex<double>    P,PImg;
//    char                    Str[100];

//    PosLat = Geo_Referencia.AUTM.Northing() + y * Geo_Referencia.mppy ;
//    PosLon = Geo_Referencia.AUTM.Easting() + x * Geo_Referencia.mppx;  // Eixo x é longitude
//    Geo_Referencia.CcUTM.Reset(Geo_Referencia.AUTM.Zone(),Geo_Referencia.AUTM.Northp(),PosLon,PosLat);
    Geo_Referencia.CcUTM.Reset(Geo_Referencia.AUTM.Latitude()+Geo_Referencia.y_Size * y,
                        Geo_Referencia.AUTM.Longitude()+Geo_Referencia.x_Size * x);

    P.real(Geo_Referencia.CcUTM.Latitude());
    P.imag(Geo_Referencia.CcUTM.Longitude());
    PImg.real(x);
    PImg.imag(y);

    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%8.4f,%8.4f) (%s)\n",x,y,P.real(),P.imag(),Geo_Referencia.CcUTM.DMSRepresentation().c_str());
    displayStatusBar("TIF Referencia",intStr,0);

}


void MainWindow::on_mouse_ImagemRotacionada(int event, int x, int y, int flags, void *) {
//    double                  PosLat,PosLon;
    int                     PosLat_A,PosLat_B,PosLon_A,PosLon_B;
    std::complex<double>    P,PImg;
    char                    Str[100];

    // Descobre fator de zoom
    cv::saveWindowParameters("Imagem TIF");


/*
    if (x == 0) {
        corX = 0;
    }

    PosLat = GeoProc.AUTM.Northing() + y * GeoProc.mppy + corY;
    PosLon = GeoProc.AUTM.Easting() + x * GeoProc.mppx + corX;  // Eixo x é longitude
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);

*/
    //////////////////////////////////////////////
    ///
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Latitude()+GeoProc.y_Size * y,
                        GeoProc.AUTM.Longitude()+GeoProc.x_Size * x);


    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());

/*
    double X,Y,Px,Py,MinX,MinY,MaxX,MaxY;
    MinX = GeoProc.AUTM.Longitude();
    MaxY = GeoProc.AUTM.Latitude();
    MaxX = GeoProc.DUTM.Longitude();
    MinY = GeoProc.DUTM.Latitude();

    Px =  (GeoProc.Iw - 1) / fabs(MaxX - MinX);
    Py =  (GeoProc.Ih - 1) / fabs(MaxY - MinY);

    P.real((x-MinX*Px)/Px);
    P.imag((y-MinY*Py)/Py);
*/
    PImg.real(x);
    PImg.imag(y);
//    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),P.real(),P.imag());

    std::string S3 = GeoProc.CcUTM.DMSRepresentation();
    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%9.6f,%9.6f) (%s)",x,y,P.real(),P.imag(),S3.c_str());
    displayStatusBar(Name_Imagem1,intStr,0);

//    sprintf(intStr,"Coordenada (%9.6f,%9.6f) (%s)",GeoProc.CcUTM.Latitude(),GeoProc.CcUTM.Longitude(),GeoProc.CcUTM.DMSRepresentation().c_str());
    QString S1,S2;

    QLocale Br;
    S1 = Br.toString(P.real(),'f',6);
    S2 = Br.toString(P.imag(),'f',6);
    sprintf(intStr,"Coordenada (%s ; %s) (%s)",S1.toStdString().c_str(),S2.toStdString().c_str(),S3.c_str());

//    cv::putText(GeoProc.image,"Teste",cvPoint(100,100),cv::FONT_HERSHEY_COMPLEX,2.0,cvScalar(0,255,255),1,LINE_8);


    QtFont font = fontQt("Times");
    cv::addText(GeoProc.image,"Teste",cv::Point(100,100),font);   //         addText( GeoProc.image, "Hello World !'", Point(50,50), font);;

    if (event == EVENT_MOUSEMOVE) {
//        std::complex<double>    P_D;
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

    if (GeoProc.CoordCentrais) return;

    if (event == EVENT_LBUTTONDOWN) {
        switch (flags) {
        case (EVENT_FLAG_SHIFTKEY + EVENT_FLAG_LBUTTON):
            MeFu.Gera_Retangulo(windowPrincipal);
            break;

        case  (EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON): // Botão esquerdo Mouse clic + CTRL

            if (GeoProc.DeslocaImagem || GeoProc.AjustaImagens) {
                if (GeoProc.Pontos.length() > 0) break;
            }

            GeoProc.Pontos.push_back(P);
            GeoProc.PImg.push_back(PImg);

            MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),true);

            uidialogPontos->Print(intStr);

            // Rotina para deslocamento da imagem
            if (GeoProc.DeslocaImagem) {
                dialogDeslocaImg->printOriginalCoords(GeoProc);
            }

            break;

        case (EVENT_FLAG_ALTKEY + EVENT_FLAG_LBUTTON):  // Botão esquerdo + ALT
            if (!GeoProc.Pontos.isEmpty()) {

//                MeFu.Print(windowPrincipal," Ponto retirado\n");
//                MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),false);

                GeoProc.Pontos.removeLast();
                GeoProc.PImg.removeLast();
                uidialogPontos->clear();

                if (dialogDeslocaImg != NULL) {
                    dialogDeslocaImg->printOriginalCoords(GeoProc);
                }

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


void MainWindow::on_mouse_ArquivosJPG(int event, int x, int y, int flags, void *) {
//    double                  PosLat,PosLon;
    int                     PosLat_A,PosLat_B,PosLon_A,PosLon_B;
    std::complex<double>    P,PImg,P_Img2;
    char                    Str[100];

/*
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
*/
    GeoProc.CcUTM.Reset(GeoProc.AUTM.Latitude()+GeoProc.y_Size * y,
                        GeoProc.AUTM.Longitude()+GeoProc.x_Size * x);


    P.real(GeoProc.CcUTM.Latitude());
    P.imag(GeoProc.CcUTM.Longitude());


/*
    double X,Y,Px,Py,MinX,MinY,MaxX,MaxY;
    MinX = GeoProc.AUTM.Longitude();
    MaxY = GeoProc.AUTM.Latitude();
    MaxX = GeoProc.DUTM.Longitude();
    MinY = GeoProc.DUTM.Latitude();

    Px =  (GeoProc.Iw - 1) / fabs(MaxX - MinX);
    Py =  (GeoProc.Ih - 1) / fabs(MaxY - MinY);

    P.real((x-MinX*Px)/Px);
    P.imag((y-MinY*Py)/Py);
*/
    PImg.real(x);
    PImg.imag(y);
//    GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),P.real(),P.imag());

    std::string S3 = GeoProc.CcUTM.DMSRepresentation();
    sprintf(intStr,"Posição ( %d , %d ) Coordenada (%9.6f,%9.6f) (%s)",x,y,P.real(),P.imag(),S3.c_str());
    displayStatusBar(Name_Imagem1,intStr,0);

//    sprintf(intStr,"Coordenada (%9.6f,%9.6f) (%s)",GeoProc.CcUTM.Latitude(),GeoProc.CcUTM.Longitude(),GeoProc.CcUTM.DMSRepresentation().c_str());
    QString S1,S2;

    QLocale Br;
    S1 = Br.toString(P.real(),'f',6);
    S2 = Br.toString(P.imag(),'f',6);
    sprintf(intStr,"Coordenada (%s ; %s) (%s)",S1.toStdString().c_str(),S2.toStdString().c_str(),S3.c_str());

    if (event == EVENT_MOUSEMOVE) {
//        std::complex<double>    P_D;
        std::complex<int>       P_Atual,P_Img_D,P_Img3;
        double                  Dist;
        P_Atual.real(x);
        P_Atual.imag(y);

        if (GeoProc.Proximidade) {
            GeoProc.Era_Proximo = true;
        }

        /*
        if (GeoProc.Pontos.length() > 0) {
            Dist = 0;
        }
        */
        double Cx,Cy;
        for (int j = 0 ; j < Coords.IMGS.Lat.length() ; j++) {
            P_Img2.real(Coords.IMGS.Lon.at(j));
            P_Img2.imag(Coords.IMGS.Lat.at(j));
            GeoProc.CcUTM.Reset(P_Img2.imag(),P_Img2.real());

            Cy = (GeoProc.CcUTM.Northing() - GeoProc.AUTM.Northing()) / ( GeoProc.mppy ) ;
            Cx = (GeoProc.CcUTM.Easting() - GeoProc.AUTM.Easting()) / ( GeoProc.mppx );

            P_Img3.real(Cx);
            P_Img3.imag(Cy);

            P_Img_D = P_Img3 - P_Atual;
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
                j = 10000;
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
       case  (EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON):
        MeFu.PrintStr(windowPrincipal,Coords.IMGS.FileName.at(GeoProc.Proximidade_Num));
       }
    }

    return;

    if (event == EVENT_LBUTTONDOWN) {
        switch (flags) {
        case (EVENT_FLAG_SHIFTKEY + EVENT_FLAG_LBUTTON):
            MeFu.Gera_Retangulo(windowPrincipal);
            break;

        case  (EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON): // Botão esquerdo Mouse clic + CTRL

            if (GeoProc.DeslocaImagem || GeoProc.AjustaImagens) {
                if (GeoProc.Pontos.length() > 0) break;
            }

            GeoProc.Pontos.push_back(P);
            GeoProc.PImg.push_back(PImg);

            MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),true);

            uidialogPontos->Print(intStr);

            // Rotina para deslocamento da imagem
            if (GeoProc.DeslocaImagem) {
                dialogDeslocaImg->printOriginalCoords(GeoProc);
            }

            break;

        case (EVENT_FLAG_ALTKEY + EVENT_FLAG_LBUTTON):  // Botão esquerdo + ALT
            if (!GeoProc.Pontos.isEmpty()) {

//                MeFu.Print(windowPrincipal," Ponto retirado\n");
//                MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),false);

                GeoProc.Pontos.removeLast();
                GeoProc.PImg.removeLast();
                uidialogPontos->clear();

                if (dialogDeslocaImg != NULL) {
                    dialogDeslocaImg->printOriginalCoords(GeoProc);
                }

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
/*
    double PosLat,PosLon;
    PosLat = Geo.AUTM.Northing() + y * Geo.mppy;
    PosLon = Geo.AUTM.Easting() + x * Geo.mppx;  // Eixo x é longitude
    Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),PosLon,PosLat);
*/
    Geo.CcUTM.Reset(Geo.AUTM.Latitude()+Geo.y_Size * y,
                        Geo.AUTM.Longitude()+Geo.x_Size * x);


    sprintf(intStr,"Posiçao ( %d , %d ) Coordenada (%4.4f,%4.4f) (%s)",x,y,Geo.CcUTM.Latitude(),Geo.CcUTM.Longitude(),Geo.CcUTM.DMSRepresentation().c_str());
    displayStatusBar("Imagem Teste",intStr,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Le_Geo_from_file(QFileInfo CompleteName)
{
//    qint64      Elapsed;
    bool        GeoReff = false;
//    double      Rel1;
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
            Geo.x_Size = Transform[1];
            Geo.y_Size = Transform[5];
            Geo.CcUTM.Reset(Geo.AUTM.Longitude() + Geo.x_Size * (Geo.Iw /2),Geo.AUTM.Latitude() + Geo.y_Size * (Geo.Ih /2));
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
//    char ** papszIter;
//    papszIter = papszMetadata;

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


//    bool WindowExists = false;
//    if (Name_Imagem1 != "") WindowExists = cv::getWindowProperty(Name_Imagem1,CV_WND_PROP_VISIBLE) > 0;
//    if (WindowExists) cv::destroyWindow(Name_Imagem1);
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

    // Faz gdalinfo no arquivo
    QString     Out;
    info_H(Name,Out);
//    ui->txt_Output->setText(Out);

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(Name.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());


    GDAL2MAT(pOldDS,imageRead);
/*
    if (Extensao == "KMZ") {
        GDAL2MAT(pOldDS,imageRead);
    } else {
        imageRead = cv::imread(Name.toStdString(),CV_LOAD_IMAGE_COLOR);
    }
*/

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

    if (ui->actionEspecificar_2->isChecked()) {
        Geo.Alt = Geo.Alt_Default;
    } else {
        Geo.Alt = GetXMPItem(stringList,"RelativeAltitude");
    }

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

        bool    E_UTM = false;
//        int     Zona = 0;
//        int     Hemisferio = 0;

//        Zona = pOldDS->GetSpatialRef()->GetUTMZone();
        const int A = pOldDS->GetSpatialRef()->IsGeographic();
        if (A) E_UTM = false; else E_UTM = true;

        /////////////////
        // pOldDS->GetSpatialRef()->IsProjected() dá crash quando a imagem está como projeção

 //       int Pr = pOldDS->GetSpatialRef()->IsProjected();
 //       int Pr = pOldDS->GetSpatialRef()->GetUTMZone(&Hemisferio);
/*
        if (fabs(Transform[0]) > 180.0) {
            E_UTM = true;
        }
        if (Transform[0] > 0) {
            Hemisferio = true;
        } else {
            Hemisferio = false;
        }
*/

/*
        if (pOldDS->GetSpatialRef()->IsProjected()) {
            E_UTM = true;
            Zona = pOldDS->GetSpatialRef()->GetUTMZone(&Hemisferio); // Hemisferio (= 0 Sul), (= 1 Norte)
        } else {
            E_UTM = false;
        }
*/

        if (!E_UTM) {  // LAT LON
            Geo.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
            Geo.Iw = pOldDS->GetRasterXSize();
            Geo.Ih = pOldDS->GetRasterYSize();
            Geo.DUTM.Reset( Transform[3] + Transform[5] * Geo.Ih,Transform[0] + Transform[1] * Geo.Iw );
            Geo.Dw = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting());
            Geo.Dh = abs(Geo.DUTM.Northing() - Geo.AUTM.Northing());
            Geo.mppx = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting()) / Geo.Iw; // LON
            Geo.mppy = -abs(Geo.DUTM.Northing() - Geo.AUTM.Northing()) / Geo.Ih; // LAT
            Geo.x_Size = Transform[1];
            Geo.y_Size = Transform[5];
            Geo.CcUTM.Reset(Geo.AUTM.Latitude() + Geo.y_Size * (Geo.Iw /2),Geo.AUTM.Longitude() + Geo.x_Size * (Geo.Ih / 2));
            Geo.Cc.real(Geo.CcUTM.Latitude());
            Geo.Cc.imag(Geo.CcUTM.Longitude());
            Geo.A.real(Geo.AUTM.Latitude());
            Geo.A.imag(Geo.AUTM.Longitude());
            Geo.D.real(Geo.DUTM.Latitude());
            Geo.D.imag(Geo.DUTM.Longitude());
        } else { // UTM

            double  Lat,Lon;
/*
            int     X1;

            Y = 10000000.0 + Transform[3]; // Considerando Northing negativo no arquivo
            X1 = floor(abs(Transform[0] / 667916.933));
            if (Transform[3] < 0) {
                Zona = 30 - X1;
                Hemisferio = true;
            } else {
                Zona = 30 + X1;
                Hemisferio = false;
            }

            X = 811778.0;

            GeographicLib::UTMUPS::Reverse(Zona,false, X ,Y,Lat,Lon);
*/

            bool Ok;
            Ok = GetLatLonfromINFO(Out,Lat,Lon);

            Geo.AUTM.Reset(Lat,Lon);
            Geo.Iw = pOldDS->GetRasterXSize();
            Geo.Ih = pOldDS->GetRasterYSize();
            Geo.mppx = Transform[1]; // LON
            Geo.mppy = Transform[5]; // LAT
            Geo.DUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * Geo.Iw,Geo.AUTM.Northing() + Geo.mppy * Geo.Ih);
            Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
            Geo.Cc.real(Geo.CcUTM.Latitude());
            Geo.Cc.imag(Geo.CcUTM.Longitude());
            Geo.Dw = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting());
            Geo.Dh = abs(Geo.DUTM.Northing() - Geo.AUTM.Northing());
            Geo.A.real(Geo.AUTM.Latitude());
            Geo.A.imag(Geo.AUTM.Longitude());
            Geo.D.real(Geo.DUTM.Latitude());
            Geo.D.imag(Geo.DUTM.Longitude());

        }
    }

    Geo.Cor_Nodata = pOldDS->GetRasterBand(1)->GetNoDataValue();

    Geometria_Img = Geo;
    Geometria_Img.FileDS = poSrcDS;  // ---------------> Aqui tem que trocar para copia

    if (!GeoReff) {
        Rotaciona_Imagem(Geo,imageRead,Destino);

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
//    double Area;
//    Area = fabs(Geometria_ImgProc.AUTM.Easting() - Geometria_ImgProc.DUTM.Easting()) *
//           fabs(Geometria_ImgProc.AUTM.Northing() - Geometria_ImgProc.DUTM.Northing());
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


//    Name_Imagem1 = Geo_Tiff_Name.toStdString();
    Name_Imagem1 = CompleteName.absoluteFilePath().toStdString();
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

/*
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
*/

    Geometria_Img.ExifXMPInfo = Out;


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
        char** papszMetadataDomain = CSLDuplicate(poSrcDS->GetMetadataDomainList());


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

//         writeGeoInfo(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc);
        writeGeoInfo(Geo_Tiff_Name,Destino,papszMetadata2,papszMetadataDomain,Geometria_ImgProc);

         if (!Mesmo_Dir) {
            Grava_Thumb(Geo_Tiff_Name);
         }

#endif

        Elapsed = timer2.elapsed();
        sprintf(intStr,"\n Imagem %s gravada em %3.3f segundos.\n",Geo_Tiff_Name.toStdString().c_str(),(Elapsed / 1000.0));
        ui->txt_Output->append(intStr);


    }

    GeoProc.Complete_Name = QFileInfo(Geo_Tiff_Name);

    ui->bt_limpaProc->setEnabled(true);


    ui->bt_DialogPontos->setEnabled(true);
    ui->actionPontos->setEnabled(true);

    ui->pushButton->setEnabled(true);
    ui->checkBox_Linhas->setEnabled(true);

    ui->bt_EXIF_XMP_List->setEnabled(true);
    ui->actionEXIF_XMP->setEnabled(true);
    ui->menuMadidas->setEnabled(true);
    ui->actionCortarTIFF->setEnabled(true);

    ui->actionSalvar_TIF_mixado->setEnabled(true);

    ui->actionLoad_KML->setEnabled(true);

    imshow(Name_Imagem1,GeoProc.image);

    GDALClose(pOldDS);


    // Se tiver uma imagem de fundo, fazer a mixagem aqui!
    if (ui->actionUsar->isChecked()) {
        on_actionUsar_triggered();
    }

    if (ui->actionCoordenadas_Centrais_na_tabela->isChecked()) on_actionCoordenadas_Centrais_na_tabela_triggered();

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


void MainWindow::closeEvent(QCloseEvent *event)
{
    qApp->quit();
}


void MainWindow::getEXIF_XML_Info(QString File)
{

    GDALAllRegister();

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
        ui->txt_Output->append(tr(" Mostrar coodenadas centrais -> DESATIVADO"));
        ui->actionOriginais->setChecked(false);
        ui->actionProcessadas->setChecked(false);

        if (actShowCoordProc != NULL) actShowCoordProc->setIcon(iconCheckpixOff);
        if (actShowCoord != NULL) actShowCoord->setIcon(iconCheckpixOff);

        Coords.Tipo = 0;
        MeFu.Draw_Cel();

        if (Name_Imagem1 == "Fotos") {
            destroyAllWindows();
            GeoProc.image.release();
            Destino.release();
            ImgWindowPontos.release();
            Name_Imagem1 = "";
        }
}


void MainWindow::on_actionOriginais_triggered()
{
    ui->txt_Output->append(tr("\n Mostrar coodenadas centrais -> Imagens da lista ORIGINAIS."));
    ui->actionDesativado->setChecked(false);
    ui->actionProcessadas->setChecked(false);

    actShowCoord->setIcon(iconCheckpix);
    actShowCoordProc->setIcon(iconCheckpixOff);


    // Se não houver imagem previamente carregada, criar uma com tamanho 10% maior em cada lado.
    int Cor_NoDATA;
    if ((Name_Imagem1 == "") || (Name_Imagem1 == "Fotos")) {
        int cols,rows;
        if (ui->actionBranco->isChecked()) Cor_NoDATA = 255; else Cor_NoDATA = 0;
        Calc_Imagem_Blank(0.30,cols,rows);
        cv::Mat A(rows,cols,CV_8UC3,Scalar(Cor_NoDATA,Cor_NoDATA,Cor_NoDATA));
        Destino = A;
        Destino.copyTo(ImgWindowPontos);
        Name_Imagem1 = "Fotos";

        double Rel1;
        // 700 pontos em Y

        if (cols > rows) {
            Rel1 = rows / 700.0;
        } else {
            Rel1 = cols / 600.0;
        }

        namedWindow(Name_Imagem1,WINDOW_GUI_EXPANDED);
        moveWindow(Name_Imagem1, 20,20);
        setMouseCallback(Name_Imagem1,on_mouse_ArquivosJPG,NULL);
        Get_Info_Window(QString::fromStdString(Name_Imagem1),GeoProc);

        resizeWindow(Name_Imagem1,static_cast<int>(cols / Rel1),static_cast<int>(rows / Rel1));

        imshow(Name_Imagem1,ImgWindowPontos);
        ui->bt_Mostar_Imagem->setEnabled(true);
    }

    Coords.Tipo = 1;


    if ((ui->actionShow->isChecked()) && (Coords.IMGS.Make != "Parrot")) {
        Coords.ShowAngle = true;
    } else {
        Coords.ShowAngle = false;
    }
    MeFu.Draw_Cel();
}



void MainWindow::Calc_Imagem_Blank(double Res, int &cols, int &rows) {
    GDALAllRegister();

    double  MinX = 1e10,MinY = 1e10,MaxX = -1e10,MaxY = -1e10;

    Imagem_Composta.Geo_File_List.clear();

    QDir directory(Informacao.Diretorio);
    QStringList Filters;
    Filters << "*.jpg";
    Imagem_Composta.Geo_File_List = directory.entryInfoList(Filters,QDir::Files | QDir::NoDotAndDotDot);


/*
    for (int i = 0; i < Imagem_Composta.Geo_File_List.length() ; i++) {
        GDALDataset *pOldDS;
        QString InputFile = Imagem_Composta.Geo_File_List.at(i).absoluteFilePath()   ;
        pOldDS = (GDALDataset *) GDALOpen(InputFile.toStdString().c_str(),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
        double cx,cy;

        cy = getLATfromEXIF(poSrcDS);
        cx = getLONfromEXIF(poSrcDS);

        ui->txt_Output->append("\n " + QString::number(cy) + " " + QString::number(cx));

        if (cx < MinX) MinX = cx;
        if (cy < MinY) MinY = cy;
        if (cx > MaxX) MaxX = cx;
        if (cy > MaxY) MaxY = cy;

        GDALClose(pOldDS);
    }
*/
    for (int i = 0; i < Imagem_Composta.Geo_File_List.length() ; i++) {
        double cx,cy;

        cy = Coords.IMGS.Lat.at(i);
        cx = Coords.IMGS.Lon.at(i);

//        ui->txt_Output->append("\n " + QString::number(cy) + " " + QString::number(cx));

        if (cx < MinX) MinX = cx;
        if (cy < MinY) MinY = cy;
        if (cx > MaxX) MaxX = cx;
        if (cy > MaxY) MaxY = cy;

    }



    // Mais 50 metros para cada lado
    GeoProc.AUTM.Reset(MaxY,MinX);
    GeoProc.DUTM.Reset(MinY,MaxX);

    double AX,AY,DX,DY;
    AX = GeoProc.AUTM.Easting();
    AY = GeoProc.AUTM.Northing();
    DX = GeoProc.DUTM.Easting();
    DY = GeoProc.DUTM.Northing();

    GeoProc.AUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),AX - 50,AY + 50);
    GeoProc.DUTM.Reset(GeoProc.DUTM.Zone(),GeoProc.DUTM.Northp(),DX + 50,DY - 50);

    GeoProc.A.real(GeoProc.AUTM.Latitude());                       // Coordenada LAT do canto superior esquerdo
    GeoProc.A.imag(GeoProc.AUTM.Longitude());
    GeoProc.D.real(GeoProc.DUTM.Latitude());                       // Coordenada LAT do canto inferior direito
    GeoProc.D.imag(GeoProc.DUTM.Longitude());


    MaxY = GeoProc.AUTM.Latitude();                       // Coordenada LAT do canto superior esquerdo
    MinX = GeoProc.AUTM.Longitude();
    MinY = GeoProc.DUTM.Latitude();                       // Coordenada LAT do canto inferior direito
    MaxX = GeoProc.DUTM.Longitude();

    GeoProc.Cc.real((MaxY + MinY) / 2);
    GeoProc.Cc.imag((MaxX + MinX) / 2);
    GeoProc.CcUTM.Reset(GeoProc.Cc.real(),GeoProc.Cc.imag());

    GeoProc.Dw = fabs(GeoProc.AUTM.Easting() - GeoProc.DUTM.Easting());                  // Tamanho horizontal no terreno em metros
    GeoProc.Dh = fabs(GeoProc.AUTM.Northing() - GeoProc.DUTM.Northing());                // Tamanho vertical no terreno em metros

    GeoProc.mppx = Res;
    GeoProc.mppy = -Res;
    GeoProc.cmppx = GeoProc.mppx * 100.0;
    GeoProc.cmppy = GeoProc.mppy * 100.0;

    cols = ((fabs(GeoProc.AUTM.Easting() - GeoProc.DUTM.Easting())) / Res);
    rows = ((fabs(GeoProc.AUTM.Northing() - GeoProc.DUTM.Northing())) / Res);

    GeoProc.Iw = cols;
    GeoProc.Ih = rows;

//    cv::Mat Img_Total(2000,2000,CV_8UC3,Scalar(0,0,0));
}


void MainWindow::on_actionProcessadas_triggered()
{
    ui->txt_Output->append("\n Mostrar coodenadas centrais -> Imagens da lista PROCESSSADOS.");
    ui->actionDesativado->setChecked(false);
    ui->actionOriginais->setChecked(false);

    actShowCoordProc->setIcon(iconCheckpix);
    actShowCoord->setIcon(iconCheckpixOff);


    Coords.Tipo = 2;
    if (ui->actionShow->isChecked()) {
        Coords.ShowAngle = true;
    } else {
        Coords.ShowAngle = false;
    }

    MeFu.Draw_Cel();
}

Plot_Coords::Plot_Coords() {
    Tipo = 0; // Sem Mostrar as coordenadas centrais na imagem
    Coords.ShowAngle = false;
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

    if (Name_Imagem1 != "") {
        AbreArquivo(GeoProc.Complete_Name,false,false);
    }

    show();
}



void MainWindow::Le_Geo_from_file2(Geometria_Imagem &Ref)
{
    bool        GeoReff = false;
    QString     Out;

    GDALAllRegister();

    info_H(Ref.Name,Out);

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

        bool    E_UTM = false,Ok = false;
        int     Zona = 0;
        int     Hemisferio = 0;

        Zona = Ref.FileDS->GetSpatialRef()->GetUTMZone();
        const int A = Ref.FileDS->GetSpatialRef()->IsGeographic();
        if (A) E_UTM = false; else E_UTM = true;


        if (!E_UTM) {  // LAT LON
            Ref.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
            Ref.Iw = Ref.FileDS->GetRasterXSize();
            Ref.Ih = Ref.FileDS->GetRasterYSize();
            Ref.DUTM.Reset( Transform[3] + Transform[5] * Ref.Ih,Transform[0] + Transform[1] * Ref.Iw );
            Ref.Dw = abs(Ref.DUTM.Easting() - Ref.AUTM.Easting());
            Ref.Dh = abs(Ref.DUTM.Northing() - Ref.AUTM.Northing());
            Ref.mppx = abs(Ref.DUTM.Easting() - Ref.AUTM.Easting()) / Ref.Iw; // LON
            Ref.mppy = -abs(Ref.DUTM.Northing() - Ref.AUTM.Northing()) / Ref.Ih; // LAT
            Ref.x_Size = Transform[1];
            Ref.y_Size = Transform[5];
            Ref.CcUTM.Reset(Ref.AUTM.Zone(),Ref.AUTM.Northp(),Ref.AUTM.Easting() + Ref.mppx * (Ref.Iw /2),Ref.AUTM.Northing() + Ref.mppy * (Ref.Ih /2));
            Ref.A.real(Ref.AUTM.Latitude());
            Ref.A.imag(Ref.AUTM.Longitude());
            Ref.D.real(Ref.DUTM.Latitude());
            Ref.D.imag(Ref.DUTM.Longitude());
        } else { // UTM
            double  Lat,Lon;
            bool    Ok;
            Ok = GetLatLonfromINFO(Out,Lat,Lon);

            Ref.AUTM.Reset(Lat,Lon);
            Ref.Iw = Ref.FileDS->GetRasterXSize();
            Ref.Ih = Ref.FileDS->GetRasterYSize();
            Ref.mppx = Transform[1]; // LON
            Ref.mppy = Transform[5]; // LAT
            Ref.DUTM.Reset(Ref.AUTM.Zone(),Ref.AUTM.Northp(),Ref.AUTM.Easting() + Ref.mppx * Ref.Iw,Ref.AUTM.Northing() + Ref.mppy * Ref.Ih);
            Ref.CcUTM.Reset(Ref.AUTM.Zone(),Ref.AUTM.Northp(),Ref.AUTM.Easting() + Ref.mppx * (Ref.Iw /2),Ref.AUTM.Northing() + Ref.mppy * (Ref.Ih /2));
            Ref.Dw = abs(Ref.DUTM.Easting() - Ref.AUTM.Easting());
            Ref.Dh = abs(Ref.DUTM.Northing() - Ref.AUTM.Northing());
            Ref.A.real(Ref.AUTM.Latitude());
            Ref.A.imag(Ref.AUTM.Longitude());
            Ref.D.real(Ref.DUTM.Latitude());
            Ref.D.imag(Ref.DUTM.Longitude());
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
        show();
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

//    cv::resize(Blend_Calculada.image,Blend_Calculada.image,Size(Kx,Ky));

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
    Kx = Geo_Referencia.Dw * Geo_ICE.Iw / Geo_ICE.Dw;
    Ky = Geo_Referencia.Dh * Geo_ICE.Ih / Geo_ICE.Dh;

    cv::resize(Geo_Referencia.image,Geo_Referencia.image,Size(Kx,Ky));
    Geo_Referencia.Iw = Geo_Referencia.image.cols;
    Geo_Referencia.Ih = Geo_Referencia.image.rows;

    Geo_Referencia.mppx = Geo_ICE.mppx;
    Geo_Referencia.mppy = Geo_ICE.mppy;
    Geo_Referencia.x_Size = Geo_ICE.x_Size;
    Geo_Referencia.y_Size = Geo_ICE.y_Size;

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

    ::ShowWindow(Geo_Referencia.hParent,SW_MINIMIZE);
//    cv::destroyWindow("TIF Referencia");
//    cv::destroyWindow(Name_Imagem1);
    imshow(Name_Imagem1 ,GeoProc.image);
    MeFu.Draw_Cel();

    show();

    Geo_Referencia.Blend_Utilizado = true;
    ui->actionSalvar_TIF_mixado->setEnabled(true);
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

    InputTif = QFileDialog::getOpenFileName(this, tr("Imagem TIF de referência"),Informacao.DiretorioResult,tr("Image (*.tif)"));
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

        updateTxt();

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

    if (ui->actionUsar->isChecked()) {
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

        hide();

        on_actionMostrar_triggered();

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
    } else {
        AbreArquivo(GeoProc.Complete_Name,false,false);
    }
}

void MainWindow::on_actionSalvar_TIF_mixado_triggered()
{
    QString     OutputTif,Dir;
    qint64      Elapsed;

    Dir = Informacao.DiretorioResult;

      if (Name_Imagem1 != "") {
        OutputTif = QFileDialog::getSaveFileName(this,"Output",Dir,"Arquivo (*.tif)",nullptr); //,QFileDialog::DontConfirmOverwrite);
        QFileInfo CompleteNameTif(OutputTif);

        if (OutputTif.isNull()) {
//            ui->txt_Output->clear();
            return;
        }

        timer2.restart();
        ui->txt_Output->setText(" Salvando arquivo " + OutputTif);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        // Salva a imagem mixada com as informações geográficas
        std::vector<int> params;
//            params.push_back(IMWRITE_JPEG_QUALITY);
//            params.push_back(50); // Mais valor -> menor compressao. Default 95
//            params.push_back(IMWRITE_PNG_COMPRESSION);
//            params.push_back(3); // Maior valor -> mais compressão. Default 3
        params.push_back(IMWRITE_TIFF_COMPRESSION);

        bool check = cv::imwrite(OutputTif.toStdString(),GeoProc.image);
        if (!check) {
            QString Path;
            Path = "Não foi possível gravar arquivo " + OutputTif;
            QMessageBox::information(this,tr("Erro"),Path);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            return;
         }

         writeGeoInfo(OutputTif,GeoProc.image,NULL,GeoProc);

         Grava_Thumb(OutputTif);

         Elapsed = timer2.elapsed();
         sprintf(intStr,"\n Imagem %s gravada em %3.3f segundos.\n",Geo_Tiff_Name.toStdString().c_str(),(Elapsed / 1000.0));
         ui->txt_Output->append(intStr);
         QString Output;
         Size_GeoTiff(OutputTif,true,Output);

         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

         Lista_Files_Result();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////



void MainWindow::on_actionDesloca_Imagem_triggered()
{
    Desloca_Img();
}

void MainWindow::on_actionCoordenadas_Centrais_na_tabela_triggered()
{
    std::complex<double>    P,PImg;
    double                  x,y;

    // Primeiro tem que testar se já foram carregadas imagens
    if (Coords.IMGS.Lon.length() == 0) {
        // Erro -> Não existem imagens com coordenadas lidas
        sprintf(intStr,"\n Erro! Necessário escolher diretório que contém as imagens.");
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append(intStr);
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        ui->actionCoordenadas_Centrais_na_tabela->setChecked(false);
        return;
    }

    if (ui->actionCoordenadas_Centrais_na_tabela->isChecked()) {

        ui->txt_Output->append("\n Mostrar coordenadas centrais na tabela -> ATIVADO.");

        on_actionLimpar_triggered(false);

        ui->CheckBox_Poligono->setEnabled(false);
        ui->checkBox_Circle->setChecked(false);
        ui->checkBox_Circle->setEnabled(false);
        on_checkBox_Circle_clicked();

        on_actionOriginais_triggered();

        ui->pushButton->setEnabled(false);

        GeoProc.CoordCentrais = true;


        if (!uidialogPontos) {
            dialogPontos = new DialogPontos(this);
            uidialogPontos = dialogPontos;
            ui->actionPontos->setEnabled(true);
        }
         dialogPontos->clear();



        for (int i = 0 ; i < Coords.IMGS.Lon.length() ; i++) {


            P.real(Coords.IMGS.Lat.at(i));
            P.imag(Coords.IMGS.Lon.at(i));

            GeoProc.CcUTM.Reset(P.real(),P.imag());

            y = (GeoProc.CcUTM.Northing() - GeoProc.AUTM.Northing()) / ( GeoProc.mppy ) ;
            x = (GeoProc.CcUTM.Easting() - GeoProc.AUTM.Easting()) / ( GeoProc.mppx );

            y = ( GeoProc.CcUTM.Latitude() - GeoProc.AUTM.Latitude() ) / GeoProc.y_Size;
            x = ( GeoProc.CcUTM.Longitude() - GeoProc.AUTM.Longitude() ) / GeoProc.x_Size;

            PImg.real(x);
            PImg.imag(y);

            GeoProc.Pontos.push_back(P);
            GeoProc.PImg.push_back(PImg);

            GeoProc.CcUTM.Reset(P.real(),P.imag());


            MeFu.Table1(windowPrincipal,GeoProc.Pontos.count(),true);

            QLocale Br;
            QString S1,S2,S3;
            S1 = Br.toString(P.real(),'f',6);
            S2 = Br.toString(P.imag(),'f',6);

            QTableWidgetItem* Item;

            // Dist
            Item = ui->tablePontos->item(i,2);

            S3 = Item->text();

            QFileInfo Name(Coords.IMGS.FileName.at(i));

            if (i) {
                sprintf(intStr,"(%4d) [%s] -> Lat = %s ; Lon = %s ; Dist = %s",(i+1),Name.fileName().toStdString().c_str(), S1.toStdString().c_str(),S2.toStdString().c_str(),S3.toStdString().c_str());
            } else {
                sprintf(intStr,"(%4d) [%s] -> Lat = %s ; Lon = %s",(i+1), Name.fileName().toStdString().c_str(),S1.toStdString().c_str(),S2.toStdString().c_str());
            }
            uidialogPontos->Print(intStr);
        }
        sprintf(intStr,"\n Percurso %s.",ui->txt_Dist->text().toStdString().c_str());
        uidialogPontos->Print(intStr);
        uidialogPontos->show();

    } else {
        on_actionLimpar_triggered(true);
        on_actionDesativado_triggered();
        destroyAllWindows();
        Name_Imagem1 = "";

        if (uidialogPontos) {
            uidialogPontos->close();
            ui->actionPontos->setEnabled(false);
        }

        ui->bt_Mostar_Imagem->setEnabled(false);
        ui->txt_Nome_Imagem->setText("");
        ui->CheckBox_Poligono->setEnabled(true);
        ui->checkBox_Circle->setEnabled(true);
        GeoProc.CoordCentrais = false;

        ui->checkBox_Circle->setChecked(true);
        ui->checkBox_Circle->setEnabled(true);

        ui->checkBox_Linhas->setChecked(false);
        on_checkBox_Circle_clicked();

        ui->txt_Output->append("\n Mostrar coordenadas centrais na tabela -> DESATIVADO.");

    }

}


///////////////////////////////////////////////////////////////////////////////////////
/// Esta rotina é de teste. Chamada por um botão que em produção fica oculto
///
void MainWindow::on_btOpenFile_clicked(){
    double          m1,m2,q1,q2,Alfa;
    double          m3,Theta_R,Theta;

    m1 = (GeoProc.Pontos.at(0).imag() - GeoProc.Pontos.at(1).imag() ) / (GeoProc.Pontos.at(0).real() - GeoProc.Pontos.at(1).real() );
    m2 = (GeoProc.Pontos.at(2).imag() - GeoProc.Pontos.at(3).imag() ) / (GeoProc.Pontos.at(2).real() - GeoProc.Pontos.at(3).real() );
    m3 = (GeoProc.Pontos.at(0).imag() - GeoProc.Pontos.at(3).imag() ) / (GeoProc.Pontos.at(0).real() - GeoProc.Pontos.at(3).real() );

    Alfa = std::atan(fabs((m1 - m2) / (1+m1*m2))) * 180.0 / PI;
    Theta_R = std::atan(fabs((m1 - m3) / (1+m1+m3)));
    Theta = Theta_R * 180.0 / PI;


    cv::Mat Out;
    Rotaciona(GeoProc.image,Out,Theta,GeoProc.PImg.at(0).imag(),GeoProc.PImg.at(0).real());

    imshow("Teste",Out);


}
/*
{
    int                         i;
    QString                     File;
    double                      Delta_Lat,Delta_Lon;
    double                      OldLat,OldLon,NewLat,NewLon;
    double                      MinX,MinY,MaxX,MaxY;
    QString                     New_File;
    GeographicLib::GeoCoords    POld,PNew;


    if (Geo_Referencia.Pontos.length()) {
        OldLat = Geo_Referencia.Pontos.at(0).real();
        OldLon = Geo_Referencia.Pontos.at(0).imag();
    } else {
        // Erro -> Não foi escolhida coordenada na imagem a ser deslocada
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("\n ERRO! Coordenada da imagem não definida!");
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//        dialogDeslocaImg->close();
//        Cancel_Desloca();
        return;
    }

    if (Geo_Tiff.Pontos.length()) {
        NewLat = Geo_Tiff.Pontos.at(0).real();
        NewLon = Geo_Tiff.Pontos.at(0).imag();
    } else {
        // Erro -> Não foi escolhida coordenada na imagem a ser deslocada
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("\n ERRO! Coordenada da imagem não definida!");
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//        dialogDeslocaImg->close();
//        Cancel_Desloca();
        return;
    }

    POld.Reset(OldLat,OldLon);
    PNew.Reset(NewLat,NewLon);
    Delta_Lat =  POld.Latitude() - PNew.Latitude();
    Delta_Lon =  POld.Longitude() - PNew.Longitude();


    // Cria diretório temporário para as imagens deslocadas
    QString newPath = Informacao.DiretorioProc + "/Desloc";
    QString newPath2 = Informacao.DiretorioProc + "/Orig";

    QDir novoDir2(newPath);
    if (!novoDir2.exists()) {
        novoDir2.mkpath(".");
    }

    QDir novoDir3(newPath2);
    if (!novoDir3.exists()) {
        novoDir3.mkpath(".");
    }

    for (i = 1 ; i < Coords.PROC.Lon.length() ; i++) {
//        if (Coords.PROC.Direcao.at(i) != -1) continue;

        File = Coords.PROC.FileName.at(i);

        QFileInfo   Old_File(File);

        if (Coords.PROC.Direcao.at(i) == 1) {
            New_File = Old_File.absolutePath() + "/Orig/" +  Old_File.completeBaseName() + "_orig.tif";
        } else if (Coords.PROC.Direcao.at(i) == -1) {
            New_File = Old_File.absolutePath() + "/Desloc/" +  Old_File.completeBaseName() + "_deslocado.tif";
        } else {
            continue;
        }

        QFile New(New_File);
        if (New.exists()) {
            New.remove(New_File);
        }

        // Depois, talves, mudar a matriz no próprio arquivo, sem copiar
        QFile::copy(File,New_File);

        if (Coords.PROC.Direcao.at(i) == 1) continue;

        GDALDataset *pOldDS;
        pOldDS = (GDALDataset *) GDALOpen(New_File.toStdString().c_str(),GA_Update);
        double transform[6] = {0,1,0,0,0,1};
        pOldDS->GetGeoTransform(transform);

        GeographicLib::GeoCoords    DUTM;

        DUTM.Reset( transform[3] + transform[5] * Coords.PROC.Ih.at(i),
                    transform[0] + transform[1] * Coords.PROC.Iw.at(i) );

        MinX = Coords.PROC.Lon_A.at(i) + Delta_Lon;    ////
//        MaxX = DUTM.Longitude() + Delta_Lon;
//        MinY = DUTM.Latitude() + Delta_Lat;
        MaxY = Coords.PROC.Lat_A.at(i) + Delta_Lat;    ////


//        AUTM.Reset(MaxY,MinX);
//        Geo_Img.DUTM.Reset(MinY,MaxX);

//        int nCols = pOldDS->GetRasterXSize();
//        int nRows = pOldDS->GetRasterYSize();
        transform[0] = MinX;
//        transform[1] = abs(Geo_Img.AUTM.Longitude() - Geo_Img.DUTM.Longitude()) / (Geo_Img.Ih);
//        transform[2] = 0;
        transform[3] = MaxY;
//        transform[4] = 0;
//        transform[5] = - abs(Geo_Img.AUTM.Latitude() - Geo_Img.DUTM.Latitude()) / (Geo_Img.Iw);
        pOldDS->SetGeoTransform(transform);
        GDALClose(pOldDS);
    }

    ui->txt_Output->append(" \n Terminado");

}
*/

void MainWindow::on_actionMostrar_informa_o_dire_es_triggered()
{
    char            intStr[100];

    ui->txt_Output->append("\n----------------------------------");
    ui->txt_Output->append("Informações das imagens originais:");

    sprintf(intStr," Ângulo de ida = %4.6f", Coords.IMGS.Media_Ida);
    ui->txt_Output->append(intStr);
    sprintf(intStr," Ângulo de volta = %4.6f", Coords.IMGS.Media_Volta);
    ui->txt_Output->append(intStr);

    sprintf(intStr," Imagens na ida = %d, imagens na volta = %d, imagens nas curvas = %d",
            Coords.IMGS.Ida,
            Coords.IMGS.Volta,
            Coords.IMGS.Curvas);
    ui->txt_Output->append(intStr);

    ui->txt_Output->append(" " + QString::number(Coords.IMGS.Num_Ida) + " idas e " +
                           QString::number(Coords.IMGS.Num_Volta) + " voltas.");

    sprintf(intStr," Última imagem da primeira linha = %d, primeira imagem da última linha = %d",
                   Coords.IMGS.Ultimo_Primeira,Coords.IMGS.Primeiro_Ultima);
    ui->txt_Output->append(intStr);

    ui->txt_Output->append("\n Informações das imagens processadas:");

    sprintf(intStr," Ângulo de ida = %4.6f", Coords.IMGS.Media_Ida);
    ui->txt_Output->append(intStr);
    sprintf(intStr," Ângulo de volta = %4.6f", Coords.IMGS.Media_Volta);
    ui->txt_Output->append(intStr);

    sprintf(intStr," Imagens na ida = %d, imagens na volta = %d, imagens nas curvas = %d",
            Coords.IMGS.Ida,
            Coords.IMGS.Volta,
            Coords.IMGS.Curvas);
    ui->txt_Output->append(intStr);

    ui->txt_Output->append(" " + QString::number(Coords.PROC.Num_Ida) + " idas e " +
                           QString::number(Coords.PROC.Num_Volta) + " voltas.");

    sprintf(intStr," Última imagem da primeira linha = %d, primeira imagem da última linha = %d",
                   Coords.PROC.Ultimo_Primeira,Coords.PROC.Primeiro_Ultima);
    ui->txt_Output->append(intStr);

    updateTxt();

}


void MainWindow::on_actionMostrar_informa_o_pilha_de_imagens_triggered()
{
    char            intStr[100];
    int             Num;


    Num = Informacao.ListadeArquivos_MergeProc.length();

    if (Num == 0) {
        ui->txt_Output->append("\n----------------------------------");
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("Pilha de imagens vazia.");
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        return;
    }

    ui->txt_Output->append("\n----------------------------------");
    ui->txt_Output->append("Ordem das imagens:");

    for (int i = 0 ; i < Num ; i++) {
        ui->txt_Output->append(Informacao.ListadeArquivos_MergeProc.at(i));
    }
}


void MainWindow::on_bt_MudaResolucao_clicked()
{
    QString File;

//    File = "F:/Drone/2023-03-16-SPS-P1/DJI_202303170223_007_Mapping2/tagged_Teste_PPK/DJI_20230317022927_0001.JPG";

     File = QFileDialog::getOpenFileName(this, tr("Imagem JPG"),"F:/Drone/2023-03-16-SPS-P1/DJI_202303170223_007_Mapping2/tagged_Teste_PPK",tr("Image (*.jpg)"));



    getEXIF_XML_Info(File);
    on_actionEXIF_XMP_triggered();

    ui->txt_Output->clear();
    ui->txt_Output->setText(Geometria_Img.ExifXMPInfo);

}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    windowPrincipal = ui;

    Informacao.Diretorio_EXE = QDir::currentPath();
    Informacao2.Diretorio_EXE = QDir::currentPath();
    Informacao.Diretorio_Img_Original = "";
    Informacao2.Diretorio_Img_Original = "";

    /////////////////////////////////////////////////////////////////////////////////
    /// Idioma
    ///
    QString P1 = QGuiApplication::inputMethod()->locale().name();
    QString P2 = QLocale::languageToString(locale().language());
/*
    ui->txt_Output->append(QLocale::system().name());
    ui->txt_Output->append(QLocale::system().nativeCountryName());
    ui->txt_Output->append(QLocale::system().nativeLanguageName());
    ui->txt_Output->append(P1);
    ui->txt_Output->append(P2);
*/
    if (((P1.toUpper() == "EN_US") || (P2.toUpper() == "ENGLISH"))) {
        on_actionIngl_s_2_triggered();
    }
    ui->txt_Output->append(tr("\n Identificação de Idioma -> ") + P1 + " " + P2);


    actKMZProc = NULL;
    actInfoProc = NULL;
    actMesclar = NULL;


    GeoProc.DeslocaImagem = false;
    Geo_Tiff.DeslocaImagem = false;

    ICE_Pressed = false;
    TIF_Pressed = false;

    ui->list_Files_Proc->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->list_Files_Result->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->list_Files->setSelectionMode(QAbstractItemView::MultiSelection);

    myProcess = new QProcess(this);


    MeFu.Atualiza_Raio_Marcacao_Coordenadas(30);


    connect(this,SIGNAL(ResetProgressBarMesclar()),this,SLOT(ResetProgressBarMesclar_EXEC()));
    connect(&mThread_Merge,SIGNAL(AtualizaProgressBarMesclar()),this,SLOT(AtualizaProgressBarMesclar_Merge()));


    QObject::connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateTxt()),Qt::DirectConnection);
//    QObject::connect(myProcess, SIGNAL(readyRead()), this, SLOT(updateTxt()),Qt::DirectConnection);


    connect(&mThread_RES,SIGNAL(finished()),this,SLOT(imprimeFimRES()),Qt::DirectConnection);
    connect(&mThread_ICE_TIF,SIGNAL(finished()),this,SLOT(imprimeFimICE_TIFF()),Qt::DirectConnection);
    connect(&mThread1,SIGNAL(finished()),this,SLOT(imprimeFimKMZ()),Qt::DirectConnection);
    connect(&mThread_Merge,SIGNAL(finished()),this,SLOT(imprimeFimMerge()),Qt::DirectConnection);


//    connect(&mThread1,SIGNAL(FimKMZ()),this,SLOT(Lista_Files_Result()),Qt::DirectConnection);

    //    iconCheckpix.load(":/imgH/checkicon3.png");
    //    iconCheck.addPixmap(iconCheckpix);
        iconCheckpix.load(":/imgH/checkicon3.png");
        iconCheckpixOff.load(":/imgH/checkicon2.png");
        iconCheck.addPixmap(QPixmap(":/imgH/checkicon3.png"),QIcon::Normal);
        iconCheck.addPixmap(QPixmap(":/imgH/checkicon2.png"),QIcon::Disabled);

//        QWidget::setWindowIcon()


    READCOUNT = 0;

    QStringList Labels;
    Labels << "Arquivo" << "Size";

    this->setWindowTitle(tr("Geo 0.670 "));  // Quando muda de idioma está alterando o nome da janela

    QLocale::setDefault(QLocale::Portuguese);
//    QLocale::setDefault(QLocale::English);

    // Ajusta parâmetos da tabela de pontos

    Monta_Tabela_Inicial();


    //////////////////////////////////// Esconde os comandos para a versão de produção

    ui->bt_Teste->setHidden(true);
//  ui->bt_Teste->setVisible(false);
    ui->btOpenFile->setHidden(true);
    ui->pushButton_2->setHidden(true);

    ui->actionMover_imagens->setEnabled(true);
    ui->actionMover_imagens->setCheckable(true);


    ui->actionM_dia->setVisible(false);

    ui->actionSave_KML->setEnabled(false);
    ui->actionLoad_KML->setEnabled(false);

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
    ui->progressBar1->setVisible(false);

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

    ui->actionHarley_3->setVisible(false);
    ui->actionMiranda_2->setVisible(false);
    ui->menuAlgoritmo->setHidden(true);

    // ui->menuBar->setHidden(true);

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




    ui->pushButton->setEnabled(false);
//    ui->checkBox_Linhas->setEnabled(false);

    ui->txt_Mesclar_Name->setVisible(false);

//    ui->txt_Nome_Imagem->setEnabled(false);

    ui->actionThread_2->setEnabled(true);
    ui->actionThread_2->setCheckable(true);
    ui->actionThread_2->setVisible(false);

    ui->bt_DialogPontos->setEnabled(false);
    ui->actionPontos->setEnabled(false);
    ui->bt_limpaProc->setEnabled(false);
    ui->bt_EXIF_XMP_List->setEnabled(false);
    ui->actionEXIF_XMP->setEnabled(false);


    QRegularExpressionValidator* rxv = new QRegularExpressionValidator(QRegularExpression("\\d*"), this); // only pos
//    QRegExpValidator* rxv = new QRegExpValidator(QRegExp("[+-]?\\d*"), this); // pos and neg

    ui->txt_Reducao->setAlignment(Qt::AlignRight);
    ui->txt_Reducao->setValidator(rxv);
    Informacao.Reducao = 3;
    ui->txt_Reducao->setText(QString::number(Informacao.Reducao));

    ui->txt_Escala->setAlignment(Qt::AlignRight);
    ui->txt_Escala->setValidator(rxv);
    Informacao.Escala1 = 0.25;
    ui->txt_Escala->setText(QString::number(Informacao.Escala1 * 100));

    ui->bt_Reduzir->setEnabled(false);

    ui->bt_ProcessarImgs->setEnabled(false);

    ui->txt_Path->setVisible(false);

    ui->bt_Cancelar_Proc->setEnabled(false);
    CancelarProc = false;

    ui->bt_Dir_Proc->setEnabled(false);

    ui->checkBox_GTiff->setEnabled(true);
    ui->checkBox_JPG->setEnabled(false);
    ui->checkBox_KMZ->setEnabled(true);


    ui->bt_Lista_Arquivos->setEnabled(false);
    ui->bt_Mesclar->setEnabled(false);

    ui->radioButton_Merge->setEnabled(true);
    ui->radioButton_Stitch->setEnabled(false);

    ui->menuMadidas->setEnabled(false);

    ui->txt_Alt_Voo->setAlignment(Qt::AlignRight);
    ui->txt_Dist->setAlignment(Qt::AlignRight);
    ui->lineEdit_Area->setAlignment(Qt::AlignRight);

    uidialogPontos = NULL;
    dialogDeslocaImg = NULL;

    ui->actionValor->setEnabled(false);
    Geo.Alt_Default = 30.0;
    ui->actionValor->setText("Valor: 30.0 m");

    ui->label_Perimetro->setText("Caminho:");
    ui->label->setVisible(false);
    ui->lineEdit_Area->setVisible(false);

    ui->actionCortarTIFF->setEnabled(false);


    if (ui->actionThread_2->isVisible()) on_actionThread_2_triggered();

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
    thumb_Mundo.load(":/imgH/Mundo.png");

    ui->actionUsar_imagens_da_primeira_e_ltima_linha->setEnabled(true);
    ui->actionUsar_imagens_da_primeira_e_ltima_linha->setVisible(true);

    // Aqui vai as configurações

    ui->txt_Reducao->setAlignment(Qt::AlignRight);
    Informacao.Reducao = 3;
    ui->txt_Reducao->setText(QString::number(Informacao.Reducao));

    ui->txt_Escala->setAlignment(Qt::AlignRight);
    Informacao.Escala1 = 0.25;
    ui->txt_Escala->setText(QString::number(Informacao.Escala1 * 100));


    QFile   Config_File(Informacao.Diretorio_EXE + "/GeoConfigFile.txt");


    if (!Config_File.exists()) { // Se não existe arquivo de configuração -> CRIAR
        Grava_Config_File();
    }

    Le_Config_File();
    Exec_Config_File();

//    ui->txt_Output->append("------------------------------------------------------");

    on_actionDesativado_triggered();

    ui->actionThread_2->setChecked(false);
    ui->checkBox_GTiff->setChecked(true);
    ui->checkBox_JPG->setChecked(false);
    ui->checkBox_KMZ->setChecked(false);
    ui->radioButton_Merge->setChecked(true);
    ui->actionEspecificar_2->setChecked(false);
    ui->actionUsar->setChecked(false);
    ui->actionUsar->setEnabled(false);



    // Redução ?
    // Escala ?
    // Altura default ?


    ui->bt_MudaResolucao->setVisible(false); // botão de teste


    //////////////////////////////////////////////////////////////////////////
    ///
    ///
        QList <QScreen *>       screens = QGuiApplication::screens();
        QList <QRect>           screensGeometry;
        int                     screensNumber = screens.size();
        QString                 screensTxt = "";

//        screensTxt = "\n Número de monitores ativos = " + QString::number(screensNumber);
        screensTxt = "\n " + tr("Número de monitores ativos") + " = " + QString::number(screensNumber);

        for (int i = 0 ; i < screensNumber ; i++) {
            screensGeometry.append(screens.at(i)->geometry());
            int H = screensGeometry.at(i).height();
            int W = screensGeometry.at(i).width();
            if (i) screensTxt += "; " + QString::number(i+1) + ": " + QString::number(W) + " x " + QString::number(H);
            else screensTxt += " -> " + QString::number(i+1) + ": " + QString::number(W) + " x " + QString::number(H);
        }
        ui->txt_Output->append(screensTxt);


        // Faz o link para a rotina de alterar automaticamente resolução se mudar de monitor
/*
        this->setAttribute(Qt::WA_NativeWindow);
        connect(this->window()->windowHandle(), SIGNAL(screenChanged(QScreen*)),this,SLOT(sscreenChanged(QScreen*)));
*/
        // Verifica se o monitor é HD ou maior
/*
        int H = screensGeometry.at(0).height();
        if (H < 1080) {
            ui->action1280x720->setChecked(true);
            on_action1280x720_triggered();
        }
*/

     QScreen *      screen = QGuiApplication::primaryScreen();
     QRect          screenGeometry = screen->geometry();

     if (screenGeometry.height() < 970) {
         ui->action1280x720->setChecked(true);
         on_action1280x720_triggered();
     }


     ///
     ///
     //////////////////////////////////////////////////////////////////////////


}


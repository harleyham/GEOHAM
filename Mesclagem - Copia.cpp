#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>

#include <QObject>
#include <QTimer>

#include <windows.h>

#include "mainwindow.h"

#include "ui_mainwindow.h"

#include "warp.h"

#include "translate.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/stitching.hpp>


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>

#include <QCoreApplication>

using namespace cv;
using namespace std;

extern  Geometria_Imagem Geo_Img;

//void MainWindow::Gera_Merge(QString NameTIF)
void MainWindow::Gera_Merge(int Cor_Nodata)
{
    char intStr[80];

    int Num_Elementos = 11 + Imagem_Composta.Geo_File_List.length();
    int Tamanho = Num_Elementos + 1;

    timer2.restart();

    ////////////////////////////// TIFF
    QString NameTIF;
    NameTIF = Imagem_Composta.Complete_Name.path() + "/" + Imagem_Composta.Complete_Name.completeBaseName() + ".tif";

    char **argv3;
    argv3 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

    int i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv3[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
    }
    argv3[Num_Elementos] = nullptr;

    strcpy(argv3[0],"gdalwarp.exe");
    strcpy(argv3[1],"-of");
    strcpy(argv3[2],"GTiff");


    strcpy(argv3[3],"-srcnodata");
    sprintf(intStr,"%d",Cor_Nodata);
    strcpy(argv3[4],intStr);

    strcpy(argv3[5],"-dstnodata");
    if (ui->actionBranco->isChecked()) {
        strcpy(argv3[6],"255");
    } else {
        strcpy(argv3[6],"0");
    }

    strcpy(argv3[7],"-overwrite");
    strcpy(argv3[8],"-co");
    strcpy(argv3[9],"COMPRESS=JPEG");

    for (i = 0 ; i < Imagem_Composta.Geo_File_List.length() ; i++) {
        strcpy(argv3[(10+i)],Imagem_Composta.Geo_File_List.at(i).fileName().toStdString().c_str());
    }

    strcpy(argv3[10+i],NameTIF.toStdString().c_str());

    warp_H(Num_Elementos,static_cast<char **>(argv3));

    for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
        free(argv3[i]);
    }


    qint64  Elapsed;

    Elapsed = timer2.elapsed();
    sprintf(intStr," Merge gerado em %3.3f segundos.",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


    // Se tiver, mandar apagar Thumb antiga.
    QFile Old_t(Informacao.DiretorioResult + "/thumb/" + Imagem_Composta.Complete_Name.baseName() + "-thumb.jpg");
    if (Old_t.exists()) {
        Old_t.remove(Informacao.DiretorioResult + "/thumb/" + Imagem_Composta.Complete_Name.baseName() + "-thumb.jpg");
    }

    Lista_Files_Imgs();
    Lista_Files_Proc();
    Lista_Files_Result();

}

void MainWindow::on_bt_Mesclar_clicked()
{
    QString     Name,NameKMZ,Complete;
    qint64      Elapsed;
    char        intStr[100];

    if (ui->actionMiranda->isChecked()) {
        if (Informacao2.TXT_File == "") {
            QMessageBox::information(this,tr("File Name"),"Arquivo TXT não definido !");
            return;
        }
    }

    Complete = QFileDialog::getSaveFileName(this, tr("Imagem a ser gravada"),Informacao.DiretorioResult,tr("Image (*.tif)"));
    if (Complete.isNull()) return;
    Imagem_Composta.Complete_Name.setFile(Complete);
    Imagem_Composta.Nome = Complete;

    ui->txt_Mesclar_Name->setText(Complete);

    ui->txt_Path->setText(" ");

    QDir::setCurrent(Informacao.DiretorioProc);

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

    ui->txt_Output->setText("    Mesclando imagens.\n");
    QCoreApplication::processEvents();

//    char **Metadata;
    GDALAllRegister();

    double Cor_Nodata;

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

        Cor_Nodata = pNewDS->GetRasterBand(1)->GetNoDataValue();

        char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());
//        char** papszIter = papszMetadata;

        sprintf(intStr,"[%d/%d] %s (%f,%f)",(k+1),Imagem_Composta.Geo_File_List.length(),Temp.toStdString().c_str(),cy,cx);
        ui->txt_Output->append(intStr);

        sprintf(intStr,"--------------------------------------------------------------------------------------------------");
        ui->txt_Output->append(intStr);

        GDALClose(pNewDS);
    }

    Scx = (Smaxx - Sminx) /2 + Sminx;
    Scy = (Smaxy - Sminy) /2 + Sminy;


    // Calcula tamanho da imagem em metros quadrados
/*
    GeographicLib::GeoCoords UL,RD;
    UL.Reset(Sminy,Sminx);
    RD.Reset(Smaxy,Smaxx);

    double Area;
    Area = fabs(UL.Easting() - RD.Easting()) * fabs(UL.Northing() - RD.Northing());

    sprintf(intStr,"(%f, %f) - (%f, %f). Centro(%f,%f).\nÁrea %d metros quadrados\n",Smaxy,Sminx,Sminy,Smaxx,Scy,Scx,static_cast<int> (Area));
    ui->txt_Output->append(intStr);

*/

    if (ui->checkBox_GTiff->isChecked()) {
        ui->txt_Output->append("\n Iniciando geração GeoTiff.");
        QCoreApplication::processEvents();

        timer2.restart();

        // Monta Tiff
         if (ui->radioButton_Merge->isChecked()) {
            mThread_Merge.NameTIF = Complete;
            mThread_Merge.Imagem_Composta = Imagem_Composta;

            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


            // O Thread do Merge está sem ser testasdo de acordo
//            if (ui->actionThread_2->isChecked()) {
//                mThread_Merge.start();
//            } else {


            if (ui->actionGDAL->isChecked()) {
               Gera_Merge(Cor_Nodata);
            } else {
               Gera_Merge_Interno(Cor_Nodata);
            }

               Size_GeoTiff(Complete, true);
//            }
         } else {
             timer2.restart();
             Mat pano;
             bool divide_images = false;
             vector<Mat> imgs;

             sprintf(intStr,"\n Iniciando Stitching.\n");
             ui->txt_Output->append(intStr);

             QDir directory(Informacao.DiretorioProc);
             QStringList lista = directory.entryList(QStringList() << "*.tif",QDir::Files);

             for (int i = 0 ; i < lista.length() ; i++) {
                 cout << lista.at(i).toStdString() << "\n";

                 String S;
                 S = Informacao.DiretorioProc.toStdString() + "/" + lista.at(i).toStdString();
                 sprintf(intStr,"Lendo %s\n",S.c_str());
                 ui->txt_Output->append(intStr);

                 Mat img = cv::imread(S);
                 if (img.empty())
                 {
                     cout << "Can't read image '" << S << "'\n";
                     return;
                 }
                 imgs.push_back(img);
             }

             Stitcher::Mode mode = Stitcher::SCANS;//        PANORAMA
             Ptr<Stitcher> stitcher = Stitcher::create(mode);
             Stitcher::Status status = stitcher->stitch(imgs, pano);
             if (status != Stitcher::OK)
             {
                 sprintf(intStr,"Erro no processamento do Stitching.");
                 ui->txt_Output->append(intStr);
             } else {
                 std::vector<int> params;
                 params.push_back(IMWRITE_TIFF_COMPRESSION);
                 cv::imwrite(Name.toStdString(),pano);

                 cout << "stitching completed successfully\n" << Name.toStdString() << " saved!";
                 sprintf(intStr,"%s",Name.toStdString().c_str());
                 ui->txt_Output->append(intStr);
                 Elapsed = timer2.elapsed();
                 sprintf(intStr,"Processado em %d segundos.",static_cast<int>(Elapsed/1000));
                 ui->txt_Output->append(intStr);
             }
         }
     }
}

void MainWindow::Gera_Merge_Interno(int Cor_NoDATA)
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

//        char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

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
            Geo_Img.DUTM.Reset(Geo_Img.AUTM.Zone(),Geo_Img.AUTM.Northp(),Geo_Img.AUTM.Easting() + Geo_Img.mppx * Geo_Img.Iw,Geo_Img.AUTM.Northing() + Geo_Img.mppy * Geo_Img.Ih);
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

    Geo_Img.Cc.real((MaxY + MinY) / 2);
    Geo_Img.Cc.imag((MaxX + MinX) / 2);

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
    cv::Mat Img_Total(Y,X,CV_8UC3,Scalar(Cor_NoDATA,Cor_NoDATA,Cor_NoDATA));


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

        img.copyTo(Img_Total(Rect(PosX,PosY,img.cols ,img.rows)),MaskImage != Cor_NoDATA);

        ui->progressBar_Mesclar->setValue(i);
    }

    MAT2GDAL(Imagem_Composta.Nome,Img_Total,NULL,Geo_Img);

    // Se tiver, mandar apagar Thumb antiga.
    QFile Old_t(Informacao.DiretorioResult + "/thumb/" + Imagem_Composta.Complete_Name.baseName() + "-thumb.jpg");
    if (Old_t.exists()) {
        Old_t.remove(Informacao.DiretorioResult + "/thumb/" + Imagem_Composta.Complete_Name.baseName() + "-thumb.jpg");
    }

    Lista_Files_Imgs();
    Lista_Files_Proc();
    Lista_Files_Result();

}

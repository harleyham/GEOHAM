#include <QMainWindow>
#include <QString>
#include <QFileDialog>
//#include <QTime>
#include <QElapsedTimer>
#include <QDir>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>


#include "GDAL_HAM.h"
#include "translate.h"
#include "warp.h"
#include "info_H.h"

using namespace std;
using namespace cv;

extern cv::Mat              imageRead;
extern Geometria_Imagem     GeoProc,Geo_ICE,Geo_Tiff,Geo_Img,Geo;


void MainWindow::on_actionTIFs_ICE_JPG_triggered()
{
    QString         InputJPG,OutputTif,Dir;
    char            intStr[100];

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
    char            intStr[100];

    QFileInfo       CompleteNameTiff(GeoProc.Name);

    if (GeoProc.Name == "") {
        QMessageBox::information(this,tr("Erro"),tr("Pontos não coletados"));
        return;
    }

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

    ///////// Aqui deve iniciar nova thread
    mThread_ICE_TIF.NameTIF = GeoProc.Name;
    mThread_ICE_TIF.Output_VRT = Output_VRT;
    mThread_ICE_TIF.NameICE = Geo_ICE.Name;
    mThread_ICE_TIF.Geo_Tiff = Geo_Tiff;
    mThread_ICE_TIF.Geo_ICE = Geo_ICE;
    if (ui->actionPreto->isChecked()) {
        mThread_ICE_TIF.Cor_Nodata = 0;
    } else {
        mThread_ICE_TIF.Cor_Nodata = 255;
    }

    Txt = "\n Comandos GDAL equivalente:\ngdal_translate -of vrt -a_srs EPSG:4326 -a_nodata " + QString::number(mThread_ICE_TIF.Cor_Nodata) + " ";
    for (int j = 0 ; j < Geo_Tiff.Pontos.length() ; j++) {
//        sprintf(intStr,"Coordenada (%f,%f) -> (%d,%d) ",Geo_Tiff.Pontos.at(j).real(),Geo_Tiff.Pontos.at(j).imag(),Geo_ICE.PImg.at(j).real(),Geo_ICE.PImg.at(j).imag());
          sprintf(intStr,"-gcp %d %d %f %f ",Geo_ICE.PImg.at(j).real(),Geo_ICE.PImg.at(j).imag(),Geo_Tiff.Pontos.at(j).imag(),Geo_Tiff.Pontos.at(j).real());
        Txt = Txt + intStr;
    }

    Txt = Txt + Geo_ICE.Name + " " + Output_VRT;

    ui->txt_Output->append(Txt + "\n");

    Txt = "gdalwarp -s_srs EPSG:4326 -t_srs EPSG:4326 -overwrite -co COMPRESS=JPEG -srcnodata 0 -dstnodata " + QString::number(mThread_ICE_TIF.Cor_Nodata) + " " + Output_VRT + " " + GeoProc.Name + "\n";
    ui->txt_Output->append(Txt);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    ActiveProgressBarMesclar_EXEC();

    mThread_ICE_TIF.start();
}

void MainWindow::on_actionCapturar_Pontos_triggered()
{
    QString InputJPG,InputTif,OutputTif,Name,Dir;
    int     No_Data;
    char    intStr[100];

    InputJPG = QFileDialog::getOpenFileName(this, tr("ICE Image"),Informacao.DiretorioResult,tr("Image (*.jpg)"));
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

    QString Out;
    info_H(InputTif,Out);

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(InputTif.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

    MainWindow::GDAL2MAT(poSrcDS,Destino);

    double Transform[6];
    pOldDS->GetGeoTransform(Transform);
    Geo.Name = InputTif;

    bool    E_UTM = false;
    int     Zona = 0;
    int     Hemisferio = 0;

    Zona = pOldDS->GetSpatialRef()->GetUTMZone();
    const int A = pOldDS->GetSpatialRef()->IsGeographic();
    if (A) E_UTM = false; else E_UTM = true;


    if (!E_UTM) {  // LAT LON
        Geo.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
        Geo.Iw = pOldDS->GetRasterXSize();
        Geo.Ih = pOldDS->GetRasterYSize();
        Geo.DUTM.Reset( Transform[3] + Transform[5] * Geo.Ih,Transform[0] + Transform[1] * Geo.Iw );
        Geo.mppx = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting()) / Geo.Iw; // LON
        Geo.mppy = -abs(Geo.DUTM.Northing() - Geo.AUTM.Northing()) / Geo.Ih; // LAT
        Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
    } else { // UTM
        double      Lat,Lon;
        bool        Ok;

        Ok = GetLatLonfromINFO(Out,Lat,Lon);

        Geo.AUTM.Reset(Lat,Lon);
        Geo.Iw = pOldDS->GetRasterXSize();
        Geo.Ih = pOldDS->GetRasterYSize();
        Geo.mppx = Transform[1]; // LON
        Geo.mppy = Transform[5]; // LAT
        Geo.DUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * Geo.Iw,Geo.AUTM.Northing() + Geo.mppy * Geo.Ih);
        Geo.CcUTM.Reset(Geo.AUTM.Zone(),Geo.AUTM.Northp(),Geo.AUTM.Easting() + Geo.mppx * (Geo.Iw /2),Geo.AUTM.Northing() + Geo.mppy * (Geo.Ih /2));
        Geo.Dw = abs(Geo.DUTM.Easting() - Geo.AUTM.Easting());
        Geo.Dh = abs(Geo.DUTM.Northing() - Geo.AUTM.Northing());
        Geo.A.real(Geo.AUTM.Latitude());
        Geo.A.imag(Geo.AUTM.Longitude());
        Geo.D.real(Geo.DUTM.Latitude());
        Geo.D.imag(Geo.DUTM.Longitude());

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


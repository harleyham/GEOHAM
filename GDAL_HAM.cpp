#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>

#include <QString>
#include <vector>

//#include "cpl_string.h"
#include "gdal_version.h"
#include "gdal_priv.h"
#include "ogr_spatialref.h"
//#include "commonutils.h"
#include "gdal_utils_priv.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "geo_exif_xmp.h"
#include "GDAL_HAM.h"
#include "info_H.h"


#ifdef XML_LARGE_SIZE
#  define XML_FMT_INT_MOD "ll"
#else
#  define XML_FMT_INT_MOD "l"
#endif

#ifdef XML_UNICODE_WCHAR_T
#  include <wchar.h>
#  define XML_FMT_STR "ls"
#else
#  define XML_FMT_STR "s"
#endif


extern  Geometria_Imagem    Geo_Img;
//extern  Plot_Coords         Coords;


////////////////////////////////////////////////////////////


void Grava_VRT_from_file(QString fileOrigem, QString fileVrt) {
    GDALDataset *pNewDS;
    GDALDriver *poDriver2 = (GDALDriver *) GDALGetDriverByName( "VRT" );
    GDALDataset *poVRTDS2;
    pNewDS = (GDALDataset *) GDALOpen(fileOrigem.toStdString().c_str(),GA_ReadOnly);
    poVRTDS2 = poDriver2->CreateCopy( fileVrt.toStdString().c_str(), pNewDS, FALSE, NULL, NULL, NULL );
    GDALClose((GDALDatasetH) poVRTDS2);
}



void MainWindow::GDAL2MAT(GDALDataset *pOldDS,cv::Mat &output_image) {
    std::vector<cv::Mat> image_list;

    int channels = pOldDS->GetRasterCount();
    int nCols = pOldDS->GetRasterBand(1)->GetXSize();
    int nRows = pOldDS->GetRasterBand(1)->GetYSize();
    int Dif_Channels;


    GDALDataType       Bits_Per_Pixel;
    int                Type;

    Bits_Per_Pixel = pOldDS->GetRasterBand(1)->GetRasterDataType();

    /*
    // Iterate over each channel
    for (int i = channels; i >= 1; --i) // Na ordem inversa pois cv::MAT é BGR
    {
        // Fetch the band
        GDALRasterBand* band = pOldDS->GetRasterBand(i);
        cv::Mat M = cv::Mat( nRows, nCols, CV_8UC1 );

        band->RasterIO(GF_Read,0,0,nCols,nRows,M.data,nCols,nRows,GDT_Byte,0,0);
        image_list.push_back(M);
    }
    */
    // Iterate over each channel without channel 4 (alpha)

    Dif_Channels = channels - 3;
    if (channels > 3) {

//        channels = 3;
    }
    for (int i = (channels - Dif_Channels) ; i >= 1 ; --i) // Na ordem inversa pois cv::MAT é BGR
    {


        // Fetch the band
        GDALRasterBand* band = pOldDS->GetRasterBand(i);

        switch (Bits_Per_Pixel) {
            case GDT_Byte:
                Type = CV_8UC1;
                break;

            case GDT_UInt16:

            case GDT_Int16:
                Type = CV_16UC1;
                break;

            default:
                Type = CV_8UC1;
        }

        cv::Mat M = cv::Mat( nRows, nCols, Type );

        band->RasterIO(GF_Read,0,0,nCols,nRows,M.data,nCols,nRows,Bits_Per_Pixel,0,0);

        image_list.push_back(M);

//        ProgressBarValue = 16.67 * (4 - i);
//        ui->progressBar_Mesclar->setValue(ProgressBarValue);
    }

    // Merge images
    cv::merge( image_list, output_image );
}

void GDAL2MAT(GDALDataset *pOldDS,cv::Mat &output_image) {
    std::vector<cv::Mat> image_list;

    int   ProgressBarValue = 0;

    int channels = pOldDS->GetRasterCount();
    int nCols = pOldDS->GetRasterBand(1)->GetXSize();
    int nRows = pOldDS->GetRasterBand(1)->GetYSize();

    /*
    // Iterate over each channel
    for (int i = channels; i >= 1; --i) // Na ordem inversa pois cv::MAT é BGR
    {
        // Fetch the band
        GDALRasterBand* band = pOldDS->GetRasterBand(i);
        cv::Mat M = cv::Mat( nRows, nCols, CV_8UC1 );

        band->RasterIO(GF_Read,0,0,nCols,nRows,M.data,nCols,nRows,GDT_Byte,0,0);
        image_list.push_back(M);
    }
    */
    // Iterate over each channel without channel 4 (alpha)
    if (channels > 3) {
        channels = 3;
    }
    for (int i = channels; i >= 1; --i) // Na ordem inversa pois cv::MAT é BGR
    {


        // Fetch the band
        GDALRasterBand* band = pOldDS->GetRasterBand(i);
        cv::Mat M = cv::Mat( nRows, nCols, CV_8UC1 );

        band->RasterIO(GF_Read,0,0,nCols,nRows,M.data,nCols,nRows,GDT_Byte,0,0);
        image_list.push_back(M);

        ProgressBarValue = 16.67 * (4 - i);
        fprintf(stderr,"%d",ProgressBarValue);
//        ui->progressBar_Mesclar->setValue(ProgressBarValue);
    }

    // Merge images
    cv::merge( image_list, output_image );
}

static QImage MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    return QImage();
}

QPixmap MAT2PIX(QString FileName) {
    cv::Mat image= cv::imread(FileName.toStdString(), cv::IMREAD_COLOR);

//    cv::Mat histEquImg;
//    equalizeHist(image, histEquImg);

    QImage qimage = MatToQImage(image);
    return (QPixmap::fromImage(qimage));
}



void GDAL2PIX(QString FileName,QPixmap pix) {

    GDALAllRegister();

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(FileName.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
//    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());


//    int channels = pOldDS->GetRasterCount();
//    int cols = pOldDS->GetRasterBand(1)->GetXSize();
//    int rows = pOldDS->GetRasterBand(1)->GetYSize();
/*
    GDALRasterBand  *poBand;
    int             nBlockXSize, nBlockYSize;
    int             bGotMin, bGotMax;
    double          adfMinMax[2];
    poBand = poDataset->GetRasterBand( 1 );
    poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
    adfMinMax[0] = poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = poBand->GetMaximum( &bGotMax );
    if( ! (bGotMin && bGotMax) )
        GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);


    float *pafScanline;
    int   nXSize = poBand->GetXSize();
    int   nYSize = poBand->GetYSize();
    pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize * nYSize);
    poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize,
                      pafScanline, nXSize, nYSize, GDT_Float32, 0, 0 );

    QImage* image = new QImage((unsigned char*)pafScanline,
                              nXSize, nYSize,
                              QImage::Format_RGB32);





     pix = pix.fromImage(qtImage);
*/
     GDALClose(pOldDS);
}

void MainWindow::MAT2GDAL(QString output,cv::Mat output_image,char **papszMetadata,double *Transform)
{
    std::vector<cv::Mat> image_list;

    int channels = output_image.channels();
    int nCols = output_image.cols;
    int nRows = output_image.rows;

    char StrValue[20];

#ifdef GDAL_WRITE

    GDALDataset *pOldDS;
    GDALDriver *pDriverTiff;

    pDriverTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
    pOldDS = pDriverTiff->Create(output.toStdString().c_str(),nCols,nRows,channels,GDT_Byte,NULL);


    if (papszMetadata) {
        pOldDS->SetMetadata( papszMetadata );
        pOldDS->SetMetadataItem("EXIF_Software","HAM");
        sprintf(StrValue,"%d",nCols);
        pOldDS->SetMetadataItem("EXIF_PixelXDimension",StrValue);
        sprintf(StrValue,"%d",nRows);
        pOldDS->SetMetadataItem("EXIF_PixelYDimension",StrValue);
    }

    pOldDS->SetProjection("WGS84");
    pOldDS->SetGeoTransform(Transform);

#else

    std::vector<int> params;
    params.push_back(cv::IMWRITE_TIFF_COMPRESSION);
    cv::imwrite(output.toStdString(),output_image);

#endif

#ifdef GDAL_WRITE
    cv::Mat channel[3];

    cv::split(output_image,channel);

    // Iterate over each channel

    for (int i = 1 ; i <= channels; i++)
    {
        // Fetch the band
        GDALRasterBand* band = pOldDS->GetRasterBand(i);
        int Cor_Nodata;
        if (ui->actionBranco->isChecked()) {
            Cor_Nodata = 255;
        } else {
            Cor_Nodata = 0;
        }

        band->SetNoDataValue(Cor_Nodata);

        band->RasterIO(GF_Write,0,0,nCols,nRows,channel[(channels - i)].data,nCols,nRows,GDT_Byte,0,0);

        ProgressBarValue = 16.67 * (i + 3);
        ui->progressBar_Mesclar->setValue(ProgressBarValue);
    }

    GDALClose(pOldDS);
    CapturaStdout = false;
#else
    writeGeoInfo(output,output_image,papszMetadata,Transform);
#endif
}

void MAT2GDAL(QString output,cv::Mat output_image,char **papszMetadata,double *Transform)
{
    std::vector<cv::Mat> image_list;

    int channels = output_image.channels();
    int nCols = output_image.cols;
    int nRows = output_image.rows;

    char StrValue[20];

    int ProgressBarValue;

#ifdef GDAL_WRITE

    GDALDataset *pOldDS;
    GDALDriver *pDriverTiff;

    pDriverTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
    pOldDS = pDriverTiff->Create(output.toStdString().c_str(),nCols,nRows,channels,GDT_Byte,NULL);


    if (papszMetadata) {
        pOldDS->SetMetadata( papszMetadata );
        pOldDS->SetMetadataItem("EXIF_Software","HAM");
        sprintf(StrValue,"%d",nCols);
        pOldDS->SetMetadataItem("EXIF_PixelXDimension",StrValue);
        sprintf(StrValue,"%d",nRows);
        pOldDS->SetMetadataItem("EXIF_PixelYDimension",StrValue);

    }

    pOldDS->SetProjection("WGS84");
    pOldDS->SetGeoTransform(Transform);

#else

    std::vector<int> params;
    params.push_back(cv::IMWRITE_TIFF_COMPRESSION);
    cv::imwrite(output.toStdString(),output_image);

#endif

#ifdef GDAL_WRITE
    cv::Mat channel[3];

    cv::split(output_image,channel);

    // Iterate over each channel

    for (int i = 1 ; i <= channels; i++)
    {
        // Fetch the band
        GDALRasterBand* band = pOldDS->GetRasterBand(i);
        int Cor_Nodata;
/*
        if (ui->actionBranco->isChecked()) {
            Cor_Nodata = 255;
        } else {
            Cor_Nodata = 0;
        }
*/
        Cor_Nodata = 255;  // Isto é acochambração. Tem que verificar a cor que o usuário escolheu

        band->SetNoDataValue(Cor_Nodata);

        band->RasterIO(GF_Write,0,0,nCols,nRows,channel[(channels - i)].data,nCols,nRows,GDT_Byte,0,0);

        ProgressBarValue = 16.67 * (i + 3);
        fprintf(stderr,"%d",ProgressBarValue);

//        ProgressBarValue = 16.67 * (i + 3);
//        ui->progressBar_Mesclar->setValue(ProgressBarValue);
    }

    GDALClose(pOldDS);
//    CapturaStdout = false;
#else
    writeGeoInfo(output,output_image,papszMetadata,Transform);
#endif
}


void MainWindow::MAT2GDAL(QString output, cv::Mat output_image, char **papszMetadata, Geometria_Imagem &Geometria_ImgProc, Imagem_Total *Imagem) {
    char                intStr[50];

    std::vector<cv::Mat> image_list;

    int channels = output_image.channels();
    int nCols = output_image.cols;
    int nRows = output_image.rows;

    char StrValue[20];
    double transform[6] = {0,1,0,0,0,1};

#ifdef GDAL_WRITE

    GDALDataset *pOldDS;
    GDALDriver *pDriverTiff;


    pDriverTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
    pOldDS = pDriverTiff->Create(output.toStdString().c_str(),nCols,nRows,channels,GDT_Byte,NULL);


    try {
        pOldDS->SetProjection("WGS84");
    } catch (int &e) {
        ui->txt_Output->append(" \n Erro ! " + QString::number(e));
    }

#else
    std::vector<int> params;
    params.push_back(cv::IMWRITE_TIFF_COMPRESSION);
    cv::imwrite(output.toStdString(),output_image,params);
#endif

#ifdef GDAL_WRITE

/*
    if (channels == 3) {
        cv::Mat channel[3];
    } else if (channels == 4) {
        cv::Mat channel[4];
    } else {
        // Erro
        return;
    }

    cv::split(output_image,channel);
*/

    std::vector<cv::Mat> channel;
    cv::split(output_image,channel);

    if (papszMetadata) {
        pOldDS->SetMetadata( papszMetadata );
        sprintf(StrValue,"%d",nCols);
        pOldDS->SetMetadataItem("EXIF_PixelXDimension",StrValue);
        sprintf(StrValue,"%d",nRows);
        pOldDS->SetMetadataItem("EXIF_PixelYDimension",StrValue);

        sprintf(StrValue,"%f",Geometria_ImgProc.Gimbal_Angle);
        pOldDS->SetMetadataItem(HAM_YAWDEGREE,StrValue);
    }

    pOldDS->SetMetadataItem("EXIF_Software",HAM_VER);
    sprintf(intStr,"%s",ui->txt_Alt_Voo->text().toStdString().c_str());
    pOldDS->SetMetadataItem(HAM_ALTITUDE,intStr);

    if (Imagem != NULL) {
        QString Sentido = "";
        sprintf(intStr,"%d",Imagem->Num);
        pOldDS->SetMetadataItem(HAM_NUM_IMAGES,intStr);
        pOldDS->SetMetadataItem(HAM_ANGULO_IDA,(QString::number(Imagem->Media_Ida)).toStdString().c_str());
        pOldDS->SetMetadataItem(HAM_ANGULO_VOLTA,(QString::number(Imagem->Media_Volta)).toStdString().c_str());
        pOldDS->SetMetadataItem(HAM_EXIF_MAKE,Imagem->Make.toStdString().c_str());
        pOldDS->SetMetadataItem(HAM_EXIF_MODEL,Imagem->Model.toStdString().c_str());


        if ((Imagem->Sentido & VAI) && (Imagem->Sentido & VOLTA))  {
            Sentido = Sentido + "BIDIRECIONAL";
        } else if (Imagem->Sentido & VAI) {
            Sentido = Sentido + "IDA";
        } else if (Imagem->Sentido & VOLTA) {
            Sentido = Sentido + "VOLTA";
        }

        if (Imagem_Composta.Primeira_Ultima_Linha && (Sentido != "BIDIRECIONAL")) {
            Sentido = Sentido + "_PUL";
        }

        if (Imagem->Sentido & CURVA) {
            Sentido = Sentido + "_CURVA";
        }

        if (Imagem->Sentido == ESCOLHIDOS) {
            Sentido = "ESCOLHIDOS";
        }
//        sprintf(intStr,"%d",Imagem->Sentido);
        pOldDS->SetMetadataItem(HAM_SENTIDO,Sentido.toStdString().c_str());

//        sprintf(intStr,"%d",Imagem->Primeira_Ultima_Linha);
//        pOldDS->SetMetadataItem(HAM_PUL,intStr);
    }


    transform[0] = Geometria_ImgProc.AUTM.Longitude();
    transform[1] = fabs(Geometria_ImgProc.AUTM.Longitude() - Geometria_ImgProc.DUTM.Longitude()) / (nCols);
    transform[2] = 0;
    transform[3] = Geometria_ImgProc.AUTM.Latitude();
    transform[4] = 0;
    transform[5] = - fabs(Geometria_ImgProc.AUTM.Latitude() - Geometria_ImgProc.DUTM.Latitude()) / (nRows);
    pOldDS->SetGeoTransform(transform);


    // Iterate over each channel

    for (int i = 1 ; i <= channels; i++)
    {
        // Fetch the band
        GDALRasterBand* band = pOldDS->GetRasterBand(i);

        //////////////////////////////////////////////////////////
        // Por algum motivo retirei a alteração de Cor_Nodata
        // Retirando dá pau quando vai escrever as imagens no processamento das imagens
/*
        int Cor_Nodata;
        if (ui->actionBranco->isChecked()) {
            Cor_Nodata = 255;
        } else {
            Cor_Nodata = 0;
        }
*/
        band->SetNoDataValue(Geometria_ImgProc.Cor_Nodata);  // Aqui estava a variável local Cor_Nodata

        band->RasterIO(GF_Write,0,0,nCols,nRows,channel[(channels - i)].data,nCols,nRows,GDT_Byte,0,0);
    }

    try {
        GDALClose(pOldDS);
    } catch (...) {
        ui->txt_Output->append(" \n Erro ! ");
    }

#else
    transform[0] = Geometria_ImgProc.AUTM.Longitude();
    transform[1] = abs(Geometria_ImgProc.AUTM.Longitude() - Geometria_ImgProc.DUTM.Longitude()) / (nCols - 1);
    transform[2] = 0;
    transform[3] = Geometria_ImgProc.AUTM.Latitude();
    transform[4] = 0;
    transform[5] = - abs(Geometria_ImgProc.AUTM.Latitude() - Geometria_ImgProc.DUTM.Latitude()) / (nRows - 1);

    writeGeoInfo(output,output_image,NULL,transform);
#endif
}


void MainWindow::writeGeoInfo(QString output,cv::Mat output_image,char **papszMetadata,double *Transform) {
    int channels;
    int nCols = output_image.cols;
    int nRows = output_image.rows;

    char StrValue[20];

    GDALDataset *pOldDS;

    pOldDS = (GDALDataset *) GDALOpen(output.toStdString().c_str(),GA_Update);

    int Cor_Nodata;
    if (ui->actionBranco->isChecked()) {
        Cor_Nodata = 255;
    } else {
        Cor_Nodata = 0;
    }

    channels = pOldDS->GetRasterCount();
    for (int i = 1 ; i <= channels ; i++) {
        pOldDS->GetRasterBand(i)->SetNoDataValue(Cor_Nodata);
    }

    if (papszMetadata) {
        pOldDS->SetMetadata( papszMetadata );
        pOldDS->SetMetadataItem("EXIF_Software","HAM_Geo_0.658");
        sprintf(StrValue,"%d",nCols);
        pOldDS->SetMetadataItem("EXIF_PixelXDimension",StrValue);
        sprintf(StrValue,"%d",nRows);
        pOldDS->SetMetadataItem("EXIF_PixelYDimension",StrValue);
    }

    pOldDS->SetProjection("WGS84");
    pOldDS->SetGeoTransform(Transform);

    GDALClose(pOldDS);

}


void MainWindow::writeGeoInfo(QString output,cv::Mat output_image,char **papszMetadata,Geometria_Imagem &Geometria_ImgProc) {
    int channels; // = output_image.channels();
    int nCols = output_image.cols;
    int nRows = output_image.rows;
    int noData = 0;
    char StrValue[20];

    GDALDataset *pOldDS;
    GDALDriver *pDriverTiff;
    double transform[6] = {0,1,0,0,0,1};


    pOldDS = (GDALDataset *) GDALOpen(output.toStdString().c_str(),GA_Update);

    int Cor_Nodata;
    if (ui->actionBranco->isChecked()) {
        Cor_Nodata = 255;
    } else {
        Cor_Nodata = 0;
    }


    channels = pOldDS->GetRasterCount();

    for (int i = 1 ; i <= channels ; i++) {
        pOldDS->GetRasterBand(i)->SetNoDataValue(Cor_Nodata);
    }

    if (papszMetadata) {
        pOldDS->SetMetadata( papszMetadata );
        pOldDS->SetMetadataItem("EXIF_Software","HAM_Geo_0.658");
        sprintf(StrValue,"%d",nCols);
        pOldDS->SetMetadataItem("EXIF_PixelXDimension",StrValue);
        sprintf(StrValue,"%d",nRows);
        pOldDS->SetMetadataItem("EXIF_PixelYDimension",StrValue);
    }

    pOldDS->SetProjection("WGS84");
    transform[0] = Geometria_ImgProc.AUTM.Longitude();
    transform[1] = abs(Geometria_ImgProc.AUTM.Longitude() - Geometria_ImgProc.DUTM.Longitude()) / nCols;
//    transform[1] = Geometria_ImgProc.mppx;
    transform[2] = 0;
    transform[3] = Geometria_ImgProc.AUTM.Latitude();
    transform[4] = 0;
    transform[5] = - abs(Geometria_ImgProc.AUTM.Latitude() - Geometria_ImgProc.DUTM.Latitude()) / nRows;
//    transform[5] = Geometria_ImgProc.mppy;
    pOldDS->SetGeoTransform(transform);

    GDALClose(pOldDS);
}

void MainWindow::writeGeoInfo(QString output,cv::Mat output_image,char **papszMetadata,char **papszMetadatadomain,Geometria_Imagem &Geometria_ImgProc) {
    int channels; // = output_image.channels();
    int nCols = output_image.cols;
    int nRows = output_image.rows;
    int noData = 0;
    char StrValue[20];

    GDALDataset *pOldDS;
    GDALDriver *pDriverTiff;
    double transform[6] = {0,1,0,0,0,1};


    pOldDS = (GDALDataset *) GDALOpen(output.toStdString().c_str(),GA_Update);

    int Cor_Nodata;
    if (ui->actionBranco->isChecked()) {
        Cor_Nodata = 255;
    } else {
        Cor_Nodata = 0;
    }


    channels = pOldDS->GetRasterCount();

    for (int i = 1 ; i <= channels ; i++) {
        pOldDS->GetRasterBand(i)->SetNoDataValue(Cor_Nodata);
    }

    if (papszMetadata) {
        if (papszMetadatadomain) {
            pOldDS->SetMetadata( papszMetadata , papszMetadatadomain[0]);
        } else {
            pOldDS->SetMetadata( papszMetadata );
        }
        pOldDS->SetMetadataItem("EXIF_Software","HAM_Geo_0.658");
        sprintf(StrValue,"%d",nCols);
        pOldDS->SetMetadataItem("EXIF_PixelXDimension",StrValue);
        sprintf(StrValue,"%d",nRows);
        pOldDS->SetMetadataItem("EXIF_PixelYDimension",StrValue);
    }

/*
    if (papszMetadatadomain) {
        pOldDS->SetMetadata(papszMetadata,papszMetadatadomain[0]);
    }
*/
    pOldDS->SetProjection("WGS84");
    transform[0] = Geometria_ImgProc.AUTM.Longitude();
    transform[1] = abs(Geometria_ImgProc.AUTM.Longitude() - Geometria_ImgProc.DUTM.Longitude()) / nCols;
//    transform[1] = Geometria_ImgProc.mppx;
    transform[2] = 0;
    transform[3] = Geometria_ImgProc.AUTM.Latitude();
    transform[4] = 0;
    transform[5] = - abs(Geometria_ImgProc.AUTM.Latitude() - Geometria_ImgProc.DUTM.Latitude()) / nRows;
//    transform[5] = Geometria_ImgProc.mppy;
    pOldDS->SetGeoTransform(transform);

    GDALClose(pOldDS);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MainWindow::Eh_GeoTiff(QString InputFile, int &No_Data) {
    GDALAllRegister();

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(InputFile.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);

    double      Transform[6];
    pOldDS->GetGeoTransform(Transform);
    No_Data = pOldDS->GetRasterBand(1)->GetNoDataValue();

    GDALClose(pOldDS);

    // Verifica se possui informação geográfica
    if ((Transform[0] == 0) && (Transform[1] == 1) &&
        (Transform[2] == 0) && (Transform[3] == 0) &&
        (Transform[4] == 0) && (Transform[5] == 1) ) {
        return (false);
    }

    return(true);
}


void MainWindow::Size_ofJPG(QString InputFile, bool Print, QString &Output) {
    char            intStr[200];
    int             X,Y,i;
    double          cx,cy,Alt,Angulo;
    char            Camera[50];
    TiposCameras    Tipos_C;
    TiposDrones     Tipos_D;
    bool            Rec = false;
    QString         Drone;

    GDALAllRegister();

/*
    QString     Out;
    info_H(InputFile,Out);

    if (Print) ui->txt_Output->append(Out);
*/

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(InputFile.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);

    X = poSrcDS->GetRasterXSize();
    Y = poSrcDS->GetRasterYSize();

    cy = getLATfromEXIF(poSrcDS);
    cx = getLONfromEXIF(poSrcDS);

    QStringList stringList;
    DecodeXMP(stringList,poSrcDS);

    Alt = GetXMPItem(stringList,"RelativeAltitude");
    Angulo = GetXMPItem(stringList,"GimbalPitchDegree");

    i = 0;
    strcpy(Camera,poSrcDS->GetMetadataItem("EXIF_Model"));
    for ( ; i < Tipos_C.myOptions.length() ; i++) {
        if (strcmp(Tipos_C.myOptions.at(i).toStdString().c_str(),Camera) == 0) {

            break;
        }
    }

    if (i < Tipos_C.myOptions.length()) {
        Rec = true;
        Drone = Tipos_D.myOptions.at(i);
     }


    GDALClose(pOldDS);

    if (Print) {
        QLocale Br;
        QString S1,S2;

        ui->txt_Output->append("-------------------\n Informações sobre: " + InputFile);


        QFileInfo Info(InputFile);

        char cbuf[32];
        S1 = Br.toString(static_cast<float> (Info.size()),'f',0);
        sprintf(intStr,"     Tamanho: %s bytes",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);

        sprintf(intStr,"     Dimensão da imagem: %d x %d",X,Y);
        ui->txt_Output->append(intStr);

        S1 = Br.toString(Alt,'f',2);
        sprintf(intStr,"     Altitude: %s metros",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);

        S1 = Br.toString(Angulo,'f',2);
        sprintf(intStr,"     Ângulo da câmera: %s graus",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);

        if (Rec) {
            sprintf(intStr,"     Camera: %s, Drone: %s",Camera,Drone.toStdString().c_str());
        } else {
            sprintf(intStr,"     Camera: %s",Camera);
        }
        ui->txt_Output->append(intStr);


        S1 = Br.toString(cy,'f',6);
        S2 = Br.toString(cx,'f',6);
        sprintf(intStr,"     Centro: (%s ; %s).", S1.toStdString().c_str(),
                                                  S2.toStdString().c_str());

        ui->txt_Output->append(intStr);
    }
}


void MainWindow::Size_GeoTiff(QString InputFile, bool Print, QString &Output) {
    GDALAllRegister();

    QString     Out;
    info_H(InputFile,Out);

    int X,Y;

    char intStr[150];

    GDALDataset *pOldDS;
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
    int     Zona = pOldDS->GetSpatialRef()->GetUTMZone();
    const int A = pOldDS->GetSpatialRef()->IsGeographic();
    if (A) E_UTM = false; else E_UTM = true;
    int     Hemisferio = 0;

    if (!E_UTM) {  // LAT LON
        width = X;
        height = Y;
        minx = Transform[0];
        miny = Transform[3] + (width - 1) * Transform[4] + (height - 1) * Transform[5];
        maxx = Transform[0] + (width - 1) * Transform[1] + (height - 1) * Transform[2];
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
        Geo_Img.CcUTM.Reset(Geo_Img.AUTM.Zone(),Geo_Img.AUTM.Northp(),Geo_Img.AUTM.Easting() + Geo_Img.mppx * (Geo_Img.Iw /2),Geo_Img.AUTM.Northing() + Geo_Img.mppy * (Geo_Img.Ih /2));
        Geo_Img.A.real(Geo_Img.AUTM.Latitude());
        Geo_Img.A.imag(Geo_Img.AUTM.Longitude());
        Geo_Img.D.real(Geo_Img.DUTM.Latitude());
        Geo_Img.D.imag(Geo_Img.DUTM.Longitude());
    } else { // UTM
        double  Lat,Lon;
        bool Ok;
        Ok = GetLatLonfromINFO(Out,Lat,Lon);

        Geo_Img.AUTM.Reset(Lat,Lon);
        Geo_Img.Iw = pOldDS->GetRasterXSize();
        Geo_Img.Ih = pOldDS->GetRasterYSize();
        Geo_Img.mppx = Transform[1]; // LON
        Geo_Img.mppy = Transform[5]; // LAT
        Geo_Img.DUTM.Reset(Geo_Img.AUTM.Zone(),Geo_Img.AUTM.Northp(),Geo_Img.AUTM.Easting() + Geo_Img.mppx * Geo_Img.Iw,Geo_Img.AUTM.Northing() + Geo_Img.mppy * Geo_Img.Ih);
        Geo_Img.CcUTM.Reset(Geo_Img.AUTM.Zone(),Geo_Img.AUTM.Northp(),Geo_Img.AUTM.Easting() + Geo_Img.mppx * (Geo_Img.Iw /2),Geo_Img.AUTM.Northing() + Geo_Img.mppy * (Geo_Img.Ih /2));
        Geo_Img.Dw = abs(Geo_Img.DUTM.Easting() - Geo_Img.AUTM.Easting());
        Geo_Img.Dh = abs(Geo_Img.DUTM.Northing() - Geo_Img.AUTM.Northing());
        Geo_Img.A.real(Geo_Img.AUTM.Latitude());
        Geo_Img.A.imag(Geo_Img.AUTM.Longitude());
        Geo_Img.D.real(Geo_Img.DUTM.Latitude());
        Geo_Img.D.imag(Geo_Img.DUTM.Longitude());
        TamX = (fabs(Geo_Img.AUTM.Easting() - Geo_Img.DUTM.Easting()) * 100.0) / X;
        TamY = (fabs(Geo_Img.AUTM.Northing() - Geo_Img.DUTM.Northing()) * 100.0) / Y;
        cy = Geo_Img.CcUTM.Latitude();
        cx = Geo_Img.CcUTM.Longitude();
        maxy = Geo_Img.AUTM.Latitude();
        minx = Geo_Img.AUTM.Longitude();
        miny = Geo_Img.DUTM.Latitude();
        maxx = Geo_Img.DUTM.Longitude();
    }


    Geo_Img.Dw = fabs(Geo_Img.AUTM.Easting() - Geo_Img.DUTM.Easting());                  // Tamanho horizontal no terreno em metros
    Geo_Img.Dh = fabs(Geo_Img.AUTM.Northing() - Geo_Img.DUTM.Northing());                // Tamanho vertical no terreno em metros
    Area = Geo_Img.Dw * Geo_Img.Dh;

//    QString Output;
    if (true) {
        QLocale Br;
        QString S1,S2;

        Output = Output + "-------------------\n Informações sobre: " + InputFile;

        QFileInfo Info(InputFile);
        char cbuf[32];
        S1 = Br.toString(static_cast<float> (Info.size()),'f',0);

        Output = Output + "\n   Tamanho: " + S1 + " bytes";
        Output = Output + "\n   Dimensão da imagem: " +  QString::number(X) + " x " + QString::number(Y);

        S1 = Br.toString(TamX,'f',2);
        S2 = Br.toString(TamY,'f',2);
        Output = Output + "\n   Resolução espacial: " + S1 + " cm/px";

        S1 = Br.toString(TamX,'f',2);
        S2 = Br.toString(TamY,'f',2);
        sprintf(intStr,"\n   Bordas: (%s ; %s) - (%s ; %s). Centro: (%s ; %s).",(Br.toString(maxy,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(minx,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(miny,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(maxx,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(cy,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(cx,'f',6)).toStdString().c_str());

        Output.append(intStr);

        S1 = Br.toString(static_cast<float> (Area),'f',0);

        Output = Output + " \n   Área: " + S1 + " metros quadrados";

     }


    if (Print) {
        QLocale Br;
        QString S1,S2;

        ui->txt_Output->append("-------------------\n Informações sobre: " + InputFile);

        QFileInfo Info(InputFile);
        char cbuf[32];
        S1 = Br.toString(static_cast<float> (Info.size()),'f',0);
        sprintf(intStr,"     Tamanho: %s bytes",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);

        sprintf(intStr,"     Dimensão da imagem: %d x %d",X,Y);
        ui->txt_Output->append(intStr);

        S1 = Br.toString(TamX,'f',2);
        S2 = Br.toString(TamY,'f',2);
        sprintf(intStr,"     Resolução espacial: %s cm/px",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);

        S1 = Br.toString(TamX,'f',2);
        S2 = Br.toString(TamY,'f',2);
        sprintf(intStr,"     Bordas: (%s ; %s) - (%s ; %s). Centro: (%s ; %s).",(Br.toString(maxy,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(minx,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(miny,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(maxx,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(cy,'f',6)).toStdString().c_str(),
                                                                          (Br.toString(cx,'f',6)).toStdString().c_str());

        ui->txt_Output->append(intStr);

        S1 = Br.toString(static_cast<float> (Area),'f',0);
        sprintf(intStr,"     Área: %s metros quadrados\n",S1.toStdString().c_str());
        ui->txt_Output->append(intStr);


        // Informações da mesclagem

        QString Informacao;

        Informacao = pOldDS->GetMetadataItem("EXIF_Software");
        if (Informacao != "") {
            ui->txt_Output->append("    Processado por: " + Informacao);
        }

        Informacao = pOldDS->GetMetadataItem("HAM_Altitude");
        if (Informacao != "") {
            ui->txt_Output->append("    Altitude de voo: " + Informacao + " metros");
        }
        Informacao = pOldDS->GetMetadataItem("HAM_Angulo_IDA");
        if (Informacao != "") {
            ui->txt_Output->append("    Ângulo IDA: " + Informacao);
        }
        Informacao = pOldDS->GetMetadataItem("HAM_Angulo_VOLTA");
        if (Informacao != "") {
            ui->txt_Output->append("    Ângulo VOLTA: " + Informacao);
        }
        Informacao = pOldDS->GetMetadataItem("HAM_Num_Images");
        if (Informacao != "") {
            ui->txt_Output->append("    Número de imagens: " + Informacao);
        }
        Informacao = pOldDS->GetMetadataItem("HAM_Sentido");
        if (Informacao != "") {
            ui->txt_Output->append("    Sentido: " + Informacao + "\n");
        }


    }

    Geo_Img.Name = InputFile;                   // Nome do Arquivo
    Geo_Img.Area = Area;                        // Área em metros quadrados
    Geo_Img.cmppx = Geo_Img.mppx * 100;                       // em cm/px
    Geo_Img.cmppy = Geo_Img.mppy * 100;

    Geo_Img.Cc.real(cy);                        // Coordenada LAT do centro da imagem
    Geo_Img.Cc.imag(cx);                        // Coordenada LON do centro da imagem
    Geo_Img.A.real(maxy);                       // Coordenada LAT do canto superior esquerdo
    Geo_Img.A.imag(minx);
    Geo_Img.D.real(miny);                       // Coordenada LAT do canto inferior direito
    Geo_Img.D.imag(maxx);

    GDALClose(poSrcDS);
//    GDALClose(pOldDS);
}

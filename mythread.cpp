#include "mythread.h"
#include "GDAL_HAM.h"
#include "translate.h"
#include "warp.h"

#include <QtCore>
#include <QDebug>
#include <QTimer>

MyThread_Ajuda::MyThread_Ajuda()
{
    toLoad = "";
    width = 0;
    height = 0;
    Arquivo = "";
//    Url = QStringLiteral("qrc:///pdfviewer/viewer.qml");
    Url = "qrc:///pdfviewer/viewer.qml";
    P_engine = NULL;
}


MyThread::MyThread()
{
    name = "";
    NameKMZ = "";
    NameTIF = "";
}

MyThread_Merge::MyThread_Merge()
{
    name = "";
    NameTIF = "";
    NameKMZ = "";
    Cor_Nodata = 255;
    Mesmo_Dir = false;
    Contador_Escala = 0;
}

MyThread_ICE_TIF::MyThread_ICE_TIF()
{
    NameTIF = "";
    NameICE = "";
    Output_VRT = "";
}


MyThread_RES::MyThread_RES()
{
    NameInput = "";
    NameOutput = "";
    XSize = 0;
}

void MyThread::run()
{
    int Num_Elementos = 9;
    int Tamanho = Num_Elementos + 1;
    char intStr[50];

    char **argv3;
    argv3 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

    int i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv3[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
    }
    argv3[Num_Elementos] = nullptr;

    sprintf(intStr,"%d",this->Cor_Nodata);

    strcpy(argv3[0],"gdal_translate.exe");
    strcpy(argv3[1],"-of");
    strcpy(argv3[2],"KMLSUPEROVERLAY");
    strcpy(argv3[3],"-a_nodata");
    strcpy(argv3[4],intStr);
    strcpy(argv3[5],"-co");
    strcpy(argv3[6],"format=png");
    strcpy(argv3[7],this->NameTIF.toStdString().c_str());
    strcpy(argv3[8],this->NameKMZ.toStdString().c_str());


    /////// Inserir timer de 5 segundos

    translate_HAM(Num_Elementos,static_cast<char **>(argv3));


    for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
        free(argv3[i]);
    }
}

void MyThread_Merge::run()
{
//    char                StrValue[100];
//    int                 nCols, nRows, channels;
    cv::Mat             imageRead;

    GDALAllRegister();

    //Le imagem KMZ
    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(NameKMZ.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

    double Transform[6];
    pOldDS->GetGeoTransform(Transform);

    Contador_Escala = 0;
    GDAL2MAT(poSrcDS,imageRead);

    Contador_Escala = 50;
//    emit AtualizaProgressBarMesclar();

    MAT2GDAL(NameTIF,imageRead,papszMetadata,Transform);

// ==>>    if (!Mesmo_Dir) Grava_Thumb(NameTIF);
}

void MyThread_ICE_TIF::run()
{
    char    intStr[100];

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
         strcpy(argv3[4],QString::number(Cor_Nodata).toStdString().c_str());
         strcpy(argv3[5],"-a_srs");
         strcpy(argv3[6],"EPSG:4326");

         for (int i = 0 ; i < Geo_Tiff.Pontos.length() ; i++) {
//            fprintf(fp,"%d;%d;%f;%f\n",Geo_ICE.PImg.at(i).real(),Geo_ICE.PImg.at(i).imag(),Geo_Tiff.Pontos.at(i).imag(),Geo_Tiff.Pontos.at(i).real());

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


         ////////////////////////////////// WARP

         Num_Elementos = 14;
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
         strcpy(argv4[8],"-srcnodata");
         strcpy(argv4[9],"0");
         strcpy(argv4[10],"-dstnodata");
         strcpy(argv4[11],QString::number(Cor_Nodata).toStdString().c_str());
         strcpy(argv4[12],Output_VRT.toStdString().c_str());
         strcpy(argv4[13],NameTIF.toStdString().c_str());

         warp_H(Num_Elementos,static_cast<char **>(argv4));

         for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
             free(argv4[i]);
         }
}


void MyThread_RES::run()
{  
        char    XSize_char[50];

        sprintf(XSize_char,"%d",XSize);

        int Num_Elementos = 7;
        int Tamanho = Num_Elementos + 1;

        char **argv3;
        argv3 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

        int i = 0;
        for (i = 0 ; i < Num_Elementos ; i ++) {
            argv3[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
        }
        argv3[Num_Elementos] = nullptr;

        strcpy(argv3[0],"gdalwarp.exe");
        strcpy(argv3[1],"-ts");
        strcpy(argv3[2],XSize_char);
        strcpy(argv3[3],"0");
        strcpy(argv3[4],"-overwrite");
        strcpy(argv3[5],NameInput.toStdString().c_str());
        strcpy(argv3[6],NameOutput.toStdString().c_str());

        warp_H(Num_Elementos,static_cast<char **>(argv3));

        for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
            free(argv3[i]);
        }
}


void MyThread_Ajuda::run()
{
    Engine.load(Url);
    Engine.rootObjects().first()->setProperty("width", width);
    Engine.rootObjects().first()->setProperty("height", height);

    Engine.rootObjects().first()->setProperty("title",Arquivo);

    Engine.rootObjects().first()->setProperty("source", toLoad);
}


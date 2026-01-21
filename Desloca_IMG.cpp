#include <QString>
#include <QFileDialog>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>



#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "math.h"
#include <cmath>


#include "dialog_desloca_img.h"

using namespace cv;

extern Dialog_Desloca_Img       *dialogDeslocaImg;

extern  Geometria_Imagem        GeoProc,Geo_Img,Geo_Tiff,Geo_Referencia;
extern  cv::Mat                 ImgWindowDesloca;
extern  std::string             Name_Imagem1;
extern  Plot_Coords             Coords;

void MainWindow::Desloca_Img() {

    Name_Imagem1 = "";

    dialogDeslocaImg  = new Dialog_Desloca_Img(this);

    connect(dialogDeslocaImg, SIGNAL(Cancel()), this, SLOT(Cancel_Desloca()));
    connect(dialogDeslocaImg, SIGNAL(Ok()), this, SLOT(Ok_Desloca()));
    connect(dialogDeslocaImg, SIGNAL(Bt_Digita_Signal()), this,SLOT(Desloca_Digita()));
    connect(dialogDeslocaImg, SIGNAL(Bt_Img_Signal()), this,SLOT(Desloca_Img_Ref()));
    connect(dialogDeslocaImg, SIGNAL(Bt_2Pontos_Signal()), this, SLOT(Desloca_2Pontos()));

    ui->txt_Output->setText("---------------------------------------------------");
    ui->txt_Output->append("\n Deslocamento de imagem\n");

    dialogDeslocaImg->setWindowTitle("Desloca Imagem");

    dialogDeslocaImg->show();
}

void MainWindow::Cancel_Desloca(bool Cancelar) {
    ui->txt_Nome_Imagem->setText("");
    ui->bt_Mostar_Imagem->setEnabled(false);
    on_actionLimpar_triggered(false);
    GeoProc.DeslocaImagem = false;
    Geo_Tiff.DeslocaImagem = false;

    Geo_Img.image.release();
    Geo_Tiff.image.release();
    GeoProc.image.release();
    ImgWindowDesloca.release();

    if (!Cancelar) return;

    QColor tc = ui->txt_Output->textColor();
    ui->txt_Output->setTextColor(QColor("red"));
    ui->txt_Output->append("\n Operação concelada!");
    ui->txt_Output->setTextColor(tc);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::Ok_Desloca() {
    GeographicLib::GeoCoords    POld,PNew;
    QString                     New_File;
    double  OldLat,OldLon,NewLat,NewLon;
    double  MinX,MinY,MaxX,MaxY;
    double  Delta_Lat,Delta_Lon;
    qint64  Elapsed;

    dialogDeslocaImg->getLatLon(NewLat,NewLon);

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

    if ((NewLat == 0.0) || (NewLon == 0.0)) {
        // Erro
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("\n ERRO! Nova coordenada não definida!");
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//        dialogDeslocaImg->close();
//        Cancel_Desloca();
        return;
    }

    dialogDeslocaImg->close();

    Geo_Img = Geo_Referencia;

//    POld.Reset(OldLat,OldLon);
//    PNew.Reset(NewLat,NewLon);
//    Delta_Lat = PNew.Latitude() - POld.Latitude();
//    Delta_Lon = PNew.Longitude() - POld.Longitude();

    Delta_Lat = Geo_Referencia.Pontos.at(0).real() - Geo_Tiff.Pontos.at(0).real();
    Delta_Lon = Geo_Referencia.Pontos.at(0).imag() - Geo_Tiff.Pontos.at(0).imag();

//    MinX = Geo_Referencia.AUTM.Longitude() + Delta_Lon;
//    MaxX = Geo_Referencia.DUTM.Longitude() + Delta_Lon;
//    MinY = Geo_Referencia.DUTM.Latitude() + Delta_Lat;
//    MaxY = Geo_Referencia.AUTM.Latitude() + Delta_Lat;


//    Geo_Img.AUTM.Reset(MaxY,MinX);
//    Geo_Img.DUTM.Reset(MinY,MaxX);
//    Geo_Img.Dw = fabs(Geo_Img.AUTM.Easting() - Geo_Img.DUTM.Easting());                  // Tamanho horizontal no terreno em metros
//    Geo_Img.Dh = fabs(Geo_Img.AUTM.Northing() - Geo_Img.DUTM.Northing());                // Tamanho vertical no terreno em metros

    QFileInfo   Old_File(Geo_Tiff.Name);

    New_File = Old_File.absolutePath() + "/" +  Old_File.completeBaseName() + "_deslocado.tif";

    QFile New(New_File);
    if (New.exists()) {
        New.remove(New_File);
    }

    timer2.start();

//   MAT2GDAL(New_File,Destino,NULL,Geo_Img);
    QFile::copy(Geo_Tiff.Name,New_File);
    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(New_File.toStdString().c_str(),GA_Update);
    double transform[6] = {0,1,0,0,0,1};
    int nCols = pOldDS->GetRasterXSize();
    int nRows = pOldDS->GetRasterYSize();

/*
    transform[0] = Geo_Img.AUTM.Longitude();
    transform[1] = abs(Geo_Img.AUTM.Longitude() - Geo_Img.DUTM.Longitude()) / (nCols);
    transform[2] = 0;
    transform[3] = Geo_Img.AUTM.Latitude();
    transform[4] = 0;
    transform[5] = - abs(Geo_Img.AUTM.Latitude() - Geo_Img.DUTM.Latitude()) / (nRows);
*/
    pOldDS->GetGeoTransform(transform);

//    double A1,A2;
//    A1 = (Geo_Referencia.PImg.at(0).imag() * Geo_Referencia.T1);
//    A2 = (Geo_Tiff.PImg.at(0).imag() * Geo_Tiff.T1);

    transform[0] = Geo_Tiff.A.imag() + Delta_Lon;
//    transform[1] = abs(Geo_Img.AUTM.Longitude() - Geo_Img.DUTM.Longitude()) / (nCols);
    transform[2] = 0;
    transform[3] = Geo_Tiff.A.real() + Delta_Lat;
    transform[4] = 0;
//    transform[5] = - abs(Geo_Img.AUTM.Latitude() - Geo_Img.DUTM.Latitude()) / (nRows);


    pOldDS->SetGeoTransform(transform);
    GDALClose(pOldDS);

    QColor tc = ui->txt_Output->textColor();
    ui->txt_Output->setTextColor(QColor("blue"));

    char    intStr[100];
    Elapsed = timer2.elapsed();
    sprintf(intStr," \n Imagem [%s] gravada em %3.3f segundos.\n",New_File.toStdString().c_str(),(Elapsed / 1000.0));
    ui->txt_Output->append(intStr);
    ui->txt_Output->setTextColor(tc);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // No fim retira flag
    destroyAllWindows();
    Cancel_Desloca(false);
}

void MainWindow::Desloca_Digita() {
    QString         Tif_Original;

    dialogDeslocaImg->Posiciona_LATLON(1);

    dialogDeslocaImg->setBoxWrite();

    dialogDeslocaImg->setDigita();

    Tif_Original = QFileDialog::getOpenFileName(this, tr("TIFF Image"),Informacao.Diretorio,tr("Image (*.tif)"));

    if (Tif_Original == "") {
        return;
     }

    dialogDeslocaImg->printFile(Tif_Original);

    dialogDeslocaImg->printOriginalCoords(GeoProc);

    AbreArquivo(QFileInfo(Tif_Original),false,false);

    Informacao.Imagem_Foco = Tif_Original;

    GeoProc.DeslocaImagem = true;
    Geo_Tiff.DeslocaImagem = true;

}

void MainWindow::Desloca_Img_Ref() {

    qint64      Elapsed;

    dialogDeslocaImg->Posiciona_LATLON(1);

    dialogDeslocaImg->setBoxReadOnly();

    dialogDeslocaImg->setImagem();

    QString InputTif,OutputTif,Name,Dir;
    int     No_Data;
    char    intStr[100];


    Informacao.Imagem_Foco = QFileDialog::getOpenFileName(this, tr("TIF a ser deslocada"),Informacao.DiretorioResult,tr("Image (*.tif)"));
    QFileInfo CompleteName(Informacao.Imagem_Foco);

    if (Informacao.Imagem_Foco == "") {
        return;
     }

    dialogDeslocaImg->printFile(Informacao.Imagem_Foco);

    sprintf(intStr,"Arquivo a ser deslocado: [%s]\n",Informacao.Imagem_Foco.toStdString().c_str());
    ui->txt_Output->append(intStr);

    Geo_Tiff.Name = Informacao.Imagem_Foco;

    Dir = CompleteName.path();

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

    dialogDeslocaImg->printFile_Ref(InputTif);

    Geo_Referencia.Name = InputTif;

    sprintf(intStr,"Arquivo TIF de referência: [%s]\n",InputTif.toStdString().c_str());
    ui->txt_Output->append(intStr);

    cv::destroyAllWindows();

//    AbreArquivo(Informacao.Imagem_Foco,false,false);
    Le_Imagem_TIF(Geo_Referencia.Name,Geo_Referencia);

    Geo_Referencia.DeslocaImagem = true;
    Geo_Tiff.DeslocaImagem = true;

    namedWindow(Geo_Referencia.Name.toStdString(), WINDOW_GUI_EXPANDED);


    frameW = Geo_Referencia.Iw;
    frameH = Geo_Referencia.Ih;

    // 600 pontos em Y
    double Rel1;
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    }

    resizeWindow(Geo_Referencia.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));


    imshow(Geo_Referencia.Name.toStdString(),Geo_Referencia.image);
//    ImgWindowDesloca.copyTo(Geo_Tiff.image);
    setMouseCallback(Geo_Referencia.Name.toStdString(),on_mouse_Desloca_TIF_1,NULL);


// Imagem TIF georeferenciada

    timer2.start();

    Le_Imagem_TIF(Geo_Tiff.Name,Geo_Tiff);
    frameW = Geo_Tiff.image.cols;
    frameH = Geo_Tiff.image.rows;

    namedWindow(Geo_Tiff.Name.toStdString(), WINDOW_GUI_EXPANDED);
    // 600 pontos em Y
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 20,700);
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 700,20);
    }

    Elapsed = timer2.elapsed();
    sprintf(intStr," Imagem de referênia lida em %3.3f segundos.\n",(Elapsed / 1000.0));
    ui->txt_Output->append(intStr);


    resizeWindow(Geo_Tiff.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow(Geo_Tiff.Name.toStdString(),Geo_Tiff.image);
    setMouseCallback(Geo_Tiff.Name.toStdString(),on_mouse_Desloca_TIF_2,NULL);
}

void MainWindow::Desloca_2Pontos() {

    dialogDeslocaImg->Posiciona_LATLON(2);

    Desloca_Img_Ref();


}


void MainWindow::Le_Imagem_TIF(QString InputTif,Geometria_Imagem &Geo_Tiff) {
    GDALAllRegister();

    QString Out;

    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(InputTif.toStdString().c_str(),GA_ReadOnly);
    GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
    char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

    MainWindow::GDAL2MAT(poSrcDS,Geo_Tiff.image_Lida);
    Geo_Tiff.image_Lida.copyTo(Geo_Tiff.image);

    double Transform[6];
    pOldDS->GetGeoTransform(Transform);
    Geo_Tiff.Name = InputTif;

    bool    E_UTM = false;
    int     Zona = 0;
    int     Hemisferio = 0;

    Zona = pOldDS->GetSpatialRef()->GetUTMZone();
    const int A = pOldDS->GetSpatialRef()->IsGeographic();
    if (A) E_UTM = false; else E_UTM = true;


    if (!E_UTM) {  // LAT LON
        Geo_Tiff.AUTM.Reset(Transform[3],Transform[0]); // LAT,LON
        Geo_Tiff.A.real(Transform[3]);
        Geo_Tiff.A.imag(Transform[0]);
        Geo_Tiff.Iw = pOldDS->GetRasterXSize();
        Geo_Tiff.Ih = pOldDS->GetRasterYSize();
        Geo_Tiff.DUTM.Reset( Transform[3] + Transform[5] * Geo_Tiff.Ih,Transform[0] + Transform[1] * Geo_Tiff.Iw );
        Geo_Tiff.mppx = abs(Geo_Tiff.DUTM.Easting() - Geo_Tiff.AUTM.Easting()) / Geo_Tiff.Iw; // LON
        Geo_Tiff.mppy = -abs(Geo_Tiff.DUTM.Northing() - Geo_Tiff.AUTM.Northing()) / Geo_Tiff.Ih; // LAT
        Geo_Tiff.CcUTM.Reset(Geo_Tiff.AUTM.Zone(),Geo_Tiff.AUTM.Northp(),Geo_Tiff.AUTM.Easting() + Geo_Tiff.mppx * (Geo_Tiff.Iw /2),Geo_Tiff.AUTM.Northing() + Geo_Tiff.mppy * (Geo_Tiff.Ih /2));
        Geo_Tiff.T1 = Transform[1];
        Geo_Tiff.T5 = Transform[5];
    } else { // UTM
        double      Lat,Lon;
        bool        Ok;

        Ok = GetLatLonfromINFO(Out,Lat,Lon);

        Geo_Tiff.AUTM.Reset(Lat,Lon);
        Geo_Tiff.Iw = pOldDS->GetRasterXSize();
        Geo_Tiff.Ih = pOldDS->GetRasterYSize();
        Geo_Tiff.mppx = Transform[1]; // LON
        Geo_Tiff.mppy = Transform[5]; // LAT
        Geo_Tiff.DUTM.Reset(Geo_Tiff.AUTM.Zone(),Geo_Tiff.AUTM.Northp(),Geo_Tiff.AUTM.Easting() + Geo_Tiff.mppx * Geo_Tiff.Iw,Geo_Tiff.AUTM.Northing() + Geo_Tiff.mppy * Geo_Tiff.Ih);
        Geo_Tiff.CcUTM.Reset(Geo_Tiff.AUTM.Zone(),Geo_Tiff.AUTM.Northp(),Geo_Tiff.AUTM.Easting() + Geo_Tiff.mppx * (Geo_Tiff.Iw /2),Geo_Tiff.AUTM.Northing() + Geo_Tiff.mppy * (Geo_Tiff.Ih /2));
        Geo_Tiff.Dw = abs(Geo_Tiff.DUTM.Easting() - Geo_Tiff.AUTM.Easting());
        Geo_Tiff.Dh = abs(Geo_Tiff.DUTM.Northing() - Geo_Tiff.AUTM.Northing());
        Geo_Tiff.A.real(Geo_Tiff.AUTM.Latitude());
        Geo_Tiff.A.imag(Geo_Tiff.AUTM.Longitude());
        Geo_Tiff.D.real(Geo_Tiff.DUTM.Latitude());
        Geo_Tiff.D.imag(Geo_Tiff.DUTM.Longitude());
        // Falta fazer a conversão de mppx/mppy para T1 e T5

    }
    GDALClose(pOldDS);

}


void MainWindow::on_bt_Teste_clicked()
{
    QString     File1,File2;
    QFileInfo   File;
    double      Angulo;


    if (Informacao.Diretorio == "") {
        ui->txt_Output->append("\n Erro. Diretório não definido.");
        return;
    }


    Angulo = dialogMesclaDesloca->get_Angulo();

    processarImgsProv(Angulo);

    Geo_Referencia.Name = Informacao.DiretorioResult + "/" + MERGE_TEMP1;

    Imagem_Composta.Geo_File_List.clear();

    Imagem_Composta.Geo_File_List.append(QFileInfo(Informacao.ListadeArquivos_MergeResult.at(0)));
    Imagem_Composta.Geo_File_List.append(QFileInfo(Informacao.ListadeArquivos_MergeResult.at(1)));

    executaMesclar(Geo_Referencia.Name);

    Le_Imagem_TIF(Geo_Referencia.Name,Geo_Referencia);

    frameW = Geo_Referencia.Iw;
    frameH = Geo_Referencia.Ih;

    Geo_Referencia.Name = "Define angulo";

//    namedWindow(Geo_Referencia.Name.toStdString(), WINDOW_GUI_EXPANDED);

    // 600 pontos em Y
    double Rel1;
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
//        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    } else {
        Rel1 = frameW / 600.0;
//        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    }

    if (Angulo == 0) resizeWindow("Define angulo",static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow("Define angulo",Geo_Referencia.image);
    setMouseCallback("Define angulo",on_mouse_Desloca_TIF_1,NULL);



/*
    // Lê imagem 1
    Geo_Referencia.Name = Coords.PROC.FileName.at(0);
    Le_Imagem_TIF(Geo_Referencia.Name,Geo_Referencia);

    frameW = Geo_Referencia.Iw;
    frameH = Geo_Referencia.Ih;

    namedWindow(Geo_Referencia.Name.toStdString(), WINDOW_GUI_EXPANDED);

    // 600 pontos em Y
    double Rel1;
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    }

    resizeWindow(Geo_Referencia.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow(Geo_Referencia.Name.toStdString(),Geo_Referencia.image);
    setMouseCallback(Geo_Referencia.Name.toStdString(),on_mouse_Desloca_TIF_1,NULL);


    // Lê imagem 2
    Geo_Tiff.Name = Coords.PROC.FileName.at(Img2+1);

    sprintf(intStr,"Arquivo 2: [%s]\n",Geo_Tiff.Name.toStdString().c_str());
    ui->txt_Output->append(intStr);

    timer2.start();

    Le_Imagem_TIF(Geo_Tiff.Name,Geo_Tiff);

    frameW = Geo_Tiff.image.cols;
    frameH = Geo_Tiff.image.rows;

    namedWindow(Geo_Tiff.Name.toStdString(), WINDOW_GUI_EXPANDED);
    // 600 pontos em Y
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 20,700);
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 700,20);
    }

    resizeWindow(Geo_Tiff.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow(Geo_Tiff.Name.toStdString(),Geo_Tiff.image);
    setMouseCallback(Geo_Tiff.Name.toStdString(),on_mouse_Desloca_TIF_2,NULL);

*/

}

/*
{
    char            intStr[100];
    qint64          Elapsed;
    int             Img1,Img2;

    QString         Temp = Informacao.DiretorioResult + "/" + MERGE_TEMP1;


    if (Informacao.Diretorio == "") {
        ui->txt_Output->append("\n Erro. Diretório não definido.");
        return;
    }

    Img2 = dialogMesclaDesloca->get_Posicao();

    if (Img2 == 20) {
        destroyAllWindows();
        dialogMesclaDesloca->close();
        QFile::remove(Informacao.DiretorioResult + "/" + MERGE_TEMP3);
        QFile::rename(Informacao.DiretorioResult + "/" + MERGE_TEMP1,Informacao.DiretorioResult + "/" + "zzz.tif");
        Lista_Files_Result();

        return;
    }

    if (Img2 == 0) {
        Geo_Referencia.Name = Coords.PROC.FileName.at(0);
        Le_Imagem_TIF(Geo_Referencia.Name,Geo_Referencia);
    } else {
        Geo_Referencia.Name = Temp;
        Le_Imagem_TIF(Temp,Geo_Referencia);
    }

    dialogMesclaDesloca->print_NumFile(Img2+1);

    Geo_Referencia.AjustaImagens = true;
    Geo_Tiff.AjustaImagens = true;

    frameW = Geo_Referencia.Iw;
    frameH = Geo_Referencia.Ih;

    namedWindow(Geo_Referencia.Name.toStdString(), WINDOW_GUI_EXPANDED);

    // 600 pontos em Y
    double Rel1;
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    }

    resizeWindow(Geo_Referencia.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow(Geo_Referencia.Name.toStdString(),Geo_Referencia.image);
    setMouseCallback(Geo_Referencia.Name.toStdString(),on_mouse_Desloca_TIF_1,NULL);



// Imagem TIF georeferenciada
    Geo_Tiff.Name = Coords.PROC.FileName.at(Img2+1);

    sprintf(intStr,"Arquivo 2: [%s]\n",Geo_Tiff.Name.toStdString().c_str());
    ui->txt_Output->append(intStr);

    timer2.start();

    Le_Imagem_TIF(Geo_Tiff.Name,Geo_Tiff);

    frameW = Geo_Tiff.image.cols;
    frameH = Geo_Tiff.image.rows;

    namedWindow(Geo_Tiff.Name.toStdString(), WINDOW_GUI_EXPANDED);
    // 600 pontos em Y
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 20,700);
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 700,20);
    }

    Elapsed = timer2.elapsed();
    sprintf(intStr," Imagem 2 lida em %3.3f segundos.\n",(Elapsed / 1000.0));
    ui->txt_Output->append(intStr);


    resizeWindow(Geo_Tiff.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow(Geo_Tiff.Name.toStdString(),Geo_Tiff.image);
    setMouseCallback(Geo_Tiff.Name.toStdString(),on_mouse_Desloca_TIF_2,NULL);

}
*/

void MainWindow::MergeSequencia() {
    on_bt_Teste_clicked();
}
/*
{
    GeographicLib::GeoCoords    POld,PNew;
    QString                     New_File;
    double  OldLat,OldLon,NewLat,NewLon;
    double  MinX,MinY,MaxX,MaxY;
    double  Delta_Lat,Delta_Lon;


    //////////////////////////////////////////////////////////////////
    /// Desloca a imagem

    if (Geo_Tiff.Pontos.length()) {
        OldLat = Geo_Tiff.Pontos.at(0).real();
        OldLon = Geo_Tiff.Pontos.at(0).imag();
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

    NewLat = Geo_Referencia.Pontos.at(0).real();
    NewLon = Geo_Referencia.Pontos.at(0).imag();


    if ((NewLat == 0.0) || (NewLon == 0.0)) {
        // Erro
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("\n ERRO! Nova coordenada não definida!");
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//        dialogDeslocaImg->close();
//        Cancel_Desloca();
        return;
    }

    Geo_Img = Geo_Tiff;

    POld.Reset(OldLat,OldLon);
    PNew.Reset(NewLat,NewLon);
    Delta_Lat = PNew.Latitude() - POld.Latitude();
    Delta_Lon = PNew.Longitude() - POld.Longitude();

    MinX = Geo_Tiff.AUTM.Longitude() + Delta_Lon;
    MaxX = Geo_Tiff.DUTM.Longitude() + Delta_Lon;
    MinY = Geo_Tiff.DUTM.Latitude() + Delta_Lat;
    MaxY = Geo_Tiff.AUTM.Latitude() + Delta_Lat;


    Geo_Img.AUTM.Reset(MaxY,MinX);
    Geo_Img.DUTM.Reset(MinY,MaxX);


    New_File = Informacao.DiretorioResult + "/" + MERGE_TEMP2;

    QFile New(New_File);
    if (New.exists()) {
        New.remove(New_File);
    }

//   MAT2GDAL(New_File,Destino,NULL,Geo_Img);
    QFile::copy(Geo_Tiff.Name,New_File);
    GDALDataset *pOldDS;
    pOldDS = (GDALDataset *) GDALOpen(New_File.toStdString().c_str(),GA_Update);
    double transform[6] = {0,1,0,0,0,1};
    int nCols = pOldDS->GetRasterXSize();
    int nRows = pOldDS->GetRasterYSize();
    transform[0] = Geo_Img.AUTM.Longitude();
    transform[1] = abs(Geo_Img.AUTM.Longitude() - Geo_Img.DUTM.Longitude()) / (nCols);
    transform[2] = 0;
    transform[3] = Geo_Img.AUTM.Latitude();
    transform[4] = 0;
    transform[5] = - abs(Geo_Img.AUTM.Latitude() - Geo_Img.DUTM.Latitude()) / (nRows);
    pOldDS->SetGeoTransform(transform);
    GDALClose(pOldDS);


    // No fim retira flag
    destroyAllWindows();
//    Cancel_Desloca(false);

    //////////////////////////////////////////////////////////////////
    /// Faz o merge com a anterior


    Imagem_Composta.Geo_File_List.clear();
    Imagem_Composta.Geo_File_List.append(Geo_Referencia.Name);
    Imagem_Composta.Geo_File_List.append(Informacao.DiretorioResult + "/" + MERGE_TEMP2);

    executaMesclar(Informacao.DiretorioResult + "/" + MERGE_TEMP3);

    QFile::remove(Informacao.DiretorioResult + "/" + MERGE_TEMP1);
    QFile::rename(Informacao.DiretorioResult + "/" + MERGE_TEMP3,Informacao.DiretorioResult + "/" + MERGE_TEMP1);


    on_bt_Teste_clicked();
}
*/
/*
void MainWindow::on_bt_Teste_clicked()
{
    QString         InputTif,OutputTif,Name,Dir;
    char            intStr[100];
    qint64          Elapsed;
    QVector<int>    Images;


    if (Informacao.Diretorio == "") {
        ui->txt_Output->append("\n Erro. Diretório não definido.");
        return;
    }

    Images.push_back(0);
    Images.push_back(1);

    //dialogDeslocaImg->printFile(Informacao.Imagem_Foco);

    sprintf(intStr,"Arquivo 1: [%s]\n",Informacao.Imagem_Foco.toStdString().c_str());
    ui->txt_Output->append(intStr);

    cv::destroyAllWindows();

    ////    Considera imagem na primeira linha. Depois tem que implementar partindo de qualquer uma,
    ///  pois em regiões de mato fica difícil escolher pontos coincidentres

    int             i = 0,Image = 0, j, k;
    double          Dist,Menor_Dist = 1e10;

    for ( ; i < Coords.PROC.Lat.length() ; i++) {
        if (Coords.PROC.Direcao.at(i) == -1) break;
    }
    j = i; // Imagem inicial da direção volta
    for ( ; i < Coords.PROC.Lat.length() ; i++) {
        if ((Coords.PROC.Direcao.at(i) == 0) || (Coords.PROC.Direcao.at(i) == 1)) break;
    }
    k = i - 1; // Imagem final da direção volta

    for ( i = j ; i <= k ; i++) {
        Dist = std::sqrt(pow((Coords.PROC.Lat.at(i) - Coords.PROC.Lat.at(3)),2) + pow(Coords.PROC.Lon.at(i) - Coords.PROC.Lon.at(3),2));
        if (Menor_Dist > Dist) {
            Menor_Dist = Dist;
            Image = i;
        }
    }

    Images.push_back(Image);

    ui->txt_Output->append("\n Imagem número " + QString::number(i));

    Geo_Referencia.Name = Coords.PROC.FileName.at(0);
    Le_Imagem_TIF(Geo_Referencia.Name,Geo_Referencia);

    Geo_Referencia.AjustaImagens = true;
    Geo_Tiff.AjustaImagens = true;

    frameW = Geo_Referencia.Iw;
    frameH = Geo_Referencia.Ih;

    namedWindow(Geo_Referencia.Name.toStdString(), WINDOW_GUI_EXPANDED);

    // 600 pontos em Y
    double Rel1;
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Referencia.Name.toStdString(), 20,20); //  Passar isto para dentro do if anterior
    }

    resizeWindow(Geo_Referencia.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow(Geo_Referencia.Name.toStdString(),Geo_Referencia.image);
    setMouseCallback(Geo_Referencia.Name.toStdString(),on_mouse_Desloca_TIF_1,NULL);



// Imagem TIF georeferenciada
    Geo_Tiff.Name = Coords.PROC.FileName.at(Images.at(1));

    sprintf(intStr,"Arquivo 2: [%s]\n",Geo_Tiff.Name.toStdString().c_str());
    ui->txt_Output->append(intStr);

    timer2.start();

    Le_Imagem_TIF(Geo_Tiff.Name,Geo_Tiff);

    frameW = Geo_Tiff.image.cols;
    frameH = Geo_Tiff.image.rows;

    namedWindow(Geo_Tiff.Name.toStdString(), WINDOW_GUI_EXPANDED);
    // 600 pontos em Y
    if (frameW > frameH) {
        Rel1 = frameH / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 20,700);
    } else {
        Rel1 = frameW / 600.0;
        moveWindow(Geo_Tiff.Name.toStdString(), 700,20);
    }

    Elapsed = timer2.elapsed();
    sprintf(intStr," Imagem 2 lida em %3.3f segundos.\n",(Elapsed / 1000.0));
    ui->txt_Output->append(intStr);


    resizeWindow(Geo_Tiff.Name.toStdString(),static_cast<int>(frameW / Rel1),static_cast<int>(frameH / Rel1));
    imshow(Geo_Tiff.Name.toStdString(),Geo_Tiff.image);
    setMouseCallback(Geo_Tiff.Name.toStdString(),on_mouse_Desloca_TIF_2,NULL);
}

*/

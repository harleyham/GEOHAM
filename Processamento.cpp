#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QTime>
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
#include "mythread.h"

using namespace std;
using namespace cv;

extern Geometria_Imagem Geo,GeoProc,Geo_Img;
extern cv::Mat imageRead,ImgWindowPontos;
extern message_function MeFu;

void MainWindow::on_bt_Dir_Proc_clicked()
{
    QDir directory = QFileDialog::getExistingDirectory(this, tr("select directory"),Informacao.DiretorioProc);

    if (directory.dirName() == ".") {
        ui->bt_Lista_Arquivos->setEnabled(false);
        ui->bt_Mesclar->setEnabled(false);
        return;
    } else {
        ui->bt_Lista_Arquivos->setEnabled(true);
        ui->bt_Mesclar->setEnabled(true);
    }

    dirOpen = directory.absolutePath();
    Informacao.DiretorioProc = dirOpen;
    ui->txt_Dir_Proc->setText(Informacao.DiretorioProc);
    ui->bt_ProcessarImgs->setEnabled(true);
    Lista_Files_Proc();
}

void MainWindow::on_txt_Escala_returnPressed()
{

}

void MainWindow::on_txt_Escala_textEdited(const QString &arg1)
{

}

void MainWindow::on_txt_Escala_editingFinished()
{
    int X;
    X = ui->txt_Escala->text().toInt();
    if ((X < 10) || (X > 100)) {
                QMessageBox::information(this,tr("Erro"),"Entre 10 e 100");
                ui->txt_Escala->setText(QString::number(Informacao.Escala1 * 100.0));
                return;
    }
    Informacao.Escala1 = (double) X / 100.0;
    ui->txt_Escala->setText(QString::number(X));
    ui->txt_Output->append("\n Escala de " + QString::number(X) + " %");

    Config.Valores.replace(CONFIG_ESCALA,X);
    Grava_Config_File();

}

//void MainWindow::processarImgs()
void MainWindow::on_bt_ProcessarImgs_clicked()
{
//    ui->list_Files->selectAll();
    SelectAll_Imgs();
    Informacao.ListadeArquivosJPG_selecionados = Informacao.ListadeArquivosJPG;

   processarImgs();

   DeSelectAll_Proc();
   DeSelectAll_Imgs();

//    ui->list_Files->selectedItems().clear();
//   ui->list_Files->clearSelection();
}

//void MainWindow::on_bt_ProcessarImgs_clicked()

void MainWindow::Le_imagem_JPG() {

    GDALAllRegister();

    for (int i = 0 ; i < 2 ; i++) {
        GDALDataset *pOldDS;

        QFileInfo File(Informacao.ListadeArquivos_MergeResult.at(i));

        Geo.Name = Informacao.ListadeArquivos_MergeResult.at(i);

//        Geo_Tiff_Name = Informacao.DiretorioResult + "/" + File.completeBaseName() + ".tif";
//        Informacao.ListadeArquivos_MergeProc.push_back(Geo_Tiff_Name);

        pOldDS = (GDALDataset *) GDALOpen(Informacao.ListadeArquivos_MergeResult.at(i).toStdString().c_str(),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
//        char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());
#ifdef GDAL
        GDAL2MAT(poSrcDS,imageRead);

#else
        imageRead = cv::imread(Informacao.ListadeArquivos_MergeResult.at(i).toStdString(),IMREAD_COLOR);
#endif

        // Faz a redução no tamanho da imagem
        if (Informacao.Escala1 != 1.0) {
            cv::resize(imageRead,imageRead,cv::Size(imageRead.cols * Informacao.Escala1,imageRead.rows * Informacao.Escala1));
        }

        QStringList stringList;

        DecodeXMP(stringList,poSrcDS);
        char ValorStr[100],InputStr[100];

        Geo.Iw = poSrcDS->GetRasterXSize();
        Geo.Ih = poSrcDS->GetRasterYSize();

        strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLength"));
        RetiraParenteses(ValorStr,InputStr);
        Geo.DistFocal = atof(ValorStr);
        strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLengthIn35mmFilm"));
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

        Geo.Cc.imag(getLONfromEXIF(poSrcDS));
        Geo.Cc.real(getLATfromEXIF(poSrcDS));

        Geo.Escala = Informacao.Escala1;
        imageRead.copyTo(Geo.image_Lida);

        Geo_JPG.append(Geo);
    }
}




void MainWindow::processarImgs()
{
    char        intStr[100];
    QString TMP = "TMP.tif";

    if (ui->actionMiranda_2->isChecked()) {
        dirOpen = QFileDialog::getOpenFileName(this, tr("Script Python"),"",tr("Arquivo (*.py)"));

        if (dirOpen.isNull()) {
            return;
        } else {
            Informacao.PyGeoRef = dirOpen;
        }

        ui->txt_Output->setText(Informacao.PyGeoRef);

        QString command("python");
        QDir::setCurrent(Informacao2.DiretorioProc);
        QStringList params1 = QStringList() << "activate" << "georref";

        ui->txt_Output->append("Processamento:\n");

        ui->txt_Path->setText(" ");

        ui->progressBar_Mesclar->setMaximum(Informacao2.ListadeArquivos.length());
        ui->progressBar_Mesclar->setValue(0);
        ui->progressBar_Mesclar->setEnabled(true);
        ui->bt_Cancelar_Proc->setEnabled(true);
        CancelarProc = false;


        QElapsedTimer   timer1;
        timer1.start();

        for (int i = 0 ; i < Informacao2.ListadeArquivos.length() ; i++) {
            QFileInfo Info = Informacao2.ListadeArquivos.at(i);
    //        QString Name = Info.fileName();
            QString Name = Info.absoluteFilePath();

            QStringList params2 = QStringList() << Informacao.PyGeoRef << Name;
            QProcess *process = new QProcess();

    //        process->start();
    //        process->start("conda",params1);

            process->start(command,params2);
            process->waitForFinished(-1);
            QString Out = process->readAll();
            process->close();
            sprintf(intStr,"(%i/%i) %s",(i+1),Informacao2.ListadeArquivos.length(),Out.toStdString().c_str());
            ui->txt_Output->append(intStr);
            ui->progressBar_Mesclar->setValue(i + 1);
            if (CancelarProc) {
                process->kill();
                i = Informacao2.ListadeArquivos.length();
             }
        }
        CancelarProc = false;
        qint64 Elapsed;
        Elapsed = timer1.elapsed();
        sprintf(intStr,"Processado em %d segundos.",static_cast<int>(Elapsed/1000));
        ui->txt_Output->append(intStr);
        ui->bt_Cancelar_Proc->setEnabled(false);
    } else { // Algoritmo Harley

       ui->txt_NumEntries_Proc->clear();

        ui->txt_Output->setText("   Processamento:\n");

        if (ui->actionEspecificar_2->isChecked()) {
            sprintf(intStr,"\n Processando com altura definida pelo usuário em %6.2f metros.\n",Geo.Alt_Default);
            ui->txt_Output->append(intStr);
        }

        ui->progressBar_Mesclar->setMaximum(Informacao.ListadeArquivosJPG_selecionados.length());

        ui->progressBar_Mesclar->setValue(0);
        ui->progressBar_Mesclar->setEnabled(true);
        ui->bt_Cancelar_Proc->setEnabled(true);
        CancelarProc = false;

        QDir::setCurrent(Informacao.DiretorioProc);

        QElapsedTimer   timer1;
        timer1.start();
        ui->list_Files_Proc->clear();

        GDALAllRegister();
/*
        int maxT = omp_get_max_threads();

#pragma omp parallel num_threads(maxT)
        {
#pragma omp for
*/
        for (int i = 0 ; i < Informacao.ListadeArquivosJPG_selecionados.length() ; i++) {
            QFileInfo Info = Informacao.ListadeArquivosJPG_selecionados.at(i);
            QString Name = Info.absoluteFilePath();
            Geo_Tiff_Name = Informacao.DiretorioProc + "/" + Info.completeBaseName() + ".tif";

            GDALDataset *pOldDS;

            pOldDS = (GDALDataset *) GDALOpen(Name.toStdString().c_str(),GA_ReadOnly);
            GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
            char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());
#ifdef GDAL
            GDAL2MAT(poSrcDS,imageRead);

#else
            imageRead = cv::imread(Name.toStdString(),IMREAD_COLOR);
#endif

            // Faz a redução no tamanho da imagem
            if (Informacao.Escala1 != 1.0) {
                cv::resize(imageRead,imageRead,cv::Size(imageRead.cols * Informacao.Escala1,imageRead.rows * Informacao.Escala1));
            }

            QStringList stringList;

            DecodeXMP(stringList,poSrcDS);
            char ValorStr[100],InputStr[100];

            Geo.Iw = poSrcDS->GetRasterXSize();
            Geo.Ih = poSrcDS->GetRasterYSize();

            strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLength"));
            RetiraParenteses(ValorStr,InputStr);
            Geo.DistFocal = atof(ValorStr);
            strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLengthIn35mmFilm"));
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
            double Angulo = 0.0;
            if (Geo.Gimbal_Angle < 0.0) Geo.Gimbal_Angle = Geo.Gimbal_Angle - Angulo;
           else Geo.Gimbal_Angle = Geo.Gimbal_Angle + Angulo;




            //// Aqui coloca a correção de coordenadas
            ///

            Geo.Cc.imag(getLONfromEXIF(poSrcDS));
            Geo.Cc.real(getLATfromEXIF(poSrcDS));

            /*
            if (Geo.Gimbal_Angle > 0) {
                Geo.Cc.imag(getLONfromEXIF(poSrcDS)-(0.000040/2.0));
                Geo.Cc.real(getLATfromEXIF(poSrcDS)-(0.000018/2.0));
            } else {
                Geo.Cc.imag(getLONfromEXIF(poSrcDS)+(0.000040/2.0));
                Geo.Cc.real(getLATfromEXIF(poSrcDS)+(0.000018/2.0));
            }
*/
            Geo.Escala = Informacao.Escala1;

            Geo.Calculos();

            Geo.FileDS = poSrcDS;

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

            /// Grava arquivo
            GDALDataset* pNewDS = GDALDataset::FromHandle(pOldDS);
            char** papszMetadata2 = CSLDuplicate(pNewDS->GetMetadata());
            char** papszMetadaDomain = CSLDuplicate(pNewDS->GetMetadataDomainList());

#ifndef GDAL_WRITE
            std::vector<int> params;
//            params.push_back(IMWRITE_JPEG_QUALITY);
//            params.push_back(50); // Mais valor -> menor compressao. Default 95
//            params.push_back(IMWRITE_PNG_COMPRESSION);
//            params.push_back(3); // Maior valor -> mais compressão. Default 3

             bool Ok;
             params.push_back(IMWRITE_TIFF_COMPRESSION);
             Ok = cv::imwrite(TMP.toStdString(),Destino);
             if (!Ok) {
                    ui->txt_Output->append(" \n Erro de escrita no disco do arquivo " + Geo_Tiff_Name);
                    return;
                }
/*
                HANDLE fh;
                fh = CreateFile(Geo_Tiff_Name.toStdString(), GENERIC_READ, 0, NULL, OPEN_EXISTING,0,NULL);
                if ((fh != NULL) && (fh != INVALID_HANDLE_VALUE)) {

                    //do
                    CloseHandle(fh);
                }
*/

             while (true) {
               int ret = rename(TMP.toStdString().c_str(),Geo_Tiff_Name.toStdString().c_str());
               if (ret == 0) {
                  break;
                }
                  Sleep(500);
                }

                writeGeoInfo(Geo_Tiff_Name,Destino,papszMetadata,Geometria_ImgProc);

                Grava_Thumb(Geo_Tiff_Name);
        #else
            if (ui->actionBranco->isChecked()) {
                Geometria_ImgProc.Cor_Nodata = 255;
            } else {
                Geometria_ImgProc.Cor_Nodata = 0;
            }

            MAT2GDAL(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc);

//            MAT2GDAL(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc,&stringList);

        #endif

            sprintf(intStr,"(%i/%i) %s",(i+1),Informacao.ListadeArquivosJPG_selecionados.length(),Name.toStdString().c_str());
            ui->txt_Output->append(intStr);
            sprintf(intStr," (%3.4f,%3.4f) - (%3.4f,%3.4f) \n",Geometria_ImgProc.AUTM.Latitude(),Geometria_ImgProc.AUTM.Longitude(),Geometria_ImgProc.DUTM.Latitude(),Geometria_ImgProc.DUTM.Longitude());
            ui->txt_Output->append(intStr);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            ui->progressBar_Mesclar->setValue(i + 1);
            if (CancelarProc) {
                i = Informacao.ListadeArquivosJPG_selecionados.length();
             }
        }
/*
        } // fecha pragma
*/
        CancelarProc = false;
        qint64 Elapsed;
        Elapsed = timer1.elapsed();


        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("blue"));
        sprintf(intStr,"Processado em %d segundos.",static_cast<int>(Elapsed/1000));
        ui->txt_Output->append(intStr);
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        ui->bt_Cancelar_Proc->setEnabled(false);


        ui->progressBar_Mesclar->setValue(0);
        ui->progressBar_Mesclar->setEnabled(false);


        Lista_Files_Proc();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_bt_Cancelar_Proc_clicked()
{
    CancelarProc = true;
}


void MainWindow::processa_Imgs() {

    int Num_Itens = ui->list_Files->selectedItems().size();

    if (Num_Itens == 0) {
        ui->list_Files->selectAll();
        Informacao.ListadeArquivosJPG_selecionados = Informacao.ListadeArquivosJPG;

        processarImgs();

        ui->list_Files->selectedItems().clear();
        ui->list_Files->clearSelection();
    } else {
        Informacao.ListadeArquivosJPG_selecionados.clear();
        for (int i = 0 ; i < Num_Itens ; i++) {
            QListWidgetItem *item = ui->list_Files->selectedItems().at(i);
            QString File;
            File = Informacao.Diretorio + "/" + item->text();
            Informacao.ListadeArquivosJPG_selecionados.append(QFileInfo(File));
        }

        DeSelectAll_Proc();

        processarImgs();



//        ui->list_Files->selectedItems().clear();
//        ui->list_Files->clearSelection();
    }
}



void MainWindow::on_actionCortarTIFF_triggered()
{
    if (GeoProc.PImg.length() < 3) {
        QMessageBox::information(this,tr("Erro"),"Necessários 3 pontos !");
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        return;
    }


    int     Xmin,Ymin,Xmax,Ymax;
    double  MinX,MinY,MaxX,MaxY;

    int     No_Data;

//    QString Geo_Tiff_Name = Informacao.DiretorioResult + "/teste_corte.tif";
    QString Geo_Tiff_Name;

    Geo_Tiff_Name = QFileDialog::getSaveFileName(this, tr("Nome do arquivo TIF"),Informacao.DiretorioResult,tr("Image (*.tif)"));

    if (Geo_Tiff_Name == "") {
        return;
     }


    /*
    bool Ant_Circulo, Ant_Linha, Ant_Poligono;
    Ant_Circulo = ui->checkBox_Circle->isChecked();
    Ant_Linha = ui->checkBox_Linhas->isChecked();
    Ant_Poligono = ui->CheckBox_Poligono->isChecked();
    ui->checkBox_Circle->setChecked(false);
    ui->checkBox_Linhas->setChecked(false);
    ui->CheckBox_Poligono->setChecked(false);
    MeFu.Draw_Cel();
*/
    Xmin = 1e8;
    Ymin = 1e8;
    Xmax = -1e8;
    Ymax = -1e8;

    MinX = 1e8;
    MinY = 1e8;
    MaxX = -1e8;
    MaxY = -1e8;

    vector<Point> pointList;

    for (int i = 0 ; i < GeoProc.PImg.length() ; i ++ ) {
        if (Xmin > GeoProc.PImg.at(i).real()) Xmin = GeoProc.PImg.at(i).real();
        if (Xmax < GeoProc.PImg.at(i).real()) Xmax = GeoProc.PImg.at(i).real();
        if (Ymin > GeoProc.PImg.at(i).imag()) Ymin = GeoProc.PImg.at(i).imag();
        if (Ymax < GeoProc.PImg.at(i).imag()) Ymax = GeoProc.PImg.at(i).imag();
        Point a;
        a.x = GeoProc.PImg.at(i).real();
        a.y = GeoProc.PImg.at(i).imag();
        pointList.push_back(a);

        if (MinY > GeoProc.Pontos.at(i).real()) MinY = GeoProc.Pontos.at(i).real();
        if (MaxY < GeoProc.Pontos.at(i).real()) MaxY = GeoProc.Pontos.at(i).real();
        if (MinX > GeoProc.Pontos.at(i).imag()) MinX = GeoProc.Pontos.at(i).imag();
        if (MaxX < GeoProc.Pontos.at(i).imag()) MaxX = GeoProc.Pontos.at(i).imag();
    }

    Rect r(Xmin,Ymin,Xmax - Xmin + 1,Ymax - Ymin + 1);

    cv::Mat dstroi(Ymax - Ymin + 1, Xmax - Xmin + 1,  CV_8UC3, Scalar(255,255,255));

    cv::Mat mask = Mat::zeros(GeoProc.image.rows,GeoProc.image.cols,GeoProc.image.type());
    cv::fillConvexPoly(mask,pointList,Scalar(255,255,255));
//    cv::Mat tmp(Destino.rows,Destino.cols,Destino.type(),Scalar(1,1,1));
//    Destino = Destino - tmp;
    cv::Mat tmp;

    ImgWindowPontos.copyTo(tmp,mask);
    tmp(r).copyTo(dstroi);             // A cor de NODATA é PRETO

    // Para fazer a cor de NODATA ser banco é necessário este segundo passo
    if (ui->actionBranco->isChecked()) {
        Mat mask2;
        inRange(dstroi,Scalar(0,0,0),Scalar(0,0,0),mask2);
        dstroi.setTo(Scalar(255,255,255), mask2);
    }

    //    imshow("Teste corte",dstroi);

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
    Geo_Img.mppx = GeoProc.mppx;
    Geo_Img.mppy = GeoProc.mppy;
    Geo_Img.x_Size = GeoProc.x_Size;
    Geo_Img.y_Size = GeoProc.y_Size;

//    Geo_Tiff_Name = Informacao.DiretorioProc + "/" + CompleteName.completeBaseName() + ".tif";
#ifdef GDAL
     MAT2GDAL(Geo_Tiff_Name,dstroi,NULL,Geo_Img);
#else
    std::vector<int> params;
//            params.push_back(IMWRITE_JPEG_QUALITY);
//            params.push_back(50); // Mais valor -> menor compressao. Default 95
//            params.push_back(IMWRITE_PNG_COMPRESSION);
//            params.push_back(3); // Maior valor -> mais compressão. Default 3
    params.push_back(IMWRITE_TIFF_COMPRESSION);
    bool check = cv::imwrite(Geo_Tiff_Name.toStdString(),dstroi);
    if (!check) {
        QString Path;
        Path = "Não foi possível gravar arquivo " + Geo_Tiff_Name;
        QMessageBox::information(this,tr("Erro"),Path);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        return;
     }

     writeGeoInfo(Geo_Tiff_Name,dstroi,NULL,Geo_Img);
#endif

     QString Output;
     Size_GeoTiff(Geo_Tiff_Name,true,Output);

     if (Geo_Tiff_Name.contains(Informacao.DiretorioResult)) {
        Grava_Thumb(Geo_Tiff_Name);
        Lista_Files_Result();
     }

/*
     ui->checkBox_Circle->setChecked(Ant_Circulo);
     ui->checkBox_Linhas->setChecked(Ant_Linha);
     ui->CheckBox_Poligono->setChecked(Ant_Poligono);
     MeFu.Draw_Cel();
*/
}

///////
/// Aparentemente a versão do GDAL que estou usando não aceita escrever KMZ
/// Mas aceita ler
/// Em 26/02/2022 tentei compilar a versão 3.4.1 do GDAL em casa, mas não consegui
///
///
void MainWindow::Grava_KMZ_from_file() { // QString fileOrigem, QString fileKMZ) {

    QString Geo_Tiff_Name,fileKMZ;

    Geo_Tiff_Name = QFileDialog::getOpenFileName(this, tr("Nome do arquivo TIF"),Informacao.DiretorioResult,tr("Image (*.tif)"));

    if (Geo_Tiff_Name == "") {
        return;
     }

    GDALAllRegister();

    QFileInfo File(Geo_Tiff_Name);

    fileKMZ = File.path() + "/" + File.completeBaseName() + ".kmz";
    GDALDataset *pNewDS;
    GDALDriver *poDriver2 = (GDALDriver *) GDALGetDriverByName( "KML" );


    GDALDataset *poVRTDS2;
    pNewDS = (GDALDataset *) GDALOpen(Geo_Tiff_Name.toStdString().c_str(),GA_ReadOnly);
    poVRTDS2 = poDriver2->CreateCopy( fileKMZ.toStdString().c_str(), pNewDS, FALSE, NULL, NULL, NULL );
    GDALClose((GDALDatasetH) poVRTDS2);
}



void MainWindow::imprimeFimMerge(void)
{
    qint64  Elapsed;
    char   intStr[100];
    Elapsed = timer2.elapsed();

    QColor tc = ui->txt_Output->textColor();
    ui->txt_Output->setTextColor(QColor("blue"));
    sprintf(intStr," GeoTiFF gerado em %3.1f segundos.",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);
    ui->txt_Output->setTextColor(tc);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    Nao_ProgressBar_Result = true;
    Lista_Files_Result();
    Nao_ProgressBar_Result = false;

    QString Output;
    Size_GeoTiff(mThread_Merge.NameTIF,false,Output);
    ui->txt_Output->append(Output);

    if (!mThread_Merge.Mesmo_Dir) DeSelectAll_Result();

    emit ResetProgressBarMesclar();
}


void MainWindow::imprimeFimICE_TIFF(void)
{
    qint64  Elapsed;
    char   intStr[100];

    Elapsed = timer2.elapsed();

    QColor tc = ui->txt_Output->textColor();
    ui->txt_Output->setTextColor(QColor("blue"));
    sprintf(intStr," Transformação em %3.1f segundos.\n",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);
    ui->txt_Output->setTextColor(tc);

    destroyAllWindows();

    AbreArquivo(QFileInfo(mThread_ICE_TIF.NameTIF),false,true);

    QString Output;
    Size_GeoTiff(mThread_ICE_TIF.NameTIF, false, Output);
    ui->txt_Output->append(Output);    

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    emit ResetProgressBarMesclar();

    Nao_ProgressBar_Result = true;
    Lista_Files_Result();
    Nao_ProgressBar_Result = false;
}

void MainWindow::updateTxt(void)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//    QByteArray data = myProcess->readAllStandardOutput();
//    ui->txt_Output->append(QString(data));
//    QApplication::processEvents();
}

void MainWindow::on_actionICE_JPG_TIF_triggered()
{
    QString InputJPG,InputTif,OutputTif,Name,Dir;
    char    intStr[100];

    InputJPG = QFileDialog::getOpenFileName(this, tr("ICE Image"),Informacao.DiretorioResult,tr("Image (*.jpg)"));
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

    AbreArquivo(QFileInfo(OutputTif),false,true); // precisa não limpar o txt_Output

    QString Output;
    Size_GeoTiff(OutputTif,true,Output);
}

void MainWindow::Redirector()
{
    redirector = new StdoutRedirector(this, StdoutRedirector::StandardOutput|StdoutRedirector::StandardError);
    connect(redirector, SIGNAL(readyRead()), this, SLOT(readData()));
}
/*
    Widget wid;
    wid.show();


    redirector = new StdoutRedirector(this, StdoutRedirector::StandardOutput|StdoutRedirector::StandardError);
//    TimerStdout = new QTimer(this);
//    TimerStdout->setInterval(5000);
//    TimerStdout->stop();
    QHBoxLayout *hbox = new QHBoxLayout(this);
    edit = new QPlainTextEdit(this);
    hbox->addWidget(edit);
//    edit->show();

    //timer->start();
//    connect(TimerStdout, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(redirector, SIGNAL(readyRead()), this, SLOT(readData()));
*/
void MainWindow::readData()
{
    TextStdout = QString::fromLocal8Bit(redirector->read(1024));
//    ui->txt_Output->append(TextStdout + "\n");
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    if (CapturaStdout) {
        int A = TextStdout.toInt();
        if (A != 0) ProgressBarValue = A; else ProgressBarValue += 2.5;
        ui->progressBar_Mesclar->setValue(ProgressBarValue);
    }
}

void MainWindow::onTimeout()
{
/*
    printf("From printf ... ");
    fflush(stdout);
    fprintf(stderr, "From fprintf...");
    std::cout<<"From std::cout..."<<std::endl;
    std::cerr<<"From std::cerr..."<<std::endl;
    qDebug()<<"From qDebug ..."<<QDateTime::currentDateTime().toString();
*/
}

void MainWindow::ResetProgressBarMesclar_EXEC() {
    CapturaStdout = false;
    ui->progressBar_Mesclar->setValue(100);
    Sleep(1000);
    ui->progressBar_Mesclar->setValue(0);
    ui->progressBar_Mesclar->setEnabled(false);
}

void MainWindow::ActiveProgressBarMesclar_EXEC() {
    if (redirector == NULL) {
        Redirector();
    }
    CapturaStdout = true;
    ui->progressBar_Mesclar->setEnabled(true);
    ui->progressBar_Mesclar->setMinimum(0);
    ui->progressBar_Mesclar->setMaximum(100);
    ui->progressBar_Mesclar->setValue(0);
    ProgressBarValue = 0;
}


void MainWindow::AtualizaProgressBarMesclar_Merge() {
    ui->progressBar_Mesclar->setValue(mThread_Merge.Contador_Escala);
}


void MainWindow::on_actionM_dia_triggered()
{

}


void MainWindow::processarImgsProv(double Angulo) {



    Informacao.ListadeArquivos_MergeProc.clear();
    for (int i = 0 ; i < 2 ; i++) {

        QFileInfo File(Informacao.ListadeArquivos_MergeResult.at(i));
        Geo_Tiff_Name = Informacao.DiretorioResult + "/" + File.completeBaseName() + ".tif";

        QListIterator<Geometria_Imagem> Geo1(Geo_JPG);
        Geometria_Imagem                Geo;


        Geo = Geo1.next();

        Geo_JPG.at(i).image_Lida.copyTo(imageRead);

        // Faz a redução no tamanho da imagem
//        if (Informacao.Escala1 != 1.0) {
//            cv::resize(imageRead,imageRead,cv::Size(imageRead.cols * Informacao.Escala1,imageRead.rows * Informacao.Escala1));
//        }

        // Aqui vai a correção de ângulo
            if (Geo.Gimbal_Angle < 0.0) Geo.Gimbal_Angle = Geo.Gimbal_Angle - Angulo;
            else Geo.Gimbal_Angle = Geo.Gimbal_Angle + Angulo;

        Geo.Calculos();


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

        /// Grava arquivo
//        GDALDataset* pNewDS = GDALDataset::FromHandle(pOldDS);
//        char** papszMetadata2 = CSLDuplicate(pNewDS->GetMetadata());
//        char** papszMetadaDomain = CSLDuplicate(pNewDS->GetMetadataDomainList());

#ifndef GDAL_WRITE
        std::vector<int> params;
//            params.push_back(IMWRITE_JPEG_QUALITY);
//            params.push_back(50); // Mais valor -> menor compressao. Default 95
//            params.push_back(IMWRITE_PNG_COMPRESSION);
//            params.push_back(3); // Maior valor -> mais compressão. Default 3

         bool Ok;
         params.push_back(IMWRITE_TIFF_COMPRESSION);
         Ok = cv::imwrite(TMP.toStdString(),Destino);
         if (!Ok) {
                ui->txt_Output->append(" \n Erro de escrita no disco do arquivo " + Geo_Tiff_Name);
                return;
            }
/*
            HANDLE fh;
            fh = CreateFile(Geo_Tiff_Name.toStdString(), GENERIC_READ, 0, NULL, OPEN_EXISTING,0,NULL);
            if ((fh != NULL) && (fh != INVALID_HANDLE_VALUE)) {

                //do
                CloseHandle(fh);
            }
*/

         while (true) {
           int ret = rename(TMP.toStdString().c_str(),Geo_Tiff_Name.toStdString().c_str());
           if (ret == 0) {
              break;
            }
              Sleep(500);
            }

            writeGeoInfo(Geo_Tiff_Name,Destino,papszMetadata,Geometria_ImgProc);

            Grava_Thumb(Geo_Tiff_Name);
    #else
        if (ui->actionBranco->isChecked()) {
            Geometria_ImgProc.Cor_Nodata = 255;
        } else {
            Geometria_ImgProc.Cor_Nodata = 0;
        }

        MAT2GDAL(Geo_Tiff_Name,Destino,NULL,Geometria_ImgProc);

//            MAT2GDAL(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc,&stringList);

    #endif

    }
/*
    } // fecha pragma
*/

}

void MainWindow::processarImgsProv0(double Angulo) {

    GDALAllRegister();

    Informacao.ListadeArquivos_MergeProc.clear();
    for (int i = 0 ; i < 2 ; i++) {
        GDALDataset *pOldDS;

        QFileInfo File(Informacao.ListadeArquivos_MergeResult.at(i));
        Geo_Tiff_Name = Informacao.DiretorioResult + "/" + File.completeBaseName() + ".tif";
        Informacao.ListadeArquivos_MergeProc.push_back(Geo_Tiff_Name);

        pOldDS = (GDALDataset *) GDALOpen(Informacao.ListadeArquivos_MergeResult.at(i).toStdString().c_str(),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
        char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());
#ifdef GDAL
        GDAL2MAT(poSrcDS,imageRead);

#else
        imageRead = cv::imread(Informacao.ListadeArquivos_MergeResult.at(i).toStdString(),IMREAD_COLOR);
#endif

        // Faz a redução no tamanho da imagem
        if (Informacao.Escala1 != 1.0) {
            cv::resize(imageRead,imageRead,cv::Size(imageRead.cols * Informacao.Escala1,imageRead.rows * Informacao.Escala1));
        }

        QStringList stringList;

        DecodeXMP(stringList,poSrcDS);
        char ValorStr[100],InputStr[100];

        Geo.Iw = poSrcDS->GetRasterXSize();
        Geo.Ih = poSrcDS->GetRasterYSize();

        strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLength"));
        RetiraParenteses(ValorStr,InputStr);
        Geo.DistFocal = atof(ValorStr);
        strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_FocalLengthIn35mmFilm"));
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

        // Aqui vai a correção de ângulo
            if (Geo.Gimbal_Angle < 0.0) Geo.Gimbal_Angle = Geo.Gimbal_Angle - Angulo;
            else Geo.Gimbal_Angle = Geo.Gimbal_Angle + Angulo;


        Geo.Cc.imag(getLONfromEXIF(poSrcDS));
        Geo.Cc.real(getLATfromEXIF(poSrcDS));

        Geo.Escala = Informacao.Escala1;

        Geo.Calculos();

        Geo.FileDS = poSrcDS;

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

        /// Grava arquivo
        GDALDataset* pNewDS = GDALDataset::FromHandle(pOldDS);
        char** papszMetadata2 = CSLDuplicate(pNewDS->GetMetadata());
        char** papszMetadaDomain = CSLDuplicate(pNewDS->GetMetadataDomainList());

#ifndef GDAL_WRITE
        std::vector<int> params;
//            params.push_back(IMWRITE_JPEG_QUALITY);
//            params.push_back(50); // Mais valor -> menor compressao. Default 95
//            params.push_back(IMWRITE_PNG_COMPRESSION);
//            params.push_back(3); // Maior valor -> mais compressão. Default 3

         bool Ok;
         params.push_back(IMWRITE_TIFF_COMPRESSION);
         Ok = cv::imwrite(TMP.toStdString(),Destino);
         if (!Ok) {
                ui->txt_Output->append(" \n Erro de escrita no disco do arquivo " + Geo_Tiff_Name);
                return;
            }
/*
            HANDLE fh;
            fh = CreateFile(Geo_Tiff_Name.toStdString(), GENERIC_READ, 0, NULL, OPEN_EXISTING,0,NULL);
            if ((fh != NULL) && (fh != INVALID_HANDLE_VALUE)) {

                //do
                CloseHandle(fh);
            }
*/

         while (true) {
           int ret = rename(TMP.toStdString().c_str(),Geo_Tiff_Name.toStdString().c_str());
           if (ret == 0) {
              break;
            }
              Sleep(500);
            }

            writeGeoInfo(Geo_Tiff_Name,Destino,papszMetadata,Geometria_ImgProc);

            Grava_Thumb(Geo_Tiff_Name);
    #else
        if (ui->actionBranco->isChecked()) {
            Geometria_ImgProc.Cor_Nodata = 255;
        } else {
            Geometria_ImgProc.Cor_Nodata = 0;
        }

        MAT2GDAL(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc);

//            MAT2GDAL(Geo_Tiff_Name,Destino,papszMetadata2,Geometria_ImgProc,&stringList);

    #endif

    }
/*
    } // fecha pragma
*/

}


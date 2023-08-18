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

extern  Geometria_Imagem    Geo_Img;
extern  Plot_Coords         Coords;
extern message_function     MeFu;

bool                        LimpaTela;


void MainWindow::Print_Caracteristicas_Mesclagem(QString File,int Sentido) {
    // Lista as opções da mesclagem

    char intStr[100];

//        ui->txt_Output->append("[" + File + "]");
        ui->txt_Output->append("\n Número de imagens = " + QString::number(Imagem_Composta.Geo_File_List.length()));

        if (Sentido == ESCOLHIDOS) {
            ui->txt_Output->append(" Imagens escolhidas pelo usuário");
        } else if (ui->actionTodas_as_imagens->isChecked()) {
            if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) {
                ui->txt_Output->append(" Todas as imagens menos as tiradas com o drone fazendo curvas");
            } else {
                ui->txt_Output->append(" Todas as imagens incluindo as tiradas com o drone fazendo curvas");
            }
        } else {
            if (Sentido & VAI) {
                ui->txt_Output->append(" Somente sentido IDA");
            }

            if (Sentido & VOLTA) {
                ui->txt_Output->append(" Somente sentido VOLTA");
            }

            if (Sentido & CURVA) {
                ui->txt_Output->append(" Inclui imagens tiradas com o drone fazendo curvas");
            } else {
                ui->txt_Output->append(" Exclui imagens tiradas com o drone fazendo curvas");
            }

            if (ui->actionUsar_imagens_da_primeira_e_ltima_linha->isChecked()) {
                ui->txt_Output->append(" Inclui imagens da primeira e última linhas");
            }
        }

        sprintf(intStr," Ângulo de ida = %4.6f", Coords.PROC.Media_Ida);
        ui->txt_Output->append(intStr);
        sprintf(intStr," Ângulo de volta = %4.6f", Coords.PROC.Media_Volta);
        ui->txt_Output->append(intStr);

        sprintf(intStr," Imagens na ida = %d, imagens na volta = %d, imagens nas curvas = %d",
                Coords.PROC.Ida,
                Coords.PROC.Volta,
                Coords.PROC.Curvas);
        ui->txt_Output->append(intStr);

        ui->txt_Output->append(" " + QString::number(Coords.PROC.Num_Ida) + " idas e " +
                               QString::number(Coords.PROC.Num_Volta) + " voltas.");

        sprintf(intStr," Última imagem da primeira linha = %d, primeira imagem da última linha = %d",
                       Coords.PROC.Ultimo_Primeira,Coords.PROC.Primeiro_Ultima);
        ui->txt_Output->append(intStr);

        updateTxt();

}

void MainWindow::on_bt_Mesclar_clicked()
{
    QStringList     Arquivos;
    QString         Complete_1,Complete_2;


    QFileDialog     *fd;

    fd = new QFileDialog();
    fd->setDirectory(Informacao.DiretorioResult);
    fd->setDefaultSuffix("tif");
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setNameFilter("Imagem (*.tif)");
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setOption(QFileDialog::DontConfirmOverwrite,true);
    if (!fd->exec()) return;
    Arquivos = fd->selectedFiles();
    Complete_1 = Arquivos.at(0);

//    Complete_1 = QFileDialog::getSaveFileName(this, tr("Imagem a ser gravada"),Informacao.DiretorioResult,tr("Image (*.tif)"));
//    if (Complete_1.isNull()) return;

    Complete_2 = Complete_1;

    LimpaTela = true;

   if (ui->actionTodas_as_imagens->isChecked()) {
       MesclarItens(Complete_1);
       return;
   }

   if (ui->actionSendido_nico_1->isChecked() && ui->actionSentido_unico_2->isChecked()) {
       QFileInfo F_1(Complete_1);

       Complete_1 = Informacao.DiretorioResult + "/" + F_1.baseName() + "_1.tif";
       Complete_2 = Informacao.DiretorioResult + "/" + F_1.baseName() + "_2.tif";
   }

   if (ui->actionSendido_nico_1->isChecked()) {
       MesclarItens(Complete_1,VAI);
       LimpaTela = false;
    }

   if (ui->actionSentido_unico_2->isChecked()) MesclarItens(Complete_2,VOLTA);

   Lista_Files_Result();
}

void MainWindow::Mesclar_Prov()
{
    QStringList     Arquivos;
    QString         Complete_1,Complete_2;

/*
    QFileDialog     *fd;

    fd = new QFileDialog();
    fd->setDirectory(Informacao.DiretorioResult);
    fd->setDefaultSuffix("tif");
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setNameFilter("Imagem (*.tif)");
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setOption(QFileDialog::DontConfirmOverwrite,true);
    if (!fd->exec()) return;

    Arquivos = fd->selectedFiles();
*/
    Complete_1 = Informacao.DiretorioResult + "/" + MERGE_TEMP1;

//    Complete_1 = QFileDialog::getSaveFileName(this, tr("Imagem a ser gravada"),Informacao.DiretorioResult,tr("Image (*.tif)"));
//    if (Complete_1.isNull()) return;

//    Complete_2 = Complete_1;

    LimpaTela = true;

   if (ui->actionTodas_as_imagens->isChecked()) {
       MesclarItens(Complete_1);
       return;
   }

/*
   if (ui->actionSendido_nico_1->isChecked() && ui->actionSentido_unico_2->isChecked()) {
       QFileInfo F_1(Complete_1);

       Complete_1 = Informacao.DiretorioResult + "/" + F_1.baseName() + "_1.tif";
       Complete_2 = Informacao.DiretorioResult + "/" + F_1.baseName() + "_2.tif";
   }

   if (ui->actionSendido_nico_1->isChecked()) {
       MesclarItens(Complete_1,VAI);
       LimpaTela = false;
    }

   if (ui->actionSentido_unico_2->isChecked()) MesclarItens(Complete_2,VOLTA);

   Lista_Files_Result();
*/
}


void MainWindow::MesclarItens(QString Complete,int Sentido) {
        Imagem_Composta.Geo_File_List.clear();


        QDir directory(Informacao.DiretorioProc);
        QStringList Filters;
        Filters << "*.tif";
        Imagem_Composta.Geo_File_List = directory.entryInfoList(Filters,QDir::Files | QDir::NoDotAndDotDot);

        if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) Sentido += CURVA;

        Seleciona_Imagens_para_Mesclar(Sentido);

        if (Imagem_Composta.Geo_File_List.length() < 2) { // Erro - menos de duas imagens
            ui->txt_Output->setText("    Erro. São necessárias pelo menos duas imagens.\n");
            QCoreApplication::processEvents();
            return;
        }

        ui->progressBar_Mesclar->setEnabled(true);

        Imagem_Composta.Altura = ui->txt_Alt_Voo->text().toDouble();
        Imagem_Composta.Num = Imagem_Composta.Geo_File_List.length();
        Imagem_Composta.Media_Ida = Coords.PROC.Media_Ida;
        Imagem_Composta.Media_Volta = Coords.PROC.Media_Volta;
        Imagem_Composta.Sentido = Sentido;
        Imagem_Composta.Primeira_Ultima_Linha = ui->actionUsar_imagens_da_primeira_e_ltima_linha->isChecked();
        Imagem_Composta.Make = Coords.PROC.Make;
        Imagem_Composta.Model = Coords.PROC.Model;
        executaMesclar(Complete);
//        Print_Caracteristicas_Mesclagem(Complete,Sentido);
        updateTxt();

//    Lista_Files_Result();

        ui->progressBar_Mesclar->setEnabled(false);
}


void MainWindow::MesclarItens(QString Complete) {
    int Sentido;

    Imagem_Composta.Geo_File_List.clear();

    int Num_Itens = ui->list_Files_Proc->selectedItems().size();

    if (Num_Itens == 0) {
        //int Sentido;
        Sentido = VAI + VOLTA;
        if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) Sentido += CURVA;
        Seleciona_Imagens_para_Mesclar(Sentido);
    } else {
        Sentido = ESCOLHIDOS;
        Imagem_Composta.Geo_File_List.clear();
        for (int i = 0 ; i < Num_Itens ; i++) {
            QListWidgetItem *item = ui->list_Files_Proc->selectedItems().at(i);
            QString File;
            File = Informacao.DiretorioProc + "/" + item->text();
            Imagem_Composta.Geo_File_List.append(QFileInfo(File));
        }
    }

    // Coloca aqui as imagens a serem mescladas caso tenha sido feito escolha de pilha

    Num_Itens = Informacao.ListadeArquivos_MergeProc.length();

    if (Num_Itens) {
        Imagem_Composta.Geo_File_List.clear();

        for (int i = (Num_Itens - 1); i >= 0 ; i--) {
            QString File;

            File = Informacao.DiretorioProc + "/" + Informacao.ListadeArquivos_MergeProc.at(i);
            Imagem_Composta.Geo_File_List.append(QFileInfo(File));
        }
    }

    if (Imagem_Composta.Geo_File_List.length() < 2) { // Erro - menos de duas imagens
        ui->txt_Output->setText("    Erro. São necessárias pelo menos duas imagens.\n");
        QCoreApplication::processEvents();
        return;
    }

    ui->progressBar_Mesclar->setEnabled(true);

//    Print_Caracteristicas_Mesclagem(Complete,0);
    Imagem_Composta.Altura = ui->txt_Alt_Voo->text().toDouble();
    Imagem_Composta.Num = Imagem_Composta.Geo_File_List.length();
    Imagem_Composta.Media_Ida = Coords.PROC.Media_Ida;
    Imagem_Composta.Media_Volta = Coords.PROC.Media_Volta;
    Imagem_Composta.Sentido = Sentido;
    Imagem_Composta.Primeira_Ultima_Linha = ui->actionUsar_imagens_da_primeira_e_ltima_linha->isChecked();
    Imagem_Composta.Make = Coords.PROC.Make;
    Imagem_Composta.Model = Coords.PROC.Model;
    executaMesclar(Complete);

    Lista_Files_Result();

    ui->progressBar_Mesclar->setEnabled(false);

    if (Num_Itens == 0) {
//        ui->list_Files_Proc->clearSelection();
    }
}


void MainWindow::Seleciona_Imagens_para_Mesclar(int Sentido) {
    // Retira as imagens das curvas se for o escolhido
    // Deixa apenas as do sentido definido e, se desejado, as imagens das linhas de borda
    // Seleciona na lista as imagens que serão utilizadas

    if (Coords.PROC.Lat.size() == 0) return;

    DeSelectAll_Proc();

    Imagem_Composta.Sentido = Sentido;

    if (Sentido == 0) {
        return;
    }

    int Teste;
    Imagem_Composta.Geo_File_List.clear();
    for (int i = 0 ; i < Coords.PROC.Lat.size() ; i++) {
        Teste = Coords.PROC.Direcao.at(i) & Sentido; // Faz o AND, valendo para VAI, VOLTA e CURVA

        if (ui->actionUsar_imagens_da_primeira_e_ltima_linha->isChecked()) {
            if ((i <= Coords.PROC.Ultimo_Primeira) || (i >= Coords.PROC.Primeiro_Ultima)) {
                Teste = 1;
            }
        }

        if (Teste) {
            Imagem_Composta.Geo_File_List.push_back(QFileInfo(Coords.PROC.FileName.at(i)));
        }
    }

    for (int i = 0 ; i < Imagem_Composta.Geo_File_List.length() ; i++) {
        QFileInfo   Imagem(Imagem_Composta.Geo_File_List.at(i));

        QList<QListWidgetItem *> Lista_Resp = ui->list_Files_Proc->findItems(Imagem.fileName(),Qt::MatchExactly);

        QListWidgetItem *item = Lista_Resp.first();

        item->setSelected(true);
    }

    ui->Selected_PROC->setVisible(true);
    ui->Selected_PROC->setText(QString::number(ui->list_Files_Proc->selectedItems().length()));

    if (ui->actionProcessadas->isChecked()) {
        MeFu.Draw_Cel();
    }
}


void MainWindow::executaMesclar(QString Complete)
{
    QString         Name,NameKMZ;
    qint64          Elapsed;
    char            intStr[100];
    double          Media,Desvio,IC,V_Min,V_Max;
    QVector<double> Val;

    Media = 0.0;
    Desvio = 0.0;

    if (ui->actionMiranda->isChecked()) {
        if (Informacao2.TXT_File == "") {
            QMessageBox::information(this,tr("File Name"),"Arquivo TXT não definido !");
            return;
        }
    }


    if (ui->radioButton_Merge->isChecked()) {
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

    if (Imagem_Composta.Geo_File_List.length() < 2) { // Erro - menos de duas imagens
        ui->txt_Output->append("    Erro. São necessárias pelo menos duas imagens.\n");
        QCoreApplication::processEvents();
        return;
    }

    if (LimpaTela) {
        ui->txt_Output->setText("    Mesclando imagens.");
    } else {
//        ui->txt_Output->append("    Mesclando imagens.");
    }

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

        ///// Calcular a media e o desvio do GSD
        Media = Media + transform[1];
        Val.push_back(transform[1]);

        Cor_Nodata = pNewDS->GetRasterBand(1)->GetNoDataValue();
/*
        char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());
//        char** papszIter = papszMetadata;

        sprintf(intStr,"[%d/%d] %s (%f,%f)",(k+1),Imagem_Composta.Geo_File_List.length(),Temp.toStdString().c_str(),cy,cx);
        ui->txt_Output->append(intStr);

        sprintf(intStr,"--------------------------------------------------------------------------------------------------");
        ui->txt_Output->append(intStr);
*/
        GDALClose(pNewDS);
    }

    Media = Media / Imagem_Composta.Geo_File_List.length();

    for (int k = 0 ; k < Imagem_Composta.Geo_File_List.length() ; k++) {
        Desvio = Desvio + pow(Val.at(k) - Media,2);
    }
    Desvio = sqrt(Desvio / Imagem_Composta.Geo_File_List.length());

//    IC = 1.96 * Desvio / sqrt(Imagem_Composta.Geo_File_List.length()); // 95% intervalo de confiança
    V_Min = Media * 0.95;
    V_Max = Media * 1.05;

    for (int k = 0 ; k < Imagem_Composta.Geo_File_List.length() ; k++) {
        if ((Val.at(k) < V_Min) || (Val.at(k) > V_Max)) {
            ui->txt_Output->append("\n Imagens com GSD diferentes não podem ser combinadas !");
            ui->txt_Output->append("\n       " + Imagem_Composta.Geo_File_List.at(k).fileName());
            return;
        }
    }

    Scx = (Smaxx - Sminx) /2 + Sminx;
    Scy = (Smaxy - Sminy) /2 + Sminy;

        ui->txt_Output->append("\n Iniciando geração GeoTiff.");
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        timer2.restart();

        // Monta Tiff

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
//               Sleep(3000);

            /*
            QTextEdit   *myTextEdit = ui->txt_Output;
            myTextEdit->moveCursor(QTextCursor::End);
            QColor tc = myTextEdit->textColor();
            myTextEdit->setTextColor(QColor("blue"));
            myTextEdit->insertPlainText(" Ok");
            myTextEdit->setTextColor(tc);
            */


               QString Output;
               Size_GeoTiff(Complete, true, Output);
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

             for (int i = 0 ; i <  lista.length() ; i++) {
//                 cout << lista.at(i).toStdString() << "\n";

                 String S;
                 S = Informacao.DiretorioProc.toStdString() + "/" + lista.at(i).toStdString();
                 sprintf(intStr,"Lendo %s\n",S.c_str());
                 ui->txt_Output->append(intStr);
                 updateTxt();

                 Mat img = cv::imread(S);
                 if (img.empty())
                 {
                     cout << "Can't read image '" << S << "'\n";
                     return;
                 }
       //          cv::resize(img,img,cv::Size(img.cols * 0.3,img.rows * 0.3),0,0);
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
                 cv::imwrite(Complete.toStdString(),pano);

                 cout << "stitching completed successfully\n" << Name.toStdString() << " saved!";
                 sprintf(intStr,"%s",Complete.toStdString().c_str());
                 ui->txt_Output->append(intStr);
                 Elapsed = timer2.elapsed();
                 sprintf(intStr,"Processado em %d segundos.",static_cast<int>(Elapsed/1000));
                 ui->txt_Output->append(intStr);
             }
         }

}


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

    /*
    Lista_Files_Imgs();
    Lista_Files_Proc();
    Lista_Files_Result();
    */
}


void MainWindow::Gera_Merge_Interno(int Cor_NoDATA)
{

//    Cor_NoDATA = 0;

    GDALAllRegister();

    int X,Y;

    char    intStr[100];
    qint64  Elapsed;

    timer2.restart();

    GeographicLib::GeoCoords UL_Geral,RD_Geral;
    double  MinX = 1e10,MinY = 1e10,MaxX = -1e10,MaxY = -1e10;
    double  MinCx = 1e10,MinCy = 1e10;
    double Cx = 0,Cy = 0;

    bool Print = true;

    Informacao.Merge.clear();
    Info_Merge  Inf;
 //////////////////////
    for (int i = 0; i < Imagem_Composta.Geo_File_List.length() ; i++) {
        GDALDataset *pOldDS;
        QString InputFile = Imagem_Composta.Geo_File_List.at(i).absoluteFilePath()   ;
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
            TamX = fabs(Transform[1]);
            TamY = fabs(Transform[5]);
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

        if (minx < MinX) MinX = minx;
        if (miny < MinY) MinY = miny;
        if (maxx > MaxX) MaxX = maxx;
        if (maxy > MaxY) MaxY = maxy;

        if (TamX < MinCx) MinCx = TamX;
        if (TamY < MinCy) MinCy = TamY;

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

    X = fabs(MaxX - MinX) / MinCx;
    Y = fabs(MaxY - MinY) / MinCy;

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

        if (PosY < 0) PosY = 0;
        if (PosX < 0) PosX = 0;

        if (ui->actionTranspar_ncia->isChecked())
            addWeighted(Img_Total(Rect(PosX,PosY,img.cols ,img.rows)),
                        0.5,img,0.5,0.0,Img_Total(Rect(PosX,PosY,img.cols ,img.rows)));
        else
            img.copyTo(Img_Total(Rect(PosX,PosY,img.cols ,img.rows)),MaskImage != Cor_NoDATA);

        ui->progressBar_Mesclar->setValue(i);
    }

    Geo_Img.Cor_Nodata = Cor_NoDATA;

    Imagem_Composta.Altura = ui->txt_Alt_Voo->text().toDouble();
    Imagem_Composta.Num = Imagem_Composta.Geo_File_List.length();
    Imagem_Composta.Media_Ida = Coords.PROC.Media_Ida;
    Imagem_Composta.Media_Volta = Coords.PROC.Media_Volta;
//    Imagem_Composta.Sentido = Sentido;
    Imagem_Composta.Primeira_Ultima_Linha = ui->actionUsar_imagens_da_primeira_e_ltima_linha->isChecked();
    Imagem_Composta.Make = Coords.PROC.Make;
    Imagem_Composta.Model = Coords.PROC.Model;


    MAT2GDAL(Imagem_Composta.Nome,Img_Total,NULL,Geo_Img,&Imagem_Composta);

    // Se tiver, mandar apagar Thumb antiga.
    QFile Old_t(Informacao.DiretorioResult + "/thumb/" + Imagem_Composta.Complete_Name.baseName() + "-thumb.jpg");
    if (Old_t.exists()) {
        Old_t.remove(Informacao.DiretorioResult + "/thumb/" + Imagem_Composta.Complete_Name.baseName() + "-thumb.jpg");
    }

    Elapsed = timer2.elapsed();
    int Tempo = Elapsed / 1000;

    if (Tempo > 0) {
////Colocar cor azul
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("blue"));
        sprintf(intStr," Processado em %d segundos.",Tempo);
        ui->txt_Output->append(intStr);
        ui->txt_Output->setTextColor(tc);
    }

    ui->progressBar_Mesclar->setValue(0);
    ui->progressBar_Mesclar->setEnabled(false);

    Print_Caracteristicas_Mesclagem(Imagem_Composta.Nome,Imagem_Composta.Sentido);
    updateTxt();
}


void MainWindow::Mesclar_Result() {
    int         Num_Elementos,Tamanho,i,Num_Arquivos;
    QString     Complete,File;
    char        intStr[100];


    Num_Arquivos = ui->list_Files_Result->selectedItems().length();
    if (Num_Arquivos < 2) {
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("\n Número de imagens precisa ser >= 2 !");
        ui->txt_Output->setTextColor(tc);
        return;
    }

    Complete = QFileDialog::getSaveFileName(this, tr("Imagem a ser gravada"),Informacao.DiretorioResult,tr("Image (*.tif)"));
    if (Complete.isNull()) return;

    ui->txt_Output->append("\n-------------------") ;
    ui->txt_Output->append(" Mesclando imagens...");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


    timer2.restart();

    Num_Elementos = Num_Arquivos + 2;
    Tamanho = Num_Elementos + 1;

    char **argv4;
    argv4 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

    i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv4[i] = static_cast<char *> (malloc( 100 * sizeof(char)));
    }
    argv4[Num_Elementos] = nullptr;

    strcpy(argv4[0],"gdalwarp.exe");


    if (true) {
        for (i = 1 ; i <= Num_Arquivos ; i++) {
            File = Informacao.DiretorioResult + "/" + Informacao.ListadeArquivos_MergeResult.at(Num_Arquivos-i);
            strcpy(argv4[i],(File.toStdString().c_str()));
        }
    } else {
        for (i = 1 ; i <= Num_Arquivos ; i++) {
            File = Informacao.DiretorioResult + "/" + ui->list_Files_Result->selectedItems().at(i-1)->text();
            strcpy(argv4[i],(File.toStdString().c_str()));
        }
    }

    strcpy(argv4[i],Complete.toStdString().c_str());

    warp_H(Num_Elementos,static_cast<char **>(argv4));

    for (i = (Num_Elementos -1 ) ; i >= 0 ; i --) {
        free(argv4[i]);
    }

    qint64  Elapsed;

    Elapsed = timer2.elapsed();
    sprintf(intStr," Merge gerado em %3.3f segundos.\n",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);
    QString Out;
    Size_GeoTiff(Complete,true,Out);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    Informacao.ListadeArquivos_MergeResult.clear();
    DeSelectAll_Result();

    Lista_Files_Result();
}




void MainWindow::on_pushButton_2_clicked()
{
    QString Name;
    Informacao.ListadeArquivos_MergeResult.clear();
    QFileInfo Info1(Informacao.ListadeArquivos_MergeProc.at(0));
    Name = Informacao.Diretorio + "/" + Info1.completeBaseName() + ".jpg";
    Informacao.ListadeArquivos_MergeResult.push_back(Name);

    QFileInfo Info2(Informacao.ListadeArquivos_MergeProc.at(1));
    Name = Informacao.Diretorio + "/" + Info2.completeBaseName() + ".jpg";
    Informacao.ListadeArquivos_MergeResult.push_back(Name);

    Le_imagem_JPG();

    dialogMesclaDesloca = new Dialog_Mescla_Deslocando(this);
    dialogMesclaDesloca->show();
    dialogMesclaDesloca->setWindowTitle("Mescla deslocando");
    dialogMesclaDesloca->setNumFiles(ui->list_Files_Proc->count());

    connect(dialogMesclaDesloca, SIGNAL(Next()),    this,   SLOT(MergeSequencia()));

    namedWindow("Define angulo", WINDOW_GUI_EXPANDED);
    moveWindow("Define angulo", 20,20);
    ui->actionTranspar_ncia->setChecked(true);

    on_bt_Teste_clicked();
}

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QTime>
#include <QDir>
#include <QMessageBox>
#include <QPixmap>
#include <QInputDialog>
#include <QTranslator>


#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs/legacy/constants_c.h>


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "translate.h"

using namespace std;
using namespace cv;

extern char                 intStr[70];
extern Geometria_Imagem     Geo,GeoProc;
extern bool                 Brazil;
extern DialogPontos*        uidialogPontos;
extern std::string          Name_Imagem1;
extern message_function     MeFu;

extern MainWindow          *Window;
extern Ui::MainWindow*     windowPrincipal;

extern QTranslator          translator;



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
    ui->actionHarley_3->setChecked(false);
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




void MainWindow::on_actionThumbnail_triggered()
{
    if (ui->actionThumbnail->isChecked()) {
        sprintf(intStr,"\n Thumbnails -> ATIVADO.");
        Config.Valores.replace(CONFIG_THUMB,1);
    } else {
       sprintf(intStr,"\n Thumbnails -> DESATIVADO.");
       Config.Valores.replace(CONFIG_THUMB,0);
    }
    ui->txt_Output->append(intStr);

    Grava_Config_File();
}


void MainWindow::on_actionPreenche_Pol_gono_triggered()
{
    if (ui->actionPreenche_Pol_gono->isChecked()) {
        Geo.Preenche = true;
        GeoProc.Preenche = true;
        sprintf(intStr,"\n Preenchimento de Polígono -> ATIVADO.");
        Config.Valores.replace(CONFIG_PREENCHE_POLIGONO,1);
    } else {
        Geo.Preenche = false;
        GeoProc.Preenche = false;
        sprintf(intStr,"\n Preenchimento de Polígono -> DESATIVADO.");
        Config.Valores.replace(CONFIG_PREENCHE_POLIGONO,0);
    }
    MeFu.Draw_Cel();
    ui->txt_Output->append(intStr);

    Grava_Config_File();

}


void MainWindow::on_actionBranco_triggered()
{
    ui->actionBranco->setChecked(true);
    ui->actionPreto->setChecked(false);
    ui->txt_Output->append("\n Cor do NODATA -> BRANCO.");
    Config.Valores.replace(CONFIG_NODATA,1);
    Grava_Config_File();

}


void MainWindow::on_actionPreto_triggered()
{
    ui->actionBranco->setChecked(false);
    ui->actionPreto->setChecked(true);
    ui->txt_Output->append("\n Cor do NODATA -> PRETO.");
    Config.Valores.replace(CONFIG_NODATA,0);
    Grava_Config_File();
}

void MainWindow::on_actionDrone_triggered()
{
    ui->actionDrone->setChecked(true);
    ui->actionGimbal->setChecked(false);
    ui->txt_Output->append("\n Referência de ângulo -> DRONE.");
    Config.Valores.replace(CONFIG_REF,0);
    Grava_Config_File();
}

void MainWindow::on_actionGimbal_triggered()
{
    ui->actionDrone->setChecked(false);
    ui->actionGimbal->setChecked(true);
    ui->txt_Output->append("\n Referência de ângulo -> GIMBAL.");
    Config.Valores.replace(CONFIG_REF,1);
    Grava_Config_File();
}


void MainWindow::on_actionGDAL_triggered()
{
    ui->actionGDAL->setChecked(true);
    ui->actionInterno->setChecked(false);
    ui->txt_Output->append("\n Algoritmo de mesclagem -> GDAL.");
    ui->progressBar_Mesclar->setVisible(false);
    Config.Valores.replace(CONFIG_ALG,0);
    Grava_Config_File();
}

void MainWindow::on_actionInterno_triggered()
{
    ui->actionGDAL->setChecked(false);
    ui->actionInterno->setChecked(true);
    ui->txt_Output->append("\n Algoritmo de mesclagem -> INTERNO.");
    ui->progressBar_Mesclar->setVisible(true);
    Config.Valores.replace(CONFIG_ALG,1);
    Grava_Config_File();
}


void MainWindow::on_actionBordas_na_imagens_processadas_triggered()
{
    if (ui->actionBordas_na_imagens_processadas->isChecked()) {
        ui->txt_Output->append("\n Gerar bordas nas imagens processadas -> ATIVADO.");
        Config.Valores.replace(CONFIG_BORDAS,1);
    } else {
        ui->txt_Output->append("\n Gerar bordas nas imagens processadas -> DESATIVADO.");
        Config.Valores.replace(CONFIG_BORDAS,0);
    }
    Grava_Config_File();
}


void MainWindow::on_actionMover_imagens_triggered()
{
    if (ui->actionMover_imagens->isChecked())     {
        ui->txt_Output->append("\n Mover imagens fora do padrão -> ATIVADO");
        Config.Valores.replace(CONFIG_MOVER,1);
    } else {
        ui->txt_Output->append("\n Mover imagens fora do padrão -> DESATIVADO");
        Config.Valores.replace(CONFIG_MOVER,0);
    }
    Grava_Config_File();
}

void MainWindow::on_actionUsar_imagens_da_primeira_e_ltima_linha_triggered()
{
    if (ui->actionUsar_imagens_da_primeira_e_ltima_linha->isChecked()) {
        ui->txt_Output->append("\n Usar imagens da primeira e última linhas -> ATIVADO");
        Config.Valores.replace(CONFIG_PUL,1);
    } else {
        ui->txt_Output->append("\n Usar imagens da primeira e última linhas -> DESATIVADO");
        Config.Valores.replace(CONFIG_PUL,0);
    }

    Grava_Config_File();

    // Verificar as condições para refazer a seleção na lista PROC
    int Sentido = 0;

    if (ui->actionSendido_nico_1->isChecked()) Sentido += VAI;
    if (ui->actionSentido_unico_2->isChecked()) Sentido += VOLTA;
    if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) {
        Sentido += CURVA;
    }

    Seleciona_Imagens_para_Mesclar(Sentido);
}

void MainWindow::on_actionUsar_imagens_do_drone_em_curva_triggered()
{

    int Sentido = 0;

    if (ui->actionSendido_nico_1->isChecked()) Sentido += VAI;
    if (ui->actionSentido_unico_2->isChecked()) Sentido += VOLTA;

    if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) {
        Sentido += CURVA;
        ui->txt_Output->append("\n Usar imagens do Drone em curva -> ATIVADO");
        Config.Valores.replace(CONFIG_CURVA,1);
    } else {
        ui->txt_Output->append("\n Usar imagens do Drone em curva -> DESATIVADO");
        Config.Valores.replace(CONFIG_CURVA,0);
    }

    Grava_Config_File();

    if (!ui->actionTodas_as_imagens->isChecked())  Seleciona_Imagens_para_Mesclar(Sentido);
}

void MainWindow::on_actionPortugu_s_triggered()
{
    QLocale::setDefault(QLocale::Portuguese);
    ui->actionPortugu_s->setChecked(true);
    ui->actionIngl_s->setChecked(false);
    ui->txt_Output->append("\n Separação milhar -> PORTUGUÊS.");
    Config.Valores.replace(CONFIG_MILHAR,1);
    Grava_Config_File();
    Brazil = true;
}

void MainWindow::on_actionIngl_s_triggered()
{
    QLocale::setDefault(QLocale::English);
    ui->actionPortugu_s->setChecked(false);
    ui->actionIngl_s->setChecked(true);
    ui->txt_Output->append("\n Separação milhar -> INGLÊS.");
    Config.Valores.replace(CONFIG_MILHAR,0);
    Grava_Config_File();
    Brazil = false;
}


void MainWindow::on_actionTodas_as_imagens_triggered()
{
    if (ui->actionTodas_as_imagens->isChecked()) {
        ui->actionSendido_nico_1->setChecked(false);
        ui->actionSentido_unico_2->setChecked(false);
        ui->txt_Output->append("\n Mesclagem de todas as imagens -> ATIVADO");
        DeSelectAll_Proc();
        ui->actionUsar_imagens_da_primeira_e_ltima_linha->setVisible(false);
    }  else {
        ui->actionTodas_as_imagens->setChecked(true);
        DeSelectAll_Proc();
    }

    Config.Valores.replace(CONFIG_TODAS,1);
    Grava_Config_File();

}


void MainWindow::on_actionSendido_nico_1_triggered()
{
    int Sentido = 0;

    Config.Valores.replace(CONFIG_TODAS,0);
    Grava_Config_File();

    if (ui->actionSendido_nico_1->isChecked()) {
        ui->actionUsar_imagens_da_primeira_e_ltima_linha->setVisible(true);

        ui->actionTodas_as_imagens->setChecked(false);
        ui->txt_Output->append("\n Mesclagem de imagens sentido único 1 -> ATIVADO");
        DeSelectAll_Proc();

        Sentido = VAI;
        if (ui->actionSentido_unico_2->isChecked()) Sentido += VOLTA;
        if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) Sentido += CURVA;

        Seleciona_Imagens_para_Mesclar(Sentido);
    }
    if (!ui->actionSendido_nico_1->isChecked()) {
        ui->txt_Output->append("\n Mesclagem de imagens sentido único 1 -> DESATIVADO");
        if (!ui->actionSentido_unico_2->isChecked()) {
            ui->actionTodas_as_imagens->setChecked(true);
            on_actionTodas_as_imagens_triggered();
//            ui->txt_Output->append("\n Mesclagem de todas as imagens -> ATIVADO");
        } else {
            DeSelectAll_Proc();

            Sentido = VOLTA;
            if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) Sentido += CURVA;

            Seleciona_Imagens_para_Mesclar(Sentido);
        }
    }
}


void MainWindow::on_actionSentido_unico_2_triggered()
{
    int Sentido = 0;

    Config.Valores.replace(CONFIG_TODAS,0);
    Grava_Config_File();

    if (ui->actionSentido_unico_2->isChecked()) {
        ui->actionUsar_imagens_da_primeira_e_ltima_linha->setVisible(true);

        ui->actionTodas_as_imagens->setChecked(false);
        ui->txt_Output->append("\n Mesclagem de imagens sentido único 2 -> ATIVADO");
        DeSelectAll_Proc();

        Sentido = VOLTA;
        if (ui->actionSendido_nico_1->isChecked()) Sentido += VAI;
        if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) Sentido += CURVA;

        Seleciona_Imagens_para_Mesclar(Sentido);
    }
    if (!ui->actionSentido_unico_2->isChecked()) {
        ui->txt_Output->append("\n Mesclagem de imagens sentido único 2 -> DESATIVADO");
        if (!ui->actionSendido_nico_1->isChecked()) {
            ui->actionTodas_as_imagens->setChecked(true);
            on_actionTodas_as_imagens_triggered();
            //ui->txt_Output->append("\n Mesclagem de todas as imagens -> ATIVADO");
        } else {
            DeSelectAll_Proc();

            Sentido = VAI;
            if (ui->actionUsar_imagens_do_drone_em_curva->isChecked()) Sentido += CURVA;

            Seleciona_Imagens_para_Mesclar(Sentido);
        }
    }
}





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


void MainWindow::on_actionLimpar_triggered(bool ShowImage)
{
    GeoProc.Pontos.clear();
    GeoProc.PImg.clear();

    if (uidialogPontos != NULL) uidialogPontos->clear();

    // Limpa tabela
    ui->tablePontos->setRowCount(0);
    Calc_Dist();

    ui->lineEdit_Area->setText("");

    if (ShowImage) {
        Destino.copyTo(GeoProc.image);
        imshow(Name_Imagem1,GeoProc.image);
    }
}

void MainWindow::on_Limpa_Quadro_clicked()
{
    ui->txt_Output->clear();
}



void MainWindow::on_pushButton_clicked()
{
  on_actionLimpar_triggered();
}


void MainWindow::on_actionSair_triggered()
{
    qApp->quit();
}



void MainWindow::on_action1280x720_triggered()
{
    if (!ui->action1920x1080->isChecked()) {
        ui->action1280x720->setChecked(true);
        return;
    }

    if (!Load_Widgets_Settings(false)) { // 720p
        ui->action1280x720->setChecked(false);
        return;
    }

    if (ui->action1280x720->isChecked()) {
        ui->action1920x1080->setChecked(false);
    }

//    Geometria_Widgets.savedGeometry = Window->saveGeometry();

//    QSize A = Window->minimumSize();

    this->setMinimumSize(1270,700);
    this->setMaximumSize(1270,700);

    if (Changed_Screen) {
        QRect A;
        A.setHeight(700);
        A.setWidth(1270);
        Changed_Screen = false;
        this->setFixedWidth(1270);
        this->setFixedHeight(700);
    } else {
        this->setGeometry(0,30,1270,700);
    }

//    Load_Widgets_Settings(false); // 720p

    Lista_Files_Proc();
    Lista_Files_Result();

}


void MainWindow::on_action1920x1080_triggered()
{
    if (!ui->action1280x720->isChecked()) {
        ui->action1920x1080->setChecked(true);
        return;
    }

    if (!Load_Widgets_Settings(true)) { // 1080p
        ui->action1920x1080->setChecked(false);
        return;
    }

    if (ui->action1920x1080->isChecked()) {
        ui->action1280x720->setChecked(false);
    }


//    Window->restoreGeometry(Geometria_Widgets.savedGeometry);

    QSize A = Window->minimumSize();

    Window->setMinimumSize(1720,960);
    Window->setMaximumSize(1720,960);
    if (Changed_Screen) {
        QRect A;
        A.setHeight(960);
        A.setWidth(1720);
        Changed_Screen = false;
        Window->setFixedWidth(1720);
        Window->setFixedHeight(960);
        Changed_Screen = false;
    } else {
        Window->setGeometry(0,30,1720,960);
    }

    Load_Widgets_Settings(true); // 1080p

    Lista_Files_Proc();
    Lista_Files_Result();
}


void MainWindow::on_actionPortugu_s_2_triggered()
{
    ui->actionPortugu_s_2->setChecked(true);
    ui->actionIngl_s_2->setChecked(false);

    qApp->removeTranslator(&translator);
    ui->retranslateUi(this);
    QStringList Labels = {"Latitude","Longitude",tr("Distância")};
    ui->tablePontos->setHorizontalHeaderLabels(Labels);

}


void MainWindow::on_actionIngl_s_2_triggered()
{
    QString lang = "English";

    if (lang == "English") {
#ifdef QT_DEBUG
        // Colocar a mensagem de erro se não achar o arquivo de tradução
        if (translator.load("C://QT_Projects//Diminui//670_QT6//english.qm")) {} else {
#else
        if (translator.load(Informacao.Diretorio_EXE + "//english.qm")) {} else {
#endif
            QColor tc = ui->txt_Output->textColor();
            ui->txt_Output->setTextColor(QColor("red"));
            ui->txt_Output->append(tr("\n Não foi possível localizar o aquivo de tradução em [") +
                                   Informacao.Diretorio_EXE + "/english.qm]");
            ui->txt_Output->setTextColor(tc);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//            QMessageBox::information(this,tr("Erro"),tr("Não está contida na imagem de referência"));
            ui->actionPortugu_s_2->setChecked(true);
            ui->actionIngl_s_2->setChecked(false);
            return;
        }
    }

    qApp->installTranslator(&translator);

//    qApp->removeTranslator()
    ui->retranslateUi(this);

    ui->actionPortugu_s_2->setChecked(false);
    ui->actionIngl_s_2->setChecked(true);
    QStringList Labels = {"Latitude","Longitude",tr("Distância")};
    ui->tablePontos->setHorizontalHeaderLabels(Labels);

}


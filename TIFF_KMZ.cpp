#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QTime>
#include <QDir>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;


void MainWindow::on_actionTIFF_KMZ_triggered()
{
   /////////////////////////////////////////////////////////// Escolhe arquivo Geo TIFF
    QString     InputTif,OutputKMZ,Name,Dir;
    QString     NameKMZ,OutTxt;
//    int         No_Data;


    InputTif = QFileDialog::getOpenFileName(this, tr("TIFF Image"),Informacao.Diretorio,tr("Image (*.tif)"));
    QFileInfo CompleteNameTif(InputTif);

    if (InputTif == "") {
        return;
     }

    Informacao.Imagem_Foco = InputTif;

    TifKMZ(true);
}

void MainWindow::TifKMZ(bool Mesmo_Dir, bool Proc)
{
    QString OutTxt,NameKMZ;
    int     No_Data;

    /// Confere se é um arquivo Geo TIFF
      if (!Eh_GeoTiff(Informacao.Imagem_Foco,No_Data)) { // Não é GeoTiff, informa erro e retorna
          OutTxt = "\n Erro!   Arquivo [" + Informacao.Imagem_Foco + "] não é um GEOTIFF.\n";

          ui->txt_Output->append(OutTxt);
          return;
      }

      ui->txt_Output->clear();
      QString Output;
      Size_GeoTiff(Informacao.Imagem_Foco,true,Output);


    ////// Faz a tranformação e salva KMZ
        QFileInfo CompleteNameTif(Informacao.Imagem_Foco);

        if (Mesmo_Dir) {
            NameKMZ = CompleteNameTif.path() + "/" + CompleteNameTif.completeBaseName() + ".kmz";
         } else {
            NameKMZ = Informacao.DiretorioResult + "/" + CompleteNameTif.completeBaseName() + ".kmz";

            //// Copia o thumbnail do TIF
            if (Proc) {
                QString Name_Thumb_Proc = Informacao.DiretorioProc + "/Thumb/" + CompleteNameTif.completeBaseName() + "-thumb.jpg" ;
                QString Name_Thumb_Result = Informacao.DiretorioResult + "/Thumb/" + CompleteNameTif.completeBaseName() + "-thumb.jpg";
                if (QFile::exists(Name_Thumb_Result))
                {
                    QFile::remove(Name_Thumb_Result);
                }
                QFile::copy(Name_Thumb_Proc, Name_Thumb_Result);
            }
         }

        QFile CompleteNameKMZ(NameKMZ);
        if (CompleteNameKMZ.exists()) {

            QMessageBox messageBox(QMessageBox::Question,
                        "Sobrescrever",
                        "Arquivo KMZ existe ! Sobrescrever ?",
                        QMessageBox::Yes | QMessageBox::No,
                        this);
                messageBox.setButtonText(QMessageBox::Yes, "Gravar");
                messageBox.setButtonText(QMessageBox::No, "Cancelar");
                int reply = messageBox.exec();

            if (reply == QMessageBox::Yes) {
                CompleteNameKMZ.remove();
            }

            if(reply == QMessageBox::No)
            {
                OutTxt = "\n Arquivo [" + NameKMZ + "] existe e não será sobrescrito.\n";

                ui->txt_Output->append(OutTxt);
                return;
            }
        }

         timer2.restart();
         OutTxt = "\n Iniciando geração [" + NameKMZ + "]";

         ui->txt_Output->append(OutTxt);

         // gdal_translate -of KMLSUPEROVERLAY -a_nodata 0 -co format=png Name NameKMZ

         mThread1.NameTIF = Informacao.Imagem_Foco;
         mThread1.NameKMZ = NameKMZ;
         mThread1.Cor_Nodata = No_Data;

         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        // timer3.timer->start(2000);

         mThread1.start();




        // Rotina de captura da saída stdout para ter o percentual de progresso
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


void MainWindow::TifKMZProc()
{

    int             Num_itens = ui->list_Files_Proc->selectedItems().length();
    QListWidgetItem *item;

    if (Num_itens == 0) {
        Num_itens = 1;
        ui->list_Files_Proc->currentItem()->setSelected(true);
    }

    item = ui->list_Files_Proc->selectedItems().at(0);

    Informacao.Imagem_Foco = Informacao.DiretorioProc + "/" + item->text();

    TifKMZ(false,true);
//    DeSelectAll_Proc();

//    Lista_Files_Result();
}


void MainWindow::TifKMZResult()
{
    QString Extensao;

//    bool    Zero = false;

    int     Num_Itens = ui->list_Files_Result->selectedItems().size();

    QListWidgetItem *item;

    if (Num_Itens == 0) {
//        Zero = true;
        ui->list_Files_Result->currentItem()->setSelected(true);
        Num_Itens = 1;
    }

    item = ui->list_Files_Result->selectedItems().at(0);

    Informacao.Imagem_Foco = Informacao.DiretorioResult + "/" + item->text();

    Extensao = QFileInfo(Informacao.Imagem_Foco).suffix().toUpper();

    if (Extensao == "KMZ") {
        KMZTiff(false);
    } else {
        TifKMZ(false);
    }

//    actKMZResult->setEnabled(false);
    DeSelectAll_Result();
//    Lista_Files_Result();
}

void MainWindow::KMZTiff(bool Mesmo_Dir)
{
    QString OutTxt;

    QString Geo_Tiff_Name;

    QFileInfo CompleteNameKMZ(Informacao.Imagem_Foco);

    if (Mesmo_Dir) {
        Geo_Tiff_Name = CompleteNameKMZ.path() + "/" + CompleteNameKMZ.completeBaseName() + ".tif";
     } else {
        Geo_Tiff_Name = Informacao.DiretorioResult + "/" + CompleteNameKMZ.completeBaseName() + ".tif";
     }

    QFile CompleteNameTIF(Geo_Tiff_Name);
    if (CompleteNameTIF.exists()) {

        QMessageBox messageBox(QMessageBox::Question,
                    "Sobrescrever",
                    "Arquivo TIF existe ! Sobrescrever ?",
                    QMessageBox::Yes | QMessageBox::No,
                    this);
            messageBox.setButtonText(QMessageBox::Yes, "Gravar");
            messageBox.setButtonText(QMessageBox::No, "Cancelar");
            int reply = messageBox.exec();

        if (reply == QMessageBox::Yes) {
            CompleteNameTIF.remove();
        }

        if(reply == QMessageBox::No)
        {
            OutTxt = "\n Arquivo [" + Geo_Tiff_Name + "] existe e não será sobrescrito.\n";

            ui->txt_Output->append(OutTxt);
            return;
        }
    }

    timer2.restart();

    OutTxt = "\n Iniciando geração [" + Geo_Tiff_Name + "]";

    ui->txt_Output->append(OutTxt);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);


    /////////////////////////////////
    // Aqui começa realmente a rotina
    //
    mThread_Merge.NameKMZ = Informacao.Imagem_Foco;
    mThread_Merge.NameTIF = Geo_Tiff_Name;
    mThread_Merge.Mesmo_Dir = Mesmo_Dir;

    ActiveProgressBarMesclar_EXEC();

    mThread_Merge.start();

    // Rotina de captura da saída stdout para ter o percentual de progresso
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


void MainWindow::on_actionKMZ_TIFF_triggered()
{
    /////////////////////////////////////////////////////////// Escolhe arquivo Geo TIFF
     QString     InputKMZ,OutputTIFF,Name,Dir;
     QString     NameKMZ,OutTxt;
//     int         No_Data;


     InputKMZ = QFileDialog::getOpenFileName(this, tr("KMZ Image"),Informacao.Diretorio,tr("Image (*.kmz)"));
     QFileInfo CompleteNameTif(InputKMZ);

     if (InputKMZ == "") {
         return;
      }

     Informacao.Imagem_Foco = InputKMZ;

     KMZTiff(true);
}

void MainWindow::imprimeFimKMZ(void)
{
    qint64  Elapsed;
    char    intStr[100];

    DeSelectAll_Proc();

    Elapsed = timer2.elapsed();

    QColor tc = ui->txt_Output->textColor();
    ui->txt_Output->setTextColor(QColor("blue"));
    sprintf(intStr," KMZ gerado em %3.3f segundos.",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);
    ui->txt_Output->setTextColor(tc);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    CapturaStdout = false;

    Nao_ProgressBar_Result = true;
    Lista_Files_Result();
    Nao_ProgressBar_Result = false;

    QString Output;
    Size_GeoTiff(mThread1.NameKMZ,false,Output);
    ui->txt_Output->append(Output);

    emit ResetProgressBarMesclar();

}

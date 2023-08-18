#include <QMainWindow>
#include <QString>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "warp.h"

#include "mythread.h"

void MainWindow::mudaRES_Result(){
    QString OutputTiff;
    int     RetornoDialog;

    bool    Zero = false;

    int Num_Itens = ui->list_Files_Result->selectedItems().size();

    QListWidgetItem *item;

    if (Num_Itens == 0) {
        Zero = true;
        ui->list_Files_Result->currentItem()->setSelected(true);
        Num_Itens = 1;
    }

    item = ui->list_Files_Result->selectedItems().at(0);

    Informacao.Imagem_Foco = Informacao.DiretorioResult + "/" + item->text();

    QFileInfo CompleteName(Informacao.Imagem_Foco);

    OutputTiff = CompleteName.path() + "/" + CompleteName.completeBaseName() + "_resize.tif";

//    if (Zero) {
        QString Output;
        Size_GeoTiff(Informacao.Imagem_Foco,true,Output);
//    }

    RetornoDialog = NovoXRes();
    if (RetornoDialog == 0) return;

    Muda_resolucao(RetornoDialog,Informacao.Imagem_Foco,OutputTiff);

//    actRESResult->setEnabled(false);
//    DeSelectAll_Result();
//    Lista_Files_Result();
}


void MainWindow::on_actionMuda_resolucao_triggered()
{
     QString InputTif,OutputTif;
     int     No_Data,RetornoDialog;


     InputTif = QFileDialog::getOpenFileName(this, tr("TIFF Original"),Informacao.Diretorio,tr("Image (*.tif)"));
     QFileInfo CompleteName(InputTif);

     if (InputTif == "") {
         return;
      }

    /// Confere se é um arquivo Geo TIFF
      if (!Eh_GeoTiff(InputTif,No_Data)) { // Não é GeoTiff, informa erro e retorna
          QString OutTxt;
          OutTxt = "\n Erro!   Arquivo [" + InputTif + "] não é um GEOTIFF.\n";

          ui->txt_Output->append(OutTxt);
          return;
      }

      QString Output = "";
      Size_GeoTiff(InputTif, false , Output);

      RetornoDialog = NovoXRes();
      if (RetornoDialog == 0) return;

//      ui->txt_Output->clear();

      ui->txt_Output->setText(Output);

    ui->txt_Path->setText(InputTif);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    OutputTif = CompleteName.path() + "/" + CompleteName.completeBaseName() + "_resize.tif";

    Muda_resolucao(RetornoDialog,InputTif,OutputTif);
}


void MainWindow::Muda_resolucao(int XSize, QString Input, QString NameTIF)
{
    QString OutTxt;
    char    intStr[100];

    timer2.restart();

    char XSize_char[50];

    sprintf(XSize_char,"%d",XSize);

    sprintf(intStr,"\n Alterando resolução.");
    ui->txt_Output->append(intStr);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    mThread_RES.NameInput = Input;
    mThread_RES.NameOutput = NameTIF;
    mThread_RES.XSize = XSize;

    ActiveProgressBarMesclar_EXEC();

    mThread_RES.start();

/*
    QString OutTxt;
    char    intStr[100];

    timer2.restart();

    char XSize_char[50];

    sprintf(XSize_char,"%d",XSize);

    sprintf(intStr,"\n Alterando resolução.");
    ui->txt_Output->append(intStr);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

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
    strcpy(argv3[5],Input.toStdString().c_str());
    strcpy(argv3[6],NameTIF.toStdString().c_str());

    warp_H(Num_Elementos,static_cast<char **>(argv3));

    for (i = (Num_Elementos -1) ; i >= 0 ; i --) {
        free(argv3[i]);
    }

    qint64  Elapsed;

    Elapsed = timer2.elapsed();
    sprintf(intStr," Transformação em %3.3f segundos.\n",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);

    QString Output;
    Size_GeoTiff(NameTIF, true,Output);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
*/
}


int MainWindow::NovoXRes(void) {
    dialogMudaRes = new DialogMudaRes(this);
    int RetornoDialog = dialogMudaRes->exec();

    if (RetornoDialog == QDialog::Rejected) {
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));

        QString OutTxt;
        OutTxt = "\n Erro!   Saida Dialog foi CANCEL.\n";

        ui->txt_Output->append(OutTxt);

        ui->txt_Output->setTextColor(tc);

        return 0;
     }

    return (RetornoDialog);
}


void MainWindow::imprimeFimRES(void)
{
    qint64  Elapsed;
    char   intStr[100];

    Elapsed = timer2.elapsed();

    QColor tc = ui->txt_Output->textColor();
    ui->txt_Output->setTextColor(QColor("blue"));
    sprintf(intStr," Alteração de resolução processada em %3.1f segundos.\n",(Elapsed/1000.0));
    ui->txt_Output->append(intStr);
    ui->txt_Output->setTextColor(tc);

//    AbreArquivo(mThread_ICE_TIF.NameTIF,false,true);

    QString Output;
    Size_GeoTiff(mThread_RES.NameOutput, false, Output);
    ui->txt_Output->append(Output);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    emit ResetProgressBarMesclar();

    DeSelectAll_Result();

    Nao_ProgressBar_Result = true;
    Lista_Files_Result();
    Nao_ProgressBar_Result = false;

    /*
    Nao_ProgressBar_Result = true;
    Lista_Files_Result();
    Nao_ProgressBar_Result = false;
    */
}


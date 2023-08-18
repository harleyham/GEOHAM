#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QDir>
#include <QFileInfo>

#include "mainwindow.h"

#include "ui_mainwindow.h"

void MainWindow::on_txt_Reducao_returnPressed()
{

}

void MainWindow::on_txt_Reducao_editingFinished()
{
    Informacao.Reducao = ui->txt_Reducao->text().toInt();
    if ((Informacao.Reducao < 2) || (Informacao.Reducao > 20)) {
                QMessageBox::information(this,tr("Erro"),"Entre 2 e 20");
                Informacao.Reducao = 3;
                ui->txt_Reducao->setText(QString::number(Informacao.Reducao));
                return;
    }
    ui->txt_Output->append("\n Redução de " + ui->txt_Reducao->text());
    Config.Valores.replace(CONFIG_REDUC,Informacao.Reducao);
    Grava_Config_File();
}


void MainWindow::on_bt_Reduzir_clicked()
{
   QString  newPath,Original,Copia,OriginalThumb,CopiaThumb;
   int      i;
   QString  NumDirS;
   char intStr[100];

    ui->txt_Path->setText(" ");

   for (i = 0 ; i <10 ; i++) {
       NumDirS = QString("%1").arg(i,2,10,QLatin1Char('0'));
       newPath = Informacao.Diretorio + "/Dim" + NumDirS;
       QDir novoDir(newPath);
       if (!novoDir.exists()) {
           novoDir.mkpath(".");
           break;
       }
   }
   dirOpenProc = newPath;
   if (i == 10) {
       QMessageBox::information(this,tr("Erro"),"Já existe diretório !");
       return;
   }


   Informacao.Diretorio = newPath;
   ui->txt_Directory->setText(Informacao.Diretorio);
   ui->bt_ProcessarImgs->setEnabled(true);
   Cria_Dir_Proc();
   Cria_Dir_Result();
   Cria_Dir_Thumb();

   ui->txt_Output->clear();
   for (i = 1 ; i < Informacao.ListadeArquivosJPG.length() ; i += Informacao.Reducao ){
       QFileInfo Info = Informacao.ListadeArquivosJPG.at(i);
       QString Name = Info.fileName();
       QString NameThumb = Info.baseName() + "-thumb.jpg";

       Original = Info.absolutePath()+ "/" + Name;
       Copia = Info.absolutePath() + "/Dim" + NumDirS + "/" + Name;

       OriginalThumb = Info.absolutePath()+ "/Thumb/" + NameThumb;
       CopiaThumb = Info.absolutePath() + "/Dim" + NumDirS + "/Thumb/" + NameThumb;

       sprintf(intStr,"[%d/%d] %s", i , (int) Informacao.ListadeArquivosJPG.length() ,Original.toStdString().c_str());
       ui->txt_Output->append(intStr);
       QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
       QFile::copy(Original,Copia);
       QFile::copy(OriginalThumb,CopiaThumb);
   }
   Lista_Files_Proc();
   Lista_Files_Result();
   Lista_Files_Imgs();
   ui->txt_Dir_Proc->setText((Informacao.DiretorioProc));
}

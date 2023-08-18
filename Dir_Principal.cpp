#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QTime>
#include <QDir>
#include <QMessageBox>
#include <QPixmap>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/stitching.hpp>

extern Geometria_Imagem GeoProc;
extern std::string      Name_Imagem1;

bool MainWindow::Cria_Dir_Proc() {
    QString  newPath;

    bool Novo = false;

    newPath = Informacao.Diretorio + "/Proc";
    QDir novoDir(newPath);
    if (!novoDir.exists()) {
        novoDir.mkpath(".");
        Novo = true;
    }
    Informacao.DiretorioProc = newPath;
    dirOpenProc = Informacao.DiretorioProc;

    //Thumb do Proc
    newPath = Informacao.Diretorio + "/Proc/Thumb";
    QDir novoDir2(newPath);
    if (!novoDir2.exists()) {
        novoDir2.mkpath(".");
    }
    return(Novo);
}


void MainWindow::Cria_Dir_Result() {
    QString  newPath;

    newPath = Informacao.Diretorio + "/Result";
    QDir novoDir(newPath);
    if (!novoDir.exists()) {
        novoDir.mkpath(".");
    }
    Informacao.DiretorioResult = newPath;
    dirOpenResult = Informacao.DiretorioResult;

    //Thumb do Result
    newPath = Informacao.Diretorio + "/Result/Thumb";
    QDir novoDir2(newPath);
    if (!novoDir2.exists()) {
        novoDir2.mkpath(".");
    }
}

void MainWindow::Cria_Dir_Movidas() {
    QString  newPath;

    newPath = Informacao.Diretorio + "/Movidas";
    QDir novoDir(newPath);
    if (!novoDir.exists()) {
        novoDir.mkpath(".");
    }
    Informacao.Diretorio_Movidas = newPath;
//    dirOpenResult = Informacao.DiretorioResult;

/*
    //Thumb do Result
    newPath = Informacao.Diretorio + "/Result/Thumb";
    QDir novoDir2(newPath);
    if (!novoDir2.exists()) {
        novoDir2.mkpath(".");
    }
*/
}

void MainWindow::Cria_Dir_Thumb() {
    QString  newPath;

    newPath = Informacao.Diretorio + "/Thumb";
    QDir novoDir(newPath);
    if (!novoDir.exists()) {
        novoDir.mkpath(".");
    }
    Informacao.DiretorioThumb = newPath;
    dirOpenThumb = Informacao.DiretorioThumb;
}



void MainWindow::on_btOpenDirectory_clicked()
{
//    char            intStr[100];

//    QDir directory;
    if (Informacao.Diretorio == "") {
        dirOpen = QFileDialog::getExistingDirectory(this, tr("Selecione Diretório"),"./");
    } else {
        dirOpen = QFileDialog::getExistingDirectory(this, tr("Selecione Diretório"),Informacao.Diretorio);
    }

//    dirOpen = directory.absolutePath();

    if (dirOpen == "") return;

    if (GeoProc.Pontos.length()) {
        on_pushButton_clicked();
    }

    ui->txt_Nome_Imagem->clear();
    ui->bt_Mostar_Imagem->setEnabled(false);

    //if (!GeoProc.image.empty()) cv::destroyWindow("Imagem");  //Isto funciona
//    cv::destroyAllWindows();

    ui->txt_Path->setText(" ");
    ui->txt_Mesclar_Name->setText(" ");
    ui->txt_Alt_Voo->setText(" ");
    ui->txt_Dir_Proc->setText(" ");
    ui->txt_Directory->setText(" ");
    ui->txt_NumEntries->setText(" ");
    ui->txt_NumEntries_Proc->setText(" ");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    Informacao.Diretorio = dirOpen;
    Informacao2.Diretorio= dirOpen;
    if (Informacao.Diretorio_Img_Original == "") {
        Informacao.Diretorio_Img_Original = dirOpen;
        Informacao2.Diretorio_Img_Original = dirOpen;
    }
    ui->txt_Directory->setText(Informacao.Diretorio);

    ui->txt_Output->setText(tr(" Diretório de imagens do Drone: ") + Informacao.Diretorio);

    ui->list_Files->clear();
    ui->list_Files_Proc->clear();
    ui->list_Files_Result->clear();

    ui->actionDesativado->setChecked(true);
    ui->actionOriginais->setChecked(false);
    ui->actionProcessadas->setChecked(false);

    cv::destroyAllWindows();
    Name_Imagem1 = "";

    Informacao.ListadeArquivos_MergeResult.clear();

    Config.Valores.replace(CONFIG_DIR,1);
    Config.Dir = Informacao.Diretorio.toStdString();
    Grava_Config_File();

    bool Novo;
    Novo = Cria_Dir_Proc();
    Cria_Dir_Result();
    Cria_Dir_Thumb();

    Cria_Dir_Movidas();


    Lista_Files_Imgs(Novo);

    ui->txt_Dir_Proc->setText(Informacao.DiretorioProc);
    Lista_Files_Proc();
    Lista_Files_Result();
    ui->bt_Reduzir->setEnabled(true);
    ui->bt_Dir_Proc->setEnabled(true);
    ui->bt_ProcessarImgs->setEnabled(true);
    ui->bt_limpaProc->setEnabled(true);

/*
    if (Informacao.ListadeArquivosJPG.length()) {
        QFileInfo Info = Informacao.ListadeArquivosJPG.at(0);
        QString Name = Info.absoluteFilePath();

        GDALAllRegister();

        GDALDataset *pOldDS;
        pOldDS = (GDALDataset *) GDALOpen(Name.toStdString().c_str(),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);
        char** papszMetadata = CSLDuplicate(poSrcDS->GetMetadata());

        QStringList stringList;

        DecodeXMP(stringList,poSrcDS);

        double Alt = GetXMPItem(stringList,"RelativeAltitude");

        sprintf(intStr,"%6.2f metros",Alt);
        ui->txt_Alt_Voo->setText(intStr);


        GDALClose(pOldDS);
    }
*/
    if (contextMenu == NULL){
        Cria_Contex_Menu_Imgs_2();
        Cria_Contex_Menu_Proc();
        Cria_Contex_Menu_Result();
    }

    DeSelectAll_Imgs();
    DeSelectAll_Proc();
    DeSelectAll_Result();

    ui->actionUsar->setChecked(false);
    Calculou_Blend = false;
    Calculou_Sobrepor = false;

    // Talves tenha que dar um destroy na janela que tem a imagem de referencia quanto troca de diretório
}


void MainWindow::Cria_Contex_Menu_Imgs()
{
    auto actSelectAllImgs = new QAction(tr("Marca todos"), this);
    auto actDeSelectAllImgs = new QAction(tr("Desmarca todos"), this);
    auto actProcess_1 = new QAction("Processar", this);
    connect(actProcess_1,SIGNAL(triggered()),this,SLOT(processa_Imgs()));
    connect(actSelectAllImgs,SIGNAL(triggered()),this,SLOT(SelectAll_Imgs()));
    connect(actDeSelectAllImgs,SIGNAL(triggered()),this,SLOT(DeSelectAll_Imgs()));
    ui->list_Files->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->list_Files->addActions({ actSelectAllImgs , actDeSelectAllImgs , actProcess_1 });
}

void MainWindow::Cria_Contex_Menu_Imgs_2()
{
    contextMenu = new QMenu();

    actSelectAllImgs = new QAction(tr("Marca todos"), contextMenu);
    actDeSelectAllImgs = new QAction(tr("Desmarca todos"), contextMenu);
    actProcess_1 = new QAction("Processar", contextMenu);
    actInfo = new QAction("Metadados", contextMenu);
    actShowCoord = new QAction("Mostra Coordenadas",contextMenu);
    act_KML = new QAction("Grava KML",contextMenu);

    actShowCoord->setIcon(iconCheckpixOff);

    connect(actProcess_1,SIGNAL(triggered()),this,SLOT(processa_Imgs()));
    connect(actSelectAllImgs,SIGNAL(triggered()),this,SLOT(SelectAll_Imgs()));
    connect(actDeSelectAllImgs,SIGNAL(triggered()),this,SLOT(DeSelectAll_Imgs()));
    connect(actInfo,SIGNAL(triggered()),this,SLOT(get_Info_Img()));
    connect(actShowCoord,SIGNAL(triggered()),this,SLOT(ShowCoord()));
    connect(act_KML,SIGNAL(triggered()),this,SLOT(Grava_KML_Voo()));

    ui->list_Files->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->list_Files->addActions({ actSelectAllImgs , actDeSelectAllImgs , actProcess_1 , actShowCoord , actInfo, act_KML});

    actInfo->setEnabled(true);
}

void MainWindow::Cria_Contex_Menu_Proc()
{
    contextMenuProc = new QMenu();

    actSelectAllProc = new QAction(tr("Marca todos"), contextMenuProc);
    actDeSelectAllProc = new QAction(tr("Desmarca todos"), contextMenuProc);
    actDelete = new QAction("Delete", contextMenuProc);
    actMesclar = new QAction("Mesclar", contextMenuProc);
    actInfoProc = new QAction("Metadados", contextMenuProc);
    actKMZProc = new QAction("TIF->KMZ", contextMenuProc);
    actShowCoordProc = new QAction("Mostra Coordenadas",contextMenuProc);
    act_KML_Proc = new QAction("Grava KML",contextMenuProc);

    actShowCoordProc->setIcon(iconCheckpixOff);

    connect(actDelete,SIGNAL(triggered()),this,SLOT(eraseItemProc()));
    connect(actMesclar,SIGNAL(triggered()),this,SLOT(on_bt_Mesclar_clicked()));
    connect(actSelectAllProc,SIGNAL(triggered()),this,SLOT(SelectAll_Proc()));
    connect(actDeSelectAllProc,SIGNAL(triggered()),this,SLOT(DeSelectAll_Proc()));
    connect(actShowCoordProc,SIGNAL(triggered()),this,SLOT(ShowCoordProc()));
    connect(actInfoProc,SIGNAL(triggered()),this,SLOT(get_Info_Proc()));
    connect(actKMZProc,SIGNAL(triggered()),this,SLOT(TifKMZProc()));
    connect(act_KML_Proc,SIGNAL(triggered()),this,SLOT(Grava_KML_Voo_Proc()));

    ui->list_Files_Proc->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->list_Files_Proc->addActions({ actSelectAllProc , actDeSelectAllProc , actDelete , actMesclar , actShowCoordProc, actInfoProc, actKMZProc, act_KML_Proc });

    actInfoProc->setEnabled(true);
    actKMZProc->setEnabled(true);
}


void MainWindow::Cria_Contex_Menu_Result() {

    contextMenuResult = new QMenu();

    actSelectAllResult = new QAction("Marca todos", this);
    actDeSelectAllResult = new QAction("Desmarca todos", this);
    actDelete_Result = new QAction("Delete", this);
    actKMZResult = new QAction("TIF <-> KMZ", this);
    actRESResult = new QAction("Muda Resolução",this);
    actMesclarResult = new QAction("Mesclar",this);
    actRenameResult = new QAction("Renomear",this);
    actInfoResult = new QAction("Metadados",this);

    connect(actDelete_Result,SIGNAL(triggered()),this,SLOT(eraseItem_Result()));
    connect(actSelectAllResult,SIGNAL(triggered()),this,SLOT(SelectAll_Result()));
    connect(actDeSelectAllResult,SIGNAL(triggered()),this,SLOT(DeSelectAll_Result()));
    connect(actKMZResult,SIGNAL(triggered()),this,SLOT(TifKMZResult()));
    connect(actRESResult,SIGNAL(triggered()),this,SLOT(mudaRES_Result()));
    connect(actMesclarResult,SIGNAL(triggered()),this,SLOT(Mesclar_Result()));
    connect(actRenameResult,SIGNAL(triggered()),this,SLOT(Rename_Result()));
    connect(actInfoResult,SIGNAL(triggered()),this,SLOT(get_Info_Result()));

    ui->list_Files_Result->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->list_Files_Result->addActions({ actSelectAllResult, actDeSelectAllResult , actDelete_Result, actRenameResult, actKMZResult, actRESResult, actInfoResult, actMesclarResult });

    actDelete_Result->setEnabled(true);

    actKMZResult->setEnabled(true);

    actMesclarResult->setEnabled(false);

    actRenameResult->setEnabled(true);

    actRESResult->setEnabled(true);
}

#include <QMainWindow>
#include <QString>
#include <QKeyEvent>
#include <QMessageBox>
#include <QList>
#include <QFile>

#include "dialog_rename.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GDAL_HAM.h"

extern Plot_Coords          Coords;
extern message_function     MeFu;



void MainWindow::Lista_Files_Imgs(bool Novo) {
    int             TSizex = 130;
    char            intStr[100];
    QString         Name;

    GDALAllRegister();

    QDir directory(Informacao.Diretorio);
    QStringList Filters,FiltersJPG;
    Filters << "*.jpg" << "*.tif";
    QFileInfoList infoList = directory.entryInfoList(Filters,QDir::Files | QDir::NoDotAndDotDot);
    Informacao.ListadeArquivos = infoList;

    FiltersJPG << "*.jpg";
    QFileInfoList infoListJPG = directory.entryInfoList(FiltersJPG,QDir::Files | QDir::NoDotAndDotDot);
    Informacao.ListadeArquivosJPG = infoListJPG;

    sprintf(intStr,"%i imagens", (int) Informacao.ListadeArquivosJPG.length());
    ui->txt_NumEntries->setAlignment(Qt::AlignRight);
    ui->txt_NumEntries->setText(intStr);

    ui->list_Files->clear();


    if (infoListJPG.length() == 0) {
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("\n    Não existem imagens JPG neste diretório!");
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        return;
    }

    QElapsedTimer timer1;
    qint64 Elapsed;
    timer1.start();

    sprintf(intStr,"\n Lendo coordenadas dos %d arquivos.", (int) Informacao.ListadeArquivosJPG.length());
    ui->txt_Output->append(intStr);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    /////////////// Pega coordenadas LAT e LON das imagens    
    Coords.IMGS.clear();

    ui->progressBar_Mesclar->setMinimum(0);
    ui->progressBar_Mesclar->setMaximum(Informacao.ListadeArquivos.length());
    ui->progressBar_Mesclar->setValue(0);
    ui->progressBar_Mesclar->setEnabled(true);

    for (int i = 0 ; i < Informacao.ListadeArquivos.length() ; i ++) {
        ui->progressBar_Mesclar->setValue(i+1);

        Name = Informacao.Diretorio + "/" + infoList.at(i).fileName();
        GDALDataset *pOldDS;
        pOldDS = (GDALDataset *) GDALOpen(Name.toStdString().c_str(),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);

        if (i == 0) {
            Coords.IMGS.Model = poSrcDS->GetMetadataItem("EXIF_Model");
            Coords.IMGS.Make = poSrcDS->GetMetadataItem("EXIF_Make");
        }

        ui->progressBar_Mesclar->setValue(i+1);

        Coords.IMGS.FileName.push_back(Name);
        Coords.IMGS.Lon.push_back(getLONfromEXIF(poSrcDS));
        Coords.IMGS.Lat.push_back(getLATfromEXIF(poSrcDS));

        QStringList stringList;
        DecodeXMP(stringList,poSrcDS);

        Coords.IMGS.Alt.push_back(GetXMPItem(stringList,"RelativeAltitude"));

        Coords.IMGS.Gimbal.push_back(GetXMPItem(stringList,"GimbalPitchDegree"));
        if (ui->actionGimbal->isChecked()) {
            Coords.IMGS.Angle.push_back(GetXMPItem(stringList,"GimbalYawDegree"));
        } else {
            Coords.IMGS.Angle.push_back(GetXMPItem(stringList,"FlightYawDegree"));
        }

        GDALClose(pOldDS);
    }

    ui->progressBar_Mesclar->setValue(0);
    ui->progressBar_Mesclar->setEnabled(false);

    Elapsed = timer1.elapsed();
    int Elapsed2 = Elapsed/1000;
    if (Elapsed2 > 2) {
        sprintf(intStr," Ok em %d segundos.",Elapsed2);
    } else {
        sprintf(intStr," Ok.");
    }

    QTextEdit   *myTextEdit = ui->txt_Output;
    myTextEdit->moveCursor(QTextCursor::End);
    QColor tc = myTextEdit->textColor();
    myTextEdit->setTextColor(QColor("blue"));
    myTextEdit->insertPlainText(intStr);
    myTextEdit->setTextColor(tc);
//    ui->txt_Output->append(intStr);


    /////// Se for Parrot retorna daqui
//    if (Coords.IMGS.Make == "Parrot") {
//        return;
//    }

    ////// Aqui fazer a verificação se a altura de voo das fotos é a mesma e se o ângulo do gimbal é -90°
    ///
    ///

    QList <double>  Altitude = Coords.IMGS.Alt;
    std::sort(Altitude.begin(),Altitude.end());

    double          Valor = Altitude.at(Altitude.length() / 2);
    double          Fator = 1.05;

    bool            Gimbal_ERR = false, Alt_ERR = false;

    ui->txt_Alt_Voo->setText(QString::number(Valor));



    if (Coords.IMGS.Make != "Parrot") {
    for (int k = 0 ; k < Coords.IMGS.Alt.length() ; k++) {
        if ((Coords.IMGS.Alt.at(k) < (Valor / Fator)) || (Coords.IMGS.Alt.at(k) > (Valor * Fator))) {
            QString A = "";
            if (!Alt_ERR) {
                Alt_ERR = true;
                QColor tc = ui->txt_Output->textColor();
                ui->txt_Output->setTextColor(QColor("red"));
                A = "\n Imagens com altitude diferente de " + QString::number(Valor) + " metros (+/- 5%)";
                ui->txt_Output->append(A);
                ui->txt_Output->setTextColor(tc);
            }
            A = Coords.IMGS.FileName.at(k) + " Alt = " + QString::number(Coords.IMGS.Alt.at(k));
            ui->txt_Output->append( A );

            if (ui->actionMover_imagens->isChecked()) {
                QString     New;
                QFileInfo   Moved_File(Coords.IMGS.FileName.at(k));
                QFile   file(Coords.IMGS.FileName.at(k));
                New = Moved_File.path() + "/Movidas/" + Moved_File.baseName() + "." + Moved_File.suffix();
                file.rename(Coords.IMGS.FileName.at(k),New);
            }
        }
    }

    for (int k = 0 ; k < Coords.IMGS.Gimbal.length() ; k++) {
        if ((Coords.IMGS.Gimbal.at(k) > -85.0) || (Coords.IMGS.Gimbal.at(k) <  -95.0)) {
            QString A = "";
            if (!Gimbal_ERR) {
                Gimbal_ERR = true;
                A = "\n Imagens com ângulo do gimbal diferente de -90°";
                QColor tc = ui->txt_Output->textColor();
                ui->txt_Output->setTextColor(QColor("red"));
                ui->txt_Output->append(A);
                ui->txt_Output->setTextColor(tc);
            }
            A = Coords.IMGS.FileName.at(k) + " Gimbal angle = " + QString::number(Coords.IMGS.Gimbal.at(k));
            ui->txt_Output->append( A );

            if (ui->actionMover_imagens->isChecked()) {
                QString     New;
                QFileInfo   Moved_File(Coords.IMGS.FileName.at(k));
                QFile   file(Coords.IMGS.FileName.at(k));
                New = Moved_File.path() + "/Movidas/" + Moved_File.baseName() + "." + Moved_File.suffix();
                file.rename(Coords.IMGS.FileName.at(k),New);
            }

        }
    }


    if (Gimbal_ERR || Alt_ERR) {
        QColor tc = ui->txt_Output->textColor();
        ui->txt_Output->setTextColor(QColor("red"));
        ui->txt_Output->append("\n Imagens acima podem causar problema para o processamento.");
        ui->txt_Output->setTextColor(tc);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if (ui->actionMover_imagens->isChecked()) {
            QString     New;
            QFileInfo   Moved_File(Coords.IMGS.FileName.at(0));
            New = Moved_File.path() + "/Movidas/";

            QColor tc = ui->txt_Output->textColor();
            ui->txt_Output->setTextColor(QColor("blue"));
            ui->txt_Output->append("\n Imagens acima movidas para " + New);
            ui->txt_Output->setTextColor(tc);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            Lista_Files_Imgs();
            return;
        }
    }

    }
    /////////////////////////

    timer1.restart();

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    //////////////////////////////////////////////////////////


    if (Coords.IMGS.Make == "Parrot") {
        ui->actionThumbnail->setChecked(false);
    } else {

    }

    if ((ui->actionThumbnail->isChecked()) && (!Novo)) {
        ui->list_Files->setViewMode(QListWidget::IconMode);
        ui->list_Files->setIconSize(QSize(TSizex, TSizex));
//        ui->list_Files->setResizeMode(QListWidget::Adjust);
//      ui->list_Files->setSizeAdjustPolicy(QListWidget::AdjustToContents);

        ui->progressBar_Files->setMaximum(Informacao.ListadeArquivos.length());
        ui->progressBar_Files->setValue(0);
        ui->progressBar_Files->setEnabled(true);
        ui->progressBar_Files->setVisible(true);

/*
        int maxT = omp_get_max_threads();

#pragma omp parallel num_threads(maxT)
        {
#pragma omp for
*/
        for (int i = 0; i < Informacao.ListadeArquivos.length() ; i++) {

            QPixmap pix;

            //extrair Thumbnail
              QString pathFileImageTemp = Informacao.Diretorio + "/Thumb" + "/" + Informacao.ListadeArquivos.at(i).baseName() + "-thumb.jpg";
              QString pathFileImageTemp_P = Informacao.Diretorio + "/Thumb" + "/" + Informacao.ListadeArquivos.at(i).baseName() + "-preview1.jpg";

            QString pathFileImage = Informacao.Diretorio + "/" + Informacao.ListadeArquivos.at(i).fileName();
            QDir dir;
            if(!dir.exists(pathFileImageTemp)) {
                QProcess processThumbnails;
#ifdef QT_DEBUG
                QString command2 = "c:/Dev/Exiv2/bin/Release/exiv2.exe"; //exiv2.exe              // Para Desenvolvimento
#else
                QString command2 = Informacao.Diretorio_EXE + "/exiv2.exe"; //exiv2.exe                // Para Produção
#endif
                QString args2;
                QString pathThumbnailsTemp;
                args2 = '"'+ command2 + '"';
//                args2 = args2 + " -f -et -l " + Informacao.DiretorioThumb + " ";
                args2 = args2 + " -f -ep -l " + Informacao.Diretorio + "/Thumb ";
                args2 = args2 + '"' + pathFileImage + '"';
                processThumbnails.start(args2);
                processThumbnails.waitForFinished(-1);

                //Extrai como preview, renomear para thumb
                if(QFileInfo::exists(pathFileImageTemp_P)) { // Rename para Thumb
                    QDir f(Informacao.Diretorio + "/Thumb");
                    f.rename(pathFileImageTemp_P,pathFileImageTemp);

 /*
                    QDir    Diretorio(Informacao.Diretorio + "/Thumb");
                    QFileInfo   Info(pathFileImageTemp_P);
                    Diretorio.rename(Info.absolutePath(),pathFileImageTemp);
*/
                }

                if(!QFileInfo::exists(pathFileImageTemp)) { // Não conseguiu extrair o Thumb, gerar a partir da imagem
                    pix = MAT2PIX(pathFileImage);
                    pix = pix.scaled(TSizex,TSizex,Qt::KeepAspectRatio);
                    pix.save(pathFileImageTemp,"JPG");
                }

             }
                // Calcula tamanho para ficar com 130 de largura
                pix.load(pathFileImageTemp);

                int h = pix.height();
                int w = pix.width();

                if (h > w) {
                    int x = TSizex * w / h;
                    pix = pix.scaled( x,TSizex, Qt::KeepAspectRatio);
                } else {
                    int y = TSizex * h / w;
                    pix = pix.scaled( TSizex,y, Qt::KeepAspectRatio);
                }

            QListWidgetItem *item2 = new QListWidgetItem(pix,Informacao.ListadeArquivos.at(i).fileName(),ui->list_Files);

            ui->list_Files->addItem(item2);

            ui->progressBar_Files->setValue( i + 1 );
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
/*
    } // pragma for
*/

        Elapsed = timer1.elapsed();
        int Elapsed2 = Elapsed/1000;
        if (Elapsed2 > 2) {
            sprintf(intStr,"\n Thumbnais IMG extraídas em %d segundos.",Elapsed2);
        } else {
            sprintf(intStr,"\n Thumbnais IMG extraídas.");
        }

//        sprintf(intStr,"\n Thumbnais IMG extraídas em %d segundos.",static_cast<int>(Elapsed/1000));
        ui->txt_Output->append(intStr);
    } else {
        ui->list_Files->setViewMode(QListWidget::ListMode);
        for (int i = 0 ; i < Informacao.ListadeArquivos.length() ; i++){
            QFileInfo Info = Informacao.ListadeArquivos.at(i);
            QString Name = Info.fileName();
            ui->list_Files->addItem(Name);
        }

    }

    ui->progressBar_Files->setVisible(false);

    if (Coords.IMGS.Make == "Parrot") return;

    // Calcula a média do ângulo, na ida e na volta
    if (ui->actionM_dia->isChecked()) {
        Coords.IMGS.Media_Ida = 0.0;
        Coords.IMGS.Media_Volta = 0.0;

        int         j = 0,k = 0;

        bool Mesma_Direcao = true;
        bool Direcao_contraria = false;
        int         i = 0;
        double      AngleMin = 20.0;
        for ( ; i < Coords.IMGS.Lat.length() ; i ++) {
            j++;
            Coords.IMGS.Media_Ida += Coords.IMGS.Angle.at(i);
            if (i == 0) AngleMin = 20.0; else AngleMin = 4.0;
            if (fabs(Coords.IMGS.Angle.at(i) - Coords.IMGS.Angle.at(i+1)) > AngleMin) {
                Mesma_Direcao = false;
                break;
            }
        }
        for ( ; i < Coords.IMGS.Lat.length() ; i ++) {

            if (i > (Coords.IMGS.Lat.length() - 4)) break;

            if ((fabs(Coords.IMGS.Angle.at(i) - Coords.IMGS.Angle.at(i+1)) < 4) &&
                (fabs(Coords.IMGS.Angle.at(i+1) - Coords.IMGS.Angle.at(i+2)) < 4) &&
                (fabs(Coords.IMGS.Angle.at(i+2) - Coords.IMGS.Angle.at(i+3)) < 4)) {
                Direcao_contraria = true;
                i = i + 2;
                break;
            }
        }
        for ( ; i < Coords.IMGS.Lat.length() ; i ++) {
            k++;
            Coords.IMGS.Media_Volta += Coords.IMGS.Angle.at(i);
            if (fabs(Coords.IMGS.Angle.at(i) - Coords.IMGS.Angle.at(i+1)) > 4) {
                Direcao_contraria = false;
                break;
            }
        }
        Coords.IMGS.Media_Ida = Coords.IMGS.Media_Ida / j;
        Coords.IMGS.Media_Volta = Coords.IMGS.Media_Volta / k;
    }

    int i = 0;

    int Dir;
    for ( ; i < Coords.IMGS.Lat.length() ; i++) {
        Dir = CURVA;
        if (fabs(Coords.IMGS.Angle.at(i) - Coords.IMGS.Media_Ida) < 10.0 ) {
            Dir = VAI;
        }
        if (fabs(Coords.IMGS.Angle.at(i) - Coords.IMGS.Media_Volta) < 10.0 ) {
            Dir = VOLTA;
        }


        Coords.IMGS.Direcao.push_back(Dir);
    }


    int Ida = 0, Volta = 0, Curvas = 0;

    for (i = 0 ; i < Coords.IMGS.Lat.length() ; i ++) {
        if (Coords.IMGS.Direcao.at(i) == VAI) Ida++;
        if (Coords.IMGS.Direcao.at(i) == VOLTA) Volta++;
        if (Coords.IMGS.Direcao.at(i) == CURVA) Curvas++;
    }

    //////////////  Número de idas e vindas

    Coords.IMGS.Ida = 0;
    Coords.IMGS.Volta = 0;
    Coords.IMGS.Curvas = 0;

    for (int i = 0 ; i < Coords.IMGS.Lat.length() ; i ++) {
        if (Coords.IMGS.Direcao.at(i) == VAI) Coords.IMGS.Ida++;
        if (Coords.IMGS.Direcao.at(i) == VOLTA) Coords.IMGS.Volta++;
        if (Coords.IMGS.Direcao.at(i) == CURVA) Coords.IMGS.Curvas++;
    }


    bool                Ida0 = true;

    int Direcao = Coords.IMGS.Direcao.at(Coords.IMGS.Direcao.size() - 1);  // Será VAI ou VOLTA
    if (Direcao == VAI) Direcao = VOLTA; else Direcao = VAI;


    for (int i = 0 ; i < (Coords.IMGS.Direcao.size() ) ; i++) {
        if (Coords.IMGS.Direcao.at(i) == CURVA) {
            if (Coords.IMGS.Ultimo_Primeira == 0) {
                Coords.IMGS.Ultimo_Primeira = i - 1;
            }
            continue;
        }
        if (Ida0) {
            if (Coords.IMGS.Direcao.at(i) == VOLTA) {
                Ida0 = false;
                Coords.IMGS.Num_Ida++;
                if (Coords.IMGS.Ultimo_Primeira == 0) {
                    Coords.IMGS.Ultimo_Primeira = i - 1;
                }
            }
        } else {
            if (Coords.IMGS.Direcao.at(i) == VAI) {
                Ida0 = true;
                Coords.IMGS.Num_Volta++;
            }
        }
    }

    for (int i = (Coords.IMGS.Direcao.size() - 1) ; i >=0 ; i--) {
        if ((Coords.IMGS.Direcao.at(i) == CURVA) || (Coords.IMGS.Direcao.at(i) == Direcao )) {
            Coords.IMGS.Primeiro_Ultima = i + 1;
            break;
        }
    }



    if (Ida0) {
        Coords.IMGS.Num_Ida++;
    } else {
        Coords.IMGS.Num_Volta++;
    }

    if ( (Coords.IMGS.Num_Ida + Coords.IMGS.Num_Volta) % 2) Coords.IMGS.Par = false;
    else Coords.IMGS.Par = true;

    ui->txt_Output->append("\n    Informações do voo (originais):");

    sprintf(intStr," Ângulo de ida = %4.6f", Coords.IMGS.Media_Ida);
    ui->txt_Output->append(intStr);
    sprintf(intStr," Ângulo de volta = %4.6f", Coords.IMGS.Media_Volta);
    ui->txt_Output->append(intStr);

    sprintf(intStr," Imagens na ida = %d, imagens na volta = %d, imagens nas curvas = %d", Ida,Volta,Curvas);
    ui->txt_Output->append(intStr);

    ui->txt_Output->append(" " + QString::number(Coords.IMGS.Num_Ida) + " idas e " +
                           QString::number(Coords.IMGS.Num_Volta) + " voltas. \n");
    updateTxt();
}

void MainWindow::Lista_Files_Proc() {
    int         TSizex = 130;
    char        intStr[100];
    QString     Name;


    QDir directory(Informacao.DiretorioProc);
    QStringList Filters;
    Filters << "*.jpg" << "*.tif";
    QFileInfoList infoList = directory.entryInfoList(Filters,QDir::Files | QDir::NoDotAndDotDot);
    Informacao.ListadeArquivosProc = infoList;

    int j = infoList.length();
    if (j == 1 ) {
        sprintf(intStr,"%i imagem", j);
     } else {
        sprintf(intStr,"%i imagens", j);
     }
    if (j == 0) {
        ui->bt_Mesclar->setEnabled(false);
        ui->progressBar_Mesclar->setEnabled(false);
    } else {
        ui->bt_Mesclar->setEnabled(true);
        ui->progressBar_Mesclar->setEnabled(true);
    }
    ui->txt_NumEntries_Proc->setAlignment(Qt::AlignRight);
    ui->txt_NumEntries_Proc->setText(intStr);
    ui->list_Files_Proc->clear();

    if (j == 0) return;

    /////////////// Pega coordenadas LAT e LON das imagens
    Coords.PROC.clear();

    double transform[6] = {0,1,0,0,0,1};
//    GeographicLib::GeoCoords    A,D;
    int                         W,H;

    for (int i = 0 ; i < Informacao.ListadeArquivosProc.length() ; i ++) {
        Name = Informacao.DiretorioProc + "/" + infoList.at(i).fileName();
        GDALDataset *pOldDS;
        pOldDS = (GDALDataset *) GDALOpen(Name.toStdString().c_str(),GA_ReadOnly);
        GDALDataset* poSrcDS = GDALDataset::FromHandle(pOldDS);

        poSrcDS->GetGeoTransform(transform);

        H = pOldDS->GetRasterYSize();
        W = pOldDS->GetRasterXSize();

        Coords.PROC.FileName.push_back(Name);
        Coords.PROC.Lat.push_back(getLATfromEXIF(pOldDS));
        Coords.PROC.Lon.push_back(getLONfromEXIF(pOldDS));
        Coords.PROC.Lon_A.push_back(transform[0]);         // Canto Superior Esquerdo
        Coords.PROC.Lat_A.push_back(transform[3]);
        Coords.PROC.Iw.push_back(W);
        Coords.PROC.Ih.push_back(H);
        Coords.PROC.mx.push_back(transform[1]);
        Coords.PROC.my.push_back(transform[5]);
        Coords.PROC.Angle.push_back(getANGLEfromEXIF(poSrcDS));

        if (i == 0) {
            strcpy(intStr,poSrcDS->GetMetadataItem("EXIF_Make"));
            Coords.PROC.Make = intStr;
            strcpy(intStr,poSrcDS->GetMetadataItem("EXIF_Model"));
            Coords.PROC.Model = intStr;
        }

        GDALClose(pOldDS);
    }

    ///////// Copia dados de direção das imagens
    Coords.PROC.Media_Ida = Coords.IMGS.Media_Ida;
    Coords.PROC.Media_Volta = Coords.IMGS.Media_Volta;

    for (int i = 0 ; i < Coords.PROC.FileName.length() ; i++) {
        QFileInfo   CompleteNameProc(Coords.PROC.FileName.at(i));
        QString     NameProc = CompleteNameProc.baseName();

        for (j = 0 ; j < Coords.IMGS.FileName.length() ; j++) {
            QFileInfo   CompleteNameImgs(Coords.IMGS.FileName.at(j));
            QString     NameImgs = CompleteNameImgs.baseName();
            if (NameProc == NameImgs) {
                Coords.PROC.Direcao.push_back(Coords.IMGS.Direcao.at(j));
                if (Coords.PROC.Angle.at(i) == ERROR_ANGLE) {
                    Coords.PROC.Angle.replace(i,Coords.IMGS.Angle.at(j));
                }
                break;
            }
        }
    }


    //////////////  Número de idas e vindas

    Coords.PROC.Ida = 0;
    Coords.PROC.Volta = 0;
    Coords.PROC.Curvas = 0;

    for (int i = 0 ; i < Coords.PROC.Lat.length() ; i ++) {
        if (Coords.PROC.Direcao.at(i) == VAI) Coords.PROC.Ida++;
        if (Coords.PROC.Direcao.at(i) == VOLTA) Coords.PROC.Volta++;
        if (Coords.PROC.Direcao.at(i) == CURVA) Coords.PROC.Curvas++;
    }

//    Coords.PROC.Num_Ida = 1;
    bool                Ida0 = true;

    int Direcao = Coords.PROC.Direcao.at(Coords.PROC.Direcao.size() - 1);  // Será VAI ou VOLTA
    if (Direcao == VAI) Direcao = VOLTA; else Direcao = VAI;

    Coords.PROC.Ultimo_Primeira = 0;
    for (int i = 0 ; i < (Coords.PROC.Direcao.size() ) ; i++) {
        if (Coords.PROC.Direcao.at(i) == CURVA) {
            if (Coords.PROC.Ultimo_Primeira == 0) {
                Coords.PROC.Ultimo_Primeira = i - 1;
            }
            continue;
        }
        if (Ida0) {
            if (Coords.PROC.Direcao.at(i) == VOLTA) {
                Ida0 = false;
                Coords.PROC.Num_Ida++;
                if (Coords.PROC.Ultimo_Primeira == 0) {
                    Coords.PROC.Ultimo_Primeira = i - 1;
                }
            }
        } else {
            if (Coords.PROC.Direcao.at(i) == VAI) {
                Ida0 = true;
                Coords.PROC.Num_Volta++;
            }
        }
    }

    for (int i = (Coords.PROC.Direcao.size() - 1) ; i >=0 ; i--) {
        if ((Coords.PROC.Direcao.at(i) == CURVA) || (Coords.PROC.Direcao.at(i) == Direcao )) {
            Coords.PROC.Primeiro_Ultima = i + 1;
            break;
        }
    }

    if (Ida0) {
        Coords.PROC.Num_Ida++;
    } else {
        Coords.PROC.Num_Volta++;
    }


    if ( (Coords.PROC.Num_Ida + Coords.PROC.Num_Volta) % 2) Coords.PROC.Par = false;
    else Coords.PROC.Par = true;

/*
    ui->txt_Output->append("\n\n " + QString::number(Coords.PROC.Num_Ida) + " idas e " +
                           QString::number(Coords.PROC.Num_Volta) + " voltas. \n");
*/
    ////////////////  Thumbs
    ///
    QElapsedTimer timer1;
    timer1.start();

    if (ui->actionThumbnail->isChecked()) {
        ui->list_Files_Proc->setViewMode(QListWidget::IconMode);
        ui->list_Files_Proc->setIconSize(QSize(TSizex, TSizex));
        ui->progressBar_Proc->setMaximum(Informacao.ListadeArquivosProc.length());
        ui->progressBar_Proc->setValue(0);
        ui->progressBar_Proc->setEnabled(true);
        ui->progressBar_Proc->setVisible(true);
/*
        int maxT = omp_get_max_threads();

#pragma omp parallel num_threads(maxT)
        {
#pragma omp for
*/
        for (int i = 0; i < Informacao.ListadeArquivosProc.length() ; i++) {

            QPixmap pix;
            ui->progressBar_Proc->setValue( i + 1 );

            //extrair Thumbnail
              QString pathFileImageTemp = Informacao.DiretorioProc + "/Thumb" + "/" + Informacao.ListadeArquivosProc.at(i).baseName() + "-thumb.jpg";

            QString pathFileImage = Informacao.DiretorioProc + "/" + Informacao.ListadeArquivosProc.at(i).fileName();
            QDir dir;
            if(!dir.exists(pathFileImageTemp)) {
                QProcess processThumbnails;
#ifdef QT_DEBUG
                QString command2 = "c:/Dev/Exiv2/bin/Release/exiv2.exe"; //exiv2.exe              // Para Desenvolvimento
#else
                QString command2 = Informacao.Diretorio_EXE + "/exiv2.exe"; //exiv2.exe                // Para Produção
#endif
                QString args2;
                QString pathThumbnailsTemp;
                args2 = '"'+ command2 + '"';
//                args2 = args2 + " -f -et -l " + Informacao.DiretorioThumb + " ";
                args2 = args2 + " -f -et -l " + Informacao.DiretorioProc + "/Thumb ";
                args2 = args2 + '"' + pathFileImage + '"';
                processThumbnails.start(args2);
                processThumbnails.waitForFinished(-1);

                if(!QFileInfo::exists(pathFileImageTemp)) { // Não conseguiu extrair o Thumb, gerar a partir da imagem
                    pix = MAT2PIX(pathFileImage);
                    pix = pix.scaled(TSizex,TSizex,Qt::KeepAspectRatio);
                    pix.save(pathFileImageTemp,"JPG");
                }
             }
                // Calcula tamanho para ficar com 130 de largura
                pix.load(pathFileImageTemp);

                int h = pix.height();
                int w = pix.width();

                if (h > w) {
                    int x = TSizex * w / h;
                    pix = pix.scaled( x,TSizex, Qt::KeepAspectRatio);
                } else {
                    int y = TSizex * h / w;
                    pix = pix.scaled( TSizex,y, Qt::KeepAspectRatio);
                }

            QListWidgetItem *item2 = new QListWidgetItem(pix,Informacao.ListadeArquivosProc.at(i).fileName(),ui->list_Files_Proc);

            ui->list_Files_Proc->addItem(item2);

            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
/*
        }
*/
        qint64 Elapsed;
        Elapsed = timer1.elapsed();
        int Elapsed2 = Elapsed/1000;
        if (Elapsed2 > 2) {
            sprintf(intStr,"\n Thumbnais PROC extraídas em %d segundos.",Elapsed2);
        } else {
            sprintf(intStr,"\n Thumbnais PROC extraídas.");
        }

//        sprintf(intStr,"\n Thumbnais PROC extraídas em %d segundos.",static_cast<int>(Elapsed/1000));
        ui->txt_Output->append(intStr);
    } else {

        ui->list_Files_Proc->setViewMode(QListWidget::ListMode);
        for (int i = 0 ; i < Informacao.ListadeArquivosProc.length() ; i++){
            QFileInfo Info = Informacao.ListadeArquivosProc.at(i);
            QString Name = Info.fileName();
            ui->list_Files_Proc->addItem(Name);
        }

    }

    ui->progressBar_Proc->setVisible(false);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    ui->txt_Output->append("\n    Informações do voo (processadas):");

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
                           QString::number(Coords.PROC.Num_Volta) + " voltas. \n");

    sprintf(intStr," Última imagem da primeira linha = %d, primeira imagem da última linha = %d",
                   Coords.PROC.Ultimo_Primeira,Coords.PROC.Primeiro_Ultima);
    ui->txt_Output->append(intStr);

    updateTxt();
}


void MainWindow::Lista_Files_Result() {
    int         TSizex = 130;
    char        intStr[100];

    QString     Extensao;
//    bool        Ok = false;

    QDir directory(Informacao.DiretorioResult);
    QStringList Filters;
    Filters << "*.tif" << "*.kmz";


    QFileInfoList infoList = directory.entryInfoList(Filters,QDir::Files | QDir::NoDotAndDotDot);
    Informacao.ListadeArquivosResult = infoList;

    int j = infoList.length();
    if (j == 1 ) {
        sprintf(intStr,"%i imagem", j);
     } else {
        sprintf(intStr,"%i imagens", j);
     }
    ui->list_Files_Result->clear();

    if (j == 0) return;

    ////////////////  Thumbs
    ///
    QElapsedTimer timer1;
    timer1.start();

    if (ui->actionThumbnail->isChecked()) {

        ui->list_Files_Result->setViewMode(QListWidget::IconMode);
        ui->list_Files_Result->setIconSize(QSize(TSizex, TSizex));

        ui->progressBar_Result->setMaximum(Informacao.ListadeArquivosResult.length());
        ui->progressBar_Result->setValue(0);

        if (!Nao_ProgressBar_Result) ui->progressBar_Result->setVisible(true);

        updateTxt();


/*
        int maxT = omp_get_max_threads();

#pragma omp parallel num_threads(maxT)
        {
#pragma omp for
*/
        for (int i = 0; i < Informacao.ListadeArquivosResult.length() ; i++) {
            bool Ok = false;
            QPixmap pix;

            //extrair Thumbnail
            QString pathFileImageTemp = Informacao.DiretorioResult + "/Thumb" + "/" + Informacao.ListadeArquivosResult.at(i).baseName() + "-thumb.jpg";

            QString pathFileImage = Informacao.DiretorioResult + "/" + Informacao.ListadeArquivosResult.at(i).fileName();

            //////////////////////////////
            // Se for KMZ e tiver um TIF com mesmo nome tentar extrair o thumb do TIF
//            Name = CompleteName.absoluteFilePath();
            Extensao = QFileInfo(pathFileImage).suffix().toUpper();


            if (Extensao == "KMZ") {
                QString Name = Informacao.DiretorioResult +  "/" + Informacao.ListadeArquivosResult.at(i).baseName() + ".tif";
                if (QFile(Name).exists()) {
                    pathFileImage = Name;
                } else if (QFile(pathFileImageTemp).exists()) {
                    Ok = false;
                } else {
                    pix = thumb_Earth;
                    Ok = true;
                }
             }

 //           Ok = false;
            if (!Ok) {
                QDir dir;
                if(!dir.exists(pathFileImageTemp)) {

                    QProcess processThumbnails;
#ifdef QT_DEBUG
                    QString command2 = "c:/Dev/Exiv2/bin/Release/exiv2.exe"; //exiv2.exe              // Para Desenvolvimento
#else
                    QString command2 = Informacao.Diretorio_EXE + "/exiv2.exe"; //exiv2.exe                // Para Produção
#endif
                    QString args2;
                    QString pathThumbnailsTemp;
                    args2 = '"'+ command2 + '"';
                    //                args2 = args2 + " -f -et -l " + Informacao.DiretorioThumb + " ";
                    args2 = args2 + " -f -et -l " + Informacao.DiretorioResult + "/Thumb ";
                    args2 = args2 + '"' + pathFileImage + '"';
                    processThumbnails.start(args2);
                    processThumbnails.waitForFinished(-1);

                    if(!QFileInfo::exists(pathFileImageTemp)) { // Não conseguiu extrair o Thumb, gerar a partir da imagem
                        pix = MAT2PIX(pathFileImage);
                        if (!(pix.data_ptr())) {
                            pix = thumb_Mundo;
                        }
                        pix = pix.scaled(TSizex,TSizex,Qt::KeepAspectRatio);
                        pix.save(pathFileImageTemp,"JPG");
                    }
                }
                // Calcula tamanho para ficar com 130 de largura
                if (pix.load(pathFileImageTemp)) {
                    int h = pix.height();
                    int w = pix.width();
                    if (h > w) {
                        int x = TSizex * w / h;
                        pix = pix.scaled( x,TSizex, Qt::KeepAspectRatio);
                    } else {
                        int y = TSizex * h / w;
                        pix = pix.scaled( TSizex,y, Qt::KeepAspectRatio);
                    }
                }
            }

//                pix = thumb_Earth;

            QListWidgetItem *item2 = new QListWidgetItem(pix,Informacao.ListadeArquivosResult.at(i).fileName(),ui->list_Files_Result);

            ui->list_Files_Result->addItem(item2);

            ui->progressBar_Result->setValue( i + 1 );
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
/*
        }
*/
        qint64 Elapsed;
        Elapsed = timer1.elapsed();
        int Elapsed2 = Elapsed/1000;
        if (Elapsed2 > 2) {
            sprintf(intStr,"\n Thumbnais RESULT extraídas em %d segundos.",Elapsed2);
        } else {
            sprintf(intStr,"\n Thumbnais RESULT extraídas.");
        }

//        sprintf(intStr,"\n Thumbnais RESULT extraídas em %d segundos.",static_cast<int>(Elapsed/1000));
        ui->txt_Output->append(intStr);
    } else {
        ui->list_Files_Result->setViewMode(QListWidget::ListMode);
        for (int i = 0 ; i < Informacao.ListadeArquivosResult.length() ; i++){
            QFileInfo Info = Informacao.ListadeArquivosResult.at(i);
            QString Name = Info.fileName();
            ui->list_Files_Result->addItem(Name);
        }

    }

    ui->progressBar_Result->setVisible(false);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


int MainWindow::keyPressEvent_H (QKeyEvent *event) {
    if (event->key() == Qt::Key_Control) {
        return (Qt::Key_Control);
    }
    return 0;
}

void MainWindow::on_list_Files_Proc_itemClicked(QListWidgetItem *item)
{
    bool CTRL_OK = false;

    int Num_Select;

    if(QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        CTRL_OK = true;

        ui->list_Files_Proc->currentItem()->setSelected(true);

        if (!Informacao.ListadeArquivos_MergeProc.contains(ui->list_Files_Proc->currentItem()->text())) {
            Informacao.ListadeArquivos_MergeProc.prepend(ui->list_Files_Proc->currentItem()->text());
            ui->txt_Output->append("------------------------");
            for (int i = 0 ; i < Informacao.ListadeArquivos_MergeProc.length() ; i++) {
                ui->txt_Output->append(Informacao.ListadeArquivos_MergeProc.at(i));
            }
        }

    } else {
        CTRL_OK = false;

        if (ui->list_Files_Proc->currentItem()->isSelected()) {
            Informacao.Imagem_Foco = Informacao.DiretorioProc + "/" + item->text();
            ui->txt_Path->setText(Informacao.Imagem_Foco);
            QString Output;
            Size_GeoTiff(Informacao.Imagem_Foco,true,Output);
        }

        int Pos = Informacao.ListadeArquivos_MergeProc.indexOf(ui->list_Files_Proc->currentItem()->text());
        if ( Pos != -1) {
            Informacao.ListadeArquivos_MergeProc.removeAt(Pos);

            ui->txt_Output->append("------------------------");
            for (int i = 0 ; i < Informacao.ListadeArquivos_MergeProc.length() ; i++) {
                ui->txt_Output->append(Informacao.ListadeArquivos_MergeProc.at(i));
            }


        }

        ui->list_Files_Proc->currentItem()->setSelected(false);
   }

    Num_Select = ui->list_Files_Proc->selectedItems().length();


    if (Num_Select > 0) {
        actDelete->setEnabled(true);
        ui->Selected_PROC->setVisible(true);
        ui->Selected_PROC->setText(QString::number(Num_Select));
    } else {
        ui->Selected_PROC->setVisible(false);
    }

    if (Num_Select > 1) {
        actMesclar->setEnabled(true);
        actInfoProc->setEnabled(false);
        actKMZProc->setEnabled(false);
    } else {
        actMesclar->setEnabled(false);
    }

    if (Num_Select == 0) {
        actDelete->setEnabled(true);
        actInfoProc->setEnabled(true);
    }

    if (Num_Select == 1) {
        actInfoProc->setEnabled(true);
        actKMZProc->setEnabled(true);
    }

    if (ui->actionProcessadas->isChecked()) {
        MeFu.Draw_Cel();
    }
}

void MainWindow::on_list_Files_Result_itemClicked(QListWidgetItem *item)
{
    bool            CTRL_OK = false;
    int             Num_Select;

    if(QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        CTRL_OK = true;
        ui->list_Files_Result->currentItem()->setSelected(true);

        if (!Informacao.ListadeArquivos_MergeResult.contains(ui->list_Files_Result->currentItem()->text())) {
            Informacao.ListadeArquivos_MergeResult.prepend(ui->list_Files_Result->currentItem()->text());
            ui->txt_Output->append("------------------------");
            for (int i = 0 ; i < Informacao.ListadeArquivos_MergeResult.length() ; i++) {
                ui->txt_Output->append(Informacao.ListadeArquivos_MergeResult.at(i));
            }
        }
    } else {
        CTRL_OK = false;

        if (ui->list_Files_Result->currentItem()->isSelected()) {
            Informacao.Imagem_Foco = Informacao.DiretorioResult + "/" + item->text();
            ui->txt_Path->setText(Informacao.Imagem_Foco);
            QString Output;
            Size_GeoTiff(Informacao.Imagem_Foco,true,Output);
//            Informacao.ListadeArquivos_MergeResult.removeLast();
        }

        int Pos = Informacao.ListadeArquivos_MergeResult.indexOf(ui->list_Files_Result->currentItem()->text());
        if ( Pos != -1) {
            Informacao.ListadeArquivos_MergeResult.removeAt(Pos);

            ui->txt_Output->append("------------------------");
            for (int i = 0 ; i < Informacao.ListadeArquivos_MergeResult.length() ; i++) {
                ui->txt_Output->append(Informacao.ListadeArquivos_MergeResult.at(i));
            }


        }


        ui->list_Files_Result->currentItem()->setSelected(false);
    }

    Num_Select = ui->list_Files_Result->selectedItems().length();

    if (Num_Select == 0) {
        actRenameResult->setEnabled(true);
        actDelete_Result->setEnabled(true);
        actRESResult->setEnabled(true);
        actKMZResult->setEnabled(true);
    }

    if (Num_Select == 1) {
        actKMZResult->setEnabled(true);
        actRESResult->setEnabled(true);
        actRenameResult->setEnabled(true);
    } else {
//        actKMZResult->setEnabled(false);
    }


    if (Num_Select > 0) {
        actDelete_Result->setEnabled(true);
        ui->Selected_RESULT->setVisible(true);
        ui->Selected_RESULT->setText(QString::number(Num_Select));
    } else {
//        actDelete_Result->setEnabled(false);
        ui->Selected_RESULT->setVisible(false);
    }

    if (Num_Select > 1) {
        actMesclarResult->setEnabled(true);
        actRenameResult->setEnabled(false);
        actRESResult->setEnabled(false);
        actKMZResult->setEnabled(false);
    } else {
        actMesclarResult->setEnabled(false);
        actRenameResult->setEnabled(true);
        actRESResult->setEnabled(true);

    }
}

void MainWindow::on_list_Files_itemClicked(QListWidgetItem *item)
{
    bool CTRL_OK = false;

    int Num_Select;

    if(QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        CTRL_OK = true;
    } else {
        CTRL_OK = false;

        if (ui->list_Files->currentItem()->isSelected()) {
            // Mostra informação do arquivo
            Informacao.Imagem_Foco = Informacao.Diretorio + "/" + item->text();
            ui->txt_Path->setText(Informacao.Imagem_Foco);
            QString Output;
            Size_ofJPG(Informacao.Imagem_Foco,true,Output);
        }

        ui->list_Files->currentItem()->setSelected(false);
   }

    Num_Select = ui->list_Files->selectedItems().length();


    if (Num_Select <= 1) {
        actInfo->setEnabled(true);
    } else {
        actInfo->setEnabled(false);
    }

    if (Num_Select > 0) {
        ui->Selected_IMG->setVisible(true);
        ui->Selected_IMG->setText(QString::number(Num_Select));
    } else {
        ui->Selected_IMG->setVisible(false);
    }

    if (ui->actionOriginais->isChecked()) {
        MeFu.Draw_Cel();
    }
}


void MainWindow::on_list_Files_itemDoubleClicked(QListWidgetItem *item)
{
    dirOpen = Informacao.Diretorio + "/" + item->text();
    QFileInfo CompleteName(dirOpen.toStdString().c_str());
    AbreArquivo(CompleteName,true,false);
    Lista_Files_Proc();
    QString Output;
    Size_GeoTiff(dirOpen,true,Output);
    DeSelectAll_Imgs();
    DeSelectAll_Proc();
}

void MainWindow::on_list_Files_Proc_itemDoubleClicked(QListWidgetItem *item)
{
    dirOpenProc = Informacao.DiretorioProc + "/" + item->text();
    QFileInfo CompleteName(dirOpenProc.toStdString().c_str());
    AbreArquivo(CompleteName,true,false);
    QString Output;
    Size_GeoTiff(dirOpenProc, true, Output);
    DeSelectAll_Proc();
}

void MainWindow::on_list_Files_Result_itemDoubleClicked(QListWidgetItem *item)
{
    int No_Data;
    dirOpenResult = Informacao.DiretorioResult + "/" + item->text();
    QFileInfo CompleteName(dirOpenResult.toStdString().c_str());

    // Verifica se é geoTiff
    if (!Eh_GeoTiff(dirOpenResult,No_Data)) { // Não é GeoTiff, informa erro e retorna
        QString OutTxt;
        OutTxt = "\n Erro!   Arquivo [" + dirOpenResult + "] não é um GEOTIFF.\n";

        ui->txt_Output->append(OutTxt);
        return;
    }

    AbreArquivo(CompleteName,true,false,true);
    QString Output;
    Size_GeoTiff(dirOpenResult,true,Output);
}


bool MainWindow::on_bt_limpaProc_clicked()
{
    bool result = true;

    ui->Selected_PROC->setText("0");

    QDir dir(Informacao.DiretorioProc);
    dir.removeRecursively();
    Cria_Dir_Proc();
    Lista_Files_Proc();
//    Lista_Files_Imgs();
//    Lista_Files_Result();

    ui->Selected_PROC->setVisible(false);

    return result;
}

///////////////////////////////////////////////////////////////////////
// Context Menu para apagar arquivos dentro dos Thumbs
void MainWindow::eraseItemProc() {

    bool    Zero = false;

    int Num_Itens = ui->list_Files_Proc->selectedItems().size();

    if (Num_Itens) {
        Zero = false;
    } else {
        Zero = true;
        ui->list_Files_Proc->currentItem()->setSelected(true);
        Num_Itens = 1;
    }

    if (!Pergunta_Delete()) {
        if (Zero) {
            DeSelectAll_Proc();
        }
        return;
    }

    for (int i = 0 ; i < Num_Itens ; i++) {
        QListWidgetItem *item = ui->list_Files_Proc->selectedItems().at(i);
        QString File;
        File = item->text();
        QFile NameFile(Informacao.DiretorioProc + "/" + File);
        if (NameFile.remove()) {
            ui->txt_Output->append("\n  Deletando arquivo " + Informacao.DiretorioProc + "/" + File);
        } else {
            ui->txt_Output->append("\n  Não foi possivel deletar o arquivo " + Informacao.DiretorioProc + "/" + File);
        }

        QFileInfo   NameFileInfo(Informacao.DiretorioProc + "/" + File);
        QString     NameFileThumb = Informacao.DiretorioProc + "/Thumb/" + NameFileInfo.baseName() + "-thumb.jpg";
        QFile       NameFile_T(NameFileThumb);       

        if (NameFile_T.remove()) {
            ui->txt_Output->append("  Deletando arquivo "  + NameFileThumb);
        } else {
            ui->txt_Output->append("  Não foi possivel deletar o arquivo "  + NameFileThumb);
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    DeSelectAll_Proc();

    Lista_Files_Proc();
//    actDelete->setEnabled(false);
    actInfoProc->setEnabled(false);
}

void MainWindow::eraseItem_Result() {


    bool    Zero = false;

    Informacao.ListadeArquivos_MergeResult.clear();

    int Num_Itens = ui->list_Files_Result->selectedItems().size();

    if (Num_Itens) {
//        A = ui->list_Files_Result->currentItem()->text();
        Zero = false;
    } else {
        Zero = true;
        ui->list_Files_Result->currentItem()->setSelected(true);
        Num_Itens = 1;
    }

    if (!Pergunta_Delete()) {
        DeSelectAll_Result();
        return;
    }


    for (int i = 0 ; i < Num_Itens ; i++) {
        QListWidgetItem *item;

/*
        if (Zero) {
            item = ui->list_Files_Result->currentItem()->setSelected();
        } else {
            item = ui->list_Files_Result->selectedItems().at(i);
        }
*/                    item = ui->list_Files_Result->selectedItems().at(i);

        QString File;
        File = Informacao.DiretorioResult + "/" + item->text();
        QFile       NameFile(File);
        QFileInfo   NameFileInfo(File);

        ////////////////// Verifica se tem .tif e .kmz
        bool KMZeTIF = false;
        QString NameKMZ,NameTIF;
        NameKMZ = Informacao.DiretorioResult + "/" + NameFileInfo.completeBaseName() + ".kmz";
        NameTIF = Informacao.DiretorioResult + "/" + NameFileInfo.completeBaseName() + ".tif";

        if (QFile(NameKMZ).exists() && QFile(NameTIF).exists()) {
            KMZeTIF = true;
        }

        if (NameFile.remove()) {
            ui->txt_Output->append("\n  Deletando arquivo " + File);
        } else {
            ui->txt_Output->append("\n  Não foi possivel deletar o arquivo " + File);
        }

        ///////////////// Se tiver .tif e .kmz não deletar a thumb

        if (!KMZeTIF) {
            QString     NameFileThumb = Informacao.DiretorioResult + "/Thumb/" + NameFileInfo.baseName() + "-thumb.jpg";
            QFile       NameFile_T(NameFileThumb);
            if (NameFile_T.remove()) {
                ui->txt_Output->append("  Deletando arquivo "  + NameFileThumb);
            } else {
                ui->txt_Output->append("  Não foi possivel deletar o arquivo "  + NameFileThumb);
            }
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    Lista_Files_Result();
//    actDelete_Result->setEnabled(false);
    DeSelectAll_Result();
}


void MainWindow::SelectAll_Result()
{
    ui->list_Files_Result->selectAll();
    actDelete_Result->setEnabled(true);
    actMesclarResult->setEnabled(true);


    actRenameResult->setEnabled(false);
    actRESResult->setEnabled(false);
    actKMZResult->setEnabled(false);

    ui->Selected_RESULT->setVisible(true);
    ui->Selected_RESULT->setText(QString::number(ui->list_Files_Result->selectedItems().length()));

}

void MainWindow::DeSelectAll_Result()
{
    ui->list_Files_Result->selectedItems().clear();
    ui->list_Files_Result->clearSelection();

    actDelete_Result->setEnabled(true);
    actRESResult->setEnabled(true);
    actRenameResult->setEnabled(true);
    actKMZResult->setEnabled(true);


    ui->Selected_RESULT->setVisible(false);
    Informacao.ListadeArquivos_MergeResult.clear();
    actMesclarResult->setEnabled(false);
}

void MainWindow::SelectAll_Proc()
{
    ui->list_Files_Proc->selectAll();
    actDelete->setEnabled(true);
    actInfoProc->setEnabled(false);
    actMesclar->setEnabled(true);
    actKMZProc->setEnabled(false);

    ui->Selected_PROC->setVisible(true);
    ui->Selected_PROC->setText(QString::number(ui->list_Files_Proc->selectedItems().length()));
    if (ui->actionProcessadas->isChecked()) MeFu.Draw_Cel();
}

void MainWindow::DeSelectAll_Proc()
{
    if (actKMZProc == NULL) return;

    ui->list_Files_Proc->selectedItems().clear();
    ui->list_Files_Proc->clearSelection();
//    actDelete->setEnabled(false);
    actKMZProc->setEnabled(true);
    actInfoProc->setEnabled(true);

    actMesclar->setEnabled(false);


    Informacao.ListadeArquivos_MergeProc.clear();

    ui->Selected_PROC->setVisible(false);
    if (ui->actionProcessadas->isChecked()) MeFu.Draw_Cel();

}

void MainWindow::SelectAll_Imgs()
{
    ui->list_Files->selectAll();

    ui->Selected_IMG->setVisible(true);
    ui->Selected_IMG->setText(QString::number(ui->list_Files->selectedItems().length()));

    if (ui->actionOriginais->isChecked()) MeFu.Draw_Cel();

}

void MainWindow::DeSelectAll_Imgs()
{
    ui->list_Files->selectedItems().clear();
    ui->list_Files->clearSelection();
    ui->Selected_IMG->setVisible(false);

    if (ui->actionOriginais->isChecked()) MeFu.Draw_Cel();
}

bool MainWindow::Pergunta_Delete() {
    QMessageBox messageBox(QMessageBox::Question,
                "Deletar",
                "Deletar arquivos selecionados ?",
                QMessageBox::Yes | QMessageBox::No,
                this);
        messageBox.setButtonText(QMessageBox::Yes, "Deletar");
        messageBox.setButtonText(QMessageBox::No, "Cancelar");
        int reply = messageBox.exec();

    if (reply == QMessageBox::Yes) {
        return true;
    }

    if(reply == QMessageBox::No)
    {
       return false;
    }
    ui->Selected_IMG->setVisible(false);
    return false;
}

void MainWindow::Rename_Result()
{
    Dialog_Rename   *dialogRename;
    QString         NameDialog;


    int Num_Select = ui->list_Files_Result->selectedItems().length();

    if (Num_Select) {
        NameDialog = ui->list_Files_Result->selectedItems().at(0)->text();
    } else {
        NameDialog = ui->list_Files_Result->currentItem()->text();
    }
    QString     NewName = NameDialog;
    QString     Name,OldName,New;


    dialogRename = new Dialog_Rename(this);


    dialogRename->Inicializa(NameDialog);
    int Retorno = dialogRename->exec();

    if (Retorno) {
        New = dialogRename->getName();
        Name = New;
        NewName = Informacao.DiretorioResult + "/" + New;
        OldName = Informacao.DiretorioResult + "/" + NameDialog;

        if (QFile::rename(OldName,NewName) == false) {
            QMessageBox::information(this,tr("Erro"),"Não foi possível renomear o arqivo.");
            return;
        }

        New = NameDialog.section('.',0,0);
        OldName = Informacao.DiretorioResult + "/Thumb/" + New + "-thumb.jpg";

        New = Name.section('.',0,0);
        NewName = Informacao.DiretorioResult + "/Thumb/" + New + "-thumb.jpg";

        QFile::rename(OldName,NewName);

        Lista_Files_Result();
    } else {
        ui->txt_Output->append(Name);
    }
}

void MainWindow::get_Info_Proc() {

    int Num_itens = ui->list_Files_Proc->selectedItems().length();


    if (ui->list_Files_Proc->selectedItems().length() > 1) {
        return;
    }

    QListWidgetItem *item;

    if (Num_itens == 0) {
        ui->list_Files_Proc->currentItem()->setSelected(true);
        Num_itens = 1;
    }

    item = ui->list_Files_Proc->selectedItems().at(0);
    getEXIF_XML_Info(Informacao.DiretorioProc + "/" + item->text());

    QString Output;
    Size_GeoTiff(Informacao.DiretorioProc + "/" + item->text(),false,Output);
    Geometria_Img.ExifXMPInfo = Geometria_Img.ExifXMPInfo + Output;
    dialogEXIF  = new DialogEXIF(this);
    dialogEXIF->show();
    dialogEXIF->setWindowTitle("EXIF-XMP information");
    dialogEXIF->print(Geometria_Img.ExifXMPInfo);

    ui->actionEXIF_XMP->setEnabled(true);

    DeSelectAll_Proc();
}

void MainWindow::get_Info_Img() {

    int                 Num_Itens = ui->list_Files->selectedItems().length();

    QListWidgetItem     *item;

    if ( Num_Itens > 1 ) {
        return;
    }

    if (Num_Itens == 0) {
        Num_Itens = 1;
        ui->list_Files->currentItem()->setSelected(true);
    }


    item = ui->list_Files->selectedItems().at(0);
    getEXIF_XML_Info(Informacao.Diretorio + "/" + item->text());

    dialogEXIF  = new DialogEXIF(this);
    dialogEXIF->show();
    dialogEXIF->setWindowTitle("EXIF-XMP information");
    dialogEXIF->print(Geometria_Img.ExifXMPInfo);

    ui->actionEXIF_XMP->setEnabled(true);

    DeSelectAll_Imgs();
}

void MainWindow::get_Info_Result() {

    int Num_itens = ui->list_Files_Result->selectedItems().length();


    if (ui->list_Files_Result->selectedItems().length() > 1) {
        return;
    }

    QListWidgetItem *item;

    if (Num_itens == 0) {
        ui->list_Files_Result->currentItem()->setSelected(true);
        Num_itens = 1;
    }

    item = ui->list_Files_Result->selectedItems().at(0);
    getEXIF_XML_Info(Informacao.DiretorioResult + "/" + item->text());

    QString Output;
    Size_GeoTiff(Informacao.DiretorioResult + "/" + item->text(),false,Output);
    Geometria_Img.ExifXMPInfo = Geometria_Img.ExifXMPInfo + Output;
    dialogEXIF  = new DialogEXIF(this);
    dialogEXIF->show();
    dialogEXIF->setWindowTitle("EXIF-XMP information");
    dialogEXIF->print(Geometria_Img.ExifXMPInfo);

    ui->actionEXIF_XMP->setEnabled(true);

    DeSelectAll_Result();

}


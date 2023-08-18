#define NOMINMAX

//#include "kml.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <QtGui>
#include <QDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QVector>

#include <opencv2/highgui/highgui.hpp>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

//extern      std::vector<Conteudo_Frame> Dados;
extern      Geometria_Imagem            GeoProc;
extern      int                         FusoAtual;
extern      double                      Declinacao;
extern      message_function            MeFu;
extern      DialogPontos*               uidialogPontos;
extern      Ui::MainWindow*             windowPrincipal;
extern      Plot_Coords                 Coords;



// Para o KML, a ordem é LON,LAT,ALT

void MainWindow::Le_KML(QWidget *parent) {

    QString             dirOpen;

//    QString             defaultFilter("KML (*.KML)");
    QString             Filters("Shapes (*.KML *.SHP)");
    dirOpen = QFileDialog::getOpenFileName(parent,parent->tr("Abrir KML"),"./",Filters);
    if (dirOpen == "") return;


    Le_KML_FromFile(dirOpen);

}

void MainWindow::Le_KML_FromFile(QString Filename) {

    QString Data_Line = "";

    // Verifica e é KML ou SHP
    QString     Extensao;
    QFileInfo   Name(Filename);
    QString     Name_KML;
    Extensao = QFileInfo(Filename).suffix().toUpper();


    ui->txt_Output->append("\n------------------------\n");
    QFile   inputFile;
    if (Extensao == "KML") {
        ui->txt_Output->append((" Lendo arquivo KML " + Filename + "\n"));
        inputFile.setFileName(Filename);
    } else {
        Name_KML = Name.path() + "/" + Name.completeBaseName() + ".kml";
        ui->txt_Output->append((" Lendo arquivo SHP " + Filename + "\n"));
        inputFile.setFileName(Name_KML);
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    if (Extensao == "SHP") {
        QProcess process;
#ifdef QT_DEBUG
               QString command2 = "c:/Dev/gdal/bin/ogr2ogr.exe"; //exiv2.exe                            // Para Desenvolvimento
#else
               QString command2 = Informacao.Diretorio_EXE + "/ogr2ogr.exe"; //exiv2.exe                // Para Produção
#endif
               QString args2;
               args2 = command2 + " " + Name_KML + " " + Filename;

               process.start(args2);
               process.waitForFinished(-1);

               QString out, err;
               out = process.readAllStandardOutput();
               err = process.readAllStandardError();
/*
               if ((out != "")) {
                   ui->txt_Output->append(" ogr2ogr -> " + out);
               }
               if ((err != "")) {
                   ui->txt_Output->append(" Erro ogr2ogr -> " + err);
               }
*/
               if (!(QFile(Name_KML).exists())) {
                   char intStr[100];
                   sprintf(intStr,"\n Erro na leitura do arquivo [%s].",Name.fileName().toStdString().c_str());
                   QColor tc = ui->txt_Output->textColor();
                   ui->txt_Output->setTextColor(QColor("red"));
                   ui->txt_Output->append(intStr);
                   ui->txt_Output->setTextColor(tc);
                   QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

                   QMessageBox::critical(this,tr("Erro"),intStr);
//                   QMessageBox::information(this,tr("Erro"),intStr);

                   return;
               }
    }


    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        QString line;

// Varre o arquivo e monta um string com desde <Polygon> ate </Polygon>
        bool            Polygon_Ok = false;
        QString         St1,St2;
        QString         St3,St4;

        St1 = "<Polygon>";
        St2 = "</Polygon>";
        St3 = "<coordinates>";
        St4 = "</coordinates>";

        while (!in.atEnd()) {
            QString line2;
            int Pos1,Pos2;
            line = in.readLine();
            if (line.contains(St1,Qt::CaseInsensitive)) { // Acha onde está a informação de cooredenas do polígono
                if (line.contains(St2,Qt::CaseInsensitive)) {
                    Pos1 = line.indexOf(St3,0,Qt::CaseInsensitive);
                    Pos2 = line.indexOf(St4,0,Qt::CaseInsensitive);
                    Data_Line = line.mid(Pos1 + St3.length(),(Pos2 - Pos1 - St4.length() + 1));
//                    ui->txt_Output->setText(Data_Line);
//                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                } else {
                    do {
                        if (in.atEnd()) break;
                        line = in.readLine();
                    } while (!line.contains("<coordinates>")); // Existem casos onde na linha que contem <cordinates> tem dados a serem lidos

                    if (line.contains(St3) && line.contains(St4)) {
                        Pos1 = line.indexOf(St3,0,Qt::CaseInsensitive);
                        Pos2 = line.indexOf(St4,0,Qt::CaseInsensitive);
                        Data_Line = line.mid(Pos1 + St3.length(),(Pos2 - Pos1 - St4.length() + 1));
                        Polygon_Ok = true;
                        break;
                    }

                    do {
                        if (in.atEnd()) break;
                        line = in.readLine();
                        if (!line.contains("</coordinates>")) {
                            Data_Line = Data_Line + line;
                        } else {
                            Polygon_Ok = true;
                            break;
                        }
                    } while(true);
                }
             }
            if (Polygon_Ok) {
                break;
            }
        }

        if (Data_Line != "") {

            double X,Y;
            double Px,Py,MinX,MinY,MaxX,MaxY;
            MinX = GeoProc.AUTM.Longitude();
            MaxY = GeoProc.AUTM.Latitude();
            MaxX = GeoProc.DUTM.Longitude();
            MinY = GeoProc.DUTM.Latitude();

            Px =  (GeoProc.Iw - 1) / fabs(MaxX - MinX);
            Py =  (GeoProc.Ih - 1) / fabs(MaxY - MinY);

//            ui->txt_Output->append(Data_Line);

            if (GeoProc.Pontos.length()) on_actionLimpar_triggered();
            QStringList list = Data_Line.split(QRegularExpression("\\s+")); // Separa em uma lista o conjunto LAT,LON,ALT
            for (int i = 0; i < list.length() ; i++) {

             ui->progressBar_Mesclar->setValue(i);
             QCoreApplication::processEvents(QEventLoop::AllEvents, 100);



              QStringList list2 = list.at((i)).split(QRegularExpression(",")); // Separa os valores
              std::complex<double>    P;
              // Coloca as coordenadas na lista Pontos
              P.real(list2.at(1).toDouble()); // Latitude
              P.imag(list2.at(0).toDouble());
              GeoProc.Pontos.push_back(P);

              // Coloca os valores x,y na lista PImg
/*
              X = (P.real() - MinX) * Px;
              Y = (MaxY - P.imag()) * Py;
*/

              GeoProc.CcUTM.Reset(P.real(),P.imag(),GeoProc.AUTM.Zone());

              X = ( ( GeoProc.CcUTM.Easting() - GeoProc.AUTM.Easting() ) / GeoProc.mppx );
              Y = -1.0 * ( ( GeoProc.AUTM.Northing() - GeoProc.CcUTM.Northing() ) / GeoProc.mppy );

              P.real((X));
              P.imag((Y));
              GeoProc.PImg.push_back(P);
            }
        } else {
                char intStr[100];
                sprintf(intStr,"\n Arquivo não possui dados compatíveis -> [%s].",Name.fileName().toStdString().c_str());
                QColor tc = ui->txt_Output->textColor();
                ui->txt_Output->setTextColor(QColor("red"));
                ui->txt_Output->append(intStr);
                ui->txt_Output->setTextColor(tc);
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                QMessageBox::critical(this,tr("Erro"),intStr);
                return;

        }

        // Verifica se na lista de pontos o primeiro é igual ao último
        if ( GeoProc.Pontos.at(0).real() == GeoProc.Pontos.at(GeoProc.Pontos.length() - 1).real()  ) {
           GeoProc.Pontos.removeLast();
           GeoProc.PImg.removeLast();
        }

        // Reescreve a tabela
        int             x,y;
        double          PosLat,PosLon;
        char            intStr[100];
        QString         S1,S2;
        QLocale         Br;
        uidialogPontos->clear();

        ui->txt_Output->append("\n Populando tabela");
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        ui->progressBar_Mesclar->setVisible(true);
        ui->progressBar_Mesclar->setEnabled(true);
        ui->progressBar_Mesclar->setMinimum(0);
        ui->progressBar_Mesclar->setMaximum(GeoProc.Pontos.length()-1);
        ui->progressBar_Mesclar->setValue(0);


            for ( int i = 0 ; i <GeoProc.Pontos.length() ; i++) {

                ui->progressBar_Mesclar->setValue(i);

                y = GeoProc.PImg.at(i).real();
                x = GeoProc.PImg.at(i).imag();
                PosLat = GeoProc.AUTM.Northing() + y * GeoProc.mppy;
                PosLon = GeoProc.AUTM.Easting() + x * GeoProc.mppx;  // Eixo x é longitude

                GeoProc.CcUTM.Reset(GeoProc.Pontos.at(i).real(),GeoProc.Pontos.at(i).imag());
//                GeoProc.CcUTM.Reset(GeoProc.AUTM.Zone(),GeoProc.AUTM.Northp(),PosLon,PosLat);

                S2 = Br.toString(GeoProc.Pontos.at(i).imag(),'f',6);
                S1 = Br.toString(GeoProc.Pontos.at(i).real(),'f',6);
                sprintf(intStr,"Coordenada (%s ; %s) (%s)",S1.toStdString().c_str(),S2.toStdString().c_str(),GeoProc.CcUTM.DMSRepresentation().c_str());
                uidialogPontos->Print(intStr);

                MeFu.Table1(windowPrincipal,(i+1),true);
             }
        MeFu.Draw_Cel();
    }
}


void MainWindow::on_actionSave_KML_triggered()
{
/*
    if (GeoProc.Alt == 0) {
        QStringList list = ui->txt_Alt_Voo->text().split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if ( list.length() != 0 ) GeoProc.Alt = list.at(0).toDouble();
    }
*/
    Grava_KML(false,false,false,true,false,true,0,GeoProc.Pontos);
}


void MainWindow::on_actionLoad_KML_triggered()
{
    Le_KML(this);
}


void MainWindow::Grava_KML(bool Track,bool Extrude,bool Shadow,bool Pontos,bool Tempo, bool Poligono, double Altitude, QList <std::complex <double> > Pontos_Coords, QString Dir) {
    int                 i;

    std::ofstream       text_file;
    std::ostringstream  Data;
    string              KML_file;


    dirOpen = QFileDialog::getSaveFileName(this,this->tr("Save KML"),Dir,this->tr("KML (*.KML)"));
    if (dirOpen == "") return;

    KML_file = dirOpen.toStdString().c_str();

    text_file.open(KML_file,ios::out);
    text_file << setprecision(9);

    ui->txt_Output->append("\n------------------------\n");
    ui->txt_Output->append((" Gravando arquivo KML " + dirOpen + "\n"));


    // Escreve os headers de um arquivo KML
    text_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
    text_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\"" << "\n";
    text_file << "xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">" << "\n";

    text_file << "<Document>" << "\n";
    text_file << "\t" << "<name>" << KML_file << "</name>" << "\n";
    text_file << "\t" << "<description>" << "Geo_HAM" << "</description>" << "\n";

    if (Track) {
        text_file << "<Placemark>" << "\n";
        text_file << "\t" << "<name>TRACK</name>" << "\n";
        text_file << "\t" << "<Style>" << "\n";
        text_file << "\t\t" << "<LineStyle>" << "\n";
        text_file << "\t\t\t" << "<color>7f00ffff</color>" << "\n"; // Cor da linha (aabbggrr), onde aa=transparencia
        text_file << "\t\t\t" << "<width>2</width>" << "\n";  // Espessura
        text_file << "\t\t" << "</LineStyle>" << "\n";
        text_file << "\t\t" << "<PolyStyle>" << "\n";
        text_file << "\t\t\t" << "<color>2F0000ff</color>" << "\n";
        text_file << "\t\t" << "</PolyStyle>" << "\n";
        text_file << "\t" << "</Style>" << "\n";
        text_file << "\t" << "<LineString>" << "\n";
        text_file << "\t\t" << "<tessellate>1</tessellate>" << "\n";

        if (Extrude) {
            text_file << "\t\t" << "<extrude>1</extrude>" << "\n";  // Faz uma linha entre o ponto e o solo
        } else {
            text_file << "\t\t" << "<extrude>0</extrude>" << "\n";  // Faz uma linha entre o ponto e o solo
        }


        text_file << "\t\t"	<< "<altitudeMode>relativeToGround</altitudeMode>" << "\n";
        text_file << "\t\t" << "<coordinates>" << "\n";

        // Loop para gravar as coordenadas com altitude
        text_file.precision(9);

        text_file << "\t\t" << Pontos_Coords.at((0)).imag() << "," << Pontos_Coords.at((0)).real() << "," << Altitude << " ";
        for (i = 1 ; i < Pontos_Coords.length() ; i ++) {
            if ( (i % 3) == 0 ) text_file << "\n\t\t";
            text_file << Pontos_Coords.at((i)).imag() << "," << Pontos_Coords.at((i)).real() << "," << Altitude << " ";
        }

        text_file << "\n\t\t" << "</coordinates>" << "\n";
        text_file << "\t" << "</LineString>" << "\n";
        text_file << "</Placemark>" << "\n";
    }


    if (Shadow) {
        // Gravar a linha na altura do solo (shadow)
        text_file << "\n" << "<Placemark>" << "\n";
        text_file << "\t" << "<name>SHADOW</name>" << "\n";
        text_file << "\t" << "<Style>" << "\n";
        text_file << "\t\t" << "<LineStyle>" << "\n";
        text_file << "\t\t\t" << "<color>80003300</color>" << "\n"; // Cor da linha (aabbggrr), onde aa=transparencia
        text_file << "\t\t\t" << "<width>3</width>" << "\n";  // Espessura
        text_file << "\t\t" << "</LineStyle>" << "\n";
        text_file << "\t" << "</Style>" << "\n";
        text_file << "\t" << "<LineString>" << "\n";
        text_file << "\t\t" << "<tessellate>0</tessellate>" << "\n";
        text_file << "\t\t"	<< "<altitudeMode>clampToGround</altitudeMode>" << "\n";
        text_file << "\t\t" << "<coordinates>" << "\n";

        text_file.precision(9);

        text_file << "\t\t" << Pontos_Coords.at((0)).imag() << "," << Pontos_Coords.at((0)).real() << "," << Altitude << " ";
        for (i = 1 ; i < Pontos_Coords.length(); i ++) {
            if ( (i % 3) == 0 ) text_file << "\n\t\t";
            text_file << Pontos_Coords.at((i)).imag() << "," << Pontos_Coords.at((i)).real() << "," << Altitude << " ";
        }

        text_file << "\n\t\t" << "</coordinates>" << "\n";
        text_file << "\t" << "</LineString>" << "\n";
        text_file << "</Placemark>" << "\n";
    }


    if (Pontos) {
        text_file << "\n" << "<Folder>" << "\n";
        text_file << "\t" << "<name>Pontos</name>" << "\n";

        // Primeiro ponto - Icone de cor verde
        text_file << "<Placemark>" << "\n";
        text_file << "\t" << "<Style>" << "\n";
        text_file << "\t" << "<IconStyle>" << "\n";
        text_file << "\t\t" << "<Icon>" << "\n";
        text_file << "\t\t\t" << "<href>http://maps.google.com/mapfiles/kml/pushpin/grn-pushpin.png</href>" << "\n";
        text_file << "\t\t" << "</Icon>" << "\n";
        text_file << "\t" << "</IconStyle>" << "\n";
        text_file << "\t" << "</Style>" << "\n";

/*
        text_file << "\t" << "<TimeStamp>" << "\n";

        Indice = &(Dados[0]);
        Converte_Data_KML(Indice,&Data);
        text_file << "\t\t" << "<when>" << Data.str() << "</when>" << "\n";

        text_file << "\t" << "</TimeStamp>" << "\n";
*/
        text_file << "\t" << "<Point>" << "\n";
        text_file << "\t\t" << "<altitudeMode>clampedToGround</altitudeMode>\n";
        text_file << "\t\t" << "<coordinates>" << Pontos_Coords.at((0)).imag() << "," << Pontos_Coords.at((0)).real() << "," << Altitude << " </coordinates>\n";

// Heading
        text_file << "\t\t" << "<heading>" << (float)90.0 << " </heading>\n";


        text_file << "\t" << "</Point>" << "\n";
        text_file << "</Placemark>" << "\n";


        // A partir do segundo ponto até o penúltimo - Icone redondo
        for (i = 1 ; i < (Pontos_Coords.length() - 1); i++) {
            text_file << "<Placemark>" << "\n";
            text_file << "\t" << "<Style>" << "\n";
            text_file << "\t" << "<IconStyle>" << "\n";
            text_file << "\t\t" << "<Icon>" << "\n";
            text_file << "\t\t\t" << "<href>http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png</href>" << "\n";
            text_file << "\t\t" << "</Icon>" << "\n";
            text_file << "\t" << "</IconStyle>" << "\n";
            text_file << "\t" << "</Style>" << "\n";

/*
            text_file << "\t" << "<TimeStamp>" << "\n";

            Converte_Data_KML(Indice,&Data);
            text_file << "\t\t" << "<when>" << Data.str() << "</when>" << "\n";

            text_file << "\t" << "</TimeStamp>" << "\n";
*/
            //Heading
                    text_file << "\t" << "<LookAt>" << "\n";
                    text_file << "\t\t" << "<altitudeMode>relativeToGround</altitudeMode>\n";
                    text_file << "\t\t" << "<longitude>" << Pontos_Coords.at((i)).imag() << "</longitude>\n";
                    text_file << "\t\t" << "<latitude>" << Pontos_Coords.at((i)).real() << "</latitude>\n";
                    text_file << "\t\t" << "<altitude>" << Altitude << "</altitude>\n";
                    text_file << "\t\t" << "<heading>" << (i * 360.0 / Pontos_Coords.length() ) << "</heading>\n";
                    text_file << "\t" << "</LookAt>" << "\n";


            text_file << "\t" << "<Point>" << "\n";
            text_file << "\t\t" << "<altitudeMode>clampedToGround</altitudeMode>\n";
            text_file << "\t\t" << "<coordinates>" << Pontos_Coords.at((i)).imag() << "," << Pontos_Coords.at((i)).real() << "," << Altitude << " </coordinates>\n";

            text_file << "\t" << "</Point>" << "\n";
            text_file << "</Placemark>" << "\n";
        }

        // Ultimo ponto - Icone de cor vermelha
        text_file << "<Placemark>" << "\n";
        text_file << "\t" << "<Style>" << "\n";
        text_file << "\t" << "<IconStyle>" << "\n";
        text_file << "\t\t" << "<Icon>" << "\n";
        text_file << "\t\t\t" << "<href>http://maps.google.com/mapfiles/kml/pushpin/red-pushpin.png</href>" << "\n";
        text_file << "\t\t" << "</Icon>" << "\n";
        text_file << "\t" << "</IconStyle>" << "\n";
        text_file << "\t" << "</Style>" << "\n";

/*
        Converte_Data_KML(Indice,&Data);
        text_file << "\t" << "<TimeStamp>" << "\n";
        text_file << "\t\t" << "<when>" << Data.str() << "</when>" << "\n";
        text_file << "\t" << "</TimeStamp>" << "\n";
*/

//Heading
        text_file << "\t" << "<LookAt>" << "\n";
        text_file << "\t\t" << "<altitudeMode>relativeToGround</altitudeMode>\n";
        text_file << "\t\t" << "<longitude>" << Pontos_Coords.at((i)).imag() << "</longitude>\n";
        text_file << "\t\t" << "<latitude>" << Pontos_Coords.at((i)).real() << "</latitude>\n";
        text_file << "\t\t" << "<altitude>" << Altitude << "</altitude>\n";
        text_file << "\t\t" << "<heading>" << 90 << "</heading>\n";
        text_file << "\t" << "</LookAt>" << "\n";

        text_file << "\t" << "<Point>" << "\n";
        text_file << "\t\t" << "<altitudeMode>clampedToGround</altitudeMode>\n";
        text_file << "\t\t" << "<coordinates>" << Pontos_Coords.at((i)).imag() << "," << Pontos_Coords.at((i)).real() << "," << Altitude << " </coordinates>\n";
        text_file << "\t" << "</Point>" << "\n";
        text_file << "</Placemark>" << "\n";

        text_file << "\n" << "</Folder>" << "\n";
    }

/*
    if (Tempo) {
        text_file << "\n\t" << "<Style id=\"multiTrack_n\">" << "\n";
        text_file << "\t\t" << "<IconStyle>" << "\n";
        text_file << "\t\t\t" << "<Icon>" << "\n";
        text_file << "\t\t\t\t" << "<href>http://earth.google.com/images/kml-icons/track-directional/track-0.png</href>" << "\n";
        text_file << "\t\t\t" << "</Icon>" << "\n";
        text_file << "\t\t" << "</IconStyle>" << "\n";

        text_file << "\t\t" << "<LineStyle>" << "\n";
        text_file << "\t\t\t" << "<color>99ffac59</color>" << "\n"; // Cor da linha (aabbggrr), onde aa=transparencia
        text_file << "\t\t\t" << "<width>6</width>" << "\n";  // Espessura
        text_file << "\t\t" << "</LineStyle>" << "\n";
        text_file << "\t" << "</Style>" << "\n";

        text_file << "\t" << "<StyleMap id=\"multiTrack\">" << "\n";

        text_file << "\t\t" << "<Pair>" << "\n";
        text_file << "\t\t\t" << "<key>normal</key>" << "\n";
        text_file << "\t\t\t" << "<styleUrl>#multiTrack_n</styleUrl>" << "\n";
        text_file << "\t\t" << "</Pair>" << "\n";

        text_file << "\t\t" << "<Pair>" << "\n";
        text_file << "\t\t\t" << "<key>highlight</key>" << "\n";
        text_file << "\t\t\t" << "<styleUrl>#multiTrack_h</styleUrl>" << "\n";
        text_file << "\t\t" << "</Pair>" << "\n";

        text_file << "\t" << "</StyleMap>" << "\n";

        text_file << "\t" << "<Style id=\"multiTrack_h\">" << "\n";
        text_file << "\t\t" << "<IconStyle>" << "\n";
        text_file << "\t\t\t" << "<scale>1.2</scale>" << "\n";
        text_file << "\t\t\t" << "<Icon>" << "\n";
        text_file << "\t\t\t\t" << "<href>http://earth.google.com/images/kml-icons/track-directional/track-0.png</href>" << "\n";
        text_file << "\t\t\t" << "</Icon>" << "\n";
        text_file << "\t\t" << "</IconStyle>" << "\n";

        text_file << "\t\t" << "<LineStyle>" << "\n";
        text_file << "\t\t\t" << "<color>99ffac59</color>" << "\n"; // Cor da linha (aabbggrr), onde aa=transparencia
        text_file << "\t\t\t" << "<width>8</width>" << "\n";  // Espessura
        text_file << "\t\t" << "</LineStyle>" << "\n";
        text_file << "\t" << "</Style>" << "\n";

        text_file << "\t" << "<Folder>" << "\n";
        text_file << "\t\t" << "<name>Tracks</name>" << "\n";
        text_file << "\t\t" << "<open>1</open>\n";

        text_file << "\t\t" << "<Placemark>" << "\n";
        text_file << "\t\t\t" << "<name>Teste</name>" << "\n";

        text_file << "\t\t" << "<styleUrl>" << "\n";
        text_file << "\t\t\t" << "#multiTrack" << "\n";
        text_file << "\t\t" << "</styleUrl>" << "\n";

        text_file << "\t\t" << "<gx:Track>" << "\n";
        // Primeiro TimeStamp
        for (i = 0 ; i < (int) (Dados.size()); i++) {
            Indice = &(Dados[i]);
            Converte_Data_KML(Indice,&Data);
            text_file << "\t\t\t" << "<when>" << Data.str() << "</when>" << "\n";
        }


        // Depois Coordenadas
        text_file.precision(9);
        Indice = &(Dados[0]);
        text_file << "\t\t\t" << "<gx:coord>" << Indice->Lon << " " << Indice->Lat << " " << Indice->Altitude << "</gx:coord> ";
        for (i = 1 ; i < (int) Dados.size() ; i ++) {
            Indice = &(Dados[i]);
            if ( (i % 2) == 0 ) text_file << "\n\t\t\t";
            text_file << "<gx:coord>" << Indice->Lon << " " << Indice->Lat << " " << Indice->Altitude << "</gx:coord> ";
        }

        text_file << "\n\t\t" << "</gx:Track>" << "\n";
        text_file << "\t\t" << "</Placemark>" << "\n";
        text_file << "\t" << "</Folder>" << "\n";
    }

*/

    if (Poligono) {
        text_file << "\t" << "<Style id=\"sn_ylw-pushpin1\">"  << "\n";
        text_file << "\t\t" << "<IconStyle>"  << "\n";
        text_file << "\t\t\t" << "<scale>1.1</scale>"  << "\n";
        text_file << "\t\t\t" << "<Icon>"  << "\n";
        text_file << "\t\t\t\t<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>"  << "\n";
        text_file << "\t\t\t" << "</Icon>"  << "\n";

        text_file << "\t\t\t" <<  "<hotSpot x=\"20\" y=\"2\" xunits=\"pixels\" yunits=\"pixels\"/>"  << "\n";
        text_file << "\t\t" << "</IconStyle>"  << "\n";

        text_file << "\t\t" << "<LineStyle>"  << "\n";
        text_file << "\t\t\t" << "<color>140000ff</color>"  << "\n";
        text_file << "\t\t" << "</LineStyle>"  << "\n";

        text_file << "\t\t" << "<PolyStyle>"  << "\n";
        text_file << "\t\t\t" << "<color>4d0000ff</color>"  << "\n";
        text_file << "\t\t" << "</PolyStyle>"  << "\n";
        text_file << "\t" << "</Style>"  << "\n";

        text_file << "\t" << "<Style id=\"sh_ylw-pushpin00\">"  << "\n";
        text_file << "\t\t" << "<IconStyle>"  << "\n";
        text_file << "\t\t\t" << "<scale>1.3</scale>"  << "\n";
        text_file << "\t\t\t" << "<Icon>"  << "\n";
        text_file << "\t\t\t\t" << "<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>"  << "\n";
        text_file << "\t\t\t" << "</Icon>"  << "\n";
        text_file << "\t\t\t\t" << "<hotSpot x=\"20\" y=\"2\" xunits=\"pixels\" yunits=\"pixels\"/>"  << "\n";
        text_file << "\t\t" << "</IconStyle>"  << "\n";
        text_file << "\t\t" << "<LineStyle>"  << "\n";
        text_file << "\t\t\t" << "<color>140000ff</color>"  << "\n";
        text_file << "\t\t" << "</LineStyle>"  << "\n";
        text_file << "\t\t" << "<PolyStyle>"  << "\n";
        text_file << "\t\t\t" << "<color>4d0000ff</color>"  << "\n";
        text_file << "\t\t" << "</PolyStyle>"  << "\n";
        text_file << "\t" << "</Style>"  << "\n";

        text_file << "\t" << "<StyleMap id=\"msn_ylw-pushpin0\">"  << "\n";
        text_file << "\t\t" << "<Pair>"  << "\n";
        text_file << "\t\t\t" << "<key>normal</key>"  << "\n";
        text_file << "\t\t\t" << "<styleUrl>#sn_ylw-pushpin1</styleUrl>"  << "\n";
        text_file << "\t\t" << "</Pair>"  << "\n";
        text_file << "\t\t" << "<Pair>"  << "\n";
        text_file << "\t\t\t" << "<key>highlight</key>"  << "\n";
        text_file << "\t\t\t" << "<styleUrl>#sh_ylw-pushpin00</styleUrl>"  << "\n";
        text_file << "\t\t" << "</Pair>"  << "\n";
        text_file << "\t" << "</StyleMap>"  << "\n";

        text_file << "\t" << "<Placemark>" << "\n";

        text_file << "\t\t" << "<name>POL</name>" << "\n";
        text_file << "\t\t" << "<styleUrl>#msn_ylw-pushpin0</styleUrl>"  << "\n";
        text_file << "\t\t" << "<Polygon>"  << "\n";
        text_file << "\t\t\t" <<	"<tessellate>1</tessellate>"  << "\n";
        text_file << "\t\t\t" <<	"<outerBoundaryIs>"  << "\n";
        text_file << "\t\t\t\t" << "<LinearRing>"  << "\n";
        text_file << "\t\t\t\t\t" << "<coordinates>"  << "\n" << "\t\t\t";
        for (i = 0 ; i < Pontos_Coords.length() ; i++) {
            text_file << Pontos_Coords.at((i)).imag() << "," << Pontos_Coords.at((i)).real() << "," << Altitude << " ";
            if ( (i % 4) == 0 ) text_file << "\n\t\t\t";
        }
        text_file << Pontos_Coords.at((0)).imag() << "," << Pontos_Coords.at((0)).real() << "," << Altitude << " ";

        text_file << "\n";
        text_file << "\t\t\t\t\t" << "</coordinates>"  << "\n";
        text_file << "\t\t\t\t" << "</LinearRing>"  << "\n";
        text_file << "\t\t\t" <<	"</outerBoundaryIs>"  << "\n";
        text_file << "\t\t" << "</Polygon>"  << "\n";
        text_file << "\t" << "</Placemark>";
    }


    text_file << "\n" << "</Document>" << "\n";
    text_file << "</kml>" << "\n";

    text_file.close();
}



///////////////////// Grava a partir das imagens JPG ou TIF individuais, fazendo a trilha do voo
///
///

void MainWindow::Grava_KML_Voo()
{
    int Num = Coords.IMGS.Lat.length();
    QList <std::complex <double> >      Pontos;
    std::complex <double>               P;
    double                              Altitude = 0;


//    Imagem_Composta.Geo_File_List.clear();

    int Num_Itens = ui->list_Files->selectedItems().size();

    if ( (ui->list_Files->selectedItems().count() == 0) || (ui->list_Files->selectedItems().count() == Num) ) {
        ui->list_Files->selectAll();
        for (int i = 0 ; i < Num ; i ++) {
            P.real(Coords.IMGS.Lat.at(i));
            P.imag(Coords.IMGS.Lon.at(i));
            Pontos.push_back(P);
        }
     } else {
        for (int i = 0 ; i < Num_Itens ; i ++) {
            QListWidgetItem *item = ui->list_Files->selectedItems().at(i);
            QString File;
            File = Informacao.Diretorio + "/" + item->text();
            for (int j = 0 ; j < Coords.IMGS.Lat.size() ; j++) {
                if (Coords.IMGS.FileName.at(j) == File) {
                    P.real(Coords.IMGS.Lat.at(j));
                    P.imag(Coords.IMGS.Lon.at(j));
                    Pontos.push_back(P);
                }
             }
        }
     }

    QStringList list = ui->txt_Alt_Voo->text().split(QRegularExpression("\\s+"));
    if ( list.length() != 0 ) Altitude = list.at(0).toDouble();

    Grava_KML(true,false,true,true,false,false,Altitude,Pontos,Informacao.Diretorio);
}


void MainWindow::Grava_KML_Voo_Proc()
{
    int Num = Coords.PROC.Lat.length();
    QList <std::complex <double> >      Pontos;
    std::complex <double>               P;
    double                              Altitude = 0;

    int Num_Itens = ui->list_Files_Proc->selectedItems().size();

    if ( (ui->list_Files_Proc->selectedItems().count() == 0) || (ui->list_Files_Proc->selectedItems().count() == Num) ) {
        ui->list_Files_Proc->selectAll();
        for (int i = 0 ; i < Num ; i ++) {
            P.real(Coords.PROC.Lat.at(i));
            P.imag(Coords.PROC.Lon.at(i));
            Pontos.push_back(P);
        }
     } else {
        for (int i = 0 ; i < Num_Itens ; i ++) {
            QListWidgetItem *item = ui->list_Files_Proc->selectedItems().at(i);
            QString File;
            File = Informacao.DiretorioProc + "/" + item->text();
            for (int j = 0 ; j < Coords.PROC.Lat.size() ; j++) {
                if (Coords.PROC.FileName.at(j) == File) {
                    P.real(Coords.PROC.Lat.at(j));
                    P.imag(Coords.PROC.Lon.at(j));
                    Pontos.push_back(P);
                }
             }
        }
     }

    QString Temp = ui->txt_Alt_Voo->text();
    QStringList list = Temp.split(QRegularExpression("\\s+"));
//    QStringList list = ui->txt_Alt_Voo->text().split(QRegExp("\\s+"),QString::SkipEmptyParts);

    if ( list.length() != 0 ) Altitude = list.at(0).toDouble();

    Grava_KML(true,false,true,true,false,false,Altitude,Pontos,Informacao.DiretorioProc);
}


/*
void Grava_GPX(QWidget *parent,bool Track,bool Shadow,bool Pontos,bool Tempo) {
    int                 i;
    Conteudo_Frame      *Indice;

    std::ofstream       text_file;
    QString             dirOpen;

    std::ostringstream  Data;
    string              GPX_file;

    double              Min_LAT,Max_LAT,Min_LON,Max_LON;

    Min_LAT = 1e10;
    Min_LON = 1e10;
    Max_LAT = -1e10;
    Max_LON = -1e10;

    for (i = 0 ; i < (int) (Dados.size() - 1); i++) {
        Indice = &(Dados[i]);
        if (Indice->Lat < Min_LAT) Min_LAT = Indice->Lat;
        if (Indice->Lat > Max_LAT) Max_LAT = Indice->Lat;
        if (Indice->Lon < Min_LON) Min_LON = Indice->Lon;
        if (Indice->Lon > Max_LON) Max_LON = Indice->Lon;
    }

    dirOpen = QFileDialog::getSaveFileName(parent,parent->tr("Save GPX"),"c:/QT_Projects",parent->tr("GPX (*.GPX)"));
    if (dirOpen == NULL) return;

    GPX_file = dirOpen.toStdString().c_str();

    text_file.open(GPX_file,ios::out);

    text_file << setprecision(9);

    // Escreve os headers de um arquivo GPX
    text_file << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << "\n";
    text_file << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"HAM\" version=\"0.5\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " <<
                 "xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">" << "\n";
    text_file << "\t" << "<metadata>" << "\n";
    text_file << "\t\t" << "<time>2016-01-01T00:00:00Z</time>" << "\n";

    text_file << "\t\t" << "<bounds minlat=\"" << Min_LAT << "\" minlon=\"" << Min_LON << "\" maxlat=\"" << Max_LAT << "\" maxlon=\"" << Max_LON <<"\"/> " << "\n";
    text_file << "\t" << "</metadata>" << "\n";

    // Escreve os pontos
    if (Pontos) {
        // Primeiro ponto - Icone de cor verde
        Indice = &(Dados[0]);
        Converte_Data_KML(Indice,&Data);

        text_file << "\t" << "<wpt" ;
        text_file << " lat=\"" << Indice->Lat << "\" lon=\"" << Indice->Lon << "\">" << "\n";
        text_file << "\t\t<ele>" << Indice->Altitude << "</ele>\n";
        text_file << "\t\t" << "<time>" << Data.str() << "</time>" << "\n";

        if (Indice->Bearing != 360.0) {
            Indice->Bearing = Indice->Bearing + Declinacao;
            text_file << "\t\t<compass>" << Indice->Bearing << "</compass>\n";
        }


        text_file << "\t\t<name>Position " << 1 << "</name>\n";
        text_file << "\t</wpt>\n";



        // A partir do segundo ponto até o penúltimo - Icone redondo
        for (i = 1 ; i < (int) (Dados.size() - 1); i++) {
            Indice = &(Dados[i]);
            Converte_Data_KML(Indice,&Data);

            text_file << "\t" << "<wpt" ;
            text_file << " lat=\"" << Indice->Lat << "\" lon=\"" << Indice->Lon << "\">" << "\n";
            text_file << "\t\t<ele>" << Indice->Altitude << "</ele>\n";
            text_file << "\t\t" << "<time>" << Data.str() << "</time>" << "\n";

            if (Indice->Bearing != 360.0) {
                Indice->Bearing = Indice->Bearing + Declinacao;
                text_file << "\t\t<compass>" << Indice->Bearing << "</compass>\n";
            }

            text_file << "\t\t<name>Position " << (i+1) << "</name>\n";
            text_file << "\t</wpt>\n";
        }

        // Ultimo ponto - Icone de cor vermelha
        Indice = &(Dados[i]);
        Converte_Data_KML(Indice,&Data);

        text_file << "\t" << "<wpt" ;
        text_file << " lat=\"" << Indice->Lat << "\" lon=\"" << Indice->Lon << "\">" << "\n";
        text_file << "\t\t<ele>" << Indice->Altitude << "</ele>\n";
        text_file << "\t\t" << "<time>" << Data.str() << "</time>" << "\n";

        if (Indice->Bearing != 360.0) {
            Indice->Bearing = Indice->Bearing + Declinacao;
            text_file << "\t\t<compass>" << Indice->Bearing << "</compass>\n";
        }

        text_file << "\t\t<name>Position " << (i+1) << "</name>\n";
        text_file << "\t</wpt>\n";

    }

    text_file << "</gpx>" << "\n";

    text_file.close();
}



bool KML_CSV_Form::Le_CSV_Cabecalho(QWidget *parent,CSV *CSV_Content,bool Metric,MainWindow *Window) {
    QString         dirOpen;
    string          Linha_txt,Linha_txt2;
    char            filename[100];
    std::ifstream   Arquivo_CSV;
    string          CSV_TEMP;
    std::size_t     Pos,Lenght;
    int             i,j;

    dirOpen = QFileDialog::getOpenFileName(parent, parent->tr("Open CSV"),"c:/QT_Projects",parent->tr("CSV File (*.CSV)"));
    if (dirOpen == NULL) return (false);

    sprintf_s(CSV_Content->Filename,"%s",dirOpen.toUtf8().data());

    ui->txt_CVS_File->setText(CSV_Content->Filename);


    /// Le o arquivo
    Arquivo_CSV.open(CSV_Content->Filename,ios::in);

    // Primeira linha contém os tipos de informação do CSV
    getline(Arquivo_CSV,Linha_txt);
    Lenght = Linha_txt.length();
    Linha_txt2 = Linha_txt;

    // Monta Vetor de strings
    CSV_Content->Tamanho = 0;

    i = 1;
    Pos = 0;
    while (true) {
        Pos = Linha_txt.find(',',i);
        if (Pos < Lenght) {
            CSV_TEMP = Linha_txt.substr(i - 1,Pos-i+1);
            CSV_Content->Tipos.push_back((CSV_TEMP));
            CSV_Content->Tamanho++;
            i = Pos + 2;
        } else {
            CSV_TEMP = Linha_txt.substr(i - 1,Lenght-i+1);
            CSV_Content->Tipos.push_back((CSV_TEMP));
            CSV_Content->Tamanho++;
            break;
         }
    }
    Arquivo_CSV.close();

    return (true);
}

bool Le_Valor_CSV(CSV *CSV_Content,int Campo,string *Valor) {
    QString         dirOpen;
    string          Linha_txt,Linha_txt2;
    char            filename[100];
    std::ifstream   Arquivo_CSV;
    string          CSV_TEMP;
    std::size_t     Pos,Lenght;
    int             i,j;


    if (Campo > CSV_Content->Tamanho) return (false);

    /// Le o arquivo
    Arquivo_CSV.open(CSV_Content->Filename,ios::in);
    if (!Arquivo_CSV.is_open()) return(false);

    // Primeira linha contém os tipos de informação do CSV
    getline(Arquivo_CSV,Linha_txt);

    Conteudo_Frame      Dados_TEMP;
    std::vector<string> Conteudo_Linha;

    CSV_TEMP = "";
    for (i = 0 ; i < CSV_Content->Tamanho ; i++)
        Conteudo_Linha.push_back(CSV_TEMP);

    // O que interessa está na segunda linha
    getline(Arquivo_CSV,Linha_txt);
    Lenght = Linha_txt.length();
    i = 1;
    j = 0;
    Pos = 0;
    while (true) {
        Pos = Linha_txt.find(',',i);
        if (Pos < Lenght) {
            CSV_TEMP = Linha_txt.substr(i - 1,Pos-i+1);
            Conteudo_Linha[j] = CSV_TEMP;
            j++;
            i = Pos + 2;
        } else {
            CSV_TEMP = Linha_txt.substr(i - 1,Lenght-i+1);
            Conteudo_Linha[j] = CSV_TEMP;
            j++;
            break;
        }
    }

    *Valor = Conteudo_Linha[Campo];

    Arquivo_CSV.close();

    return (true);
}


bool KML_CSV_Form::Le_CSV(QWidget *parent,CSV *CSV_Content,bool Metric,MainWindow *Window) {

    QString         dirOpen;
    string          Linha_txt,Linha_txt2;
    std::ifstream   Arquivo_CSV;
    string          CSV_TEMP;
    std::size_t     Pos,Lenght;
    int             i,j;

    dirOpen = QFileDialog::getOpenFileName(parent, parent->tr("Open CSV"),"c:/QT_Projects",parent->tr("CSV File (*.CSV)"));
    if (dirOpen == NULL) return (false);

    sprintf_s(CSV_Content->Filename,"%s",dirOpen.toUtf8().data());

    ui->txt_CVS_File->setText(CSV_Content->Filename);


    /// Le o arquivo
    Arquivo_CSV.open(CSV_Content->Filename,ios::in);

    // Primeira linha contém os tipos de informação do CSV
    getline(Arquivo_CSV,Linha_txt);
    Lenght = Linha_txt.length();
    Linha_txt2 = Linha_txt;

    // Monta Vetor de strings
    CSV_Content->Tamanho = 0;

    i = 1;
    Pos = 0;
    while (true) {
        Pos = Linha_txt.find(',',i);
        if (Pos < Lenght) {
            CSV_TEMP = Linha_txt.substr(i - 1,Pos-i+1);
            CSV_Content->Tipos.push_back((CSV_TEMP));
            CSV_Content->Tamanho++;
            i = Pos + 2;
        } else {
            CSV_TEMP = Linha_txt.substr(i - 1,Lenght-i+1);
            CSV_Content->Tipos.push_back((CSV_TEMP));
            CSV_Content->Tamanho++;
            break;
         }
    }

    Conteudo_Frame      Dados_TEMP;
    double              Pontos[4];
    std::vector<string> Conteudo_Linha;

    CSV_TEMP = "";
    for (i = 0 ; i < CSV_Content->Tamanho ; i++)
        Conteudo_Linha.push_back(CSV_TEMP);


    Dados_TEMP.Fuso = FusoAtual;

    // Lê os Dados para cada linha
    while (getline(Arquivo_CSV,Linha_txt)) {
        Lenght = Linha_txt.length();
        i = 1;
        j = 0;
        Pos = 0;
        while (true) {
            Pos = Linha_txt.find(',',i);
            if (Pos < Lenght) {
                CSV_TEMP = Linha_txt.substr(i - 1,Pos-i+1);
                Conteudo_Linha[j] = CSV_TEMP;
                j++;
                i = Pos + 2;
            } else {
                CSV_TEMP = Linha_txt.substr(i - 1,Lenght-i+1);
                Conteudo_Linha[j] = CSV_TEMP;
                j++;
                break;
            }
        }

        Pontos[0] = std::stod(Conteudo_Linha[CSV_Content->LAT]);// Latitude
        Pontos[1] = std::stod(Conteudo_Linha[CSV_Content->LON]);// Longitude
        Pontos[2] = std::stod(Conteudo_Linha[CSV_Content->ALT]);// Altitude

        Pontos[3] = std::stod(Conteudo_Linha[CSV_Content->BUSSOLA]);// Altitude

        if (!Metric) Pontos[2] = Pontos[2] * .3048;
        Dados_TEMP.Altitude = Pontos[2];
        Dados_TEMP.Lat = Pontos[0];
        Dados_TEMP.Lat_TXT[0] = '/0';
        Dados_TEMP.Lon = Pontos[1];
        Dados_TEMP.Lon_TXT[0] = '/0';

        //Ano
        CSV_TEMP = Conteudo_Linha[12].substr(0,4);
        Dados_TEMP.Ano = std::stoi(CSV_TEMP);
        //Mes
        CSV_TEMP = Conteudo_Linha[12].substr(5,2);
        Dados_TEMP.Mes = std::stoi(CSV_TEMP);
        //Dia
        CSV_TEMP = Conteudo_Linha[12].substr(8,2);
        Dados_TEMP.Dia = std::stoi(CSV_TEMP);
        //Hora
        CSV_TEMP = Conteudo_Linha[12].substr(11,2);
        Dados_TEMP.Hora = std::stoi(CSV_TEMP);
        //Minuto
        CSV_TEMP = Conteudo_Linha[12].substr(14,2);
        Dados_TEMP.Minuto = std::stoi(CSV_TEMP);
        //Segundo
        CSV_TEMP = Conteudo_Linha[12].substr(17,2);
        Dados_TEMP.Segundo = std::stoi(CSV_TEMP);

        Dados.push_back(Dados_TEMP);
    }

    Arquivo_CSV.close();

    return (true);

}


bool KML_CSV_Form::Le_CSV_From_Choice(QWidget *parent,CSV *CSV_Content,CSV_Escolha *Escolha,bool Metric,MainWindow *Window) {

    QString         dirOpen;
    string          Linha_txt,Linha_txt2;
    std::ifstream   Arquivo_CSV;
    string          CSV_TEMP;
    std::size_t     Pos,Lenght;
    int             i,j;


    /// Le o arquivo
    Arquivo_CSV.open(CSV_Content->Filename,ios::in);

    // Primeira linha contém os tipos de informação do CSV
    getline(Arquivo_CSV,Linha_txt);

    Conteudo_Frame      Dados_TEMP;
    double              Pontos[4];
    std::vector<string> Conteudo_Linha;

    CSV_TEMP = "";
    for (i = 0 ; i < CSV_Content->Tamanho ; i++)
        Conteudo_Linha.push_back(CSV_TEMP);


    Dados_TEMP.Fuso = FusoAtual;

    // Lê os Dados para cada linha
    while (getline(Arquivo_CSV,Linha_txt)) {
        Lenght = Linha_txt.length();
        i = 1;
        j = 0;
        Pos = 0;
        while (true) {
            Pos = Linha_txt.find(',',i);
            if (Pos < Lenght) {
                CSV_TEMP = Linha_txt.substr(i - 1,Pos-i+1);
                Conteudo_Linha[j] = CSV_TEMP;
                j++;
                i = Pos + 2;
            } else {
                CSV_TEMP = Linha_txt.substr(i - 1,Lenght-i+1);
                Conteudo_Linha[j] = CSV_TEMP;
                j++;
                break;
            }
        }

        Pontos[0] = std::stod(Conteudo_Linha[CSV_Content->LAT]);// Latitude
        Pontos[1] = std::stod(Conteudo_Linha[CSV_Content->LON]);// Longitude
        Pontos[2] = std::stod(Conteudo_Linha[CSV_Content->ALT]);// Altitude

        if (Escolha->BUSSOLA) {
           Pontos[3] = std::stod(Conteudo_Linha[CSV_Content->BUSSOLA]);// Bussola
        } else {
           Pontos[3] = 360.0;
        }
        if (!Metric) Pontos[2] = Pontos[2] * .3048; // Converte de pes para metros
        Dados_TEMP.Altitude = Pontos[2];
        Dados_TEMP.Lat = Pontos[0];
        Dados_TEMP.Lat_TXT[0] = '/0';
        Dados_TEMP.Lon = Pontos[1];
        Dados_TEMP.Lon_TXT[0] = '/0';
        Dados_TEMP.Bearing = Pontos[3];

        //Ano
        CSV_TEMP = Conteudo_Linha[CSV_Content->DATETIME].substr(0,4);
        Dados_TEMP.Ano = std::stoi(CSV_TEMP);
        //Mes
        CSV_TEMP = Conteudo_Linha[CSV_Content->DATETIME].substr(5,2);
        Dados_TEMP.Mes = std::stoi(CSV_TEMP);
        //Dia
        CSV_TEMP = Conteudo_Linha[CSV_Content->DATETIME].substr(8,2);
        Dados_TEMP.Dia = std::stoi(CSV_TEMP);
        //Hora
        CSV_TEMP = Conteudo_Linha[CSV_Content->DATETIME].substr(11,2);
        Dados_TEMP.Hora = std::stoi(CSV_TEMP);
        //Minuto
        CSV_TEMP = Conteudo_Linha[CSV_Content->DATETIME].substr(14,2);
        Dados_TEMP.Minuto = std::stoi(CSV_TEMP);
        //Segundo
        CSV_TEMP = Conteudo_Linha[CSV_Content->DATETIME].substr(17,2);
        Dados_TEMP.Segundo = std::stoi(CSV_TEMP);

        Dados.push_back(Dados_TEMP);
    }

    Arquivo_CSV.close();

    return (true);

}
*/

/*
void Converte_Data_KML(Conteudo_Frame *TEMP, std::ostringstream *BB) {

    BB->str("");

    *BB << TEMP->Ano;
    *BB << "-";
    *BB << setfill('0') << setw(2) << TEMP->Mes << "-";
    *BB << setfill('0') << setw(2) << TEMP->Dia << "T";
    *BB << setfill('0') << setw(2) << (TEMP->Hora - TEMP->Fuso) << ":";
    *BB << setfill('0') << setw(2) << TEMP->Minuto << ":";
    *BB << setfill('0') << setw(2) << TEMP->Segundo << "Z";
}
*/

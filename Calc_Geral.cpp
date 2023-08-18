#include <QMainWindow>
#include <QMessageBox>

#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

extern Geometria_Imagem GeoProc;

// extern Ui::MainWindow* windowPrincipal;

extern  std::string Name_Imagem1;

void MainWindow::Dist_2Pontos(void)
{

    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B,Dist;
    GeographicLib::GeoCoords    A,B;

    PosLat_A = GeoProc.PImg.at(GeoProc.PImg.length() - 2).real();
    PosLon_A = GeoProc.PImg.at(GeoProc.PImg.length() - 2).imag();
    PosLat_B = GeoProc.PImg.at(GeoProc.PImg.length() - 1).real();
    PosLon_B = GeoProc.PImg.at(GeoProc.PImg.length() - 1).imag();
    cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 0, 255, 0), 4 );
    cv::imshow(Name_Imagem1,GeoProc.image);
}



float MainWindow::Calc_Perimetro() {

     return 0;
}

float MainWindow::Calc_Area() {
    return 0;
}


float MainWindow::Calc_Dist()
{
    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B,Dist;
    GeographicLib::GeoCoords    A,B;

    Dist = 0;

    QLocale Br;
    QString S1;

    // Apenas um ponto
    if (GeoProc.PImg.length() < 2) {
        S1 = Br.toString(Dist,'f',2);
//        sprintf(intStr,"%f m",Dist);
        ui->txt_Dist->setText(S1 + " m");
        return (Dist);
    }

    for (int i = 2 ; i <= (GeoProc.PImg.length()) ; i ++) {
        PosLat_A = GeoProc.Pontos.at(i - 2).real();
        PosLon_A = GeoProc.Pontos.at(i - 2).imag();
        PosLat_B = GeoProc.Pontos.at(i - 1).real();
        PosLon_B = GeoProc.Pontos.at(i - 1).imag();

        A.Reset(PosLat_A,PosLon_A);
        B.Reset(PosLat_B,PosLon_B);

        Dist = Dist + sqrt(pow((A.Northing()-B.Northing()),2) + pow(A.Easting()-B.Easting(),2));

        PosLat_A = GeoProc.PImg.at(i - 2).real();
        PosLon_A = GeoProc.PImg.at(i - 2).imag();
        PosLat_B = GeoProc.PImg.at(i - 1).real();
        PosLon_B = GeoProc.PImg.at(i - 1).imag();
        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 0, 255, 0), 1 );
    }
    cv::imshow(Name_Imagem1,GeoProc.image);


    S1 = Br.toString(Dist,'f',2);
//        sprintf(intStr,"%f m",Dist);
    ui->txt_Dist->setText(S1 + " m");
    return (Dist);
}


void MainWindow::on_action2_pontos_triggered()
{
    on_actionCaminho_triggered();
    return;

    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B,Dist;
    GeographicLib::GeoCoords    A,B;
    char                        intStr[100];


    PosLat_A = GeoProc.Pontos.at(GeoProc.PImg.length() - 2).real();
    PosLon_A = GeoProc.Pontos.at(GeoProc.PImg.length() - 2).imag();
    PosLat_B = GeoProc.Pontos.at(GeoProc.PImg.length() - 1).real();
    PosLon_B = GeoProc.Pontos.at(GeoProc.PImg.length() - 1).imag();

    A.Reset(PosLat_A,PosLon_A);
    B.Reset(PosLat_B,PosLon_B);

    Dist = sqrt(pow((A.Northing()-B.Northing()),2) + pow(A.Easting()-B.Easting(),2));

    sprintf(intStr," Distância = %f\n",Dist);
    ui->txt_Output->append(intStr);

    PosLat_A = GeoProc.PImg.at(GeoProc.PImg.length() - 2).real();
    PosLon_A = GeoProc.PImg.at(GeoProc.PImg.length() - 2).imag();
    PosLat_B = GeoProc.PImg.at(GeoProc.PImg.length() - 1).real();
    PosLon_B = GeoProc.PImg.at(GeoProc.PImg.length() - 1).imag();
    cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 0, 255, 0), 4 );
    imshow(Name_Imagem1,GeoProc.image);

}

void MainWindow::on_actionCaminho_triggered()
{
    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B,Dist;
    GeographicLib::GeoCoords    A,B;
    char                        intStr[100];

    Dist = 0;

    // Tem que ter pelo menos 2 pontos
    if (GeoProc.PImg.length() < 2) {
        QMessageBox::information(this,tr("Erro"),"Pelo menos 2 pontos");
        return;
    }

    for (int i = 2 ; i <= (GeoProc.PImg.length()) ; i ++) {
        PosLat_A = GeoProc.Pontos.at(i - 2).real();
        PosLon_A = GeoProc.Pontos.at(i - 2).imag();
        PosLat_B = GeoProc.Pontos.at(i - 1).real();
        PosLon_B = GeoProc.Pontos.at(i - 1).imag();

        A.Reset(PosLat_A,PosLon_A);
        B.Reset(PosLat_B,PosLon_B);

        Dist = Dist + sqrt(pow((A.Northing()-B.Northing()),2) + pow(A.Easting()-B.Easting(),2));

        PosLat_A = GeoProc.PImg.at(i - 2).real();
        PosLon_A = GeoProc.PImg.at(i - 2).imag();
        PosLat_B = GeoProc.PImg.at(i - 1).real();
        PosLon_B = GeoProc.PImg.at(i - 1).imag();
        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 0, 255, 0), 1 );
    }

    imshow(Name_Imagem1,GeoProc.image);

    QString S1;
    QLocale Br;
    S1 = Br.toString(Dist,'f',2);
    ui->txt_Output->append("     Distância: " + S1 + " metros");
}


void MainWindow::on_action_rea_triggered(bool No_Print)
{
    float                       PosLon_A,PosLat_A,PosLon_B,PosLat_B,Dist;
    double                      Area,x0,y0,x1,y1;
    GeographicLib::GeoCoords    A,B;
    char                        intStr[100];

    std::vector<cv::Point>      fillContSingle;

    Dist = 0;
    Area = 0;

    if (No_Print) {

    }

    // Tem que ter pelo menos 3 pontos
    if (GeoProc.PImg.length() < 3) {
        if (!No_Print) QMessageBox::information(this,tr("Erro"),"Pelo menos 3 pontos");
        return;
    }

    bool Status;
    Status = ui->checkBox_Linhas->isChecked();
    ui->checkBox_Linhas->setChecked(false);
    on_checkBox_Linhas_clicked();
    ui->checkBox_Linhas->setChecked(Status);


    int i;
    for (i = 2 ; i <= (GeoProc.PImg.length()) ; i ++) {
        PosLat_A = GeoProc.Pontos.at(i - 2).real();
        PosLon_A = GeoProc.Pontos.at(i - 2).imag();
        PosLat_B = GeoProc.Pontos.at(i - 1).real();
        PosLon_B = GeoProc.Pontos.at(i - 1).imag();

        A.Reset(PosLat_A,PosLon_A);
        B.Reset(PosLat_B,PosLon_B);

        x0 = A.Easting();
        y0 = A.Northing();
        x1 = B.Easting();
        y1 = B.Northing();
        Dist = Dist + sqrt(pow(y1 - y0,2) + pow(x1-x0,2));
        Area = Area + (x0*y1 - y0*x1);

        PosLat_A = GeoProc.PImg.at(i - 2).real();
        PosLon_A = GeoProc.PImg.at(i - 2).imag();
        PosLat_B = GeoProc.PImg.at(i - 1).real();
        PosLon_B = GeoProc.PImg.at(i - 1).imag();

        cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 1 );
//        imshow("Imagem",GeoProc.image);

        fillContSingle.push_back(cv::Point(PosLat_A,PosLon_A));

    }
    PosLat_A = GeoProc.Pontos.at(i - 2).real();
    PosLon_A = GeoProc.Pontos.at(i - 2).imag();
    PosLat_B = GeoProc.Pontos.at(0).real();
    PosLon_B = GeoProc.Pontos.at(0).imag();

    A.Reset(PosLat_A,PosLon_A);
    B.Reset(PosLat_B,PosLon_B);

    x0 = A.Easting();
    y0 = A.Northing();
    x1 = B.Easting();
    y1 = B.Northing();
    Dist = Dist + sqrt(pow(y1 - y0,2) + pow(x1-x0,2));
    Area = Area + (x0*y1 - y0*x1);

    Area = fabs(Area) / 2;

    PosLat_A = GeoProc.PImg.at(0).real();
    PosLon_A = GeoProc.PImg.at(0).imag();
    PosLat_B = GeoProc.PImg.at(i - 2).real();
    PosLon_B = GeoProc.PImg.at(i - 2).imag();
    cv::line(GeoProc.image,cv::Point(PosLat_A,PosLon_A),cv::Point(PosLat_B,PosLon_B),cv::Scalar( 28, 255, 222), 1 );
//    imshow("Imagem",GeoProc.image);

//    sprintf(intStr," Perímetro = %6.2f metros, Área = %6.2f metros quadrados\n",Dist, Area);

      QLocale Br;
      QString S1 = Br.toString(Dist,'f',2);
      QString S2 = Br.toString(Area,'f',1);

      sprintf(intStr,"     Perímetro: %s metros, Área: %s metros quadrados\n",S1.toStdString().c_str(),S2.toStdString().c_str());
      if (!No_Print) ui->txt_Output->append(intStr);
      if (GeoProc.Poligono) {
          ui->txt_Dist->setText(S1 + " m");
          ui->lineEdit_Area->setText(S2 + " m2");
      }

    fillContSingle.push_back(cv::Point(PosLat_B,PosLon_B));

    std::vector<std::vector<cv::Point> > fillContAll;
    fillContAll.push_back(fillContSingle);

    cv::Mat overlay;
    double alpha = 0.2;

    GeoProc.image.copyTo(overlay);

    cv::fillPoly(overlay,fillContAll,cv::Scalar( 28, 255, 222));
    cv::addWeighted(overlay,alpha,GeoProc.image,1 - alpha,0,GeoProc.image);

    imshow(Name_Imagem1,GeoProc.image);

}


#include <QMainWindow>

#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

extern Geometria_Imagem GeoProc;
extern Geometria_Imagem Geo;
extern message_function MeFu;

void MainWindow::on_CheckBox_Poligono_clicked()
{
    char intStr[100];


    if (ui->CheckBox_Poligono->isChecked()) {
        sprintf(intStr,"\n Cálculo por poligono ativado.\n");
//        ui->txt_Output->append(intStr);
        Geo.Poligono = true;
        GeoProc.Poligono = true;
        ui->label_Perimetro->setText("Perímetro:");
        ui->label->setVisible(true);
        ui->lineEdit_Area->setVisible(true);
        on_action_rea_triggered(true);
        MeFu.Draw_Cel();
    } else {
       sprintf(intStr,"\n Cálculo por poligono desativado.\n");
//       ui->txt_Output->append(intStr);
       Geo.Poligono = false;
       GeoProc.Poligono = false;
       ui->label_Perimetro->setText("Caminho:");
       ui->label->setVisible(false);
       ui->lineEdit_Area->setVisible(false);
       Calc_Dist();
       MeFu.Draw_Cel();
    }
}


void MainWindow::on_checkBox_Linhas_clicked()
{
    char intStr[100];

    if (ui->checkBox_Linhas->isChecked()) {
        sprintf(intStr,"\n Linhas ativadas.\n");
        Geo.Line = true;
        GeoProc.Line = true;
    } else {
        sprintf(intStr,"\n Linhas desativadas.\n");
        Geo.Line = false;
        GeoProc.Line = false;
    }

    MeFu.Draw_Cel();
//    ui->txt_Output->append(intStr);
}

void MainWindow::on_checkBox_Circle_clicked()
{
    char intStr[100];

    if (ui->checkBox_Circle->isChecked()) {
        sprintf(intStr,"\n Circulos ativados.\n");
        Geo.Circle = true;
        GeoProc.Circle = true;
    } else {
       sprintf(intStr,"\n Circulos desativados.\n");
       Geo.Circle = false;
       GeoProc.Circle = false;
    }

    MeFu.Draw_Cel();
//    ui->txt_Output->append(intStr);
}

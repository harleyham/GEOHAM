#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QTime>
#include <QDir>
#include <QMessageBox>
#include <QPixmap>

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config_file.h"

using namespace std;

Arquivo_Config_File::Arquivo_Config_File() {

    Dir = "";
    TemDir = false;

    Tipos.clear();
    Tipos.append(CONFIG_THUMB_TXT);
    Tipos.append(CONFIG_PREENCHE_POLIGONO_TXT);
    Tipos.append(CONFIG_NODATA_TXT);
    Tipos.append(CONFIG_REF_TXT);
    Tipos.append(CONFIG_ALG_TXT);
    Tipos.append(CONFIG_BORDAS_TXT);
    Tipos.append(CONFIG_MOVER_TXT);
    Tipos.append(CONFIG_MESCLAGEM_TXT);
    Tipos.append(CONFIG_PUL_TXT);
    Tipos.append(CONFIG_CURVA_TXT);
    Tipos.append(CONFIG_MILHAR_TXT);
    Tipos.append(CONFIG_TODAS_TXT);
    Tipos.append(CONFIG_LINHAS_TXT);
    Tipos.append(CONFIG_CIRCULOS_TXT);
    Tipos.append(CONFIG_POLIGONO_TXT);
    Tipos.append(CONFIG_REDUC_TXT);
    Tipos.append(CONFIG_ESCALA_TXT);
    Tipos.append(CONFIG_ALTVOO_TXT);
    Tipos.append(CONFIG_ALTVOO_VALOR_TXT);

    Tipos.append(CONFIG_DIR_TXT);

    Valores.clear();
    Valores.append(1);
    Valores.append(1);
    Valores.append(1);
    Valores.append(1);
    Valores.append(1);
    Valores.append(0);
    Valores.append(1);
    Valores.append(1);
    Valores.append(1);
    Valores.append(0);
    Valores.append(1);
    Valores.append(1);
    Valores.append(0);
    Valores.append(1);
    Valores.append(0);
    Valores.append(3);
    Valores.append(25);
    Valores.append(0);  // Não Selecionado
    Valores.append(30); // Altura padrão 30 metros

    Valores.append(0);

}

void MainWindow::Exec_Config_File() {

    bool A;
    QString Text;

    for (int i = 0 ; i < Config.Tipos.length() ; i ++) {
        switch(i) {
        case 0:  // Thumbnail
            ui->actionThumbnail->setChecked(Config.Valores.at(0));
            on_actionThumbnail_triggered();
            break;
        case 1: // Poligono
            ui->actionPreenche_Pol_gono->setChecked(Config.Valores.at(1));
            on_actionPreenche_Pol_gono_triggered();
            break;
        case 2: // NODATA Branco ou Preto
            A = Config.Valores.at(2) ? true : false;
            ui->actionPreto->setChecked(!A);
            ui->actionBranco->setChecked(A);
            if (ui->actionBranco->isChecked())  on_actionBranco_triggered(); else on_actionPreto_triggered();
            break;
        case 3: // Referencia de ângulo
            A = Config.Valores.at(3) ? true : false;
            ui->actionDrone->setChecked(!A);
            ui->actionGimbal->setChecked(A);
            if (ui->actionDrone->isChecked()) on_actionDrone_triggered(); else on_actionGimbal_triggered();
            break;
        case 4: // Algoritmo GDAL ou interno
            A = Config.Valores.at(4) ? true : false;
            ui->actionGDAL->setChecked(!A);
            ui->actionInterno->setChecked(A);
            if (ui->actionInterno->isChecked()) on_actionInterno_triggered(); else on_actionGDAL_triggered();
            break;
        case 5: // Desenhar bordas em cada imagem processada
            ui->actionBordas_na_imagens_processadas->setChecked(Config.Valores.at(5));
            on_actionBordas_na_imagens_processadas_triggered();
            break;
        case 6: // MOVER
            A = Config.Valores.at(6) ? true : false;
            ui->actionMover_imagens->setChecked(A);
            on_actionMover_imagens_triggered();
            break;
        case 7: // MESCLAGEM
            break;
        case 8: // Primeira e ultima linha
            A = Config.Valores.at(8) ? true : false;
            ui->actionUsar_imagens_da_primeira_e_ltima_linha->setChecked(A);
            on_actionUsar_imagens_da_primeira_e_ltima_linha_triggered();
            break;
        case 9: // Usar imagem das curvas
            A = Config.Valores.at(9) ? true : false;
            ui->actionUsar_imagens_do_drone_em_curva->setChecked(A);
            on_actionUsar_imagens_do_drone_em_curva_triggered();
            break;
        case 10: // Separção de milhar notação Portugues ou inglês
            A = Config.Valores.at(10) ? true : false;
            ui->actionPortugu_s->setChecked(A);
            if (ui->actionPortugu_s->isChecked()) on_actionPortugu_s_triggered();
            else on_actionIngl_s_triggered();
            break;
        case 11: // Todas as imagens
            A = Config.Valores.at(11) ? true : false;
            ui->actionTodas_as_imagens->setChecked(A);
            on_actionTodas_as_imagens_triggered();
            break;
        case 12: // Linhas
            A = Config.Valores.at(12) ? true : false;
            ui->checkBox_Linhas->setChecked(A);
            break;
        case 13: // Circulos
            A = Config.Valores.at(13) ? true : false;
            ui->checkBox_Circle->setChecked(A);
            break;
        case 14: // Poligono
            A = Config.Valores.at(14) ? true : false;
            ui->CheckBox_Poligono->setChecked(A);
            break;
        case 15: // Redução
            ui->txt_Reducao->setAlignment(Qt::AlignRight);
            Informacao.Reducao = Config.Valores.at(15);
            ui->txt_Reducao->setText(QString::number(Informacao.Reducao));
            break;
        case 16: // Escala
            ui->txt_Escala->setAlignment(Qt::AlignRight);
            Informacao.Escala1 = (double) Config.Valores.at(16) / 100;
            ui->txt_Escala->setText(QString::number(Informacao.Escala1 * 100));
            break;
        case 17:
            A = Config.Valores.at(CONFIG_ALTVOO) ? true : false;
            ui->actionEspecificar_2->setChecked(A);
            on_actionEspecificar_2_triggered();
            break;
        case 18:
                char    intStr[50];
//                Geo.Alt_Default = Config.Valores.at(CONFIG_ALTVOO_VALOR);
                Text = "Valor: " + QString::number(Config.Valores.at(CONFIG_ALTVOO_VALOR)) + " m";
                ui->actionValor->setText(Text);
                sprintf(intStr,"\n Altura do voo definida para %d metros\n",Config.Valores.at(CONFIG_ALTVOO_VALOR));
                ui->txt_Output->append(intStr);
            break;

       case 19: // Dir
            if (Config.Valores.at(CONFIG_DIR)) {
                Informacao.Diretorio = QString::fromStdString(Config.Dir);
            }
            break;

        default:
            break;
        }
    }
}

void MainWindow::Grava_Config_File() {
    QFile           Config_File(Informacao.Diretorio_EXE + "/GeoConfigFile.txt");
    std::ofstream   text_file;
    int             i;

    text_file.open(Config_File.fileName().toStdString(),ios::out);
    text_file << "GEOHAM V0.668 CONFIG FILE\n";

    for (i = 0 ; i < Config.Tipos.size() ; i ++) {
        text_file << Config.Tipos.at(i).toStdString() << "\t" << Config.Valores.at(i) << "\n";
    }

    if (Config.Valores.at(i-1) == 1) {
        text_file << Config.Dir << "\n";
    }


    text_file.close();
}

void MainWindow::Le_Config_File() {
    QFile           Config_File(Informacao.Diretorio_EXE + "/GeoConfigFile.txt");
    std::ifstream   text_file;
    string          Linha,Tipo;
    vector<string>  col1,col2;
    int             i = 0;
    bool            TemDir = false;

    text_file.open(Config_File.fileName().toStdString(),ios::in);

    getline(text_file,Linha);
    while (!text_file.eof()) {
        getline(text_file,Linha);

        size_t  pos = Linha.find('\t');

        string val1,val2;

        val1 = Linha.substr(0,pos);
        val2 = Linha.substr(pos+1,Linha.length() - pos);

        if (val1 == "") break;

        Config.Valores.replace(i,stoi(val2));        
        i++;

        if ((val1 == CONFIG_DIR_TXT) && (Config.Valores.at(i-1) == 1)) {
            Config.TemDir = true;
            break;
        }

//        ui->txt_Output->append(QString::fromStdString(val1) + "-----" + QString::fromStdString(val2));
    }

    if (Config.TemDir) {
        getline(text_file,Linha);
        Config.Dir = Linha;
    }

    text_file.close();
}

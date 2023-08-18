#define USE_STD_NAMESPACE
#define CCUTIL_IMPORTS

#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QScreen>
#include <QtCore>
#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

bool MainWindow::Load_Widgets_Settings(bool Res) {
    QString Filename;
    double x,y,width,height;

    if (Res) {
#ifdef QT_DEBUG
        Filename = "C:/QT_Projects/Diminui/670_QT6/mainwindow - 1280.ui";
#else
        Filename = Informacao.Diretorio_EXE + "/mainwindow - 1280.ui";
#endif
    } else {
#ifdef QT_DEBUG
        Filename = "C:/QT_Projects/Diminui/670_QT6/mainwindow - 720.ui";
#else
        Filename = Informacao.Diretorio_EXE + "/mainwindow - 720.ui";
#endif
    }
    QFile file(Filename);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
            qDebug() << "Cannot read file" << file.errorString();
            QColor tc = ui->txt_Output->textColor();
            ui->txt_Output->setTextColor(QColor("red"));
            ui->txt_Output->append(tr("\n Não foi possível abrir o arquivo [") +
                                   Filename + "]");
            ui->txt_Output->setTextColor(tc);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            return(false);
//            exit(10);
     }

     QXmlStreamReader xmlReader(&file);
     xmlReader.setDevice(&file);

     while (!xmlReader.isEndDocument()) {
        xmlReader.readNext();
        if (xmlReader.isStartElement())  {
            QString name = xmlReader.name().toString();
            QString element;
            if (name == "widget") {
                element = xmlReader.attributes().value("name").toString();
                if (element == "line") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->line->setGeometry(x,y,width,height);
                }
                if (element == "line_2") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->line_2->setGeometry(x,y,width,height);
                }
                if (element == "line_3") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->line_3->setGeometry(x,y,width,height);
                }
                if (element == "line_4") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->line_4->setGeometry(x,y,width,height);
                }

                if (element == "txt_Output") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->txt_Output->setGeometry(x,y,width,height);
                }
                if (element == "Limpa_Quadro") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->Limpa_Quadro->setGeometry(x,y,width,height);
                }

                if (element == "btOpenDirectory") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->btOpenDirectory->setGeometry(x,y,width,height);
                }
                if (element == "txt_Directory") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->txt_Directory->setGeometry(x,y,width,height);
                }
                if (element == "txt_NumEntries") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->txt_NumEntries->setGeometry(x,y,width,height);
                }

                if (element == "txt_Dir_Proc") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->txt_Dir_Proc->setGeometry(x,y,width,height);
                 }
                if (element == "txt_NumEntries_Proc") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->txt_NumEntries_Proc->setGeometry(x,y,width,height);
                }

                if (element == "bt_ProcessarImgs") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->bt_ProcessarImgs->setGeometry(x,y,width,height);
                }
                if (element == "bt_Cancelar_Proc") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->bt_Cancelar_Proc->setGeometry(x,y,width,height);
                }
                if (element == "bt_Mesclar") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->bt_Mesclar->setGeometry(x,y,width,height);
                }

                if (element == "progressBar_Mesclar") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->progressBar_Mesclar->setGeometry(x,y,width,height);
                 }

                if (element == "progressBar_Files") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->progressBar_Files->setGeometry(x,y,width,height);
                 }
                if (element == "progressBar_Proc") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->progressBar_Proc->setGeometry(x,y,width,height);
                 }
                if (element == "progressBar_Result") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->progressBar_Result->setGeometry(x,y,width,height);
                 }


                if (element == "txt_Nome_Imagem") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->txt_Nome_Imagem->setGeometry(x,y,width,height);
                 }
                if (element == "bt_Mostar_Imagem") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->bt_Mostar_Imagem->setGeometry(x,y,width,height);
                }

                if (element == "label_Reducao_4") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->label_Reducao_4->setGeometry(x,y,width,height);
                }
                if (element == "Selected_IMG") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->Selected_IMG->setGeometry(x,y,width,height);
                }
                if (element == "list_Files") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->list_Files->setGeometry(x,y,width,height);
                }

                if (element == "label_Reducao_5") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->label_Reducao_5->setGeometry(x,y,width,height);
                }
                if (element == "Selected_PROC") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->Selected_PROC->setGeometry(x,y,width,height);
                }
                if (element == "list_Files_Proc") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->list_Files_Proc->setGeometry(x,y,width,height);
                }

                if (element == "label_Reducao_7") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->label_Reducao_7->setGeometry(x,y,width,height);
                }
                if (element == "Selected_RESULT") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->Selected_RESULT->setGeometry(x,y,width,height);
                }
                if (element == "list_Files_Result") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->list_Files_Result->setGeometry(x,y,width,height);
                }

                if (element == "tablePontos") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->tablePontos->setGeometry(x,y,width,height);
                }
                if (element == "pushButton") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->pushButton->setGeometry(x,y,width,height);
                }
                ui->tablePontos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                if (element == "label") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->label->setGeometry(x,y,width,height);
                }
                if (element == "lineEdit_Area") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->lineEdit_Area->setGeometry(x,y,width,height);
                }
                if (element == "label_Perimetro") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->label_Perimetro->setGeometry(x,y,width,height);
                }
                if (element == "txt_Dist") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->txt_Dist->setGeometry(x,y,width,height);
                }
                if (element == "checkBox_Linhas") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->checkBox_Linhas->setGeometry(x,y,width,height);
                }
                if (element == "checkBox_Circle") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->checkBox_Circle->setGeometry(x,y,width,height);
                }
                if (element == "CheckBox_Poligono") {
                    GetWidgetGeometry(xmlReader,x,y,width,height);
                    ui->CheckBox_Poligono->setGeometry(x,y,width,height);
                }

            }
       }
   }
   return(true);
}

void MainWindow::GetWidgetGeometry(QXmlStreamReader &xmlReader,double &x,double &y,double &width,double &height)  {
    xmlReader.readNextStartElement();
    while (xmlReader.name().toString() != "x") {
        xmlReader.readNextStartElement();
    }
//    name = xmlReader.name().toString();
    x = xmlReader.readElementText().toDouble();

    xmlReader.readNextStartElement();
    while (xmlReader.name().toString() != "y") {
        xmlReader.readNextStartElement();
    }
//    name = xmlReader.name().toString();
    y = xmlReader.readElementText().toDouble();

    xmlReader.readNextStartElement();
    while (xmlReader.name().toString() != "width") {
        xmlReader.readNextStartElement();
    }
//    name = xmlReader.name().toString();
    width = xmlReader.readElementText().toDouble();

    xmlReader.readNextStartElement();
    while (xmlReader.name().toString() != "height") {
        xmlReader.readNextStartElement();
    }
//    name = xmlReader.name().toString();
    height = xmlReader.readElementText().toDouble();

}

void MainWindow::sscreenChanged(QScreen* Screen) {

    ui->txt_Output->append("   MUDOU DE JANELA!");
    QRect           screenGeometry;
    QString         screenTxt = "";

    screenTxt = "Monitor ativo = ";
    screenGeometry = Screen->geometry();
    int H = screenGeometry.height();
    int W = screenGeometry.width();
    screenTxt += " -> " + QString::number(W) + " x " + QString::number(H);

    ui->txt_Output->append(screenTxt);

    Changed_Screen = true;
    if (H < 1080) {
        on_action1280x720_triggered();
    } else {
        on_action1920x1080_triggered();
    }
}

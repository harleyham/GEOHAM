#include <QMessageBox>

//#include "qpdfwidget.h"
#include "mainwindow.h"
//#include "dialogmanual2.h"

#include <QQuickView>

#include <QGuiApplication>
#include <QQmlApplicationEngine>


//QQmlApplicationEngine engine;

void MainWindow::on_actionSobre_triggered()
{
    dialogSobre  = new DialogSobre(this);
    dialogSobre->show();
    dialogSobre->setWindowTitle("Sobre");

    dialogSobre->limpa();
    dialogSobre->print("    Geo v 0.667");
    dialogSobre->print("    Freeware\n");
    dialogSobre->print(" Polícia Federal\n"
                       " Diretoria Técnico-Científica\n"
                       " \n"
                       " Desenvolvido por\n"
                       "    PCF Harley Angelo de Moraes\n"
                       "\n"
                       "\n"
                       "\n"
                       "   Bibliotecas:\n"
                       " Qt 6.4\n"
                       " OpenCV 4.5.5\n"
                       " zlib 1.2.11\n"
                       " Proj 9.0\n"
                       " GDAL 3.5\n"
                       " GeographicLib 1.52\n"
                       " GEOS 3.11");
}


void MainWindow::on_actionManual_triggered()
{
    QString Arquivo;

#ifdef QT_DEBUG
        #define ArquivoManual  "c:/QT_Projects/Deploy/667/Manual_0667.pdf"
    Arquivo = "c:/QT_Projects/Deploy/667_QT6/Manual_0667.pdf";
#else
        #define ArquivoManual = Informacao.Diretorio_EXE + "/Manual_0667.pdf";
    Arquivo = Informacao.Diretorio_EXE + "/Manual_0667.pdf";
#endif


//    QUrl toLoad = QUrl::fromUserInput("c:/QT_Projects/667/Manual_0667.pdf");
    QUrl toLoad = QUrl::fromUserInput(Arquivo);


//    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///pdfviewer/viewer.qml")));
    engine.rootObjects().first()->setProperty("width", 700);
    engine.rootObjects().first()->setProperty("height", 1000);
    engine.rootObjects().first()->setProperty("source", toLoad);

/*
    QQuickView *view = new QQuickView;

    view->setProperty("source",toLoad);
    view->setResizeMode(QQuickView::SizeViewToRootObject);
    view->setSource(QUrl(QStringLiteral("qrc:///pdfviewer/viewer.qml")));

    view->show();
*/


/*

    QString     ArquivoManual;

    dialogManual2 = new DialogManual2(this);
    m_pPdfWidget = new QPdfWidget();
    m_pPdfWidget->setParent(dialogManual2);
#ifdef QT_DEBUG
        ArquivoManual = "c:/QT_Projects/Deploy/Geo_665/Manual_0665.pdf";
#else
        ArquivoManual = Informacao.Diretorio_EXE + "/Manual_0665.pdf";
#endif

        if ( m_pPdfWidget->loadFile(ArquivoManual)) {
                dialogManual2->show();
                dialogManual2->setWindowTitle("Manual V 0.667");

                m_pPdfWidget->resize(1495,985);
                m_pPdfWidget->setPage(1);
                m_pPdfWidget->zoomReset(1.4);
        } else {
            QMessageBox::information(0, "Erro", "Não foi possível abrir aquivo do Manual");
//            dialogManual2->close();
            return;
        }
*/
}

/*
void MainWindow::on_actionManual_triggered()
{
    dialogManual  = new DialogManual(this);
    dialogManual->show();
    dialogManual->setWindowTitle("Manual");

    dialogManual->limpa();

    QString ArquivoManual;
#ifdef QT_DEBUG
        ArquivoManual = "c:/QT_Projects/Deploy/Geo_641/Manual_0641.txt";
#else
        ArquivoManual = Informacao.Diretorio_EXE + "/Manual_0641.txt";
#endif

    QFile file(ArquivoManual);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
        dialogManual->close();
        return;
    }

    QTextStream in(&file);

    dialogManual->showFile(ArquivoManual);

    file.close();
}
*/


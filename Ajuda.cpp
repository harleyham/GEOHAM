#include <QMessageBox>

//#include "qpdfwidget.h"
#include "mainwindow.h"
//#include "dialogmanual2.h"

#include "pdf.h"

//#include "mythread.h"

#include <QQuickView>

#include <QGuiApplication>
//#include <QQmlApplicationEngine>


//QQmlApplicationEngine engine;

void MainWindow::on_actionSobre_triggered()
{
    dialogSobre  = new DialogSobre(this);
    dialogSobre->show();
//    dialogSobre->setWindowTitle(tr("Sobre"));

    dialogSobre->limpa();
    dialogSobre->print("    Geo v 0.670");
    dialogSobre->print("    Freeware\n");
    dialogSobre->print(tr(" Polícia Federal"));
    dialogSobre->print(tr(" Diretoria Técnico-Científica"));
    dialogSobre->print(tr(" Desenvolvido por"));
    dialogSobre->print(tr("   PCF Harley Angelo de Moraes\n"));
    dialogSobre->print(tr("   Bibliotecas:"));
    dialogSobre->print(" Qt 6.4\n"
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

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height() - 300;
    int width = height;// * 0.8;

#ifdef QT_DEBUG
        #define ArquivoManual  "c:/QT_Projects/Deploy/670/Manual_0670.pdf"
    Arquivo = "c:/QT_Projects/Deploy/670/Manual_0670.pdf";
#else
        #define ArquivoManual = Informacao.Diretorio_EXE + "/Manual_0670.pdf";
    Arquivo = Informacao.Diretorio_EXE + "/Manual_0670.pdf";
#endif

    dialog_PDF = new PDF_WINDOW(this);
    dialog_PDF->show();
    dialog_PDF->open(QUrl::fromLocalFile("C:/QT_Projects/Deploy/670/Manual_0670.pdf"));
    dialog_PDF->setGeometry(50,50,width,height);
}

/*
    QUrl toLoad = QUrl::fromUserInput(Arquivo);


    mThread_Ajuda.Arquivo = Arquivo;
    mThread_Ajuda.height = height;
    mThread_Ajuda.width = width;
    mThread_Ajuda.toLoad = toLoad;
    mThread_Ajuda.thread = engine.thread();
//    engine.load(QUrl("qrc:///pdfviewer/viewer.qml"));
    mThread_Ajuda.P_engine = &engine;
    mThread_Ajuda.start();
*/

/*
    engine.load(QUrl(QStringLiteral("qrc:///pdfviewer/viewer.qml")));

    engine.rootObjects().first()->setProperty("width", width);

    engine.rootObjects().first()->setProperty("height", height);

    engine.rootObjects().first()->setProperty("title",Arquivo);

    engine.rootObjects().first()->setProperty("source", toLoad);
}
*/
/*
void MainWindow::on_actionManual_triggered()
{
    QString Arquivo;

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height() - 300;
    int width = height * 0.9;

#ifdef QT_DEBUG
        #define ArquivoManual  "c:/QT_Projects/Deploy/668/Manual_0668.pdf"
    Arquivo = "c:/QT_Projects/Deploy/668/Manual_0668.pdf";
#else
        #define ArquivoManual = Informacao.Diretorio_EXE + "/Manual_0668.pdf";
    Arquivo = Informacao.Diretorio_EXE + "/Manual_0668.pdf";
#endif

    QUrl toLoad = QUrl::fromUserInput(Arquivo);

    engine.load(QUrl(QStringLiteral("qrc:///pdfviewer/viewer.qml")));

    engine.rootObjects().first()->setProperty("width", width);
    engine.rootObjects().first()->setProperty("height", height);

    engine.rootObjects().first()->setProperty("title",Arquivo);

    engine.rootObjects().first()->setProperty("source", toLoad);
}
*/

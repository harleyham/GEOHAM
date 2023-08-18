#include <QApplication>
#include "mainwindow.h"
#include "mytimer.h"

#include "stdoutredirector.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>

MainWindow *Window;

Info_Dir::Info_Dir()
{
    Reducao = 10;
    Escala1 = 1.0;
    Escala2 = 1.0;
}

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
    break;
    }
    QFile outFile("c:\\QT_Projects\\log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << "\n";
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    Window = &w;

    qInstallMessageHandler(myMessageHandler);
    // a.setQuitOnLastWindowClosed(false);
    // w.setAttribute( Qt::WA_DeleteOnClose );
    w.show();

    return a.exec();

}

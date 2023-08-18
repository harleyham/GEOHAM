#ifndef PDF_H
#define PDF_H

#include <QMainWindow>

/*
QT_BEGIN_NAMESPACE
namespace Ui { class PDF_WINDOW; }
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui {
class PDF_WINDOW : public QMainWindow;
}
*/

namespace Ui {
class   PDF_WINDOW;
}

class QPdfDocument;
class QPdfView;
class QSpinBox;
//QT_END_NAMESPACE

class ZoomSelector;

class PDF_WINDOW : public QMainWindow
{
    Q_OBJECT


public:
    explicit PDF_WINDOW(QMainWindow *parent = nullptr);
    ~PDF_WINDOW();

public slots:
    void open(const QUrl &docLocation);

private slots:
    void bookmarkSelected(const QModelIndex &index);
    void pageSelected(int page);

    // action handlers
    void on_actionOpen_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionPrevious_Page_triggered();
    void on_actionNext_Page_triggered();
    void on_actionContinuous_triggered();
    void on_actionBack_triggered();
    void on_actionForward_triggered();

    void on_actionFirst_triggered();

    void on_actionLast_triggered();

private:
    Ui::PDF_WINDOW *ui;
    ZoomSelector *m_zoomSelector;
    QSpinBox *m_pageSelector;

    QPdfDocument *m_document;

};

#endif // PDF_H

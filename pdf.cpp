#include "pdf.h"
#include "ui_pdf.h"

#include "zoomselector.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <QPdfBookmarkModel>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QtMath>

#include <QWidget>
#include <QMainWindow>

const qreal zoomMultiplier = qSqrt(2.0);

PDF_WINDOW::PDF_WINDOW(QMainWindow *parent)
    : QMainWindow(parent)
    , ui(new Ui::PDF_WINDOW)
    , m_zoomSelector(new ZoomSelector(this))
    , m_pageSelector(new QSpinBox(this))
    , m_document(new QPdfDocument(this))
{
    ui->setupUi(this);

    m_zoomSelector->setMaximumWidth(150);
    ui->mainToolBar->insertWidget(ui->actionZoom_In, m_zoomSelector);

    ui->mainToolBar->insertWidget(ui->actionForward, m_pageSelector);
    connect(m_pageSelector, &QSpinBox::valueChanged, this, &PDF_WINDOW::pageSelected);
    auto nav = ui->pdfView->pageNavigator();
    connect(nav, &QPdfPageNavigator::currentPageChanged, m_pageSelector, &QSpinBox::setValue);
    connect(nav, &QPdfPageNavigator::backAvailableChanged, ui->actionBack, &QAction::setEnabled);
    connect(nav, &QPdfPageNavigator::forwardAvailableChanged, ui->actionForward, &QAction::setEnabled);

    connect(m_zoomSelector, &ZoomSelector::zoomModeChanged, ui->pdfView, &QPdfView::setZoomMode);
    connect(m_zoomSelector, &ZoomSelector::zoomFactorChanged, ui->pdfView, &QPdfView::setZoomFactor);
    m_zoomSelector->reset();

    QPdfBookmarkModel *bookmarkModel = new QPdfBookmarkModel(this);
    bookmarkModel->setDocument(m_document);

    ui->bookmarkView->setModel(bookmarkModel);
    connect(ui->bookmarkView, SIGNAL(activated(QModelIndex)), this, SLOT(bookmarkSelected(QModelIndex)));
//    connect(ui->bookmarkView, &QAbstractItemView::activated, this, &PDF_WINDOW::bookmarkSelected);

    ui->pdfView->setDocument(m_document);

    connect(ui->pdfView, &QPdfView::zoomFactorChanged,
            m_zoomSelector, &ZoomSelector::setZoomFactor);

    ui->bookmarkView->setEnabled(true);
    ui->tabWidget->setTabEnabled(1, false); // disable 'Pages' tab for now
    ui->tabWidget->setTabEnabled(0, true);
    m_zoomSelector->Fit();
    m_zoomSelector->zoomModeChanged(QPdfView::ZoomMode::FitInView);

    ui->actionContinuous->setChecked(true);  // Permite o scroll com o mouse
    on_actionContinuous_triggered();

    ui->actionOpen->setEnabled(false);

}

PDF_WINDOW::~PDF_WINDOW()
{
    delete ui;
}

void PDF_WINDOW::open(const QUrl &docLocation)
{
    if (docLocation.isLocalFile()) {
        m_document->load(docLocation.toLocalFile());
        const auto documentTitle = m_document->metaData(QPdfDocument::MetaDataField::Title).toString();
//        setWindowTitle(!documentTitle.isEmpty() ? documentTitle : QStringLiteral("PDF Viewer"));
          setWindowTitle(!documentTitle.isEmpty() ? QStringLiteral("Manual") : QStringLiteral("PDF Viewer"));
        pageSelected(0);
        m_pageSelector->setMaximum(m_document->pageCount() - 1);
    } else {
//        qCDebug(lcExample) << docLocation << "is not a valid local file";
        QMessageBox::critical(this, tr("Failed to open"), tr("%1 is not a valid local file").arg(docLocation.toString()));
    }
//    qCDebug(lcExample) << docLocation;
}

void PDF_WINDOW::bookmarkSelected(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    const int page = index.data(int(QPdfBookmarkModel::Role::Page)).toInt();
    const qreal zoomLevel = index.data(int(QPdfBookmarkModel::Role::Level)).toReal();
    ui->pdfView->pageNavigator()->jump(page, {}, zoomLevel);
}

void PDF_WINDOW::pageSelected(int page)
{
    auto nav = ui->pdfView->pageNavigator();
    nav->jump(page, {}, nav->currentZoom());
}

void PDF_WINDOW::on_actionOpen_triggered()
{
    QUrl toOpen = QFileDialog::getOpenFileUrl(this, tr("Choose a PDF"), QUrl(), "Portable Documents (*.pdf)");
    if (toOpen.isValid())
        open(toOpen);
}

void PDF_WINDOW::on_actionQuit_triggered()
{
    QApplication::quit();
}

void PDF_WINDOW::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About PdfViewer"),
        tr("An example using QPdfDocument"));
}

void PDF_WINDOW::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void PDF_WINDOW::on_actionZoom_In_triggered()
{
    ui->pdfView->setZoomFactor(ui->pdfView->zoomFactor() * zoomMultiplier);
}

void PDF_WINDOW::on_actionZoom_Out_triggered()
{
    ui->pdfView->setZoomFactor(ui->pdfView->zoomFactor() / zoomMultiplier);
}

void PDF_WINDOW::on_actionPrevious_Page_triggered()
{
    auto nav = ui->pdfView->pageNavigator();
    nav->jump(nav->currentPage() - 1, {}, nav->currentZoom());
}

void PDF_WINDOW::on_actionNext_Page_triggered()
{
    auto nav = ui->pdfView->pageNavigator();
    nav->jump(nav->currentPage() + 1, {}, nav->currentZoom());
}

void PDF_WINDOW::on_actionContinuous_triggered()
{
    ui->pdfView->setPageMode(ui->actionContinuous->isChecked() ?
                                 QPdfView::PageMode::MultiPage :
                                 QPdfView::PageMode::SinglePage);
}

void PDF_WINDOW::on_actionBack_triggered()
{
    ui->pdfView->pageNavigator()->back();
}

void PDF_WINDOW::on_actionForward_triggered()
{
    ui->pdfView->pageNavigator()->forward();
}

void PDF_WINDOW::on_actionFirst_triggered()
{
    auto nav = ui->pdfView->pageNavigator();
    nav->jump(0, {}, nav->currentZoom());
}


void PDF_WINDOW::on_actionLast_triggered()
{
    auto nav = ui->pdfView->pageNavigator();
    int Last = m_document->pageCount();
    nav->jump(Last - 1, {}, nav->currentZoom());
}

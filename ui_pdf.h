/********************************************************************************
** Form generated from reading UI file 'pdf.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PDF_H
#define UI_PDF_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtPdfWidgets/QPdfView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PDF_WINDOW
{
public:
    QAction *actionOpen;
    QAction *actionQuit;
    QAction *actionAbout;
    QAction *actionAbout_Qt;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *actionPrevious_Page;
    QAction *actionNext_Page;
    QAction *actionContinuous;
    QAction *actionBack;
    QAction *actionForward;
    QAction *actionFirst;
    QAction *actionLast;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QTabWidget *tabWidget;
    QWidget *bookmarkTab;
    QVBoxLayout *verticalLayout_3;
    QTreeView *bookmarkView;
    QWidget *pagesTab;
    QPdfView *pdfView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuView;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PDF_WINDOW)
    {
        if (PDF_WINDOW->objectName().isEmpty())
            PDF_WINDOW->setObjectName("PDF_WINDOW");
        PDF_WINDOW->resize(700, 600);
        PDF_WINDOW->setUnifiedTitleAndToolBarOnMac(true);
        actionOpen = new QAction(PDF_WINDOW);
        actionOpen->setObjectName("actionOpen");
        actionOpen->setCheckable(false);
        actionOpen->setEnabled(true);
        QIcon icon;
        QString iconThemeName = QString::fromUtf8("document-open");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QString::fromUtf8(":/icons/images/document-open.svgz"), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionOpen->setIcon(icon);
        actionQuit = new QAction(PDF_WINDOW);
        actionQuit->setObjectName("actionQuit");
        QIcon icon1;
        iconThemeName = QString::fromUtf8("application-exit");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon1 = QIcon::fromTheme(iconThemeName);
        } else {
            icon1.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionQuit->setIcon(icon1);
        actionAbout = new QAction(PDF_WINDOW);
        actionAbout->setObjectName("actionAbout");
        QIcon icon2;
        iconThemeName = QString::fromUtf8("help-about");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon2 = QIcon::fromTheme(iconThemeName);
        } else {
            icon2.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionAbout->setIcon(icon2);
        actionAbout_Qt = new QAction(PDF_WINDOW);
        actionAbout_Qt->setObjectName("actionAbout_Qt");
        actionZoom_In = new QAction(PDF_WINDOW);
        actionZoom_In->setObjectName("actionZoom_In");
        QIcon icon3;
        iconThemeName = QString::fromUtf8("zoom-in");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon3 = QIcon::fromTheme(iconThemeName);
        } else {
            icon3.addFile(QString::fromUtf8(":/icons/images/zoom-in.svgz"), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionZoom_In->setIcon(icon3);
        actionZoom_Out = new QAction(PDF_WINDOW);
        actionZoom_Out->setObjectName("actionZoom_Out");
        QIcon icon4;
        iconThemeName = QString::fromUtf8("zoom-out");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon4 = QIcon::fromTheme(iconThemeName);
        } else {
            icon4.addFile(QString::fromUtf8(":/icons/images/zoom-out.svgz"), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionZoom_Out->setIcon(icon4);
        actionPrevious_Page = new QAction(PDF_WINDOW);
        actionPrevious_Page->setObjectName("actionPrevious_Page");
        QIcon icon5;
        iconThemeName = QString::fromUtf8("go-previous-view-page");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon5 = QIcon::fromTheme(iconThemeName);
        } else {
            icon5.addFile(QString::fromUtf8(":/icons/images/go-previous-view-page.svgz"), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionPrevious_Page->setIcon(icon5);
        actionNext_Page = new QAction(PDF_WINDOW);
        actionNext_Page->setObjectName("actionNext_Page");
        QIcon icon6;
        iconThemeName = QString::fromUtf8("go-next-view-page");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon6 = QIcon::fromTheme(iconThemeName);
        } else {
            icon6.addFile(QString::fromUtf8(":/icons/images/go-next-view-page.svgz"), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionNext_Page->setIcon(icon6);
        actionContinuous = new QAction(PDF_WINDOW);
        actionContinuous->setObjectName("actionContinuous");
        actionContinuous->setCheckable(true);
        QIcon icon7;
        iconThemeName = QString::fromUtf8("applications-office");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon7 = QIcon::fromTheme(iconThemeName);
        } else {
            icon7.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionContinuous->setIcon(icon7);
        actionBack = new QAction(PDF_WINDOW);
        actionBack->setObjectName("actionBack");
        actionBack->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/images/go-previous-view.svgz"), QSize(), QIcon::Normal, QIcon::Off);
        actionBack->setIcon(icon8);
        actionForward = new QAction(PDF_WINDOW);
        actionForward->setObjectName("actionForward");
        actionForward->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/images/go-next-view.svgz"), QSize(), QIcon::Normal, QIcon::Off);
        actionForward->setIcon(icon9);
        actionFirst = new QAction(PDF_WINDOW);
        actionFirst->setObjectName("actionFirst");
        actionLast = new QAction(PDF_WINDOW);
        actionLast->setObjectName("actionLast");
        centralWidget = new QWidget(PDF_WINDOW);
        centralWidget->setObjectName("centralWidget");
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(centralWidget);
        widget->setObjectName("widget");
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        splitter = new QSplitter(widget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName("tabWidget");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setTabPosition(QTabWidget::West);
        tabWidget->setDocumentMode(false);
        bookmarkTab = new QWidget();
        bookmarkTab->setObjectName("bookmarkTab");
        verticalLayout_3 = new QVBoxLayout(bookmarkTab);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(2, 2, 2, 2);
        bookmarkView = new QTreeView(bookmarkTab);
        bookmarkView->setObjectName("bookmarkView");
        sizePolicy.setHeightForWidth(bookmarkView->sizePolicy().hasHeightForWidth());
        bookmarkView->setSizePolicy(sizePolicy);
        bookmarkView->setHeaderHidden(true);

        verticalLayout_3->addWidget(bookmarkView);

        tabWidget->addTab(bookmarkTab, QString());
        pagesTab = new QWidget();
        pagesTab->setObjectName("pagesTab");
        tabWidget->addTab(pagesTab, QString());
        splitter->addWidget(tabWidget);
        pdfView = new QPdfView(splitter);
        pdfView->setObjectName("pdfView");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(10);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pdfView->sizePolicy().hasHeightForWidth());
        pdfView->setSizePolicy(sizePolicy1);
        splitter->addWidget(pdfView);

        verticalLayout_2->addWidget(splitter);


        verticalLayout->addWidget(widget);

        PDF_WINDOW->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PDF_WINDOW);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 700, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName("menuHelp");
        menuView = new QMenu(menuBar);
        menuView->setObjectName("menuView");
        PDF_WINDOW->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PDF_WINDOW);
        mainToolBar->setObjectName("mainToolBar");
        mainToolBar->setMovable(false);
        mainToolBar->setFloatable(false);
        PDF_WINDOW->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(PDF_WINDOW);
        statusBar->setObjectName("statusBar");
        PDF_WINDOW->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionQuit);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionAbout_Qt);
        menuView->addAction(actionZoom_In);
        menuView->addAction(actionZoom_Out);
        menuView->addSeparator();
        menuView->addAction(actionFirst);
        menuView->addAction(actionLast);
        menuView->addAction(actionPrevious_Page);
        menuView->addAction(actionNext_Page);
        menuView->addSeparator();
        menuView->addAction(actionContinuous);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionZoom_Out);
        mainToolBar->addAction(actionZoom_In);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionBack);
        mainToolBar->addAction(actionForward);

        retranslateUi(PDF_WINDOW);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PDF_WINDOW);
    } // setupUi

    void retranslateUi(QMainWindow *PDF_WINDOW)
    {
        PDF_WINDOW->setWindowTitle(QCoreApplication::translate("PDF_WINDOW", "PDF Viewer", nullptr));
        actionOpen->setText(QCoreApplication::translate("PDF_WINDOW", "Open...", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("PDF_WINDOW", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionQuit->setText(QCoreApplication::translate("PDF_WINDOW", "Quit", nullptr));
#if QT_CONFIG(shortcut)
        actionQuit->setShortcut(QCoreApplication::translate("PDF_WINDOW", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAbout->setText(QCoreApplication::translate("PDF_WINDOW", "About", nullptr));
        actionAbout_Qt->setText(QCoreApplication::translate("PDF_WINDOW", "About Qt", nullptr));
        actionZoom_In->setText(QCoreApplication::translate("PDF_WINDOW", "Zoom In", nullptr));
#if QT_CONFIG(shortcut)
        actionZoom_In->setShortcut(QCoreApplication::translate("PDF_WINDOW", "Ctrl+=", nullptr));
#endif // QT_CONFIG(shortcut)
        actionZoom_Out->setText(QCoreApplication::translate("PDF_WINDOW", "Zoom Out", nullptr));
#if QT_CONFIG(shortcut)
        actionZoom_Out->setShortcut(QCoreApplication::translate("PDF_WINDOW", "Ctrl+-", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPrevious_Page->setText(QCoreApplication::translate("PDF_WINDOW", "Previous Page", nullptr));
#if QT_CONFIG(shortcut)
        actionPrevious_Page->setShortcut(QCoreApplication::translate("PDF_WINDOW", "PgUp", nullptr));
#endif // QT_CONFIG(shortcut)
        actionNext_Page->setText(QCoreApplication::translate("PDF_WINDOW", "Next Page", nullptr));
#if QT_CONFIG(shortcut)
        actionNext_Page->setShortcut(QCoreApplication::translate("PDF_WINDOW", "PgDown", nullptr));
#endif // QT_CONFIG(shortcut)
        actionContinuous->setText(QCoreApplication::translate("PDF_WINDOW", "Continuous", nullptr));
        actionBack->setText(QCoreApplication::translate("PDF_WINDOW", "Back", nullptr));
#if QT_CONFIG(tooltip)
        actionBack->setToolTip(QCoreApplication::translate("PDF_WINDOW", "back to previous view", nullptr));
#endif // QT_CONFIG(tooltip)
        actionForward->setText(QCoreApplication::translate("PDF_WINDOW", "Forward", nullptr));
#if QT_CONFIG(tooltip)
        actionForward->setToolTip(QCoreApplication::translate("PDF_WINDOW", "forward to next view", nullptr));
#endif // QT_CONFIG(tooltip)
        actionFirst->setText(QCoreApplication::translate("PDF_WINDOW", "First", nullptr));
#if QT_CONFIG(shortcut)
        actionFirst->setShortcut(QCoreApplication::translate("PDF_WINDOW", "Home", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLast->setText(QCoreApplication::translate("PDF_WINDOW", "Last", nullptr));
#if QT_CONFIG(shortcut)
        actionLast->setShortcut(QCoreApplication::translate("PDF_WINDOW", "End", nullptr));
#endif // QT_CONFIG(shortcut)
        tabWidget->setTabText(tabWidget->indexOf(bookmarkTab), QCoreApplication::translate("PDF_WINDOW", "Bookmarks", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(pagesTab), QCoreApplication::translate("PDF_WINDOW", "Pages", nullptr));
        menuFile->setTitle(QCoreApplication::translate("PDF_WINDOW", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("PDF_WINDOW", "Help", nullptr));
        menuView->setTitle(QCoreApplication::translate("PDF_WINDOW", "View", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PDF_WINDOW: public Ui_PDF_WINDOW {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PDF_H

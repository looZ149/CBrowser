#include "mainWindow.h"
#include "bookMarks.h"
#include <QWebEngineView>
#include <QToolBar>
#include <QStatusBar>
#include <QLineEdit>
#include <QMenuBar>
#include <QApplication>
#include <QDockWidget>
#include <string>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("CBrowser");
    resize(1200, 800);
    setupUI();
    setupConnections();
}

MainWindow::~MainWindow() {
    //Qt object tree handles m_view and m_urlBar (i hope?)
}

void MainWindow::setupUI() {
    m_view = new QWebEngineView(this);
    setCentralWidget(m_view);
    m_view->load(QUrl("https://www.jofacademy.eu"));

    //Sidebar
    auto* sideBar = new QDockWidget(m_view);
    sideBar->setDockLocation(Qt::LeftDockWidgetArea);
    sideBar->setWindowTitle("Side bar");
    addDockWidget(Qt::LeftDockWidgetArea, sideBar);


    //Menu Bar
    auto* fileMenu = menuBar()->addMenu("&Browser");
    auto* bookMarks = fileMenu->addAction("Bookmarks");
    auto* quitAction = fileMenu->addAction("Quit");

    //We can't access "app" here directly, qApp works, or just close the Window works probably too?
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    connect(bookMarks, &QAction::triggered, [this]()
    {
        std::string url = m_urlBar->text().toStdString();
        CBrowserBookMarks::SafeBookMark(url);
    });

    // Navbar
    auto* navBar = addToolBar("Navigation");
    navBar->addSeparator();

    m_urlBar = new QLineEdit();
    m_urlBar->setPlaceholderText("Enter URL");
    m_urlBar->setClearButtonEnabled(true);
    m_urlBar->setMinimumWidth(600);
    navBar->addWidget(m_urlBar);

    m_backAction = navBar->addAction("Back");
    m_forwardAction = navBar->addAction("Forward");
    m_reloadAction = navBar->addAction("Reload");

    connect(m_backAction, &QAction::triggered, m_view, &QWebEngineView::back);
    connect(m_reloadAction, &QAction::triggered, m_view, &QWebEngineView::forward);
    connect(m_reloadAction, &QAction::triggered, m_view, &QWebEngineView::reload);

    statusBar()->showMessage("Ready");

}

void MainWindow::setupConnections() {

    //UrlBar -> WebView
    connect(m_urlBar, &QLineEdit::returnPressed, [this]() {
        m_view->load(QUrl::fromUserInput(m_urlBar->text()));
    });

    //WebView -> UrlBar

    connect(m_view, &QWebEngineView::urlChanged, [this](const QUrl &url) {
        m_urlBar->setText(url.toString());
    });

}

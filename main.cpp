#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QTabWidget>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineDownloadRequest>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineView>

class BrowserTab : public QWidget {
    Q_OBJECT
public:
    BrowserTab(QWebEngineProfile *profile, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        m_view = new QWebEngineView();
        m_page = new QWebEnginePage(profile, m_view);
        m_view->setPage(m_page);

        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(m_view);

        // handle links that open new windows (target=_blank)
        connect(m_page, &QWebEnginePage::createWindow, this, &BrowserTab::handleCreateWindow);
    }

    QWebEngineView* view() const { return m_view; }
    QWebEnginePage* page() const { return m_page; }

signals:
    void requestNewTab(QUrl url);

private slots:
    QWebEngineView* handleCreateWindow(QWebEnginePage::WebWindowType type) {
        // Create a temporary view; we emit a signal to let the app open a proper tab
        auto temp = new QWebEngineView();
        connect(temp, &QWebEngineView::urlChanged, this, [this,temp](const QUrl &u){
            // Once URL is set, emit requestNewTab and schedule deletion of temp
            emit requestNewTab(u);
            temp->deleteLater();
        });
        return temp;
    }

private:
    QWebEngineView *m_view;
    QWebEnginePage *m_page;
};

class BrowserWindow : public QMainWindow {
    Q_OBJECT
public:
    BrowserWindow() {
        setWindowTitle("QuteBrowser");

        // Profile — shared across tabs (use different profile for incognito)
        profile = new QWebEngineProfile("QuteProfile", this);
        profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
        profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);

        // Central tab widget
        tabs = new QTabWidget();
        tabs->setTabsClosable(true);
        setCentralWidget(tabs);

        // Toolbar
        auto *toolbar = addToolBar("Navigation");
        QAction *backAct = toolbar->addAction("◀");
        QAction *forwardAct = toolbar->addAction("▶");
        QAction *reloadAct = toolbar->addAction("⟳");
        address = new QLineEdit();
        address->setPlaceholderText("https://...");
        address->setClearButtonEnabled(true);
        toolbar->addWidget(address);
        QAction *newTabAct = toolbar->addAction("+");

        // Connect actions
        connect(newTabAct, &QAction::triggered, this, &BrowserWindow::openBlankTab);
        connect(backAct, &QAction::triggered, this, &BrowserWindow::navigateBack);
        connect(forwardAct, &QAction::triggered, this, &BrowserWindow::navigateForward);
        connect(reloadAct, &QAction::triggered, this, &BrowserWindow::reloadPage);
        connect(address, &QLineEdit::returnPressed, this, &BrowserWindow::loadFromAddress);

        connect(tabs, &QTabWidget::tabCloseRequested, this, &BrowserWindow::closeTab);
        connect(tabs, &QTabWidget::currentChanged, this, &BrowserWindow::tabChanged);

        // download handling
        connect(profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::onDownloadRequested);

        // open a first tab
        openBlankTab(QUrl("https://example.com"));
    }

protected:
    void closeEvent(QCloseEvent *ev) override {
        // Could ask to confirm active downloads, unsaved state, etc.
        QMainWindow::closeEvent(ev);
    }

private slots:
    void openBlankTab() { openBlankTab(QUrl("about:blank")); }
    void openBlankTab(QUrl url) {
        BrowserTab *tab = new BrowserTab(profile, this);
        int idx = tabs->addTab(tab, "New Tab");
        tabs->setCurrentIndex(idx);

        // connect signals for navigation & title updates
        connect(tab->view(), &QWebEngineView::urlChanged, this, [this,tab](const QUrl &u){
            int i = tabs->indexOf(tab);
            if (i != -1) tabs->setTabText(i, u.host().isEmpty() ? u.toString() : u.host());
            if (tabs->currentWidget() == tab) address->setText(u.toString());
        });
        connect(tab->view(), &QWebEngineView::titleChanged, this, [this,tab](const QString &t){
            int i = tabs->indexOf(tab);
            if (i != -1) tabs->setTabText(i, t);
        });

        // handle new-window requests from the tab
        connect(tab, &BrowserTab::requestNewTab, this, [this](QUrl u) {
            openBlankTab(u);
        });

        tab->view()->setUrl(url);
    }

    void closeTab(int index) {
        QWidget *w = tabs->widget(index);
        if (!w) return;
        tabs->removeTab(index);
        w->deleteLater();
        if (tabs->count() == 0) openBlankTab(QUrl("https://example.com"));
    }

    void tabChanged(int idx) {
        BrowserTab *tab = qobject_cast<BrowserTab*>(tabs->widget(idx));
        if (!tab) return;
        QUrl u = tab->view()->url();
        address->setText(u.toString());
    }

    void loadFromAddress() {
        QString text = address->text();
        QUrl url = QUrl::fromUserInput(text);
        if (!url.isValid()) {
            QMessageBox::warning(this, "Invalid URL", "The address appears invalid.");
            return;
        }
        BrowserTab *tab = qobject_cast<BrowserTab*>(tabs->currentWidget());
        if (tab) tab->view()->setUrl(url);
    }

    void navigateBack() {
        auto tab = qobject_cast<BrowserTab*>(tabs->currentWidget());
        if (tab && tab->view()->history()->canGoBack()) tab->view()->back();
    }
    void navigateForward() {
        auto tab = qobject_cast<BrowserTab*>(tabs->currentWidget());
        if (tab && tab->view()->history()->canGoForward()) tab->view()->forward();
    }
    void reloadPage() {
        auto tab = qobject_cast<BrowserTab*>(tabs->currentWidget());
        if (tab) tab->view()->reload();
    }

    void onDownloadRequested(QWebEngineDownloadItem *download) {
        QString suggested = download->path();
        QString filename = QFileDialog::getSaveFileName(this, "Save File", suggested);
        if (filename.isEmpty()) {
            download->cancel();
            return;
        }
        download->setPath(filename);
        download->accept();

        connect(download, &QWebEngineDownloadItem::downloadProgress, this, [download](qint64 rec, qint64 total){
            // Could update a progress UI
            // qDebug() << "Download progress:" << rec << "/" << total;
        });
        connect(download, &QWebEngineDownloadItem::finished, this, [this,download](){
            if (download->state() == QWebEngineDownloadItem::Completed) {
                QMessageBox::information(this, "Download completed", "Saved to: " + download->path());
            } else if (download->state() == QWebEngineDownloadItem::Cancelled) {
                QMessageBox::information(this, "Download cancelled", "Download was cancelled.");
            } else {
                QMessageBox::warning(this, "Download failed", "Download failed.");
            }
        });
    }

private:
    QTabWidget *tabs;
    QLineEdit *address;
    QWebEngineProfile *profile;
};

#include "main.moc"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Optional: tweak flags or set organization/application metadata
    QApplication::setApplicationName("QuteBrowser");

    BrowserWindow w;
    w.resize(1200, 800);
    w.show();
    return app.exec();
}

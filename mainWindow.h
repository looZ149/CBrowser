#ifndef CBROWSER_MAINWINDOW_H
#define CBROWSER_MAINWINDOW_H

#include <QMainWindow>

class QWebEngineView;
class QLineEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void setupConnections();

    QWebEngineView* m_view;
    QLineEdit* m_urlBar;

    QAction *m_backAction;
    QAction *m_forwardAction;
    QAction *m_reloadAction;
};

#endif //CBROWSER_MAINWINDOW_H
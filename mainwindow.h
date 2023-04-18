
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QListWidget>
#include <QString>
#include <QDir>
#include <QThread>

#include <archiveinoutdialog.h>
#include <loadingpopup.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setFilePath(QDir path);
    void setoutputPath(QDir path);

protected:
    void loadFile();

private:
    Ui::MainWindow *ui;

    QListWidget *listWidget;

    QString filePath;
    QString outputPath;

    QLabel *statusLabel;

    loadingPopUp *loading_pop_up;
    archiveInOutDialog *inOut;

private slots:
    void on_actionExit_triggered();

    void on_actionLoad_triggered();

    void on_actionExport_triggered();

    void on_actionAbout_triggered();

public slots:
    void receivePaths(QString archive);

signals:
    void sendExtractFinished();
};

#endif // MAINWINDOW_H


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <QListWidget>
#include <QString>
#include <QDir>
#include <QThread>
#include <QTreeWidget>

#include <test_case_types.h>
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

private:
    void resizeEvent(QResizeEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

    void loadFile();
    void loadFile(QString filePath);

    void addTreeRoot(const test &t);
    void addTreeChild(QTreeWidgetItem *parent, const test_result &r);

private:
    Ui::MainWindow *ui;

    QTreeWidget *treeWidget;

    QString filePath;
    QString outputPath;

    QLabel *statusLabel;
    QLabel *picLabel;
    QLabel *guideLabel;

    loadingPopUp *loading_pop_up;
    archiveInOutDialog *inOut;

public slots:
    void receivePaths(QString archive);

private slots:
    void on_actionExit_triggered();

    void on_actionLoad_triggered();

    void on_actionAbout_triggered();

    void recieveDragNDropComplete(QStringList files);

signals:
    void sendExtractFinished();
    void sendDragNDropComplete(QStringList files);
};

#endif // MAINWINDOW_H

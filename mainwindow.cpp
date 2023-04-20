
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QList>
#include <QPushButton>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QBrush>
#include <QColor>
#include <QPainter>

#include <filesystem>

#include <aboutdialog.h>
#include <customtreewidget.h>

#include <archiveparser.h>

#include "zip_file.hpp"

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    this->setAcceptDrops(true);

    this->tabWidget = nullptr;

    QPixmap map(":/img/img/file_open_img.png");
    this->picLabel = new QLabel(this);

    QRect rect(this->rect().center().x() - (map.size().width() / 2),
               this->rect().center().y() - (map.size().height() / 2),
               map.size().width(),
               map.size().height());

    this->picLabel->setGeometry(rect);
    this->picLabel->setPixmap(map);

    this->guideLabel = new QLabel(this);
    this->guideLabel->setText("Double click or Drag and Drop\nTo load file");
    this->guideLabel->adjustSize();
    this->guideLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->guideLabel->setGeometry(this->rect().center().x() - (this->guideLabel->frameRect().width() / 2),
                                  (this->rect().center().y() - (this->guideLabel->frameRect().height() / 2)) + 75,
                                  this->guideLabel->rect().width(),
                                  this->guideLabel->rect().height());

    this->statusLabel = new QLabel(this->ui->statusbar);
    this->statusLabel->setText("No File");

    this->ui->statusbar->addPermanentWidget(this->statusLabel);
    this->ui->statusbar->setVisible(true);

    this->filePath = "";

    connect(this, &MainWindow::sendDragNDropComplete, this, &MainWindow::recieveDragNDropComplete);    

    this->outputPath = QDir::tempPath() + "/icombine";
    QDir dir;
    if(!dir.exists(this->outputPath)){
        dir.mkdir(this->outputPath);
    }
}

MainWindow::~MainWindow()
{
    delete this->ui;
    delete this->statusLabel;
    delete this->guideLabel;
    if(this->tabWidget != nullptr){
        delete this->tabWidget;
        this->tabWidget = nullptr;
    }        
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionLoad_triggered()
{
    this->loadFile();
}

void MainWindow::receivePaths(QString archive)
{
    this->filePath = archive;
}


void MainWindow::on_actionAbout_triggered()
{
    aboutDialog abt;
    abt.setModal(true);
    abt.exec();
}

void MainWindow::recieveDragNDropComplete(QStringList files)
{
    foreach(const auto &file, files){
        this->loadFile(file);
    }
}

void MainWindow::recieveTabChanged(int index)
{
    customTreeWidget *tree = static_cast<customTreeWidget*>(this->tabWidget->widget(index));
    this->statusLabel->setText(tree->getPath());
}

void MainWindow::recieveTabClose(int index)
{
    this->tabWidget->removeTab(index);
    if(this->tabWidget->count() == 0){
        this->tabWidget->close();
        delete this->tabWidget;
        this->tabWidget = nullptr;
        this->statusLabel->setText("No File");
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    QPixmap map(this->picLabel->pixmap());
    QRect rect(this->rect().center().x() - (map.size().width() / 2),
               this->rect().center().y() - (map.size().height() / 2),
               map.size().width(),
               map.size().height());

    this->picLabel->setGeometry(rect);

    this->guideLabel->setGeometry(this->rect().center().x() - (this->guideLabel->frameRect().width() / 2),
                                  (this->rect().center().y() - (this->guideLabel->frameRect().height() / 2)) + 75,
                                  this->guideLabel->rect().width(),
                                  this->guideLabel->rect().height());
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    this->loadFile();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls()){
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QStringList acceptedTypes;
    acceptedTypes << "tpp";

    QStringList validDroppedFiles;
    foreach(const auto &url, e->mimeData()->urls()){
        QString fname = url.toLocalFile();
        QFileInfo info(fname);
        if(info.exists()){
            if(acceptedTypes.contains(info.suffix().trimmed())){
                validDroppedFiles << fname;
            }
        }
    }
    emit this->sendDragNDropComplete(validDroppedFiles);
}

void MainWindow::loadFile()
{
    this->inOut = new archiveInOutDialog(this);
    connect(this->inOut, &archiveInOutDialog::sendPaths, this, &MainWindow::receivePaths);
    this->inOut->exec();
    delete this->inOut;

    if(this->filePath.size() == 0){
        return;
    }

    this->loadFile(this->filePath);
}

void MainWindow::loadFile(QString filePath)
{
    QFileInfo info(filePath);
    if(!info.exists()){
        return;
    }

    if(this->tabWidget == nullptr){
        this->tabWidget = new QTabWidget(this);
        this->tabWidget->setTabsClosable(true);
        this->setCentralWidget(this->tabWidget);
        this->ui->centralwidget->setContentsMargins(0, 0, 0, 0);
        connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::recieveTabChanged);
        connect(this->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::recieveTabClose);
    }

    customTreeWidget *newTree = new customTreeWidget(filePath);
    newTree->setColumnCount(3);
    newTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    newTree->setDropIndicatorShown(false);
    newTree->setStyleSheet("QHeaderView::section {"
                                    "   background: rgb(238, 240, 241);"
                                    "   border: 0px;"
                                    "   border-right: 0px;"
                                    "   border-left: 0px;"
                                    "}");
    newTree->setHeaderHidden(true);
    this->tabWidget->addTab(newTree, info.baseName());
    this->tabWidget->setCurrentWidget(newTree);

    this->statusLabel->setText(filePath);

    std::vector<test> *results = new std::vector<test>();
    QFuture<void> future = QtConcurrent::run(&MainWindow::extract, this, filePath, this->outputPath, results);

    this->loading_pop_up = new loadingPopUp(this);
    connect(this, &MainWindow::sendExtractFinished, this->loading_pop_up, &loadingPopUp::receiveClose);
    this->loading_pop_up->setModal(true);
    this->loading_pop_up->startSpinner();
    this->loading_pop_up->exec();
    this->loading_pop_up->stopSpinner();
    delete this->loading_pop_up;

    for(const auto &test: *results){
        this->addTreeRoot(newTree, test);
    }
    newTree->resizeColumnToContents(0);
    delete results;
}

void MainWindow::extract(MainWindow *window, QString filePath, QString outputPath, std::vector<test> *results)
{
    fs::path zip_p(fs::absolute(filePath.toStdString()));
    fs::path zip_extract_p((outputPath.toStdString() + "/" + zip_p.stem().string()));

    qDebug() << "Starting extract";

    miniz_cpp::zip_file zip_file(zip_p.string());
    fs::create_directory(zip_extract_p);
    zip_file.extractall(zip_extract_p.string());

    fs::path test_result_zip_p((zip_extract_p.string() + "/BttFiles/BttTestResults.zip"));
    miniz_cpp::zip_file test_result_zip(test_result_zip_p.string());
    fs::path test_result_extract_zip_p((test_result_zip_p.parent_path().string() + "/" + test_result_zip_p.stem().string()));
    fs::create_directory(test_result_extract_zip_p);
    test_result_zip.extractall(test_result_extract_zip_p.string());

    archiveParser parser(zip_extract_p.string());
    qDebug() << "Extract finished";

    qDebug() << "\n";
    std::vector<test> r = parser.parse();
    for(const auto &item: r){
        results->push_back(item);
    }

    emit window->sendExtractFinished();
}

void MainWindow::addTreeRoot(QTreeWidget *tree, const test &t)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(tree);

    treeItem->setText(0, QString::fromStdString(t.test_name));

    for(const auto &item: t.results){
        this->addTreeChild(tree, treeItem, item);
    }
    treeItem->setExpanded(true);
}

void MainWindow::addTreeChild(QTreeWidget *tree, QTreeWidgetItem *parent, const test_result &r)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(parent);

    for(int i = 0; i < tree->columnCount(); i++){
        treeItem->setBackground(i, QBrush(QColor(238, 240, 241)));
    }

    treeItem->setText(0, QString::fromStdString(r.test_date));
    treeItem->setText(1, QString::fromStdString(r.test_time));
    treeItem->setText(2, QString::fromStdString(r.test_outcome));
    if(r.test_outcome == "passed"){
        treeItem->setIcon(2, QIcon(":/img/img/check_icon.svg"));
    }else if(r.test_outcome == "inconclusive"){
        treeItem->setIcon(2, QIcon(":/img/img/minus_icon.svg"));
    }else{
        treeItem->setIcon(2, QIcon(":/img/img/cross_icon.svg"));
    }
}


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

#include <archiveparser.h>

#include "zip_file.hpp"

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);

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

    this->inOut = new archiveInOutDialog(this);
    this->loading_pop_up = new loadingPopUp(this);

    this->filePath = "";

    connect(this->inOut, &archiveInOutDialog::sendPaths, this, &MainWindow::receivePaths);
    connect(this, &MainWindow::sendExtractFinished, this->loading_pop_up, &loadingPopUp::receiveClose);

    this->outputPath = QDir::tempPath() + "/icombine";
    QDir dir;
    if(!dir.exists(this->outputPath)){
        dir.mkdir(this->outputPath);
    }
}

MainWindow::~MainWindow()
{
    delete this->ui;
    delete this->treeWidget;
    delete this->inOut;
    delete this->statusLabel;
    delete this->guideLabel;
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

void MainWindow::loadFile()
{
    this->inOut->exec();

    if(this->filePath.size() == 0){
        return;
    }

    this->treeWidget = new QTreeWidget(this);
    this->setCentralWidget(this->treeWidget);
    this->ui->centralwidget->setContentsMargins(0, 0, 0, 0);
    this->treeWidget->setColumnCount(3);
    this->treeWidget->setDropIndicatorShown(false);
    this->treeWidget->setStyleSheet("QHeaderView::section {"
                                        "   background: rgb(238, 240, 241);"
                                        "   border: 0px;"
                                        "   border-right: 0px;"
                                        "   border-left: 0px;"
                                        "}");
    this->treeWidget->setHeaderHidden(true);

    this->statusLabel->setText(this->filePath);

    std::vector<test> *results = new std::vector<test>();
    QFuture<void> future = QtConcurrent::run([=](){
        fs::path zip_p(fs::absolute(this->filePath.toStdString()));
        fs::path zip_extract_p((this->outputPath.toStdString() + "/" + zip_p.stem().string()));

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

        emit this->sendExtractFinished();
    });       

    this->loading_pop_up->setModal(true);
    this->loading_pop_up->startSpinner();
    this->loading_pop_up->exec();
    this->loading_pop_up->stopSpinner();
    delete this->loading_pop_up;

    for(const auto &test: *results){
        this->addTreeRoot(test);
    }
    this->treeWidget->resizeColumnToContents(0);
    delete results;
}

void MainWindow::addTreeRoot(const test &t)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(this->treeWidget);

    treeItem->setText(0, QString::fromStdString(t.test_name));

    for(const auto &item: t.results){
        this->addTreeChild(treeItem, item);
    }
    treeItem->setExpanded(true);
}

void MainWindow::addTreeChild(QTreeWidgetItem *parent, const test_result &r)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(parent);

    for(int i = 0; i < this->treeWidget->columnCount(); i++){
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

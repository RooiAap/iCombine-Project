
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QList>
#include <QPushButton>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

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

    this->listWidget = new QListWidget(this);
    this->setCentralWidget(this->listWidget);
    this->ui->centralwidget->setContentsMargins(0, 0, 0 ,0);

    this->statusLabel = new QLabel(this->ui->statusbar);
    this->statusLabel->setText("No File");

    this->ui->statusbar->addPermanentWidget(this->statusLabel);
    this->ui->statusbar->setVisible(true);

    this->inOut = new archiveInOutDialog(this);
    this->loading_pop_up = new loadingPopUp(this);

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
    delete this->listWidget;
    delete this->inOut;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionLoad_triggered()
{
    this->loadFile();
}

void MainWindow::on_actionExport_triggered()
{
    qDebug() << "Export File";
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

void MainWindow::loadFile()
{
    this->inOut->exec();

    if(this->filePath.size() == 0){
        return;
    }

    this->statusLabel->setText(this->filePath);

    archiveParser *parser = new archiveParser();    
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

        parser->setDirectoryPath(zip_extract_p.string());
        qDebug() << "Extract finished";
        emit this->sendExtractFinished();
    });

    this->loading_pop_up->setModal(true);
    this->loading_pop_up->startSpinner();
    this->loading_pop_up->exec();
    this->loading_pop_up->stopSpinner();
    delete this->loading_pop_up;

    qDebug() << "\n";
    std::vector<test> results = parser->parse();
    delete parser;

    for(const auto &test: results){
        this->listWidget->addItem(QString::fromStdString(test.test_name));
    }
}


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QPushButton>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QBrush>
#include <QColor>
#include <QHeaderView>

#include <filesystem>
#include <regex>

#include "aboutdialog.h"
#include "archiveparser.h"
#include "customtreewidget.h"
#include "customtreewidgetitem.h"
#include "grouptreewidgetitem.h"
#include "collectiontreewidgetitem.h"
#include "exportcompletebox.h"

#include "zip_file.hpp"

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    this->setAcceptDrops(true);

    this->tabWidget = nullptr;

    this->acceptedTypes << "tpp";

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

    this->ui->actionShow_History->setChecked(false);

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

void MainWindow::on_actionExport_All_triggered()
{
    customTreeWidget *tree = static_cast<customTreeWidget*>(this->tabWidget->currentWidget());

    QString suggestedName = "data.csv";
    QString filename = QFileDialog::getSaveFileName(this, "Export Data", suggestedName, "CSV (*.csv)");
    if(filename.length() == 0){
        return;
    }

    QFile exportFile(filename);
    if(exportFile.open(QIODevice::WriteOnly)){
        exportFile.write("Name, Total Tests, Tests Passed, Tests Failed, % Complete\n");
        for(int i = 0; i < tree->topLevelItemCount(); i++){
            groupTreeWidgetItem *item = static_cast<groupTreeWidgetItem*>(tree->topLevelItem(i));
            for(int j = 0; j < item->childCount(); j++){
                collectionTreeWidgetItem *child = static_cast<collectionTreeWidgetItem*>(item->child(j));

                QTextStream stream(&exportFile);
                stream << QString::fromStdString(item->getCardGroup().group_name + "(" + child->getCollection().collection_name + ")") << ", ";
                stream << child->getTotalTests() << ", ";
                stream << child->getTestsPassed() << ", ";
                stream << child->getTestsFailed() << ", ";
                stream << QString::number(((child->getTestsPassed()*1.0) / (child->getTotalTests()*1.0))*100, 'f', 1);
                stream << "\n";
            }
        }
        exportFile.close();
    }
    exportCompleteBox box(this);
    box.exec();
}

void MainWindow::on_actionExpand_All_triggered()
{
    if(this->tabWidget != nullptr){
        if(this->tabWidget->currentWidget() != nullptr){
            QTreeWidget *tree = static_cast<QTreeWidget*>(this->tabWidget->currentWidget());
            for(int i = 0; i < tree->topLevelItemCount(); i++){
                QTreeWidgetItem *item = tree->topLevelItem(i);                
                item->setExpanded(true);
                for(int j = 0; j < item->childCount(); j++){
                    QTreeWidgetItem *child = item->child(j);
                    child->setExpanded(true);
                    for(int k = 0; k < child->childCount(); k++){
                        child->child(k)->setExpanded(true);
                    }
                }
            }
        }
    }
}


void MainWindow::on_actionCollapse_All_triggered()
{
    if(this->tabWidget != nullptr){
        if(this->tabWidget->currentWidget() != nullptr){
            QTreeWidget *tree = static_cast<QTreeWidget*>(this->tabWidget->currentWidget());
            for(int i = 0; i < tree->topLevelItemCount(); i++){
                QTreeWidgetItem *item = tree->topLevelItem(i);
                item->setExpanded(false);
                for(int j = 0; j < item->childCount(); j++){
                    QTreeWidgetItem *child = item->child(j);
                    child->setExpanded(false);
                    for(int k = 0; k < child->childCount(); k++){
                        child->child(k)->setExpanded(false);
                    }
                }
            }
        }
    }
}

void MainWindow::on_actionShow_History_toggled(bool arg1)
{
    if(!arg1){
        for(int i = 0; i < this->tabWidget->count(); i++){
            customTreeWidget *tree = static_cast<customTreeWidget*>(this->tabWidget->widget(i));
            this->hideTreeElements(tree);
        }
    }else{
        for(int i = 0; i < this->tabWidget->count(); i++){
            customTreeWidget *tree = static_cast<customTreeWidget*>(this->tabWidget->widget(i));
            this->showTreeElements(tree);
        }
    }
}

void MainWindow::showTreeElements(QTreeWidget *tree)
{
    for(int i = 0; i < tree->topLevelItemCount(); i++){
        QTreeWidgetItem *item = tree->topLevelItem(i);
        for(int j = 0; j < item->childCount(); j++){
            QTreeWidgetItem *child = item->child(j);
            for(int k = 0; k < child->childCount(); k++){
                QTreeWidgetItem *grandChild = child->child(k);
                for(int l = 0; l < grandChild->childCount()-1; l++){
                    grandChild->child(l)->setHidden(false);
                }
            }
        }
    }
}

void MainWindow::hideTreeElements(QTreeWidget *tree)
{
    for(int i = 0; i < tree->topLevelItemCount(); i++){
        QTreeWidgetItem *item = tree->topLevelItem(i);
        for(int j = 0; j < item->childCount(); j++){
            QTreeWidgetItem *child = item->child(j);
            for(int k = 0; k < child->childCount(); k++){
                QTreeWidgetItem *grandChild = child->child(k);
                for(int l = 0; l < grandChild->childCount()-1; l++){
                    grandChild->child(l)->setHidden(true);
                }
            }
        }
    }
}

void MainWindow::recieveDragNDropComplete(QStringList files)
{
    foreach(const auto &file, files){
        this->loadFile(file);
    }
}

void MainWindow::recieveTabChanged(int index)
{
    if(index >= 0){
        customTreeWidget *tree = static_cast<customTreeWidget*>(this->tabWidget->widget(index));
        this->statusLabel->setText(tree->getFilePath());
    }
}

void MainWindow::recieveTabClose(int index)
{
    this->tabWidget->removeTab(index);
    if(this->tabWidget->count() == 0){
        delete this->tabWidget;
        this->tabWidget = nullptr;
        this->statusLabel->setText("No File");
        this->ui->actionExport_All->setEnabled(false);
        this->ui->actionExpand_All->setEnabled(false);
        this->ui->actionCollapse_All->setEnabled(false);
        this->ui->actionShow_History->setEnabled(false);
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
        foreach(const auto &url, e->mimeData()->urls()){
            if(url.isLocalFile()){
                QFileInfo fileInfo(url.toLocalFile());
                if(!this->acceptedTypes.contains(fileInfo.suffix().trimmed())){
                    e->setDropAction(Qt::IgnoreAction);
                    return;
                }
            }else{
                e->setDropAction(Qt::IgnoreAction);
                return;
            }
        }
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if(e->dropAction() == Qt::IgnoreAction){
        return;
    }

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
    QString fileFilter = "Archives (";
    foreach(const QString &type, this->acceptedTypes){
        fileFilter = fileFilter + ("*." + type + " ");
    }
    fileFilter = fileFilter + ")";

    this->inOut = new archiveInOutDialog(this, fileFilter);
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
        connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::recieveTabChanged);
        connect(this->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::recieveTabClose);

        this->ui->actionExport_All->setEnabled(true);
        this->ui->actionExpand_All->setEnabled(true);
        this->ui->actionCollapse_All->setEnabled(true);
        this->ui->actionShow_History->setEnabled(true);
    }

    customTreeWidget *newTree = new customTreeWidget(filePath);
    newTree->setColumnCount(6);
    newTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    newTree->setDropIndicatorShown(false);
    newTree->setStyleSheet("QHeaderView::section {"
                                    "   background: rgb(238, 240, 241);"
                                    "   border: 0px;"
                                    "   border-right: 0px;"
                                    "   border-left: 0px;"
                                    "}");
    newTree->setHeaderHidden(true);
    newTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(newTree, &customTreeWidget::customContextMenuRequested, newTree, &customTreeWidget::receiveContextMenuRequest);

    std::string s = info.fileName().toStdString();
    size_t pos = s.find_last_of('.', std::string::npos);

    this->tabWidget->addTab(newTree, QString::fromStdString(s.substr(0, pos)));
    this->tabWidget->setCurrentWidget(newTree);

    this->statusLabel->setText(filePath);

    std::vector<test> *results = new std::vector<test>();
    connect(this, &MainWindow::sendTreeOutputFolder, newTree, &customTreeWidget::receiveTreeOutputFolder);
    QFuture<void> future = QtConcurrent::run(&MainWindow::extract, this, filePath, this->outputPath, results);

    this->loading_pop_up = new loadingPopUp(this);
    connect(this, &MainWindow::sendExtractFinished, this->loading_pop_up, &loadingPopUp::receiveClose);
    this->loading_pop_up->setModal(true);
    this->loading_pop_up->startSpinner();
    this->loading_pop_up->exec();
    this->loading_pop_up->stopSpinner();
    delete this->loading_pop_up;

    group mastercard;
    mastercard.group_name = "MASTERCARD";
    mastercard.contact.collection_name = "Contact";
    mastercard.contactless.collection_name = "Contactless";
    std::regex mastercard_contact_pattern("(M-TIP)");
    std::regex mastercard_contactless_pattern("(MCD|MCM|MSI|COM)");
    for(auto &t: *results){
        if(!t.used){
            if(std::regex_search(t.test_name, mastercard_contact_pattern)){
                mastercard.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, mastercard_contactless_pattern)){
                mastercard.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    group visa;
    visa.group_name = "VISA";
    visa.contact.collection_name = "Contact";
    visa.contactless.collection_name = "Contactless";
    std::regex visa_contact_pattern("(VISA\.TC\.0)");
    std::regex visa_contactless_pattern("(VISA\.TC\.1)");
    for(auto &t: *results){
        if(!t.used){
            if(std::regex_search(t.test_name, visa_contact_pattern)){
                visa.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, visa_contactless_pattern)){
                visa.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    group amex;
    amex.group_name = "AMEX";
    amex.contact.collection_name = "Contact";
    amex.contactless.collection_name = "Contactless";
    std::regex amex_contact_pattern("(AXP_EMV|AXP_RCP|AXP_MAG)");
    std::regex amex_contactless_pattern("(AXP_EP)");
    for(auto &t: *results){
        if(!t.used){
            if(std::regex_search(t.test_name, amex_contact_pattern)){
                amex.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, amex_contactless_pattern)){
                amex.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    group diners;
    diners.group_name = "DINERS/DISCOVER";
    diners.contact.collection_name = "Contact";
    diners.contactless.collection_name = "Contactless";
    std::regex diners_contact_pattern("(DGN_Connect_L3_CT)|(DGN_L3_CT)");
    std::regex diners_contactless_pattern("(DGN_Connect_L3_CL)|(DGN_L3_CL)");
    for(auto &t: *results){
        if(!t.used){
            if(std::regex_search(t.test_name, diners_contact_pattern)){
                diners.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, diners_contactless_pattern)){
                diners.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }
    std::regex discover_pattern("TAIS");
    std::regex discover_contact_pattern("Contact\\\\");
    std::regex discover_contactless_pattern("Contactless\\\\");
    for(auto &t: *results){
        if(!t.used){
            if(std::regex_search(t.test_name, discover_pattern)){
                std::string contents;
                QString filename = QString::fromStdString(t.results.front().file);
                QFile f(filename);
                if(f.open(QIODevice::ReadOnly)){
                    contents = f.readAll().toStdString();
                    f.close();
                }
                if(std::regex_search(contents, discover_contact_pattern)){
                    diners.contactless.tests.push_back(t);
                    t.used = true;
                }else if(std::regex_search(contents, discover_contactless_pattern)){
                    diners.contact.tests.push_back(t);
                    t.used = true;
                }
            }
        }
    }

    group jcb;
    jcb.group_name = "JCB";
    jcb.contact.collection_name = "Contact";
    jcb.contactless.collection_name = "Contactless";
    std::regex jcb_contact_pattern("Contact\\\\");
    std::regex jcb_contactless_pattern("Contactless\\\\");
    for(auto &t: *results){
        if(!t.used){
            std::string contents;
            QString filename = QString::fromStdString(t.results.front().file);
            if(filename.contains("JCB")){
                QFile f(filename);
                if(f.open(QIODevice::ReadOnly)){
                    contents = f.readAll().toStdString();
                    f.close();
                }
                if(std::regex_search(contents, jcb_contact_pattern)){
                    jcb.contactless.tests.push_back(t);
                    t.used = true;
                }else if(std::regex_search(contents, jcb_contactless_pattern)){
                    jcb.contact.tests.push_back(t);
                    t.used = true;
                }
            }
        }else{
            continue;
        }
    }

    std::vector<group> group_results = {mastercard, visa, amex, diners, jcb};

    for(const auto &g: group_results){
        this->addTreeGroup(newTree, g);
    }
    if(this->ui->actionShow_History->isChecked()){
        this->showTreeElements(newTree);
    }else{
        this->hideTreeElements(newTree);
    }
    newTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
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
    emit window->sendTreeOutputFolder(QString::fromStdString(parser.getDirectoryPath()));
    qDebug() << "\n";
    std::vector<test> r = parser.parse();
    for(const auto &item: r){
        results->push_back(item);
    }

    emit window->sendExtractFinished();
}

void MainWindow::addTreeGroup(QTreeWidget *tree, const group &g)
{
    if(g.contact.tests.size() == 0 && g.contactless.tests.size() == 0){
        return;
    }

    groupTreeWidgetItem *treeItem = new groupTreeWidgetItem(g, tree);

    if(g.contact.tests.size() != 0){
        this->addTreeCollection(tree, treeItem, g.contact);
    }
    if(g.contactless.tests.size() != 0){
        this->addTreeCollection(tree, treeItem, g.contactless);
    }
}

void MainWindow::addTreeCollection(QTreeWidget *tree, QTreeWidgetItem *parent, const card_collection &c)
{
    collectionTreeWidgetItem *treeItem = new collectionTreeWidgetItem(c, parent);

    for(const auto &t: c.tests){
        this->addTreeTest(tree, treeItem, t);
    }
}

void MainWindow::addTreeTest(QTreeWidget *tree, QTreeWidgetItem *parent, const test &t)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(parent, 4);    

    for(int i = 0; i < tree->columnCount(); i++){
        treeItem->setBackground(i, QBrush(QColor(226, 230, 232)));
    }

    treeItem->setText(0, QString::fromStdString(t.test_name));

    for(const auto &r: t.results){
        this->addTreeTestResult(tree, treeItem, r);
    }
    treeItem->setExpanded(false);
    treeItem->sortChildren(0, Qt::SortOrder::AscendingOrder);
}

void MainWindow::addTreeTestResult(QTreeWidget *tree, QTreeWidgetItem *parent, const test_result &r)
{
    customTreeWidgetItem *treeItem = new customTreeWidgetItem(parent, QString::fromStdString(r.file));

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

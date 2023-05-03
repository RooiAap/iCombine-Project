
#include "customtreewidget.h"
#include "customtreewidgetitem.h"
#include "grouptreewidgetitem.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>

#include <filesystem>

customTreeWidget::customTreeWidget(QString path, QWidget *parent): QTreeWidget(parent)
{
    this->filePath = path;
}

customTreeWidget::~customTreeWidget()
{
    QDir dir(this->getOutputPath());
    dir.removeRecursively();
}

void customTreeWidget::setOutputPath(QString path)
{
    this->outputPath = path;
}

QString customTreeWidget::getOutputPath() const
{
    return this->outputPath;
}

QString customTreeWidget::getFilePath() const
{
    return this->filePath;
}

void customTreeWidget::receiveTreeOutputFolder(QString folder)
{
    this->setOutputPath(folder);
}

void customTreeWidget::receiveContextMenuRequest(QPoint pos)
{
    this->contextMenuPosition = pos;

    QTreeWidgetItem *item = this->itemAt(pos);
    QMenu menu(this);
    if(item->type() == 1){
        QAction *exportDataAction = new QAction("Export Data", this);
        connect(exportDataAction, &QAction::triggered, this, &customTreeWidget::receiveExportData);

        menu.addAction(exportDataAction);
    }else if(item->type() == 3){
        QAction *showFileAction = new QAction("Show Log", this);
        connect(showFileAction, &QAction::triggered, this, &customTreeWidget::receiveShowFile);

        QAction *showFolderAction = new QAction("Show in Explorer", this);
        connect(showFolderAction, &QAction::triggered, this, &customTreeWidget::receiveShowFolder);

        menu.addAction(showFileAction);
        menu.addAction(showFolderAction);
    }
    menu.exec(this->mapToGlobal(pos));
}

void customTreeWidget::receiveShowFile()
{
    customTreeWidgetItem *item = static_cast<customTreeWidgetItem*>(this->itemAt(this->contextMenuPosition));
    QDesktopServices::openUrl(QUrl::fromLocalFile(item->getFilePath()));
}

void customTreeWidget::receiveShowFolder()
{
    customTreeWidgetItem *item = static_cast<customTreeWidgetItem*>(this->itemAt(this->contextMenuPosition));
    QProcess::startDetached("explorer.exe", {"/select,", QDir::toNativeSeparators(item->getFilePath())});
}

void customTreeWidget::receiveExportData()
{
    groupTreeWidgetItem *item = static_cast<groupTreeWidgetItem*>(this->itemAt(this->contextMenuPosition));

    QString filename = QFileDialog::getSaveFileName(this, "Export Group");

    QFile exportFile(filename);
    if(exportFile.open(QIODevice::WriteOnly)){
        QTextStream stream(&exportFile);

        stream << "Name, Total Tests, Tests Passed, Tests Failed\n";
        stream << QString::fromStdString(item->getCardGroup().group_name) + ", ";
        stream << item->getTotalTests() << ", ";
        stream << item->getTestsPassed() << ", ";
        stream << item->getTestsFailed();

        exportFile.close();
    }

    QMessageBox box(this);
    box.setWindowTitle("Export");
    box.setText("Export Complete!");
    box.setIconPixmap(QPixmap(":/img/img/check_icon.svg"));
    box.exec();
}

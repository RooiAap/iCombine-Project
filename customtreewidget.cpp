
#include "customtreewidget.h"
#include "customtreewidgetitem.h"

#include <QDir>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QDebug>

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

    QAction *showFileAction = new QAction("Show Log", this);
    connect(showFileAction, &QAction::triggered, this, &customTreeWidget::receiveShowFile);

    QAction *showFolderAction = new QAction("Show in Explorer", this);
    connect(showFolderAction, &QAction::triggered, this, &customTreeWidget::receiveShowFolder);

    QMenu menu(this);
    menu.addAction(showFileAction);
    menu.addAction(showFolderAction);
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

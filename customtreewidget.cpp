
#include "customtreewidget.h"

#include <QDir>
#include <QDebug>

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

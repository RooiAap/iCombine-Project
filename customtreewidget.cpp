
#include "customtreewidget.h"

#include <QDir>

customTreeWidget::customTreeWidget(QString path, QWidget *parent): QTreeWidget(parent)
{
    this->filePath = path;
}

customTreeWidget::~customTreeWidget()
{

}

QString customTreeWidget::getPath() const
{
    return this->filePath;
}

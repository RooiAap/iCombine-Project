
#include "customtreewidget.h"

customTreeWidget::customTreeWidget(QString path, QWidget *parent): QTreeWidget(parent)
{
    this->filePath = path;
}

QString customTreeWidget::getPath() const
{
    return this->filePath;
}

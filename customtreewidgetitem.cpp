
#include "customtreewidgetitem.h"

customTreeWidgetItem::customTreeWidgetItem(QTreeWidgetItem *parent, QString filePath): QTreeWidgetItem(parent)
{
    this->filePath = filePath;
}

QString customTreeWidgetItem::getFilePath() const
{
    return this->filePath;
}



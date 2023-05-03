
#include "customtreewidgetitem.h"

customTreeWidgetItem::customTreeWidgetItem(QTreeWidgetItem *parent, QString filePath): QTreeWidgetItem(parent, nullptr, 3)
{
    this->filePath = filePath;
}

QString customTreeWidgetItem::getFilePath() const
{
    return this->filePath;
}




#ifndef CUSTOMTREEWIDGETITEM_H
#define CUSTOMTREEWIDGETITEM_H

#include <QTreeWidgetItem>

#include <QWidget>
#include <QString>


class customTreeWidgetItem : public QTreeWidgetItem
{
public:
    customTreeWidgetItem(QTreeWidgetItem *parent, QString filePath);

    QString getFilePath() const;

private:
    QString filePath;
};

#endif // CUSTOMTREEWIDGETITEM_H

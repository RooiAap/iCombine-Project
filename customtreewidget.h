
#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QTreeWidget>

#include <QObject>
#include <QWidget>

#include <QString>

class customTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    customTreeWidget(QString path, QWidget *parent=nullptr);
    ~customTreeWidget();

    QString getPath() const;

private:
    QString filePath;
};

#endif // CUSTOMTREEWIDGET_H

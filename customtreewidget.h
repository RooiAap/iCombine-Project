
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

    void setOutputPath(QString path);

    QString getFilePath() const;
    QString getOutputPath() const;

private:
    QString filePath;
    QString outputPath;

public slots:
    void receiveTreeOutputFolder(QString folder);
};

#endif // CUSTOMTREEWIDGET_H

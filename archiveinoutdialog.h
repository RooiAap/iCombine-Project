#ifndef ARCHIVEINOUTDIALOG_H
#define ARCHIVEINOUTDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class archiveInOutDialog;
}

class archiveInOutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit archiveInOutDialog(QWidget *parent = nullptr);
    ~archiveInOutDialog();

    QPushButton* getOkButton() const;

private slots:
    void on_inputFileChooser_clicked();

    void on_okButton_clicked();
    void on_cancelButton_clicked();

protected slots:
    void checkDataValidity();

signals:
    void sendPaths(QString archive);

private:
    Ui::archiveInOutDialog *ui;
};

#endif // ARCHIVEINOUTDIALOG_H

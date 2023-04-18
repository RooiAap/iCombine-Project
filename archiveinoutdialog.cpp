#include "archiveinoutdialog.h"
#include "ui_archiveinoutdialog.h"

#include <QString>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QPixmap>

#include <filenotpopup.h>

archiveInOutDialog::archiveInOutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::archiveInOutDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("");

    connect(this->ui->inputArchivePathEntry, &QLineEdit::textChanged, this, &archiveInOutDialog::checkDataValidity);
}

archiveInOutDialog::~archiveInOutDialog()
{
    delete ui;
}

void archiveInOutDialog::on_inputFileChooser_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);
    this->ui->inputArchivePathEntry->clear();
    this->ui->inputArchivePathEntry->insert(filename);
}

void archiveInOutDialog::on_okButton_clicked()
{
    QString archive = this->ui->inputArchivePathEntry->displayText();
    if(!QFile::exists(archive)){
        fileNotPopUp popUp;
        popUp.exec();
        return;
    }
    emit sendPaths(archive);
    this->close();
}


void archiveInOutDialog::on_cancelButton_clicked()
{
    emit sendPaths("");
    this->close();
}

void archiveInOutDialog::checkDataValidity()
{
    this->ui->okButton->setEnabled(this->ui->inputArchivePathEntry->displayText().size() > 0);
}

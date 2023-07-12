#include "archiveinoutdialog.h"
#include "ui_archiveinoutdialog.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QStringList>
#include <QLineEdit>
#include <QPixmap>

#include "filenotpopup.h"

archiveInOutDialog::archiveInOutDialog(QWidget *parent, QString fileFilter) :
    QDialog(parent),
    ui(new Ui::archiveInOutDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("");

    this->fileFilter = fileFilter;

    connect(this->ui->inputArchivePathEntry, &QLineEdit::textChanged, this, &archiveInOutDialog::checkDataValidity);
}

archiveInOutDialog::~archiveInOutDialog()
{
    delete ui;
}

void archiveInOutDialog::on_inputFileChooser_clicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Load File(s)", QString(), this->fileFilter);
    this->ui->inputArchivePathEntry->clear();

    std::stringstream ss;
    for(const QString &s: filenames){
        ss << s.toStdString() << ",";
    }

    QString files = QString::fromStdString(ss.str());
    this->ui->inputArchivePathEntry->insert(files);
}

void archiveInOutDialog::on_okButton_clicked()
{
    QString archives = this->ui->inputArchivePathEntry->displayText();

    std::stringstream stream(archives.toStdString());
    std::string segment;
    std::vector<std::string> archiveList;
    while (std::getline(stream, segment, ',')) {
        archiveList.push_back(segment);
    }

    for(int i = 0; i < archiveList.size(); i++){
        QString qArchive = QString::fromStdString(archiveList.at(i));
        if(!QFile::exists(qArchive)){
            archiveList.erase(archiveList.begin() + i);
            fileNotPopUp popUp(qArchive);
            popUp.exec();
        }
    }

    emit sendPaths(archiveList);
    this->close();
}


void archiveInOutDialog::on_cancelButton_clicked()
{
    std::vector<std::string> v;
    v.clear();
    emit sendPaths(v);
    this->close();
}

void archiveInOutDialog::checkDataValidity()
{
    this->ui->okButton->setEnabled(this->ui->inputArchivePathEntry->displayText().size() > 0);
}

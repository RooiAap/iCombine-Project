#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QPixmap>

#include <version.h>

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    QString labelText = "Version " + version.toString() + "\n" + "(Qt " + QT_VERSION_STR + ")";
    this->ui->versionLabel->setText(labelText);
    this->ui->versionLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->ui->versionLabel->adjustSize();

    QRect currentRect = this->ui->versionLabel->geometry();
    this->ui->versionLabel->setGeometry(this->rect().center().x() - (currentRect.width() / 2),
                                        currentRect.y(),
                                        currentRect.width(),
                                        currentRect.height());
}

aboutDialog::~aboutDialog()
{
    delete ui;
}

void aboutDialog::on_okButton_clicked()
{
    this->close();
}


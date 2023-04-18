#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QPixmap>

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
}

aboutDialog::~aboutDialog()
{
    delete ui;
}

void aboutDialog::on_okButton_clicked()
{
    this->close();
}


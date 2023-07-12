#include "filenotpopup.h"
#include "ui_filenotpopup.h"

fileNotPopUp::fileNotPopUp(QString filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileNotPopUp)
{
    ui->setupUi(this);
    this->setWindowTitle("");
    QApplication::beep();
}

fileNotPopUp::~fileNotPopUp()
{
    delete ui;
}

void fileNotPopUp::on_okButton_clicked()
{
    this->close();
}


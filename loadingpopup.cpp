#include "loadingpopup.h"
#include "ui_loadingpopup.h"

#include <QWidget>
#include <QByteArray>
#include <QPalette>

loadingPopUp::loadingPopUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadingPopUp)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::Desktop);
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    this->setPalette(pal);

    this->spinnerGif = new QMovie(":/gif/gif/rounded_blocks.gif", QByteArray(), this->ui->spinnerLabel);
    if(spinnerGif->isValid()){
        this->ui->spinnerLabel->setMovie(spinnerGif);
        this->ui->spinnerLabel->setVisible(true);        
    }else{
        qDebug() << "Invalid GIF";
        this->destroy(true);
    }
}

loadingPopUp::~loadingPopUp()
{    
    delete this->spinnerGif;
    delete ui;
}

void loadingPopUp::receiveClose()
{
    this->close();
}

void loadingPopUp::startSpinner()
{
    if(this->spinnerGif->isValid()){
        qDebug() << "Starting spinner";
        spinnerGif->start();
        this->ui->spinnerLabel->show();
    }
}

void loadingPopUp::stopSpinner()
{
    if(this->spinnerGif->isValid()){
        qDebug() << "Stopping spinner";
        this->spinnerGif->stop();
        this->ui->spinnerLabel->show();
    }
}

#include "exportcompletebox.h"
#include "ui_exportcompletebox.h"

#include <QRect>
#include <QPixmap>

exportCompleteBox::exportCompleteBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exportCompleteBox)
{
    this->ui->setupUi(this);

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    QRect icon_rect = this->ui->iconLabel->geometry();
    this->ui->iconLabel->setGeometry(this->rect().center().x() - (icon_rect.size().width() / 2),
                                     icon_rect.y(),
                                     icon_rect.size().width(),
                                     icon_rect.size().height());

    this->ui->textLabel->adjustSize();
    QRect text_rect = this->ui->textLabel->geometry();
    this->ui->textLabel->setGeometry(this->rect().center().x() - (text_rect.width() / 2),
                                     text_rect.y(),
                                     text_rect.width(),
                                     text_rect.height());

    QRect button_rect = this->ui->okButton->geometry();
    this->ui->okButton->setGeometry(this->rect().center().x() - (button_rect.width() / 2),
                                    button_rect.y(),
                                    button_rect.width(),
                                    button_rect.height());
}

exportCompleteBox::~exportCompleteBox()
{
    delete this->ui;
}

void exportCompleteBox::on_okButton_clicked()
{
    this->close();
}


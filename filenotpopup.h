#ifndef FILENOTPOPUP_H
#define FILENOTPOPUP_H

#include <QDialog>

namespace Ui {
class fileNotPopUp;
}

class fileNotPopUp : public QDialog
{
    Q_OBJECT

public:
    explicit fileNotPopUp(QWidget *parent = nullptr);
    ~fileNotPopUp();

private slots:
    void on_okButton_clicked();

private:
    Ui::fileNotPopUp *ui;
};

#endif // FILENOTPOPUP_H

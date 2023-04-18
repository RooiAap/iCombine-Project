#ifndef LOADINGPOPUP_H
#define LOADINGPOPUP_H

#include <QDialog>

#include <QLabel>
#include <QMovie>

namespace Ui {
class loadingPopUp;
}

class loadingPopUp : public QDialog
{
    Q_OBJECT

public:
    explicit loadingPopUp(QWidget *parent = nullptr);
    ~loadingPopUp();

    void startSpinner();
    void stopSpinner();

public slots:
    void receiveClose();

private:
    Ui::loadingPopUp *ui;

    QTimer *timer;
    QMovie *spinnerGif;
};

#endif // LOADINGPOPUP_H

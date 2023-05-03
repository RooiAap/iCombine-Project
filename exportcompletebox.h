#ifndef EXPORTCOMPLETEBOX_H
#define EXPORTCOMPLETEBOX_H

#include <QDialog>

namespace Ui {
class exportCompleteBox;
}

class exportCompleteBox : public QDialog
{
    Q_OBJECT

public:
    explicit exportCompleteBox(QWidget *parent = nullptr);
    ~exportCompleteBox();

private slots:
    void on_okButton_clicked();

private:
    Ui::exportCompleteBox *ui;
};

#endif // EXPORTCOMPLETEBOX_H

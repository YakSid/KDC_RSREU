#ifndef DETAILKTR_H
#define DETAILKTR_H

#include <QDialog>

namespace Ui {
class DetailKTR;
}

class DetailKTR : public QDialog
{
    Q_OBJECT

public:
    explicit DetailKTR(QWidget *parent = nullptr);
    ~DetailKTR();

private:
    Ui::DetailKTR *ui;
};

#endif // DETAILKTR_H

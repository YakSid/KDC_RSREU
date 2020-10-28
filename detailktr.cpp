#include "detailktr.h"
#include "ui_detailktr.h"

DetailKTR::DetailKTR(QWidget *parent) : QDialog(parent), ui(new Ui::DetailKTR)
{
    ui->setupUi(this);

    /* Протестить сначала это
    Qt::WindowFlags flags = Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);*/
}

DetailKTR::~DetailKTR()
{
    delete ui;
}

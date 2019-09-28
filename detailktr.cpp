#include "detailktr.h"
#include "ui_detailktr.h"

DetailKTR::DetailKTR(QWidget *parent) : QDialog(parent), ui(new Ui::DetailKTR)
{
    ui->setupUi(this);
}

DetailKTR::~DetailKTR()
{
    delete ui;
}

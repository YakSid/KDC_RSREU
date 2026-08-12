#include "detailktr.h"
#include "ui_detailktr.h"

DetailKTR::DetailKTR(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DetailKTR)
{
    ui->setupUi(this);
    this->setStyleSheet("QPushButton:disabled {"
                        "background-color: darkGrey;"
                        "border: 2px solid darkGrey;}");
}

DetailKTR::~DetailKTR()
{
    delete ui;
}

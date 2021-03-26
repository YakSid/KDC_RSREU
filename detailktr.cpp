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

    this->setStyleSheet("QPushButton:disabled {"
                        "background-color: darkGrey;"
                        "border: 2px solid darkGrey;}");
}

DetailKTR::~DetailKTR()
{
    delete ui;
}

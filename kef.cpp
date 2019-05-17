#include "kef.h"
#include "ui_kef.h"

kef::kef(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kef)
{
    ui->setupUi(this);
}

kef::~kef()
{
    delete ui;
}

void kef::on_pushButton_clicked()
{
    close();
}

#include "startdialog.h"
#include "ui_startdialog.h"

StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog)
{
    ui->setupUi(this);
}

StartDialog::~StartDialog()
{
    delete ui;
}

void StartDialog::on_Start_clicked()
{
    StartMode = 1;
    StartDialog::close();
}

/*void StartDialog::on_Continue_clicked()
{
    StartMode = 2;
    StartDialog::close();
}*/

void StartDialog::on_lineEdit_textChanged(const QString &arg1)
{
    Path = arg1;
}

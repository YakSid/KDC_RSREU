#include "kef.h"
#include "ui_kef.h"

kef::kef(QWidget *parent) : QDialog(parent), ui(new Ui::kef)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

kef::~kef()
{
    delete ui;
}

// TODO: [later] Подсвечивать кэфы при изменении зелёным и красным в mw (и тут)
void kef::getKefs(float kef, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kots, int ktsp,
                  int kmol)
{
    QString tmp = QString::number(static_cast<double>(kef), 'f', 1);
    tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
    ui->kef_2->setText(tmp);

    tmp = QString::number(static_cast<double>(znahimost), 'f', 1);
    tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
    ui->kzn->setText(tmp);

    ui->kdog->setText(QString::number(kdog));
    ui->krv->setText(QString::number(krv));
    ui->kzp->setText(QString::number(kzp));
    ui->kvo->setText(QString::number(kvo));
    ui->kot->setText(QString::number(kot));
    ui->kpr->setText(QString::number(kots));
    ui->ktsp->setText(QString::number(ktsp));
    ui->kmol->setText(QString::number(kmol));
}

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

void kef::setStartKeffs(float kef, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kots, int ktsp,
                        int kmol)
{
    QString tmp = QString::number(static_cast<double>(kef), 'f', 1);
    tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
    ui->kefStart->setText(tmp);

    tmp = QString::number(static_cast<double>(znahimost), 'f', 1);
    tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
    ui->kznStart->setText(tmp);

    ui->kdogStart->setText(QString::number(kdog));
    ui->krvStart->setText(QString::number(krv));
    ui->kzpStart->setText(QString::number(kzp));
    ui->kvoStart->setText(QString::number(kvo));
    ui->kotStart->setText(QString::number(kot));
    ui->kprStart->setText(QString::number(kots));
    ui->ktspStart->setText(QString::number(ktsp));
    ui->kmolStart->setText(QString::number(kmol));
}

void kef::setCurrentKeffs(float kef, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kots, int ktsp,
                          int kmol)
{
    // TODO: [later] [min] Подсвечивать кэфы при изменении зелёным и красным тут тоже
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

void kef::compareAndPaint()
{
    // NOTE: потом-потом оптимизировать
    QPalette greenPal, redPal, blackPal;
    greenPal.setColor(QPalette::WindowText, Qt::darkGreen);
    redPal.setColor(QPalette::WindowText, Qt::darkRed);
    blackPal.setColor(QPalette::WindowText, Qt::black);
    //! TODO: СЕЙЧАСЖЕ сделать перерасчёт кэффов
    /*QVariantList startKeffs({ ui->startKTR->text().toFloat(), ui->startKSC->text().toFloat(),
                              ui->startKGDP->text().toFloat(), _strDoubleToFloat(ui->startKPSP->text()),
                              _strDoubleToFloat(ui->kefStart->text()) });
    QList<QLabel *> labels({ ui->lb_ktr, ui->lb_ksc, ui->lb_kgdp, ui->lb_kpsp, ui->lb_kef });
    for (int i = 0; i < startKeffs.size(); i++) {
        if (keffs[i].toFloat() > startKeffs[i].toFloat()) {
            labels[i]->setPalette(greenPal);
        } else if (keffs[i].toFloat() < startKeffs[i].toFloat()) {
            labels[i]->setPalette(redPal);
        } else {
            labels[i]->setPalette(blackPal);
        }
    }*/
}

#include "kef.h"
#include "ui_kef.h"

kef::kef(QWidget *parent) : QDialog(parent), ui(new Ui::kef)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    this->setStyleSheet("QPushButton:disabled {"
                        "background-color: darkGrey;"
                        "border: 2px solid darkGrey;}");
}

kef::~kef()
{
    delete ui;
}

void kef::setStartKeffs(int ktr, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kpr, int ktok,
                        int ktsp)
{
    ui->ktrStart->setText(QString::number(ktr));
    auto tmp = QString::number(static_cast<double>(znahimost), 'f', 1);
    tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
    ui->kznStart->setText(tmp);
    ui->kdogStart->setText(QString::number(kdog));
    ui->krvStart->setText(QString::number(krv));
    ui->kzpStart->setText(QString::number(kzp));
    ui->kvoStart->setText(QString::number(kvo));
    ui->kotStart->setText(QString::number(kot));
    ui->kprStart->setText(QString::number(kpr));
    ui->ktspStart->setText(QString::number(ktsp));
    ui->ktokStart->setText(QString::number(ktok));
}

void kef::setCurrentKeffs(int ktr, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kpr, int ktok,
                          int ktsp)
{
    ui->ktr->setText(QString::number(ktr));
    QString tmp = QString::number(static_cast<double>(znahimost), 'f', 1);
    tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
    ui->kzn->setText(tmp);
    ui->kdog->setText(QString::number(kdog));
    ui->krv->setText(QString::number(krv));
    ui->kzp->setText(QString::number(kzp));
    ui->kvo->setText(QString::number(kvo));
    ui->kot->setText(QString::number(kot));
    ui->kpr->setText(QString::number(kpr));
    ui->ktsp->setText(QString::number(ktsp));
    ui->ktok->setText(QString::number(ktok));

    _compareAndPaint();
}

void kef::_compareAndPaint()
{
    QPalette greenPal, redPal, blackPal;
    greenPal.setColor(QPalette::WindowText, Qt::darkGreen);
    redPal.setColor(QPalette::WindowText, Qt::darkRed);
    blackPal.setColor(QPalette::WindowText, Qt::black);
    QList<QLineEdit *> startKeffsLines { ui->ktrStart, ui->kznStart, ui->kdogStart, ui->krvStart,  ui->kzpStart,
                                         ui->kvoStart, ui->kotStart, ui->kprStart,  ui->ktspStart, ui->ktokStart };
    QList<QLineEdit *> currentKeffsLines { ui->ktr, ui->kzn, ui->kdog, ui->krv,  ui->kzp,
                                           ui->kvo, ui->kot, ui->kpr,  ui->ktsp, ui->ktok };
    QList<QLabel *> labels { ui->lbl_18, ui->lbl_19, ui->lbl_20, ui->lbl_21, ui->lbl_22,
                             ui->lbl_23, ui->lbl_24, ui->lbl_25, ui->lbl_26, ui->lbl_17 };
    for (int i = 0; i < startKeffsLines.size(); i++) {
        if (i == 1) {
            //Для случаев когда в ячейке double
            QString curStrValWrong = currentKeffsLines[i]->text();
            auto curStrValueRight = curStrValWrong.replace(curStrValWrong.indexOf(','), 1, '.');
            auto currentValue = curStrValueRight.toDouble();
            QString startStrValWrong = startKeffsLines[i]->text();
            auto startStrValueRight = startStrValWrong.replace(startStrValWrong.indexOf(','), 1, '.');
            auto startValue = startStrValueRight.toDouble();
            if (currentValue > startValue) {
                labels[i]->setPalette(greenPal);
            } else if (currentValue < startValue) {
                labels[i]->setPalette(redPal);
            } else {
                labels[i]->setPalette(blackPal);
            }
        } else {
            if (currentKeffsLines[i]->text().toInt() == startKeffsLines[i]->text().toInt()) {
                labels[i]->setPalette(blackPal);
            } else if (currentKeffsLines[i]->text().toInt() > startKeffsLines[i]->text().toInt()) {
                labels[i]->setPalette(greenPal);
            } else {
                labels[i]->setPalette(redPal);
            }
        }
    }
}

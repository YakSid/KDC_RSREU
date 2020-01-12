#ifndef CKOLDOG_H
#define CKOLDOG_H

#include <QString>
#include <QDate>
#include <QVector>
#include <fragment.h>

class CKolDog
{
private:
    //! Основные параметры
    QString id;
    QString name;
    QDate date;
    uint validity;
    bool complWithReq; // Соответствие требованиям
    float znachimost;
    int effektivnost;
    int ktr;
    float kef;
    float kpsp;
    int kgdp;
    int ksc;
    QDate endDate;
    // Дополнительные коэффициенты
    int kdog;
    int krv;
    int kvo;
    int kzp;
    int kot;
    int ktsp;
    int kots;
    float sum;

public:
    //! Фрагменты
    QVector<fragment *> fragments;
    QString getId();
    QString getName();
    QDate getDate();
    uint getValidity();
    bool getComplWithReq();
    float getZnachimost();
    int getEffektivnost();
    int getKtr();
    float getKef();
    float getKpsp();
    int getKgdp();
    int getKsc();
    QDate getEndDate();
    int getKdog();
    int getKrv();
    int getKvo();
    int getKzp();
    int getKot();
    int getKtsp();
    int getKots();
    float getSum();

public:
    CKolDog();

    void setMainParameters(QString id, QString name, QDate date, uint validity, bool complWithReq, float znachimost,
                           int effektivnost, int ktr, float kef, float kpsp, int kgdp, int ksc, QDate endDate, int kdog,
                           int krv, int kvo, int kzp, int kot, int ktsp, int kots, float sum);
};

#endif // CKOLDOG_H

#ifndef CKOLDOG_H
#define CKOLDOG_H

#include <QString>
#include <QDate>
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
    //! Параметры фрагментов
    //

public:
    CKolDog();

    void setMainParameters(QString id, QString name, QDate date, uint validity, bool complWithReq, float znachimost,
                           int effektivnost, int ktr, float kef, float kpsp, int kgdp, int ksc, QDate endDate, int kdog,
                           int krv, int kvo, int kzp, int kot, int ktsp, int kots, float sum);
};

#endif // CKOLDOG_H

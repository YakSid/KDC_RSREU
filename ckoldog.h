#ifndef CKOLDOG_H
#define CKOLDOG_H

#include <QString>
#include <QDate>
#include <QVector>
#include <fragment.h>

class CKolDog
{
public:
    CKolDog();
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
    int getKmol();
    float getSum();
    void setMainParameters(QString id, QString name, QDate date, uint validity, bool complWithReq, float znachimost,
                           int effektivnost, int ktr, float kef, float kpsp, int kgdp, int ksc, QDate endDate, int kdog,
                           int krv, int kvo, int kzp, int kot, int ktsp, int kots, int kmol, float sum);
    //! Вставить фрагмент после pos фрагмента
    void addFragAfter(qint32 pos, fragment *frag);
    //! Вставить фрагмент на первую позицию
    void addFragOnFirstPos(fragment *frag);

public:
    //! Фрагменты
    QList<fragment *> fragments;

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
    int kmol;
    float sum;
};

#endif // CKOLDOG_H

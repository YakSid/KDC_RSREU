#ifndef CKOLDOG_H
#define CKOLDOG_H

#include <QString>
#include <QDate>
#include <QVector>
#include <QVariant>
#include <fragment.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class CKolDog
{
public:
    CKolDog();
    ~CKolDog();
    QString getId();
    QString getName();
    QDate getDate();
    uint getValidity();
    bool getComplWithReq();
    float getZnachimost();
    //! WARNING: ОшибкаКэф оставить либо kef либо effectivnost и стандартизировать везде int float double
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
    void setMainParameters(QString id, QString name, QDate date, int validity, bool complWithReq, float znachimost,
                           int effektivnost, int ktr, float kef, float kpsp, int kgdp, int ksc, QDate endDate, int kdog,
                           int krv, int kvo, int kzp, int kot, int ktsp, int kots, int kmol, float sum);
    //! Получить выводимую пятёрку коэффициентов списком (Ктр, Ксц, Кгдп, Кпсп, Кэф)
    QVariantList getFiveCurrentKeffs();
    //! Вставить фрагмент после pos фрагмента
    void addFragAfter(qint32 pos, fragment *frag);
    //! Вставить фрагмент на первую позицию
    void addFragOnFirstPos(fragment *frag);
    //! Высчитать текущие коэффициенты
    void calculateCurrentKeffs();
    //! Узнать размер объекта
    qint32 getMySize() const { return (sizeof(name) + 90); }
    //! Сформировать json документ из КД для сохранения
    QJsonDocument *packKolDogToJson();

private:
    //! Обнулить коэффициенты
    void _resetKeffs();

public:
    //! Фрагменты
    QList<fragment *> fragments;

private:
    //! Основные параметры
    QString id;
    QString name;
    QDate date;
    int validity;
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

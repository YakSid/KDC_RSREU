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
    QString getId() const { return id; }
    QString getName() const { return name; }
    QDate getDate() const { return date; }
    int getValidity() const { return validity; }
    bool getComplWithReq() const { return complWithReq; }
    float getZnachimost() const { return znachimost; }
    //! TODO: [later] стандартизировать везде int float double
    int getStartKtr() const { return startKtr; }
    float getStartKef() const { return startKef; }
    float getStartKpsp() const { return startKpsp; }
    int getStartKgdp() const { return startKgdp; }
    int getStartKsc() const { return startKsc; }
    int getKtr() const { return ktr; }
    float getKef() const { return kef; }
    float getKpsp() const { return kpsp; }
    int getKgdp() const { return kgdp; }
    int getKsc() const { return ksc; }
    QDate getEndDate() const { return endDate; }
    int getKdog() const { return kdog; }
    int getKrv() const { return krv; }
    int getKvo() const { return kvo; }
    int getKzp() const { return kzp; }
    int getKot() const { return kot; }
    int getKtsp() const { return ktsp; }
    int getKots() const { return kots; }
    int getKmol() const { return kmol; }
    float getSum() const { return sum; }
    void setMainParameters(QString id, QDate date, int validity, bool complWithReq, float znachimost, int ktr,
                           float kpsp, int kgdp, int ksc, QDate endDate, int kdog, int krv, int kvo, int kzp, int kot,
                           int ktsp, int kots, int kmol, float sum);
    void setId(QString newId) { id = newId; }
    void setName(QString newName) { name = newName; }
    void setDate(QDate newDate) { date = newDate; }
    void setValidity(int newValidity) { validity = newValidity; }
    void setComplWithReq(bool newComplWithReq) { complWithReq = newComplWithReq; }
    void setZnachimost(float newZnachimost) { znachimost = newZnachimost; }
    void setStartKtr(int newStartKtr) { startKtr = newStartKtr; }
    void setStartKef(float newStartKef) { startKef = newStartKef; }
    void setStartKpsp(float newStartKpsp) { startKpsp = newStartKpsp; }
    void setStartKgdp(int newStartKgdp) { startKgdp = newStartKgdp; }
    void setStartKsc(int newStartKsc) { startKsc = newStartKsc; }
    void setKtr(int newKtr) { ktr = newKtr; }
    void setKef(float newKef) { kef = newKef; }
    void setKpsp(float newKpsp) { kpsp = newKpsp; }
    void setKgdp(int newKgdp) { kgdp = newKgdp; }
    void setKsc(int newKsc) { ksc = newKsc; }
    void setEndDate(QDate newEndDate) { endDate = newEndDate; }
    void setKdog(int newKdog) { kdog = newKdog; }
    void setKrv(int newKrv) { krv = newKrv; }
    void setKvo(int newKvo) { kvo = newKvo; }
    void setKzp(int newKzp) { kzp = newKzp; }
    void setKot(int newKot) { kot = newKot; }
    void setKtsp(int newKtsp) { ktsp = newKtsp; }
    void setKots(int newKots) { kots = newKots; }
    void setKmol(int newKmol) { kmol = newKmol; }
    void setSum(float newSum) { sum = newSum; }
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
    //Начальные параметры (не меняются после первого присвоения)
    int startKtr;
    float startKef;
    float startKpsp;
    int startKgdp;
    int startKsc;
};

#endif // CKOLDOG_H

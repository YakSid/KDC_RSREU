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
    CKolDog(double paramA, double paramB);
    ~CKolDog();
    //! Установка параметров для формулы КЭФ
    void setParameters(double paramA, double paramB);
    QString getId() const { return id; }
    QString getName() const { return name; }
    QDate getDate() const { return date; }
    int getValidity() const { return validity; }
    bool getComplWithReq() const { return complWithReq; }
    float getZnachimost() const { return znachimost; }
    float getStartZnachimost() const { return startZnachimost; }
    //! TODO: [Улучшение кода] стандартизировать везде int float double
    int getStartKtr() const { return startKtr; }
    float getStartKef() const { return startKef; }
    float getStartKpsp() const { return startKpsp; }
    int getStartKgdp() const { return startKgdp; }
    int getStartKsc() const { return startKsc; }
    int getKtr() const { return ktr; }
    double getKef() const { return kef; }
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
    int getKpr() const { return kpr; }
    int getKtok() const { return ktok; }
    int getKmol() const { return kmol; }
    float getSum() const { return sum; }
    int getStartKdog() const { return startKdog; }
    int getStartKrv() const { return startKrv; }
    int getStartKvo() const { return startKvo; }
    int getStartKzp() const { return startKzp; }
    int getStartKot() const { return startKot; }
    int getStartKtsp() const { return startKtsp; }
    int getStartKpr() const { return startKpr; }
    int getStartKtok() const { return startKtok; }
    void setMainParameters(QString id, QDate date, int validity, bool complWithReq, float znachimost, int ktr,
                           float kpsp, int kgdp, int ksc, QDate endDate, int kdog, int krv, int kvo, int kzp, int kot,
                           int ktsp, int kpr, int ktok, int kmol, float sum);
    //!Кпр и Кток нет в базе, поэтому высчитываю их сам и записываю в адреса
    void calculateKprAndKtok(qint32 &kpr, qint32 &ktok);

    void setId(QString newId) { id = newId; }
    void setName(QString newName) { name = newName; }
    void setDate(QDate newDate) { date = newDate; }
    void setValidity(int newValidity) { validity = newValidity; }
    void setComplWithReq(bool newComplWithReq) { complWithReq = newComplWithReq; }
    void setZnachimost(float newZnachimost) { znachimost = newZnachimost; }
    void setStartZnachimost(float newStartZnachimost) { startZnachimost = newStartZnachimost; }
    void setStartKtr(int newStartKtr) { startKtr = newStartKtr; }
    void setStartKef(float newStartKef) { startKef = newStartKef; }
    void setStartKpsp(float newStartKpsp) { startKpsp = newStartKpsp; }
    void setStartKgdp(int newStartKgdp) { startKgdp = newStartKgdp; }
    void setStartKsc(int newStartKsc) { startKsc = newStartKsc; }
    void setStartMinorKeffs(int skdog, int skrv, int skvo, int skzp, int skot, int sktsp, int skpr, int sktok);
    void setKtr(int newKtr) { ktr = newKtr; }
    void setKef(double newKef) { kef = newKef; }
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
    void setKpr(int newKpr) { kpr = newKpr; }
    void setKtok(int newKtok) { ktok = newKtok; }
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
    //! Высчитать значимость кзн
    float calculateKzn();
    //! Пересчитать КЭФ
    double calculateKef();
    //! Увеличить минорный кэф этого раздела
    void incrementMinorKeff(QString razdAbr);
    //! Уменьшить минорный кэф этого раздела
    void decrementMinorKeff(QString razdAbr);
    //! id первого фрагмента из раздела
    qint32 findFirstInRazd(QString razdAbr);

private:
    //! Обнулить коэффициенты
    void _resetKeffs();

public:
    //! Фрагменты
    QList<fragment *> fragments;

private:
    // Основные параметры
    QString id;
    QString name;
    QDate date;
    int validity;
    bool complWithReq; // Соответствие требованиям
    float znachimost;
    int ktr;
    double kef;
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
    int kpr;
    int ktok;

    int kmol; // Коэффициент вопроса, а не раздела
    float sum;

    //Начальные основные коэффициенты (не меняются после первого присвоения)
    float startZnachimost;
    int startKtr;
    float startKef;
    float startKpsp;
    int startKgdp;
    int startKsc;
    //Начальные дополнительные коэффициенты
    int startKdog;
    int startKrv;
    int startKvo;
    int startKzp;
    int startKot;
    int startKtsp;
    int startKpr;
    int startKtok;

    // Параметры для формулы КЭФ
    double m_paramA { 1.5 };
    double m_paramB { 1.3 };
};

#endif // CKOLDOG_H

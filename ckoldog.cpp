#include "ckoldog.h"
#include <QDebug>

CKolDog::CKolDog() {}

CKolDog::~CKolDog()
{
    //Узнать размер объекта
    /*
    qint32 mySize = getMySize();
    for (auto frag : fragments) {
        mySize += frag->getMySize();
    }
    qDebug() << "My size was: " << mySize;
    */

    for (auto frag : fragments)
        delete frag;
    fragments.clear();
}

void CKolDog::setMainParameters(QString id, QString name, QDate date, int validity, bool complWithReq, float znachimost,
                                int effektivnost, int ktr, float kef, float kpsp, int kgdp, int ksc, QDate endDate,
                                int kdog, int krv, int kvo, int kzp, int kot, int ktsp, int kots, int kmol, float sum)
{
    this->id = id;
    this->name = name;
    this->date = date;
    this->validity = validity;
    this->complWithReq = complWithReq;
    this->znachimost = znachimost;
    this->effektivnost = effektivnost;
    this->ktr = ktr;
    this->kef = kef;
    this->kpsp = kpsp;
    this->kgdp = kgdp;
    this->ksc = ksc;
    this->endDate = endDate;
    this->kdog = kdog;
    this->krv = krv;
    this->kvo = kvo;
    this->kzp = kzp;
    this->kot = kot;
    this->ktsp = ktsp;
    this->kots = kots;
    this->kmol = kmol;
    this->sum = sum;
}

QVariantList CKolDog::getFiveCurrentKeffs()
{
    return QVariantList({ ktr, ksc, kgdp, kpsp, kef });
}

void CKolDog::addFragAfter(qint32 pos, fragment *frag)
{
    fragments.insert(pos + 1, frag);
}

void CKolDog::addFragOnFirstPos(fragment *frag)
{
    fragments.insert(0, frag);
}

void CKolDog::calculateCurrentKeffs()
{
    _resetKeffs();
    for (auto frag : fragments) {
        if (frag->isViDoSv()) {
            //узнать какой раздел и куда плюсовать и плюсовать
            QString razdel = frag->getRazdel();
            if (razdel == "ПСП") {
                kpsp++;
            } else if (razdel == "ДОГ") {
                kdog++;
                ktr++;
            } else if (razdel == "РВ") {
                krv++;
                ktr++;
            } else if (razdel == "ВО") {
                kvo++;
                ktr++;
            } else if (razdel == "ГДП") {
                kgdp++;
            } else if (razdel == "ЗП") {
                kzp++;
                ktr++;
            } else if (razdel == "ОТ") {
                kot++;
                ktr++;
            } else if (razdel == "ТСП") {
                ktsp++;
                ktr++;
            } else if (razdel == "СЦ") {
                ksc++;
            } else if (razdel == "ТОК") {
                kmol++;
                ktr++;
            } else if (razdel == "ПР") {
                kots++;
                ktr++;
            }
            kef++;
        } else if (frag->isUt()) {
            kpsp += static_cast<float>(0.3);
        }
    }
}

QJsonDocument *CKolDog::packKolDogToJson()
{
    auto jDoc = new QJsonDocument();
    auto jObjInsideDoc = jDoc->object();

    QJsonObject mainSettings;
    mainSettings.insert("id", id);
    mainSettings.insert("name", name);
    mainSettings.insert("dateStr", date.toString("dd.MM.yyyy"));
    mainSettings.insert("validity", validity);
    mainSettings.insert("complWithReq", complWithReq);
    mainSettings.insert("znachimost", znachimost);
    mainSettings.insert("effektivnost", effektivnost);
    mainSettings.insert("ktr", ktr);
    mainSettings.insert("kef", kef);
    mainSettings.insert("kpsp", kpsp);
    mainSettings.insert("kgdp", kgdp);
    mainSettings.insert("ksc", ksc);
    mainSettings.insert("endDateStr", endDate.toString("dd.MM.yyyy"));
    mainSettings.insert("kdog", kdog);
    mainSettings.insert("krv", krv);
    mainSettings.insert("kvo", kvo);
    mainSettings.insert("kzp", kzp);
    mainSettings.insert("kot", kot);
    mainSettings.insert("ktsp", ktsp);
    mainSettings.insert("kots", kots);
    mainSettings.insert("kmol", kmol);
    mainSettings.insert("sum", sum);
    jObjInsideDoc["mainSettings"] = mainSettings;

    QJsonArray jArray;
    for (auto frag : fragments) {
        auto jFrag = new QJsonObject();
        jFrag->insert("text", frag->getText());
        jFrag->insert("kachestvo", frag->getKachestvo());
        jFrag->insert("akt", frag->getAkt());
        jFrag->insert("voprosABR", frag->getVoprosABR());
        jFrag->insert("razdel", frag->getRazdel());
        jFrag->insert("size", frag->getSize());
        jFrag->insert("changed", frag->isChanged());
        jFrag->insert("newAdded", frag->isNewAdded());
        jFrag->insert("visible", frag->isVisible());
        jFrag->insert("ViDoSv", frag->isViDoSv());
        jFrag->insert("Ut", frag->isUt());
        jArray.append(*jFrag);
    }
    jObjInsideDoc["fragments"] = jArray;

    jDoc->setObject(jObjInsideDoc);
    return jDoc;
}

void CKolDog::_resetKeffs()
{
    //Основные
    ktr = 0;
    ksc = 0;
    kgdp = 0;
    kpsp = 0;
    kef = 0;
    //Подробные
    kdog = 0;
    krv = 0;
    kvo = 0;
    kzp = 0;
    kot = 0;
    // WARNING: ОшибкаКэф kots, kpr, kmol пройти по всему проекту и узнать их
    kots = 0;
    // kpr = 0;
    kmol = 0;
    ktsp = 0;
}

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

void CKolDog::setMainParameters(QString id, QDate date, int validity, bool complWithReq, float znachimost, int ktr,
                                float kpsp, int kgdp, int ksc, QDate endDate, int kdog, int krv, int kvo, int kzp,
                                int kot, int ktsp, int kpr, int ktok, int kmol, float sum)
{
    this->id = id;
    this->date = date;
    this->validity = validity;
    this->complWithReq = complWithReq;
    this->znachimost = znachimost;
    this->ktr = ktr;
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
    this->kpr = kpr;
    this->ktok = ktok;
    this->kmol = kmol;
    this->sum = sum;
}

void CKolDog::calculateKprAndKtok(qint32 &kpr, qint32 &ktok)
{
    qint32 pr = 0, tok = 0;
    for (auto frag : fragments) {
        if (frag->isViDoSv()) {
            if (frag->getRazdel() == "ПР") {
                pr++;
            } else if (frag->getRazdel() == "ТОК") {
                tok++;
            }
        }
    }
    kpr = pr;
    ktok = tok;
}

void CKolDog::setStartMinorKeffs(int skdog, int skrv, int skvo, int skzp, int skot, int sktsp, int skpr, int sktok)
{
    startKdog = skdog;
    startKvo = skvo;
    startKzp = skzp;
    startKot = skot;
    startKtsp = sktsp;
    startKpr = skpr;
    startKtok = sktok;
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
                ktok++;
                ktr++;
            } else if (razdel == "ПР") {
                kpr++;
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
    mainSettings.insert("startZnachimost", startZnachimost);
    mainSettings.insert("startKtr", startKtr);
    mainSettings.insert("startKef", startKef);
    mainSettings.insert("startKpsp", startKpsp);
    mainSettings.insert("startKgdp", startKgdp);
    mainSettings.insert("startKsc", startKsc);
    mainSettings.insert("ktr", ktr);
    mainSettings.insert("kef", kef);
    mainSettings.insert("kpsp", kpsp);
    mainSettings.insert("kgdp", kgdp);
    mainSettings.insert("ksc", ksc);
    mainSettings.insert("endDateStr", endDate.toString("dd.MM.yyyy"));
    mainSettings.insert("startKdog", startKdog);
    mainSettings.insert("startKrv", startKrv);
    mainSettings.insert("startKvo", startKvo);
    mainSettings.insert("startKzp", startKzp);
    mainSettings.insert("startKot", startKot);
    mainSettings.insert("startKtsp", startKtsp);
    mainSettings.insert("startKpr", startKpr);
    mainSettings.insert("startKtok", startKtok);
    mainSettings.insert("kdog", kdog);
    mainSettings.insert("krv", krv);
    mainSettings.insert("kvo", kvo);
    mainSettings.insert("kzp", kzp);
    mainSettings.insert("kot", kot);
    mainSettings.insert("ktsp", ktsp);
    mainSettings.insert("kpr", kpr);
    mainSettings.insert("ktok", ktok);
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

float CKolDog::calculateKzn()
{
    qint32 Npsp = 0;
    for (auto frag : fragments) {
        if (frag->isViDoSv() && frag->getRazdel() == "ПСП")
            Npsp++;
    }
    //кзн=(ктр+ксц+кгдп+Nпсп)/M*100% (M-число всех пунктов КД,Nпсп-колвоПСПпунктов с vidosv)
    qint32 tmp = (ktr + ksc + kgdp + Npsp) * 10000
            / fragments.size(); // 1000, а не 100, чтобы потом посчитать знак после запятой
    this->znachimost = tmp / 100 + (tmp % 100 * 0.01);
    return znachimost;
}

void CKolDog::incrementMinorKeff(QString razdAbr)
{
    if (razdAbr == "ДОГ") {
        kdog++;
    } else if (razdAbr == "РВ") {
        krv++;
    } else if (razdAbr == "ВО") {
        kvo++;
    } else if (razdAbr == "ЗП") {
        kzp++;
    } else if (razdAbr == "ОТ") {
        kot++;
    } else if (razdAbr == "ТСП") {
        ktsp++;
    } else if (razdAbr == "ПР") {
        kpr++;
    } else if (razdAbr == "ТОК") {
        ktok++;
    }
}

void CKolDog::decrementMinorKeff(QString razdAbr)
{
    if (razdAbr == "ДОГ") {
        kdog--;
    } else if (razdAbr == "РВ") {
        krv--;
    } else if (razdAbr == "ВО") {
        kvo--;
    } else if (razdAbr == "ЗП") {
        kzp--;
    } else if (razdAbr == "ОТ") {
        kot--;
    } else if (razdAbr == "ТСП") {
        ktsp--;
    } else if (razdAbr == "ПР") {
        kpr--;
    } else if (razdAbr == "ТОК") {
        ktok--;
    }
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
    kpr = 0;
    ktok = 0;
    kmol = 0;
    ktsp = 0;
}

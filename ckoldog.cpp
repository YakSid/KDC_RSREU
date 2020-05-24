#include "ckoldog.h"
#include <QDebug>

QString CKolDog::getId()
{
    return id;
}

QString CKolDog::getName()
{
    return name;
}

QDate CKolDog::getDate()
{
    return date;
}

uint CKolDog::getValidity()
{
    return validity;
}

bool CKolDog::getComplWithReq()
{
    return complWithReq;
}

float CKolDog::getZnachimost()
{
    return znachimost;
}

int CKolDog::getEffektivnost()
{
    return effektivnost;
}

int CKolDog::getKtr()
{
    return ktr;
}

float CKolDog::getKef()
{
    return kef;
}

float CKolDog::getKpsp()
{
    return kpsp;
}

int CKolDog::getKgdp()
{
    return kgdp;
}

int CKolDog::getKsc()
{
    return ksc;
}

QDate CKolDog::getEndDate()
{
    return endDate;
}

int CKolDog::getKdog()
{
    return kdog;
}

int CKolDog::getKrv()
{
    return krv;
}

int CKolDog::getKvo()
{
    return kvo;
}

int CKolDog::getKzp()
{
    return kzp;
}

int CKolDog::getKot()
{
    return kot;
}

int CKolDog::getKtsp()
{
    return ktsp;
}

int CKolDog::getKots()
{
    return kots;
}

int CKolDog::getKmol()
{
    return kmol;
}

float CKolDog::getSum()
{
    return sum;
}

CKolDog::CKolDog() {}

CKolDog::~CKolDog()
{
    for (auto frag : fragments)
        delete frag;
    fragments.clear();
}

void CKolDog::setMainParameters(QString id, QString name, QDate date, uint validity, bool complWithReq,
                                float znachimost, int effektivnost, int ktr, float kef, float kpsp, int kgdp, int ksc,
                                QDate endDate, int kdog, int krv, int kvo, int kzp, int kot, int ktsp, int kots,
                                int kmol, float sum)
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
    // TODO: высчитывание текущих коэффициентов
}

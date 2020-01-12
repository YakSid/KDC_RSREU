#include "ckoldog.h"

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

float CKolDog::getSum()
{
    return sum;
}

CKolDog::CKolDog() {}

void CKolDog::setMainParameters(QString id, QString name, QDate date, uint validity, bool complWithReq,
                                float znachimost, int effektivnost, int ktr, float kef, float kpsp, int kgdp, int ksc,
                                QDate endDate, int kdog, int krv, int kvo, int kzp, int kot, int ktsp, int kots,
                                float sum)
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
    this->sum = sum;
}

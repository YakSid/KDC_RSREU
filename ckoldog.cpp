#include "ckoldog.h"

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

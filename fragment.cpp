#include "fragment.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

fragment::fragment() {}

fragment::~fragment() {}

void fragment::SetPositions(int posStart, int posEnd)
{
    firstPos = posStart;
    lastPos = posEnd;
    size = lastPos - firstPos;
}

void fragment::Resize()
{
    size = lastPos - firstPos;
}

qint32 fragment::getVoprosNumber()
{
    QSqlQuery querySelect;
    querySelect.prepare("SELECT Вопросы.Код FROM Вопросы WHERE Вопросы.вопрос = :val1 AND Вопросы.раздел = :val2");
    querySelect.bindValue(":val1", voprosABR);
    querySelect.bindValue(":val2", razdel);
    if (!querySelect.exec()) {
        qDebug() << querySelect.lastError().text();
    }
    if (querySelect.next()) {
        return querySelect.value(0).toInt();
    } else {
        return -1;
    }
}

QVariantList fragment::getKeffsDelta(const fragment *pastFrag)
{
    //! Дельты которые являются одной из основных дельт, но ещё не понятно какой
    qint32 unknownRazdDelta = 0, prevFragUnknownRazdDelta = 0;
    //! Основные дельты
    qint32 deltaKTR = 0, deltaKSC = 0, deltaKGDP = 0;
    double deltaKPSP = 0.0, deltaKEF = 0.0;

    if (pastFrag->razdel != razdel) {
        //Раздел изменился
        //Вычитаем прошлые бонусы если были
        if (pastFrag->isViDoSv()) {
            prevFragUnknownRazdDelta -= 1;
            deltaKEF -= 1.0;
        } else if (pastFrag->isUt()) {
            deltaKPSP -= 0.3;
        }
        //Вычисляем новые если есть
        if (isViDoSv()) {
            unknownRazdDelta += 1;
            deltaKEF += 1.0;
        } else if (isUt()) {
            deltaKPSP += 0.3;
        }
    } else {
        //Раздел не изменился
        if (pastFrag->isViDoSv()) {
            if (!isViDoSv()) {
                unknownRazdDelta -= 1;
                deltaKEF -= 1.0;
            }
        } else {
            if (isViDoSv()) {
                unknownRazdDelta += 1;
                deltaKEF += 1.0;
            }
        }
        if (pastFrag->isUt()) {
            if (!isUt()) {
                deltaKPSP -= 0.3;
            }
        } else {
            if (isUt()) {
                deltaKPSP += 0.3;
            }
        }
    }
    //Определяем какой основной дельтой являются найденные дельты
    QString pastRazd = pastFrag->getRazdel();
    if (pastRazd == "СЦ") {
        deltaKSC += prevFragUnknownRazdDelta;
    } else if (pastRazd == "ГДП") {
        deltaKGDP += prevFragUnknownRazdDelta;
    } else if (pastRazd == "ПСП") {
        deltaKPSP += static_cast<double>(prevFragUnknownRazdDelta);
    } else {
        deltaKTR += prevFragUnknownRazdDelta;
    }
    if (razdel == "СЦ") {
        deltaKSC += unknownRazdDelta;
    } else if (razdel == "ГДП") {
        deltaKGDP += unknownRazdDelta;
    } else if (razdel == "ПСП") {
        deltaKPSP += static_cast<double>(unknownRazdDelta);
    } else {
        deltaKTR += unknownRazdDelta;
    }

    QVariantList delta = { deltaKTR, deltaKSC, deltaKGDP, deltaKPSP, deltaKEF };
    return delta;
}

QVariantList fragment::getKeffsDeltaFromZero()
{
    double unknownRazdDelta = 0;
    qint32 deltaKTR = 0, deltaKSC = 0, deltaKGDP = 0;
    double deltaKPSP = 0.0, deltaKEF = 0.0;

    if (kachestvo == "Вы" || kachestvo == "До" || kachestvo == "Св") {
        unknownRazdDelta = 1;
        deltaKEF = 1;
    } else if (kachestvo == "Ут" && razdel == "ПСП") {
        unknownRazdDelta = 0.3;
    }

    if (razdel == "СЦ") {
        deltaKSC += static_cast<int>(unknownRazdDelta);
    } else if (razdel == "ГДП") {
        deltaKGDP += static_cast<int>(unknownRazdDelta);
    } else if (razdel == "ПСП") {
        deltaKPSP += unknownRazdDelta;
    } else {
        deltaKTR += static_cast<int>(unknownRazdDelta);
    }

    QVariantList delta = { deltaKTR, deltaKSC, deltaKGDP, deltaKPSP, deltaKEF };
    return delta;
}

QVariantList fragment::getKeffsDeltaToZero()
{
    auto inverseDelta = getKeffsDeltaFromZero();

    QVariantList delta = { -inverseDelta[0].toInt(), -inverseDelta[1].toInt(), -inverseDelta[2].toInt(),
                           -inverseDelta[3].toDouble(), -inverseDelta[4].toDouble() };
    return delta;
}

QString fragment::getAffectsOnMinorKeffs()
{
    if (isViDoSv()) {
        if (razdel == "ДОГ" || razdel == "РВ" || razdel == "ВО" || razdel == "ЗП" || razdel == "ОТ" || razdel == "ТСП"
            || razdel == "ПР" || razdel == "ТОК") {
            return razdel;
        }
    }
    return "";
}

void fragment::updateFlagsViDoSvUt()
{
    if (kachestvo == "Вы" || kachestvo == "До" || kachestvo == "Св") {
        setViDoSv(true);
        setUt(false);
    } else {
        setViDoSv(false);
        if (kachestvo == "Ут" && getRazdel() == "ПСП") {
            setUt(true);
        } else {
            setUt(false);
        }
    }
}

#include "fragment.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

fragment::fragment() {}

fragment::~fragment() {}

void fragment::SetPositions(int p1, int p2)
{
    PositionOfFirst = p1;
    PositionOfLast = p2;
    Size = PositionOfLast - PositionOfFirst;
}

void fragment::SetArguments(QString txt, QString kach, QString akt)
{
    text = txt;
    Kachestvo = kach;
    Akt = akt;
}

void fragment::Resize()
{
    Size = PositionOfLast - PositionOfFirst;
}

qint32 fragment::getVoprosNumber()
{
    QSqlQuery querySelect;
    querySelect.prepare("SELECT Вопросы.Код FROM Вопросы WHERE Вопросы.вопрос = :val1 AND Вопросы.раздел = :val2");
    querySelect.bindValue(":val1", VoprosABR);
    querySelect.bindValue(":val2", Razdel);
    if (!querySelect.exec()) {
        qDebug() << querySelect.lastError().text();
    }
    if (querySelect.next()) {
        return querySelect.value(0).toInt();
    } else {
        return -1;
    }
}

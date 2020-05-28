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

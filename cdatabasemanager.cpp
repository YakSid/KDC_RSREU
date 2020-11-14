#include "cdatabasemanager.h"
#include <QSqlQuery>
#include "cconstants.h"

// TODO: [Улучшение] Переделать по нормальному подключение БД и стартовый диалог
CDatabaseManager::CDatabaseManager(QString name)
{
    if (name.isEmpty())
        name = QDir::currentPath() + "\\БД МК.mdb";
    QString ACC = "DRIVER={Microsoft Access Driver (*.mdb)}; FIL={MS Access}; DBQ=";
    QString tmpPath = name;
    while (tmpPath.contains('/'))
        tmpPath.replace(tmpPath.indexOf('/'), 1, '\\');
    ACC += tmpPath;
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setDatabaseName(ACC);
    m_db.open();

    //Подготовка таблицы ТЗакон локально
    if (TOrder.size()) {
        for (auto order : TOrder) {
            delete order;
        }
        TOrder.clear();
    }
    QSqlQuery queryLawSelect;
    queryLawSelect.prepare(
            "SELECT ТЗакон.КодЗакона, ТЗакон.НазвЗакона, ТЗакон.ДатаПринятия, ТЗакон.ДатаИзмен FROM ТЗакон");
    if (!queryLawSelect.exec()) {
        qDebug() << queryLawSelect.lastError().text();
    }
    while (queryLawSelect.next()) {
        auto order = new structOrder();
        order->id = queryLawSelect.value(0).toInt();
        order->name = queryLawSelect.value(1).toString();
        order->dateAdoptation = queryLawSelect.value(2).toDate();
        order->dateChange = queryLawSelect.value(3).toDate();
        TOrder.append(order);
    }
}

CDatabaseManager::~CDatabaseManager()
{
    for (auto order : TOrder) {
        delete order;
    }
    TOrder.clear();

    if (m_db.isOpen()) {
        m_db.close();
    }
}

QSqlDatabase *CDatabaseManager::getDatabase()
{
    return &m_db;
}

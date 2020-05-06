#include "cdatabasemanager.h"

CDatabaseManager::CDatabaseManager()
{
    QString ACC = "DRIVER={Microsoft Access Driver (*.mdb)}; FIL={MS Access}; DBQ=";
    QString tmpPath = QDir::currentPath() + "\\БД МК.mdb";
    while (tmpPath.contains('/'))
        tmpPath.replace(tmpPath.indexOf('/'), 1, '\\');
    ACC += tmpPath;
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setDatabaseName(ACC);
    m_db.open();
}

CDatabaseManager::~CDatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

QSqlDatabase *CDatabaseManager::getDatabase()
{
    return &m_db;
}

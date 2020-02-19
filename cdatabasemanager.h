#ifndef CDATABASEMANAGER_H
#define CDATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QDir>

class CDatabaseManager
{
public:
    CDatabaseManager();
    ~CDatabaseManager();
    QSqlDatabase *getDatabase();

private:
    QSqlDatabase m_db;
};

#endif // CDATABASEMANAGER_H

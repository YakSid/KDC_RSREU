#ifndef MODELLIST_H
#define MODELLIST_H

#include <QtSql>
#include <QDateTime>

class modelList : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    modelList();

protected:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MODELLIST_H

#ifndef MODELSECOND_H
#define MODELSECOND_H

#include <QtSql>
#include <QDateTime>

class modelSecond : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    modelSecond();

protected:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MODELSECOND_H

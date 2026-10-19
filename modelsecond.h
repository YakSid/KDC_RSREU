#ifndef MODELSECOND_H
#define MODELSECOND_H

#include <QDateTime>
#include <QtSql>

class modelSecond : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit modelSecond(QObject *parent = nullptr);
    ~modelSecond();

protected:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // MODELSECOND_H

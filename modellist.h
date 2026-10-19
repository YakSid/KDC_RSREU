#ifndef MODELLIST_H
#define MODELLIST_H

#include <QDateTime>
#include <QtSql>

class modelList : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit modelList(QObject *parent = nullptr);
    ~modelList();

protected:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MODELLIST_H

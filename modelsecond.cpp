#include "modelsecond.h"

modelSecond::modelSecond() {}

QVariant modelSecond::data(const QModelIndex &index, int role) const
{
    if (index.column() > 1) {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        }
    }
    if (index.column() == 1) {
        if (role == Qt::DisplayRole) {
            QModelIndex NewIndex = QAbstractItemModel::createIndex(index.row(), index.column() - 1);
            QSqlQuery a_query;
            QString val1 = NewIndex.data().toString();
            a_query.prepare("SELECT ТУчреждение.ИмяУчреждения FROM ТУчреждение WHERE "
                            "ТУчреждение.КодУчреждения = :val1");
            a_query.bindValue(":val1", val1);
            if (!a_query.exec()) {
                qDebug() << a_query.lastError().text();
            }
            if (!a_query.next()) {
                qDebug() << a_query.lastError().text();
            }
            return a_query.value(0).toString();
        }
    }
    return QSqlTableModel::data(index, role);
}

#include "modelsecond.h"
#include "cconstants.h"

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
            return INSTITUTE_NAMES.value(NewIndex.data().toString());
        }
    }
    if (index.column() == 10) {
        return QString::number(index.row());
    }
    return QSqlTableModel::data(index, role);
}

QVariant modelSecond::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QSqlTableModel::headerData(section, orientation, role);
}

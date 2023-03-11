#include "modellist.h"
#include "cconstants.h"

QMap<QString, QString> INSTITUTE_NAMES;

modelList::modelList() {}

QVariant modelList::data(const QModelIndex &index, int role) const
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
    if (index.column() == 2) {
        if (role == Qt::DisplayRole) {
            QModelIndex NewIndex = QAbstractItemModel::createIndex(index.row(), index.column() + 4);
            QModelIndex NewIndex2 = QAbstractItemModel::createIndex(index.row(), index.column() + 7);
            return 1.5 * QSqlTableModel::data(NewIndex, role).toDouble()
                    + QSqlTableModel::data(NewIndex2, role).toDouble();
        }
        if (role == Qt::FontRole) {
            QFont Font;
            Font.setBold(true);
            return Font;
        }
    }
    if (index.column() == 5) {
        if (role == Qt::DisplayRole) {
            QModelIndex NewIndex2 = QAbstractItemModel::createIndex(index.row(), index.column() + 2);
            QModelIndex NewIndex3 = QAbstractItemModel::createIndex(index.row(), index.column() + 3);
            QModelIndex NewIndex4 = QAbstractItemModel::createIndex(index.row(), index.column() + 1);
            QModelIndex NewIndex5 = QAbstractItemModel::createIndex(index.row(), index.column() + 4);
            QVariant tmp = 1.5 * QSqlTableModel::data(NewIndex4, role).toDouble()
                    + QSqlTableModel::data(NewIndex5, role).toDouble();
            QVariant got = 1.3 * tmp.toDouble() + QSqlTableModel::data(NewIndex2, role).toDouble()
                    + QSqlTableModel::data(NewIndex3, role).toDouble();
            return got;
        }
        if (role == Qt::FontRole) {
            QFont Font;
            Font.setBold(true);
            return Font;
        }
    }
    if (index.column() == 2 || index.column() == 4 || index.column() == 5) {
        QString tmp;
        if (role == Qt::DisplayRole) {
            tmp = QString::number(QSqlTableModel::data(index, role).toDouble(), 'f', 1);
            return tmp.replace(tmp.indexOf('.'), 1, ',');
        }
    }
    if (index.column() == 3) {
        if (role == Qt::DisplayRole) {
            return QSqlTableModel::data(index, role).toDate();
        }
    }
    return QSqlTableModel::data(index, role);
}

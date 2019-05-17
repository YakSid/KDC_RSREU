#ifndef LISTKD_H
#define LISTKD_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include "QStandardItem"
#include "modellist.h"
#include <QSortFilterProxyModel>
#include <QDir>
#include "startdialog.h"
#include "detailktr.h"

namespace Ui {
class ListKD;
}

class QSqlRelationalTableModel;

class ListKD : public QDialog
{
    Q_OBJECT

public:
    explicit ListKD(QWidget *parent = nullptr);
    ~ListKD();
    QString SelectedKD = "";

private slots:
    void on_Ref_clicked();

    void on_DetailKTR_clicked();

    void on_SelectKD_clicked();

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::ListKD *ui;
    modelList *modelForList;
    QSqlDatabase DatabaseForList;
    QSortFilterProxyModel *proxyModel;
    //del this \/
    StartDialog sDialog;
};

#endif // LISTKD_H

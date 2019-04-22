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
    int SelectedKD=-1;

private slots:
    void on_Ref_clicked();

    void on_DetailKTR_clicked();

    void on_SelectKD_clicked();

private:
    Ui::ListKD *ui;
    //QSqlRelationalTableModel *modelForList;
    modelList *modelForList;
    QSqlDatabase DatabaseForList;
};

#endif // LISTKD_H

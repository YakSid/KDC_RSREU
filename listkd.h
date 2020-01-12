#ifndef LISTKD_H
#define LISTKD_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include "QStandardItem"
#include "modellist.h"
#include <QSortFilterProxyModel>
#include <QDir>
#include "startdialog.h"

namespace Ui {
class ListKD;
}

class ListKD : public QDialog
{
    Q_OBJECT

public:
    explicit ListKD(QWidget *parent = nullptr);
    ~ListKD();
    QString SelectedKD = "";
    bool WantGo = false;

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
    enum EViewMode { eStandardView, eDetailView };
    EViewMode viewMode { eStandardView };

private:
    void _prepareView(EViewMode mode);
};

#endif // LISTKD_H

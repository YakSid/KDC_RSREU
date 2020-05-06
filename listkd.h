#ifndef LISTKD_H
#define LISTKD_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include "QStandardItem"
#include "modellist.h"
#include <QSortFilterProxyModel>
#include <QDir>
#include "startdialog.h"
#include "cdatabasemanager.h"

enum EViewMode { eStandardView, eDetailView };
//! Значение опции выбора
enum ESearchState : qint8 {
    eSearchUnknown = -1,
    eSearchKodKD,
    eSearchKeyUchrejdenie,
    eSearchDate,
    eSearchKtr,
    eSearchKsc,
    eSearchKtsc,
    eSearchKgdp,
    eSearchKpsp,
    eSearchKzn,
    eSearchEffectivnost
};

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

    void on_cmb_search_currentIndexChanged(int index);

    void on_ln_search_textChanged(const QString &arg1);

private:
    void _prepareView(EViewMode mode);
    //! Поиск
    void _search(ESearchState state, QString text);
    //! Отобразить скрытые ключи
    void _showHiddenKeys();

private:
    Ui::ListKD *ui;
    CDatabaseManager *m_db;
    modelList *modelForList;
    QSortFilterProxyModel *proxyModel;
    EViewMode viewMode { eStandardView };
    //! Опция выбора
    ESearchState searchState { eSearchKodKD };
    QString prevSearchRequest;
};

#endif // LISTKD_H

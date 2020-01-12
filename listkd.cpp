#include "listkd.h"
#include "ui_listkd.h"

ListKD::ListKD(QWidget *parent) : QDialog(parent), ui(new Ui::ListKD)
{
    QString ACC = "DRIVER={Microsoft Access Driver (*.mdb)}; FIL={MS Access}; DBQ=";
    QString tmpPath = QDir::currentPath() + "\\БД МК.mdb";
    while (tmpPath.contains('/'))
        tmpPath.replace(tmpPath.indexOf('/'), 1, '\\');
    ACC += tmpPath;
    DatabaseForList = QSqlDatabase::addDatabase("QODBC");
    DatabaseForList.setDatabaseName(ACC);
    if (!DatabaseForList.open()) {
        QMessageBox::critical(this, "Master KDA - Error", DatabaseForList.lastError().text());
        exit(4);
    }

    ui->setupUi(this);
    ui->SelectKD->setEnabled(false);

    _prepareView(eStandardView);
}

ListKD::~ListKD()
{
    delete ui;
}

void ListKD::on_Ref_clicked() {}

void ListKD::on_DetailKTR_clicked()
{
    modelForList->clear();
    if (viewMode == eStandardView) {
        _prepareView(eDetailView);
        viewMode = eDetailView;
    } else {
        _prepareView(eStandardView);
        viewMode = eStandardView;
    }
}

void ListKD::on_SelectKD_clicked()
{
    WantGo = true;
    ListKD::close();
}

void ListKD::on_tableView_clicked(const QModelIndex &index)
{
    SelectedKD = proxyModel->data(index.siblingAtColumn(0), Qt::DisplayRole).toString();
    ui->SelectKD->setEnabled(true);
}

void ListKD::_prepareView(EViewMode mode)
{
    // TODO: Сделать второй класс модели листа для детализированного показа и разблокировать кнопку "Детализировать"
    modelForList = new modelList();
    modelForList->setTable("Договор");

    if (mode == eStandardView) {
        modelForList->removeColumns(3, 2);
        modelForList->removeColumns(6, 6);
        modelForList->removeColumns(9, 10);
        modelForList->removeColumns(9, 7);
    } else {
        modelForList->removeColumns(27, 6);
        modelForList->removeColumns(16, 5);
        modelForList->removeColumns(8, 5);
        modelForList->removeColumns(5, 2);
        modelForList->removeColumn(3);
        modelForList->removeColumn(2);
        modelForList->removeColumn(13);
        modelForList->removeColumn(1);
    }
    modelForList->removeColumn(1);

    modelForList->select();
    modelForList->setEditStrategy(QSqlTableModel::OnManualSubmit);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(modelForList);
    ui->tableView->setModel(proxyModel);
    ui->tableView->sortByColumn(1, Qt::AscendingOrder);
    // TODO: сделать правильную сортировку столбца даты
    modelForList->insertColumns(1, 2);
    modelForList->setHeaderData(0, Qt::Horizontal, tr("Код КД"));
    modelForList->setHeaderData(1, Qt::Horizontal, tr("Учреждение"));
    modelForList->setHeaderData(2, Qt::Horizontal, tr("Ктсц"));
    modelForList->setHeaderData(3, Qt::Horizontal, tr("Дата создания"));
    modelForList->setHeaderData(4, Qt::Horizontal, tr("Кзн"));

    if (mode == eStandardView) {
        // меняем столбцы для стандартного вида
        ui->tableView->horizontalHeader()->swapSections(4, 8);
        ui->tableView->horizontalHeader()->swapSections(5, 9);
        ui->tableView->horizontalHeader()->swapSections(2, 3);
        ui->tableView->horizontalHeader()->swapSections(3, 6);
        ui->tableView->horizontalHeader()->swapSections(4, 5);
        ui->tableView->horizontalHeader()->swapSections(5, 6);
    } else {
        // меняем столбцы для детализированного вида
    }
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->resizeSection(1, 369);
}

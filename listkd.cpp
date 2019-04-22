#include "listkd.h"
#include "ui_listkd.h"

#define ACCESS "DRIVER={Microsoft Access Driver (*.mdb)};" \
            "FIL={MS Access};DBQ=C:\\users\\Yakov\\desktop\\БД Диплом.mdb"

ListKD::ListKD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListKD)
{
    DatabaseForList = QSqlDatabase::addDatabase("QODBC");
    DatabaseForList.setDatabaseName(ACCESS);
    if (!DatabaseForList.open())
    {
        QMessageBox::critical(this, "Master KDA - Error", DatabaseForList.lastError().text());
        exit(4);
    }
    modelForList = new modelList();
    modelForList->setTable("Договор");
    //modelForList->setRelation(0, QSqlRelation("ТУчреждение", "КодУчреждения", "ИмяУчреждения"));
    modelForList->removeColumns(3,2);
    modelForList->removeColumns(6,6);
    modelForList->removeColumns(9,4);
    modelForList->removeColumns(9,13);
    modelForList->removeColumn(1);
    modelForList->select();
    modelForList->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->setupUi(this);

    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(modelForList);
    ui->tableView->setModel(proxyModel);
    //ui->tableView->setModel(modelForList);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);
    modelForList->insertColumns(1,2);
    modelForList->setHeaderData(0, Qt::Horizontal, tr("Код КД"));
    modelForList->setHeaderData(1, Qt::Horizontal, tr("Учреждение"));
    modelForList->setHeaderData(2, Qt::Horizontal, tr("Ктсц"));
    modelForList->setHeaderData(3, Qt::Horizontal, tr("Дата создания"));
    modelForList->setHeaderData(4, Qt::Horizontal, tr("Кзн"));
    ui->tableView->horizontalHeader()->swapSections(4,8);
    ui->tableView->horizontalHeader()->swapSections(5,9);
    ui->tableView->horizontalHeader()->swapSections(2,3);
    ui->tableView->horizontalHeader()->swapSections(3,6);
    ui->tableView->horizontalHeader()->swapSections(4,5);
    ui->tableView->horizontalHeader()->swapSections(5,6);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->resizeSection(1, 369);
}

ListKD::~ListKD()
{
    delete ui;
}

void ListKD::on_Ref_clicked()
{

}

void ListKD::on_DetailKTR_clicked()
{

}

void ListKD::on_SelectKD_clicked()
{
    SelectedKD = 0;
    ListKD::close();
}

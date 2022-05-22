#include "listkd.h"
#include "ui_listkd.h"

#include <QDesktopWidget>

ListKD::ListKD(QWidget *parent, QString dbName) : QDialog(parent), ui(new Ui::ListKD)
{
    ui->setupUi(this);

    ui->SelectKD->setEnabled(false);
    ui->cmb_search->setCurrentIndex(1);
    m_db = new CDatabaseManager(dbName);

    _prepareView(eStandardView);

    this->setStyleSheet("QPushButton:disabled {"
                        "background-color: darkGrey;"
                        "border: 2px solid darkGrey;}");

    //Центрируем окно по центру экрана
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(this);
    QPoint center = rect.center();
    int x = center.x() - (width() / 2);
    int y = center.y() - (height() / 2);
    center.setX(x);
    center.setY(y);
    move(center);
}

ListKD::~ListKD()
{
    qDebug() << "listDialog with db was deleted";
    delete ui;
    delete m_db;
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
    SelectedKD = proxyModel->data(index.sibling(index.row(), 0), Qt::DisplayRole).toString();
    ui->SelectKD->setEnabled(true);
}

void ListKD::_prepareView(EViewMode mode)
{
    // TODO: [старое, нужно?] Сделать второй класс модели листа для детализированного показа и разблокировать кнопку
    // "Детализировать"
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

    modelForList->insertColumns(1, 2);
    modelForList->setHeaderData(0, Qt::Horizontal, tr("Код КД"));
    modelForList->setHeaderData(1, Qt::Horizontal, tr("Учреждение"));
    modelForList->setHeaderData(2, Qt::Horizontal, tr("Ктсц"));
    modelForList->setHeaderData(3, Qt::Horizontal, tr("Дата заключения"));
    modelForList->setHeaderData(4, Qt::Horizontal, tr("Кзн"));
    modelForList->setHeaderData(5, Qt::Horizontal, tr("Эффективн.Кэф"));

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

void ListKD::_search(ESearchState state, QString text)
{
    if (state != eSearchUnknown) {
        for (int i = 0; i < proxyModel->rowCount(); i++) {
            const auto &index = proxyModel->index(i, state);
            QString indexText = ui->tableView->model()->data(index).toString().toLower();
            if (!indexText.contains(text.toLower())) {
                ui->tableView->setRowHidden(index.row(), true);
            }
        }
    }
}

void ListKD::_showHiddenKeys()
{
    QModelIndex index;
    for (int i = 0; i < proxyModel->rowCount(); i++) {
        index = proxyModel->index(i, 0);
        ui->tableView->setRowHidden(index.row(), false);
    }
}

void ListKD::on_ln_search_textChanged(const QString &arg1)
{
    if (prevSearchRequest.length() > arg1.length()) {
        if (searchState == eSearchUnknown) {
            QMessageBox msg;
            msg.setText("Выбрана не поддерживаемая опция поиска");
            msg.setWindowTitle("Поиск невозможен");
            msg.exec();
        }
    } else {
        _showHiddenKeys();
    }
    _search(searchState, arg1);
}
void ListKD::on_cmb_search_currentIndexChanged(int index)
{
    searchState = static_cast<ESearchState>(index);
    if (!ui->ln_search->text().isEmpty()) {
        _showHiddenKeys();
        _search(searchState, ui->ln_search->text());
    }
}

#include "listkd.h"
#include "ui_listkd.h"

#include <QScrollBar>
#include <QDesktopWidget>

ListKD::ListKD(QWidget *parent, QString dbName) : QDialog(parent), ui(new Ui::ListKD)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    ui->Ref->setVisible(false);
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

    connect(ui->tableView->verticalScrollBar(), &QScrollBar::valueChanged, this, &ListKD::onSliderMoved);
    ui->tableView->verticalScrollBar()->setVisible(false);
}

ListKD::~ListKD()
{
    qDebug() << "listDialog with db was deleted";
    delete ui;
    delete m_db;
}

void ListKD::onSliderMoved(int value)
{
    ui->lw_id->verticalScrollBar()->setValue(value);
}

void ListKD::on_Ref_clicked() {}

void ListKD::on_DetailKTR_clicked()
{
    if (modelForList)
        modelForList->clear();
    if (modelForSecond)
        modelForSecond->clear();
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
    setCursor(Qt::WaitCursor);

    if (mode == eStandardView) {
        modelForList = new modelList();
        modelForList->setTable("Договор");
        modelForList->removeColumns(3, 2);
        modelForList->removeColumns(6, 6);
        modelForList->removeColumns(9, 10);
        modelForList->removeColumns(9, 7);
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

        // меняем столбцы для стандартного вида
        ui->tableView->horizontalHeader()->swapSections(4, 8);
        ui->tableView->horizontalHeader()->swapSections(5, 9);
        ui->tableView->horizontalHeader()->swapSections(2, 3);
        ui->tableView->horizontalHeader()->swapSections(3, 6);
        ui->tableView->horizontalHeader()->swapSections(4, 5);
        ui->tableView->horizontalHeader()->swapSections(5, 6);

        if (!ui->lw_id->count()) {
            for (int i = 0; i <= modelForList->rowCount(); i++) {
                if (i == 0)
                    ui->lw_id->addItem("№");
                else
                    ui->lw_id->addItem(QString::number(i));
            }
        }

        // ui->tableView->verticalHeader()->setVisible(false);
    } else {
        modelForSecond = new modelSecond();
        modelForSecond->setTable("Договор");
        modelForSecond->removeColumns(28, 6);
        modelForSecond->removeColumns(16, 5);
        modelForSecond->removeColumns(8, 5);
        modelForSecond->removeColumns(5, 2);
        modelForSecond->removeColumn(3);
        modelForSecond->removeColumn(2);
        modelForSecond->removeColumn(1);
        modelForSecond->removeColumn(1);
        modelForSecond->removeColumns(2, 3);

        modelForSecond->select();
        modelForSecond->setEditStrategy(QSqlTableModel::OnManualSubmit);
        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(modelForSecond);
        ui->tableView->setModel(proxyModel);
        ui->tableView->sortByColumn(1, Qt::AscendingOrder);

        modelForSecond->insertColumn(1);

        modelForSecond->setHeaderData(0, Qt::Horizontal, tr("Код КД"));
        modelForSecond->setHeaderData(1, Qt::Horizontal, tr("Название"));
        modelForSecond->setHeaderData(2, Qt::Horizontal, tr("Ктр"));
        modelForSecond->setHeaderData(3, Qt::Horizontal, tr("ДОГ"));
        modelForSecond->setHeaderData(4, Qt::Horizontal, tr("РВ"));
        modelForSecond->setHeaderData(5, Qt::Horizontal, tr("ВО"));
        modelForSecond->setHeaderData(6, Qt::Horizontal, tr("ЗП"));
        modelForSecond->setHeaderData(7, Qt::Horizontal, tr("ОТ"));
        modelForSecond->setHeaderData(8, Qt::Horizontal, tr("ТСП"));
        modelForSecond->setHeaderData(9, Qt::Horizontal, tr("ТОК(МОЛ)"));

        // ui->tableView->verticalHeader()->setVisible(true);
        if (!ui->lw_id->count()) {
            for (int i = 0; i <= modelForSecond->rowCount(); i++) {
                if (i == 0)
                    ui->lw_id->addItem(" ");
                else
                    ui->lw_id->addItem(QString::number(i));
            }
        }
    }

    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->resizeSection(1, 369);

    setCursor(Qt::ArrowCursor);
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

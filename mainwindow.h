#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QtWidgets>
#include "startdialog.h"
#include "listkd.h"

namespace Ui {
class MainWindow;
}

class QSqlRelationalTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_treeWidgetRazd_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
    StartDialog sDialog;
    ListKD lDialog;
    QSqlRelationalTableModel *model;
    QSqlDatabase Database;
    QString SelectedKD;
    //Переменные для работы с навигацией по разделам
    void Tree_InsertItem (QTreeWidgetItem *, QString);
    int Tree_count;
    QTreeWidgetItem *Tree_currentItem;
    int Tree_currentColumn;
    //
};

#endif // MAINWINDOW_H

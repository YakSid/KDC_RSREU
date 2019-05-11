#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QtWidgets>
#include "startdialog.h"
#include "listkd.h"
#include "fragment.h"

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

    void on_TextCenter_cursorPositionChanged();

    void on_DeleteClause_clicked();

    void on_GoRight_clicked();

    void on_GoLeft_clicked();

    void on_NewClause_clicked();

private:
    Ui::MainWindow *ui;
    StartDialog sDialog;
    ListKD lDialog;
    QSqlRelationalTableModel *model;
    QSqlDatabase Database;
    QString SelectedKD;
    //Переменные для работы с навигацией по разделам
    void Tree_InsertItem (QTreeWidgetItem *, QString);
    void RecountPositions (int idfrag, int delta);
    QTreeWidgetItem *Tree_currentItem;
    int Tree_currentColumn;
    //
    fragment *frag;
    QVector <fragment*> Fragments;
    bool TextCenterIsBlocked = true;
    bool FragmentChanges = false;
    int SelectedFragment = -1;
    int OldSizeOfSelectedFragment = -1;
};

#endif // MAINWINDOW_H

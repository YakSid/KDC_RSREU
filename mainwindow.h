#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
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

private:
    Ui::MainWindow *ui;
    StartDialog sDialog;
    ListKD lDialog;
    QSqlRelationalTableModel *model;
    QSqlDatabase Database;
    QString SelectedKD;
};

#endif // MAINWINDOW_H

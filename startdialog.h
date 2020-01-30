#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

    int StartMode { 0 };
    QString authorName;

private slots:
    void on_Start_clicked();

    void on_pb_accept_clicked();

    void on_pb_addAuthor_clicked();

    void on_pb_deleteAuthor_clicked();

    void on_pb_changeAuthor_clicked();

private:
    void _addAuthor(QString name);
    QStringList _getAllAuthors();
    void _showMessage(QString text);
    void _updateLogfile(QStringList authors);
    void _clearLogfile();

private:
    Ui::StartDialog *ui;
    bool firstStart { true };
};

#endif // STARTDIALOG_H

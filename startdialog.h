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
    int StartMode=0;
    QString Path;

private slots:
    void on_Start_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::StartDialog *ui;
};

#endif // STARTDIALOG_H

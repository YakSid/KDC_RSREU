#ifndef KEF_H
#define KEF_H

#include <QDialog>

namespace Ui {
class kef;
}

class kef : public QDialog
{
    Q_OBJECT

public:
    explicit kef(QWidget *parent = nullptr);
    ~kef();

private slots:
    void on_pushButton_clicked();

private:
    Ui::kef *ui;
};

#endif // KEF_H

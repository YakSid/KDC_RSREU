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

public slots:
    void getKefs(int ktr, int ksc, int kgdp, float kpsp, float kef, float znahimost, int kdog, int krv, int kzp,
                 int kvo, int kot, int kots, int ktsp);

private:
    Ui::kef *ui;
};

#endif // KEF_H

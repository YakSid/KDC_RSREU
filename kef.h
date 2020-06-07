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
    void setStartKeffs(float kef, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kots, int ktsp,
                       int kmol);
    void setCurrentKeffs(float kef, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kots, int ktsp,
                         int kmol);
    //! Сравнение и раскрашивание
    void compareAndPaint();

private:
    Ui::kef *ui;
};

#endif // KEF_H

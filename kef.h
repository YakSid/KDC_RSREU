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
    void setStartKeffs(int ktr, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kpr, int ktok,
                       int ktsp);
    void setCurrentKeffs(int ktr, float znahimost, int kdog, int krv, int kzp, int kvo, int kot, int kpr, int ktok,
                         int ktsp);

private:
    //! Сравнить текущие коэффициенты с начальными и раскрасить
    void _compareAndPaint();

private:
    Ui::kef *ui;
};

#endif // KEF_H

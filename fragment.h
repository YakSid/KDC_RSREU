#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QString>

class fragment
{
public:
    fragment();
    ~fragment();
    void SetArguments(QString txt, QString kach, QString akt);
    void SetPositions(int p1, int p2);
    void Resize();
    //! Вычслить номер вопроса фрагмента зная его раздел и аббревиатуру вопроса
    qint32 getVoprosNumber();

public:
    QString text;
    qint32 PositionOfFirst, PositionOfLast, Size;
    QString Kachestvo, Akt, VoprosABR, Razdel;
    bool changed { false };
    bool isVisible { false };
};

#endif // FRAGMENT_H

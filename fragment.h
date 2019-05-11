#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QString>

class fragment
{
public:
    fragment(QString txt, int p1, int p2, QString kach, QString akt);
    ~fragment();
    QString text;
    int PositionOfFirst, PositionOfLast, Size;
    QString Kachestvo, Akt, VoprosABR, Razdel;
    void Resize();
};

#endif // FRAGMENT_H

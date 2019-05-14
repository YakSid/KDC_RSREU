#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QString>

class fragment
{
public:
    fragment();
    ~fragment();
    QString text;
    int PositionOfFirst, PositionOfLast, Size;
    QString Kachestvo, Akt, VoprosABR, Razdel;
    void SetArguments(QString txt, QString kach, QString akt);
    void SetPositions(int p1, int p2);
    void Resize();
};

#endif // FRAGMENT_H

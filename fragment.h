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

public:
    QString text;
    int PositionOfFirst, PositionOfLast, Size;
    QString Kachestvo, Akt, VoprosABR, Razdel;
    bool changed { false };
};

#endif // FRAGMENT_H

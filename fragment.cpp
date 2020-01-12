#include "fragment.h"

fragment::fragment() {}

fragment::~fragment() {}

void fragment::SetPositions(int p1, int p2)
{
    PositionOfFirst = p1;
    PositionOfLast = p2;
    Size = PositionOfLast - PositionOfFirst;
}

void fragment::SetArguments(QString txt, QString kach, QString akt)
{
    text = txt;
    Kachestvo = kach;
    Akt = akt;
}

void fragment::Resize()
{
    Size = PositionOfLast - PositionOfFirst;
}

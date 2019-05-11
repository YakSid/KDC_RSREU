#include "fragment.h"

fragment::fragment(QString txt, int p1, int p2, QString kach, QString akt)
{
    text = txt;
    PositionOfFirst = p1;
    PositionOfLast = p2;
    Size = PositionOfLast - PositionOfFirst;
    Kachestvo = kach;
    Akt = akt;
}

fragment::~fragment()
{

}

void fragment::Resize()
{
    Size = PositionOfLast - PositionOfFirst;
}

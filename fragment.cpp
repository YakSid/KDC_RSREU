#include "fragment.h"

fragment::fragment(QString txt, int p1, int p2)
{
    text = txt;
    PositionOfFirst = p1;
    PositionOfLast = p2;
}

fragment::~fragment()
{

}

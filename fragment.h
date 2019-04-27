#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QString>

class fragment
{
public:
    fragment(QString, int, int);
    ~fragment();
    QString text;
    int PositionOfFirst, PositionOfLast, Razdel;
};

#endif // FRAGMENT_H

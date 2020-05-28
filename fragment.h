#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QString>

class fragment
{
public:
    fragment();
    ~fragment();
    void SetPositions(int posStart, int posEnd);
    void Resize();
    //! Вычслить номер вопроса фрагмента зная его раздел и аббревиатуру вопроса
    qint32 getVoprosNumber();

    void setText(QString newText) { text = newText; }
    void setKachestvo(QString newKachestvo) { kachestvo = newKachestvo; }
    void setAkt(QString newAkt) { akt = newAkt; }
    void setVoprosABR(QString newVoprosABR) { voprosABR = newVoprosABR; }
    void setRazdel(QString newRazdel) { razdel = newRazdel; }
    void setPositionFirst(qint32 pos) { firstPos = pos; }
    void setPositionLast(qint32 pos) { lastPos = pos; }
    void setSize(qint32 newSize) { size = newSize; }
    void setChanged(bool state) { changed = state; }
    void setVisible(bool state) { visible = state; }

    QString getText() const { return text; }
    QString getKachestvo() const { return kachestvo; }
    QString getAkt() const { return akt; }
    QString getVoprosABR() const { return voprosABR; }
    QString getRazdel() const { return razdel; }
    qint32 getPositionFirst() const { return firstPos; }
    qint32 getPositionLast() const { return lastPos; }
    qint32 getSize() const { return size; }
    bool isChanged() const { return changed; }
    bool isVisible() const { return visible; }

private:
    //Основные параметры фрагмента
    QString text;
    QString kachestvo;
    QString akt;
    QString voprosABR;
    QString razdel;
    //Временные параметры фрагмента
    qint32 firstPos;
    qint32 lastPos;
    //Размер фрагмента
    qint32 size;
    //Состояния фрагмента
    bool changed { false };
    bool visible { false };
};

#endif // FRAGMENT_H

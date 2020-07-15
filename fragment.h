#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QString>
#include <QVariantList>

class fragment
{
public:
    fragment();
    ~fragment();
    void SetPositions(int posStart, int posEnd);
    void Resize();
    //! Вычислить номер вопроса фрагмента зная его раздел и аббревиатуру вопроса
    qint32 getVoprosNumber();
    //! Обновить состояние флагов isViDoSv и isUt
    void updateFlagsViDoSvUt();
    //Главные коэффициенты
    //! Вычислить как изменятся кэффы текущего фрагмента после изменений фрагмента в сравнении с его версией до изменений
    QVariantList getKeffsDelta(const fragment *pastFrag);
    //! Вычислить как изменятся кэффы если этот фрагмент новый
    QVariantList getKeffsDeltaFromZero();
    //! Вычислить как изменятся кэффы если этот фрагмент удалить
    QVariantList getKeffsDeltaToZero();
    //Минорные кэффы
    //! Узнать на какой минорный кэф влияет фрагмент
    QString getAffectsOnMinorKeffs();

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
    void setNewAdded(bool state) { newAdded = state; }
    void setViDoSv(bool state) { ViDoSv = state; }
    void setUt(bool state) { Ut = state; }

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
    bool isNewAdded() const { return newAdded; }
    bool isViDoSv() const { return ViDoSv; }
    //! Если значение "Ут", но раздел не "ПСП", то вернёт false
    bool isUt() const { return Ut; }
    //! Узнать размер объекта
    qint32 getMySize() const { return (sizeof(text) + sizeof(kachestvo) + sizeof(akt) + sizeof(razdel) + 4); }

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
    bool newAdded { false };
    bool visible { false };
    //! Имеет качество Вы, До или Св
    bool ViDoSv { false };
    //! Имеет качество Ут
    bool Ut { false };
};

#endif // FRAGMENT_H

#ifndef KNOWLEDGEBASE_H
#define KNOWLEDGEBASE_H

#include "fragment.h"
#include "cconstants.h"
#include <QDialog>

enum EFragmentsViewMode { eLaw, eTypicalKD, eAllKD };

namespace Ui {
class knowledgebase;
}

class knowledgebase : public QDialog
{
    Q_OBJECT

public:
    explicit knowledgebase(QWidget *parent = nullptr);
    ~knowledgebase();
    void prepareWindowWithoutFrag();

signals:
    void startTransportFrag(fragment *frag);

public slots:
    void getFragment(fragment *frag);

private slots:
    void on_pb_unlock_clicked();

    void on_rb_law_fragments_toggled(bool checked);

    void on_rb_typical_fragments_kd_toggled(bool checked);

    void on_rb_all_fragments_kd_toggled(bool checked);

    void on_ch_all_acts_toggled(bool checked);
    //! Делает выборку по заданным параметрам из базы данных, вызывается в каждом изменении аргументов
    void _select();

    void on_pb_insert_into_kd_clicked();

    void on_pb_next_clicked();

    void on_pb_prev_clicked();

    void on_pb_showList_clicked();

    void on_cmb_razdel_currentTextChanged(const QString &arg1);

private:
    void _changeViewMode(EFragmentsViewMode newViewMode);
    //! Провести начальную подготовку окна, неважно в каком режим с фрагментом или без
    void _prepareWindow();

private:
    Ui::knowledgebase *ui;
    EFragmentsViewMode m_currentViewMode { eTypicalKD };
    //! Номер фрагмента из подготовленного списка, -1 - изначальный
    qint32 currentFragmentNumber { -1 };
    qint32 m_currentVoprosNumber;
    QString originalText { "" };
    bool m_allActs { true }; // NOTE: временно true, потом сделать false
    bool m_unlocked { false };
    QList<QString> fragmentsForShow;
    struct LawInfo {
        QString order;
        QString adoptationDate;
        QString changeDate;
    };
    QList<LawInfo *> lawsInformations;
};

#endif // KNOWLEDGEBASE_H

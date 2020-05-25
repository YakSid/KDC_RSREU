#ifndef KNOWLEDGEBASE_H
#define KNOWLEDGEBASE_H

#include "fragment.h"
#include <QDialog>

const QStringList ListAct = { "КЗОТ | Решение общетрудовых вопросов", "ОБР | Решение отраслевых вопросов (образования)",
                              "ОТС | Решение по вопросам Отраслевого соглашения",
                              "ТРОТС | решение по вопросам Территориального соглашения" };
const QStringList ListRazd = { "ПСП | Социальное партнерство",
                               "ДОГ | Трудовой договор, занятость",
                               "РВ | Рабочее время",
                               "ВО | Время отдыха",
                               "ГДП | Гарантии профкома",
                               "ЗП | Заработная плата",
                               "ОТ | Охрана труда",
                               "ТСП | Трудовые споры",
                               "СЦ | Социально-бытовое обслуживание",
                               "ТОК | Труд отдельных категорий работников",
                               "ПР | Произв. экон. вопросы" };
const QStringList ListQuality = { "Ан | Аналоги положений",
                                  "Ут | Уточнение положений",
                                  "До | Реализация дозволений",
                                  "Вы | Повышение условий",
                                  "Св | Свои, специфические проблемы",
                                  "Др | Другое",
                                  "Фр | Формулировка",
                                  "Ни | Понижение условий" };
const QStringList AbbreviationAct = { "КЗОТ", "ОБР", "ОТС", "ТРОТС" };
const QStringList AbbreviationRazd = { "ПСП", "ДОГ", "РВ", "ВО", "ГДП", "ЗП", "ОТ", "ТСП", "СЦ", "ТОК", "ПР" };
const QStringList AbbreviationQuality = { "Ан", "Ут", "До", "Вы", "Св", "Др", "Фр", "Ни" };

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

private:
    void _setUpVopros();
    void _changeViewMode(EFragmentsViewMode newViewMode);

private:
    Ui::knowledgebase *ui;
    EFragmentsViewMode viewMode { eTypicalKD };
    QStringList m_listVopros;
    QStringList m_abbreviationVopros;
    //! Номер фрагмента из подготовленного списка, -1 - изначальный
    qint32 currentFragmentNumber { -1 };
    qint32 currentVoprosNumber;
    QString originalText { "" };
    bool m_allActs { false };
    QList<QString> fragmentsForShow;
};

#endif // KNOWLEDGEBASE_H

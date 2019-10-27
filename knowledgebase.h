#ifndef KNOWLEDGEBASE_H
#define KNOWLEDGEBASE_H

#include "fragment.h"
#include <QDialog>

namespace Ui {
class knowledgebase;
}

class knowledgebase : public QDialog
{
    Q_OBJECT

public:
    explicit knowledgebase(QWidget *parent = nullptr);
    ~knowledgebase();
    void setUpVopros();

    QStringList ListAct = { "КЗОТ | Решение общетрудовых вопросов", "ОБР | Решение отраслевых вопросов (образования)",
                            "ОТС | Решение по вопросам Отраслевого соглашения",
                            "ТРОТС | решение по вопросам Территориального соглашения" };
    QStringList ListRazd = { "ПСП | Социальное партнерство",
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
    QStringList ListQuality = { "Ан | Аналоги положений",
                                "Ут | Уточнение положений",
                                "До | Реализация дозволений",
                                "Вы | Повышение условий",
                                "Св | Свои, специфические проблемы",
                                "Др | Другое",
                                "Фр | Формулировка",
                                "Ни | Понижение условий" };
    QStringList AbbreviationAct = { "КЗОТ", "ОБР", "ОТС", "ТРОТС" };
    QStringList AbbreviationRazd = { "ПСП", "ДОГ", "РВ", "ВО", "ГДП", "ЗП", "ОТ", "ТСП", "СЦ", "ТОК", "ПР" };
    QStringList AbbreviationQuality = { "Ан", "Ут", "До", "Вы", "Св", "Др", "Фр", "Ни" };
    QStringList ListVopros;
    QStringList AbbreviationVopros;

public slots:
    void getFragment(fragment *frag);

private:
    Ui::knowledgebase *ui;
};

#endif // KNOWLEDGEBASE_H

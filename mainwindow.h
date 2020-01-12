#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QtWidgets>
#include "detailktr.h"
#include "kef.h"
#include "knowledgebase.h"
#include "ckoldog.h"

//! Задачи
//! TODO: сворачивание не главных окон
//! TODO: окно детализации КТР
//! TODO: база знаний
//! TODO: окно эффективности по разделам
//! TODO: работа с файлом проекта (верхняя панель)
//! TODO: везде сделать лэйауты не больше 1280x720, даже меньше можно
//! TODO: ~переводчик, словарь сокращений. К концу работы. И спросить нужен ли он вообще

namespace Ui {
class MainWindow;
}

class QSqlRelationalTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void s_sentFragment(fragment *frag);
    void s_sentKefs(int ktr, int ksc, int kgdp, float kpsp, float kef, float znahimost, int kdog, int krv, int kzp,
                    int kvo, int kot, int kots, int ktsp);

private slots:
    void on_treeWidgetRazd_itemClicked(QTreeWidgetItem *item, int column);

    void on_TextCenter_cursorPositionChanged();

    void on_DeleteClause_clicked();

    void on_GoRight_clicked();

    void on_GoLeft_clicked();

    void on_NewClause_clicked();

    void on_Razd_currentIndexChanged(int index);

    void on_Question_currentIndexChanged(int index);

    void on_Act_currentIndexChanged(int index);

    void on_Quality_currentIndexChanged(int index);

    void on_Effekt_po_razd_clicked();

    void on_BazeKnowledge_clicked();

    void on_pb_clearField_clicked();

private:
    //! ----------МЕТОДЫ----------
    //

private:
    Ui::MainWindow *ui;
    StartDialog sDialog;
    ListKD lDialog;
    QSqlRelationalTableModel *model;
    QSqlDatabase Database;
    QString SelectedKD;
    //Переменные для работы с навигацией по разделам
    void Tree_InsertItem(QTreeWidgetItem *, QString);
    void RecountPositions(int idfrag, int delta);
    QTreeWidgetItem *Tree_currentItem;
    int Tree_currentColumn;
    //
    CKolDog *currentKolDog;
    bool TextCenterIsBlocked = true;
    int SelectedFragment = -1;
    int OldSizeOfSelectedFragment = -1;
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
    void SetUpVopros();
    bool QuestionNotSelected = false;
    QString ArgLine; //Строчка параметров
};

#endif // MAINWINDOW_H

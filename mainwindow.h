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
//! TODO: [3] окно детализации КТР
//! TODO: [10] [mdl] сделать удобнее базу знаний
//! TODO: [8] [mdl] окно эффективности по разделам, прописать формулы
//! TODO: [1] Продолжение работы (загрузка или продолжение) и стартовое окно для этого
//! TODO: [1] работа с панелью меню
//! TODO: [4] [min] layout mainwindow настроить нормально
//! TODO: [5] [6] [13] [Ош]
//! [min] На странице "Список КД" к слову эффект. добавить "Кэф"
//! TODO: [12] [min] Эффект. по разд. убрать Ктр, Ксц, Кгдп, Кпсп и добавить Кмол.
//! TODO: [7] Выделять изменённые фрагменты курсивом или фоном
//! TODO: Кэф на 3 формах имеет разные значения, сверить

//! TODO: Дополнительно в конце: Сделать режимы работы
//! TODO: Дополнительно в конце: ~переводчик, словарь сокращений. К концу работы. И спросить нужен ли он вообще

//! Тип отображаемого раздела в центральном окне
enum EDisplayedSection {
    ePSPSection,
    eDOGSection,
    eRVSection,
    eVOSection,
    eGDPSection,
    eZPSection,
    eOTSection,
    eTSPSection,
    eSCSection,
    eTOCSection,
    ePRSection,
    eAllSections
};
enum EWorkMode {
    //! Ничего не выбрано
    eBasicMode,
    //! Фрагмент договора выбран
    eItemSelectedMode,
    //! Фрагмент договора выбран и перемещён вправо
    eRightFrameMode
};

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

    void setWorkMode(EWorkMode newMode);

signals:
    void s_sentFragment(fragment *frag);
    void s_sentKefs(int ktr, int ksc, int kgdp, float kpsp, float kef, float znahimost, int kdog, int krv, int kzp,
                    int kvo, int kot, int kots, int ktsp);

private slots:
    void on_te_textCenter_cursorPositionChanged();

    void on_pb_newFrag_clicked();

    void on_pb_deleteFrag_clicked();

    void on_GoRight_clicked();

    void on_GoLeft_clicked();

    void on_Razd_currentIndexChanged(int index);

    void on_Question_currentIndexChanged(int index);

    void on_Act_currentIndexChanged(int index);

    void on_Quality_currentIndexChanged(int index);

    void on_Effekt_po_razd_clicked();

    void on_BazeKnowledge_clicked();

    void on_pb_clearField_clicked();

    void on_TextRight_textChanged();

    void on_tw_navigator_cellClicked(int row, int column);

    void on_btn_showFullText_clicked();

    void on_actionSave_triggered();

private:
    //! ----------МЕТОДЫ----------
    //! Подготовка главного окна
    void _prepareView();
    void _fillCentralField(EDisplayedSection selectedSection);
    void _addFragmentToCentralField(fragment *frag, QTextCursor cursor);
    void _recountPositions(int idfrag, int delta);
    void _setUpQuestion();
    //

private:
    Ui::MainWindow *ui;
    CDatabaseManager *m_db;
    QTextDocument *m_document;
    knowledgebase *kBase;
    StartDialog sDialog;
    ListKD lDialog;
    QSqlRelationalTableModel *model;
    QSqlDatabase Database;
    EWorkMode m_currentWorkMode;
    QString SelectedKD;
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
    bool QuestionNotSelected = false;
    QString ArgLine; //Строчка параметров
};

#endif // MAINWINDOW_H

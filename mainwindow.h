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
#include "cjsonmanager.h"

//! [min] На странице "Список КД" к слову эффект. добавить "Кэф"

//! TODO: [Улучшение] Дополнительно в конце: ~переводчик, словарь сокращений. К концу работы. И спросить нужен ли он вообще

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

public slots:
    void insertFragFromKB(fragment *frag);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_te_textCenter_cursorPositionChanged();
    void on_pb_newFrag_clicked();
    void on_pb_deleteFrag_clicked();
    void on_GoRight_clicked();
    void on_GoLeft_clicked();
    void on_Razd_currentIndexChanged(int index);
    void on_Effekt_po_razd_clicked();
    void on_BazeKnowledge_clicked();
    void on_pb_clearField_clicked();
    //! Навигатор по разделам
    void on_tw_navigator_cellClicked(int row, int column);
    void on_btn_showFullText_clicked();
    //! Менюбар
    void on_actionMakeDoc_triggered();
    void on_actionStartAnotherKD_triggered();
    void on_actionSaveProject_triggered();
    //
    void on_pb_cancel_clicked();

private:
    //! Подготовка документа и главного окна
    void _prepareMainWindow(QString docId);
    void _prepareMainWindowFromJson(QJsonDocument jDoc);
    void _fillCentralField(EDisplayedSection selectedSection);
    void _addFragmentToCentralField(fragment *frag);
    //! Пересчитывает начальную и конечную позицию фрагмента в тексте для всех фрагментов КД (false - без первого в текущем, true - его тоже пересчитать)
    void _recountPositions(int idfrag, int delta, bool withFirstOfCurrent = false);
    //! Выделить область в центральном поле
    void _setSelectionInCentral(qint32 posStart, qint32 posEnd);
    //! Убрать выделение выбора из центрального окна
    void _clearSelectionInCentral(qint32 posStart, qint32 posEnd);
    void _clearSelectionInCentral();
    // Выделить фрагмент по позициям в центральном поле
    void _markAsChanged(qint32 posStart, qint32 posEnd);
    void _markAsNewAdded(qint32 posStart, qint32 posEnd);
    void _markAsChangedAndNewAdded(qint32 posStart, qint32 posEnd);
    //! Удалить выбранный фрагмент отовсюду
    void _deleteSelectedFrag();

    //! Заменить точку на запятую и округлить до 1 знака после запятой
    QString _doubleToFloatString(double value);
    //! Заменить запятую на точку и проверить знаки после запятой
    float _strDoubleToFloat(QString value);

    //! Вывод сообщений на экран
    void _showMessage(QString text, QString title = "Master KDA - База знаний");
    bool _showQuestion(QString text, QString title = "Master KDA - База знаний", QString textYes = "Да",
                       QString textNo = "Нет");

    //! Заполнить поля текущих главных коэффициентов
    void _fillCurrentKeffs(QVariantList keffs);
    //! Посчитать новые значения главных коэффициентов применив дельту
    QVariantList _calculateKeffsWithDelta(QVariantList delta);
    //! Узнать изменились ли главные коэффициенты проверив каждую дельту на отличие от нуля
    bool _isKeffsChanged(QVariantList delta);

    //! Подготовить правое поле с параметрами к вставке пункта
    void _prepareSettingsInRight(QString fragAkt, QString fragRazdel, QString fragQuality, QString fragQuestionABR);

private:
    Ui::MainWindow *ui;
    CJSONManager *m_jsonManager;
    CDatabaseManager *m_db;
    knowledgebase *kBase;
    kef *kefDialog;
    StartDialog *sDialog;
    ListKD *lDialog { nullptr };
    QSqlRelationalTableModel *model;
    QSqlDatabase Database;
    EWorkMode m_currentWorkMode;
    bool m_navigatorButtonEnabled = false;
    bool m_addNewFrag = false;
    bool m_addFirst = false;
    QString SelectedKD;
    CKolDog *currentKolDog;
    bool TextCenterIsBlocked = true;
    int SelectedFragment = -1;
};

#endif // MAINWINDOW_H

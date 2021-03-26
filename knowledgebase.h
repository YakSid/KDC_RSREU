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
    void on_pb_insert_into_file_clicked();

private:
    void _changeViewMode(EFragmentsViewMode newViewMode);
    //! Провести начальную подготовку окна, неважно в каком режим с фрагментом или без
    void _prepareWindow();

    //! Вывод сообщений на экран
    void _showMessage(QString text, QString title = "Master KDA");
    bool _showQuestion(QString text, QString title = "Master KDA", QString textYes = "Да", QString textNo = "Нет");

private:
    Ui::knowledgebase *ui;
    EFragmentsViewMode m_currentViewMode { eTypicalKD };
    //! Номер фрагмента из подготовленного списка, -1 - изначальный
    qint32 currentFragmentNumber { -1 };
    //! Изначально оригинальный фраг, а после - с параметрами необходимыми для поиска
    fragment *m_originalFrag { nullptr };
    QString originalText { "" };
    bool m_allActs { false };
    bool m_unlocked { false };
    // TODO: [Улучшение кода] Переделать эти листы в структуру
    QList<QString> fragmentsForShow;
    //! Собранные id КД для поиска и отображения их названий
    QList<QString> idForNames;
    QList<QString> namesForShow;
    QList<structOrder *> ordersForShow;
    //! Файл для вынесения дополнительных пунктов
    QString fileWithAdd = "";

    //! Показывать ли подсказку
    bool m_showHelp { true };
};

#endif // KNOWLEDGEBASE_H

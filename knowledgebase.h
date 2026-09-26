#ifndef KNOWLEDGEBASE_H
#define KNOWLEDGEBASE_H

#include "cconstants.h"
#include "fragment.h"
#include <QDialog>

enum class ELockState { unlocked, locked };

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
    //! Фрагмент отправлен в основное окно
    void s_fragmentSentFromKB(fragment *frag);

public slots:
    void prepare(fragment *frag);
    void open() override;
    int exec() override;

private slots:
    //! Делает выборку по заданным параметрам из базы данных, вызывается в каждом изменении аргументов
    void _select();

    void on_pb_unlock_clicked();
    void on_pb_insert_into_kd_clicked();
    void on_pb_showList_clicked();
    void on_cmb_razdel_currentTextChanged(const QString &arg1);
    void on_pb_insert_into_file_clicked();
    // Основное окно текста фрагмента
    void on_pb_next_clicked();
    void on_pb_prev_clicked();
    // Дополнительное окно законов
    void on_pb_nextLaw_clicked();
    void on_pb_prevLaw_clicked();

private:
    //! Провести начальную подготовку окна, неважно в каком режим с фрагментом или без
    void _prepareUi();
    //! Блокировать ui во время просмотра фрагментов
    void _lockUi(bool lock);

    //! Вывод сообщений на экран
    void _showMessage(QString text, QString title = "Master KDA");
    bool _showQuestion(QString text, QString title = "Master KDA", QString textYes = "Да", QString textNo = "Нет");

    //! Закон с данным индексом
    void _showLaw(qint32 index);
    //! Отобразить фрагмент с данным индексом
    void _showFragment(qint32 index);

private:
    Ui::knowledgebase *ui;

    // Состояние ui
    ELockState m_lockState{ELockState::locked};

    // Настройки
    //! Путь к файлу для вынесения дополнительных пунктов
    QString m_savedFragmentsPath = "";
    //! Показывать ли подсказку
    bool m_showHelp{true};

    // Данные фрагмента
    //! Изначально оригинальный фраг, а после - с параметрами необходимыми для поиска
    fragment *m_originalFrag{nullptr};
    //! Текст оригинального фрагмента, пришедшего в Базу Знаний
    QString m_originalText{""};
    //! Номер фрагмента из подготовленного списка, -1 - изначальный
    qint32 m_currentFragmentNumber{-1};

    // ======Список запрошенных данных======
    // ...фрагментов
    QList<QString> m_fragmentsForShow;
    //! Названия учреждений, откуда взят фрагмент
    QList<QString> m_institutionNames;
    // ...законов
    //! Титульная информация закона без самого текста закона
    QList<structOrder *> m_lawHeadersForShow;
    //! Сами тексты законов
    QStringList m_lawsForShow;
    //! Текущий порядковый номер отображаемого закона
    qint32 m_currentLaw{-1};
    // ============================
};

#endif // KNOWLEDGEBASE_H

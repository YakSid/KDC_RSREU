#include "knowledgebase.h"
#include "ui_knowledgebase.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

knowledgebase::knowledgebase(QWidget *parent) : QDialog(parent), ui(new Ui::knowledgebase)
{
    ui->setupUi(this);
    _changeViewMode(eTypicalKD);
}

knowledgebase::~knowledgebase()
{
    delete ui;
}

void knowledgebase::_setUpVopros()
{
    if (ui->cmb_razdel->currentIndex() == 0) {
        m_listVopros.append({ "Стороны и их полномочия", "Разрешение споров", "Изменение, заключение и контроль КД",
                              "Ответственность сторон", "Сроки и действие КД", "Консультация, переговоры сторон",
                              "Цель заключения, правовые основы", "Информирование и знакомство с КД",
                              "Обязательства сторон", "Другие вопросы", "Определения, содержание КД" });
        m_abbreviationVopros.append({ "ПЛН", "РСП", "ИЗК", "ОТВ", "СДД", "ПЕГ", "ЦЕЗ", "ИНФ", "ОБЯ", "ДРГ", "ОПР" });
    }
    if (ui->cmb_razdel->currentIndex() == 1) {
        m_listVopros.append({ "Заключение ТД", "Срок ТД", "Изменение ТД", "Повышение квалификации", "Занятость",
                              "Увольнение", "Сокращение", "Гарантия, компенсации", "Обязательства сторон" });
        m_abbreviationVopros.append({ "ЗТД", "СТД", "ИТД", "ПК", "ЗАН", "УВ", "СОК", "ГАР", "ОБЯ" });
    }
    if (ui->cmb_razdel->currentIndex() == 2) {
        m_listVopros.append({ "Продолжительность РВ", "Режим работы", "Сверхурочные", "Нормирование труда",
                              "Дисциплина", "Гарантии, компенсации", "Обязательства сторон" });
        m_abbreviationVopros.append({ "ПР", "РЕЖ", "СВХ", "НОР", "ДИС", "ГАР", "ОБЯ" });
    }
    if (ui->cmb_razdel->currentIndex() == 3) {
        m_listVopros.append({ "Перерыв в течение рабочего дня", "Выходные дни", "Ежегодный отпуск",
                              "Дополнительный отпуск", "Гарантии, компенсации", "Обязательства сторон" });
        m_abbreviationVopros.append({ "ПЕР", "ВЫХ", "ОТП", "ДОВ", "ГАР", "ОБЯ" });
    }
    if (ui->cmb_razdel->currentIndex() == 4) {
        m_listVopros.append({ "Права профкома, профсоюза", "Условия предоставляемые для деятельности",
                              "Не члены профсоюза", "Льготы неосвобожденному профактиву",
                              "Льготы освобожденному профактиву", "Согласования", "Льготы членам профсоюза",
                              "Обязательства сторон" });
        m_abbreviationVopros.append({ "ПКМ", "УСЛ", "НЧП", "ЛГН", "ЛГО", "СГЛ", "ЛЧП", "ОБГ" });
    }
    if (ui->cmb_razdel->currentIndex() == 5) {
        m_listVopros.append({ "Система оплаты", "Надбавки", "Доплаты за не нормальные условия", "Сроки выплат",
                              "Индексация", "Внебюджетные средства", "Премии", "Гарантии, компенсации",
                              "Обязательства сторон" });
        m_abbreviationVopros.append({ "СИС", "НАД", "ДОП", "СРК", "ИНД", "ВНБ", "ПРМ", "ГАР", "ОБЯ" });
    }
    if (ui->cmb_razdel->currentIndex() == 6) {
        m_listVopros.append({ "Средства на ОТ", "Соглашение по ОТ", "Мероприятия по улучшению ОТ",
                              "Аттестация рабочих мест", "Обеспечение средствами защиты",
                              "Страхование и проф. обследование", "Контроль за ОТ", "Гарантии, компенсации",
                              "Обязательства сторон", "Ответственность сторон" });
        m_abbreviationVopros.append({ "СУМ", "СОГ", "МЕР", "АТТ", "ИСЗ", "ПРФ", "КОН", "ГАР", "ОБЯ", "ОТВ" });
    }
    if (ui->cmb_razdel->currentIndex() == 7) {
        m_listVopros.append({ "Обращения, жалобы", "Индивидуальные", "Коллективные", "Комиссия по труд. спорам" });
        m_abbreviationVopros.append({ "ЖЛБ", "ИНД", "КОЛ", "КОМ" });
    }
    if (ui->cmb_razdel->currentIndex() == 8) {
        m_listVopros.append({ "Социальное страхование", "Медиц. обслуживание", "Жилищные вопросы",
                              "Организация отдыха работников", "Услуги (бытовые, спорт, культура и пр.)",
                              "Контроль в соцсфере", "Материальная помощь, социальная защита",
                              "Мероприятия и средства" });
        m_abbreviationVopros.append({ "СТР", "МЕД", "ЖИЛ", "ОТД", "УСЛ", "КОН", "МАТ", "МСР" });
    }
    if (ui->cmb_razdel->currentIndex() == 9) {
        m_listVopros.append(
                { "Совместители", "Руководители", "Женщины, семейные", "Молодые, учащиеся", "Гарантии, компенсации" });
        m_abbreviationVopros.append({ "СОВ", "РУК", "ЖЕН", "МОЛ", "ГАР" });
    }
    if (ui->cmb_razdel->currentIndex() == 10) {
        m_listVopros.append({ "Производственно-экономические", "Внебюджетная деятельность", "Обязательства сторон" });
        m_abbreviationVopros.append({ "ПРЭ", "ВБД", "ОБЯ" });
    }
}

void knowledgebase::_changeViewMode(EFragmentsViewMode newViewMode)
{
    switch (newViewMode) {
    case eLaw:
        ui->lb_quality->setText("Возможность:");
        ui->ch_all_acts->setVisible(true);
        ui->lb_act->setVisible(true);
        ui->cmb_act->setVisible(true);
        ui->frame->setVisible(true);
        break;
    case eTypicalKD:
        ui->lb_quality->setText("Качество:");
        ui->ch_all_acts->setChecked(false);
        ui->ch_all_acts->setVisible(false);
        ui->lb_act->setVisible(true);
        ui->cmb_act->setVisible(true);
        ui->frame->setVisible(false);
        break;
    case eAllKD:
        ui->lb_quality->setText("Качество:");
        ui->ch_all_acts->setChecked(false);
        ui->ch_all_acts->setVisible(false);
        ui->lb_act->setVisible(false);
        ui->cmb_act->setVisible(false);
        ui->frame->setVisible(false);
        break;
    }
}

void knowledgebase::getFragment(fragment *frag)
{
    ui->cmb_act->addItems(ListAct);
    ui->cmb_razdel->addItems(ListRazd);
    ui->cmb_quality->addItems(ListQuality);
    for (int i = 0; i < ListAct.size(); i++) {
        if (frag->Akt == AbbreviationAct[i]) {
            ui->cmb_act->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListRazd.size(); i++) {
        if (frag->Razdel == AbbreviationRazd[i]) {
            ui->cmb_razdel->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListQuality.size(); i++) {
        if (frag->Kachestvo == AbbreviationQuality[i]) {
            ui->cmb_quality->setCurrentIndex(i);
            break;
        }
    }
    _setUpVopros();
    ui->cmb_question->addItems(m_listVopros);
    for (int i = 0; i < m_listVopros.size(); i++) {
        if (frag->VoprosABR == m_abbreviationVopros[i]) {
            ui->cmb_question->setCurrentIndex(i);
            break;
        }
    }

    ui->te_text->setText(frag->text);
    _select();
}

void knowledgebase::on_pb_unlock_clicked()
{
    if (ui->cmb_razdel->isEnabled()) {
        ui->pb_unlock->setText("Разблокировать");
        ui->cmb_razdel->setEnabled(false);
        ui->cmb_act->setEnabled(false);
        ui->cmb_question->setEnabled(false);
        ui->cmb_quality->setEnabled(false);
    } else {
        ui->pb_unlock->setText("Заблокировать");
        ui->cmb_razdel->setEnabled(true);
        ui->cmb_act->setEnabled(true);
        ui->cmb_question->setEnabled(true);
        ui->cmb_quality->setEnabled(true);
    }
}

void knowledgebase::on_rb_law_fragments_toggled(bool checked)
{
    if (checked)
        _changeViewMode(eLaw);
}

void knowledgebase::on_rb_typical_fragments_kd_toggled(bool checked)
{
    if (checked)
        _changeViewMode(eTypicalKD);
}

void knowledgebase::on_rb_all_fragments_kd_toggled(bool checked)
{
    if (checked)
        _changeViewMode(eAllKD);
}

void knowledgebase::on_ch_all_acts_toggled(bool checked)
{
    m_allActs = checked;
}

void knowledgebase::_select()
{
    fragmentsForShow.clear();
    // TODO: Выбор фрагментов из бд для показа
    /*QSqlQuery querySelect;
    querySelect.prepare("SELECT * FROM Договор WHERE Договор.[#Дог] = :val1");
    querySelect.bindValue(":val1", SelectedKD);
    if (!querySelect.exec()) {
        qDebug() << querySelect.lastError().text();
    }
    if (querySelect.next()) {
        // Заполнение параметров класса договора
        qDebug() << querySelect.value(0).toString();
        fragmentsForShow.append("text_that_found");
    }*/
}

void knowledgebase::on_pb_insert_into_kd_clicked()
{
    //! Вставить нынешний text в поле редактирования mainwindow (с соответствующим изменением размеров)
    // TODO: сделать метод добавления нового пункта в кд
    // TODO: сделать метод вставки из БЗ в окно редактирования
}

void knowledgebase::on_pb_next_clicked()
{
    //! Показать следующий фрагмент
}

void knowledgebase::on_pb_prev_clicked()
{
    //! Показать предыдущий фрагмент
}

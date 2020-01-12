#include "knowledgebase.h"
#include "ui_knowledgebase.h"

#include <QDebug>

knowledgebase::knowledgebase(QWidget *parent) : QDialog(parent), ui(new Ui::knowledgebase)
{
    ui->setupUi(this);
}

knowledgebase::~knowledgebase()
{
    delete ui;
}

void knowledgebase::setUpVopros()
{
    if (ui->razdel->currentIndex() == 0) {
        ListVopros.append({ "Стороны и их полномочия", "Разрешение споров", "Изменение, заключение и контроль КД",
                            "Ответственность сторон", "Сроки и действие КД", "Консультация, переговоры сторон",
                            "Цель заключения, правовые основы", "Информирование и знакомство с КД",
                            "Обязательства сторон", "Другие вопросы", "Определения, содержание КД" });
        AbbreviationVopros.append({ "ПЛН", "РСП", "ИЗК", "ОТВ", "СДД", "ПЕГ", "ЦЕЗ", "ИНФ", "ОБЯ", "ДРГ", "ОПР" });
    }
    if (ui->razdel->currentIndex() == 1) {
        ListVopros.append({ "Заключение ТД", "Срок ТД", "Изменение ТД", "Повышение квалификации", "Занятость",
                            "Увольнение", "Сокращение", "Гарантия, компенсации", "Обязательства сторон" });
        AbbreviationVopros.append({ "ЗТД", "СТД", "ИТД", "ПК", "ЗАН", "УВ", "СОК", "ГАР", "ОБЯ" });
    }
    if (ui->razdel->currentIndex() == 2) {
        ListVopros.append({ "Продолжительность РВ", "Режим работы", "Сверхурочные", "Нормирование труда", "Дисциплина",
                            "Гарантии, компенсации", "Обязательства сторон" });
        AbbreviationVopros.append({ "ПР", "РЕЖ", "СВХ", "НОР", "ДИС", "ГАР", "ОБЯ" });
    }
    if (ui->razdel->currentIndex() == 3) {
        ListVopros.append({ "Перерыв в течение рабочего дня", "Выходные дни", "Ежегодный отпуск",
                            "Дополнительный отпуск", "Гарантии, компенсации", "Обязательства сторон" });
        AbbreviationVopros.append({ "ПЕР", "ВЫХ", "ОТП", "ДОВ", "ГАР", "ОБЯ" });
    }
    if (ui->razdel->currentIndex() == 4) {
        ListVopros.append({ "Права профкома, профсоюза", "Условия предоставляемые для деятельности",
                            "Не члены профсоюза", "Льготы неосвобожденному профактиву",
                            "Льготы освобожденному профактиву", "Согласования", "Льготы членам профсоюза",
                            "Обязательства сторон" });
        AbbreviationVopros.append({ "ПКМ", "УСЛ", "НЧП", "ЛГН", "ЛГО", "СГЛ", "ЛЧП", "ОБГ" });
    }
    if (ui->razdel->currentIndex() == 5) {
        ListVopros.append({ "Система оплаты", "Надбавки", "Доплаты за не нормальные условия", "Сроки выплат",
                            "Индексация", "Внебюджетные средства", "Премии", "Гарантии, компенсации",
                            "Обязательства сторон" });
        AbbreviationVopros.append({ "СИС", "НАД", "ДОП", "СРК", "ИНД", "ВНБ", "ПРМ", "ГАР", "ОБЯ" });
    }
    if (ui->razdel->currentIndex() == 6) {
        ListVopros.append({ "Средства на ОТ", "Соглашение по ОТ", "Мероприятия по улучшению ОТ",
                            "Аттестация рабочих мест", "Обеспечение средствами защиты",
                            "Страхование и проф. обследование", "Контроль за ОТ", "Гарантии, компенсации",
                            "Обязательства сторон", "Ответственность сторон" });
        AbbreviationVopros.append({ "СУМ", "СОГ", "МЕР", "АТТ", "ИСЗ", "ПРФ", "КОН", "ГАР", "ОБЯ", "ОТВ" });
    }
    if (ui->razdel->currentIndex() == 7) {
        ListVopros.append({ "Обращения, жалобы", "Индивидуальные", "Коллективные", "Комиссия по труд. спорам" });
        AbbreviationVopros.append({ "ЖЛБ", "ИНД", "КОЛ", "КОМ" });
    }
    if (ui->razdel->currentIndex() == 8) {
        ListVopros.append({ "Социальное страхование", "Медиц. обслуживание", "Жилищные вопросы",
                            "Организация отдыха работников", "Услуги (бытовые, спорт, культура и пр.)",
                            "Контроль в соцсфере", "Материальная помощь, социальная защита",
                            "Мероприятия и средства" });
        AbbreviationVopros.append({ "СТР", "МЕД", "ЖИЛ", "ОТД", "УСЛ", "КОН", "МАТ", "МСР" });
    }
    if (ui->razdel->currentIndex() == 9) {
        ListVopros.append(
                { "Совместители", "Руководители", "Женщины, семейные", "Молодые, учащиеся", "Гарантии, компенсации" });
        AbbreviationVopros.append({ "СОВ", "РУК", "ЖЕН", "МОЛ", "ГАР" });
    }
    if (ui->razdel->currentIndex() == 10) {
        ListVopros.append({ "Производственно-экономические", "Внебюджетная деятельность", "Обязательства сторон" });
        AbbreviationVopros.append({ "ПРЭ", "ВБД", "ОБЯ" });
    }
}

void knowledgebase::getFragment(fragment *frag)
{
    ui->akt->addItems(ListAct);
    ui->razdel->addItems(ListRazd);
    ui->quality->addItems(ListQuality);
    for (int i = 0; i < ListAct.size(); i++) {
        if (frag->Akt == AbbreviationAct[i]) {
            ui->akt->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListRazd.size(); i++) {
        if (frag->Razdel == AbbreviationRazd[i]) {
            ui->razdel->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListQuality.size(); i++) {
        if (frag->Kachestvo == AbbreviationQuality[i]) {
            ui->quality->setCurrentIndex(i);
            break;
        }
    }
    setUpVopros();
    ui->question->addItems(ListVopros);
    for (int i = 0; i < ListVopros.size(); i++) {
        if (frag->VoprosABR == AbbreviationVopros[i]) {
            ui->question->setCurrentIndex(i);
            break;
        }
    }

    ui->text->setText(frag->text);
}

void knowledgebase::on_pb_unlock_clicked()
{
    if (ui->razdel->isEnabled()) {
        ui->pb_unlock->setText("Разблокировать");
        ui->razdel->setEnabled(false);
        ui->akt->setEnabled(false);
        ui->question->setEnabled(false);
        ui->quality->setEnabled(false);
    } else {
        ui->pb_unlock->setText("Заблокировать");
        ui->razdel->setEnabled(true);
        ui->akt->setEnabled(true);
        ui->question->setEnabled(true);
        ui->quality->setEnabled(true);
    }
}

void knowledgebase::on_rb_fragments_toggled(bool checked)
{
    if (checked) {
        ui->lb_quality->setText("Возможность:");
    } else {
        ui->lb_quality->setText("Качество:");
    }
}

void knowledgebase::_select()
{
    //! Делает выборку по заданным параметрам из базы данных, вызывается в каждом изменении аргументов
    // TODO: сделать метод _select
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

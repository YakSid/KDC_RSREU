#include "knowledgebase.h"
#include "ui_knowledgebase.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

knowledgebase::knowledgebase(QWidget *parent) : QDialog(parent), ui(new Ui::knowledgebase)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _changeViewMode(eTypicalKD);
}

// TODO: [11] [mdl] загрузку фрагментов немного изменить по блокноту
knowledgebase::~knowledgebase()
{
    qDebug() << "destructor knowledgebase";
    delete ui;
}

void knowledgebase::_changeViewMode(EFragmentsViewMode newViewMode)
{
    switch (newViewMode) {
    case eLaw:
        ui->lb_quality->setText("Возможность:");
        ui->ch_all_acts->setVisible(true);
        ui->frame->setVisible(true);
        break;
    case eTypicalKD:
        ui->lb_quality->setText("Качество:");
        ui->ch_all_acts->setChecked(false);
        ui->ch_all_acts->setVisible(false);
        ui->frame->setVisible(false);
        break;
    case eAllKD:
        ui->lb_quality->setText("Качество:");
        ui->ch_all_acts->setChecked(false);
        ui->ch_all_acts->setVisible(false);
        ui->frame->setVisible(false);
        break;
    }
    m_currentViewMode = newViewMode;
    _select();
}

void knowledgebase::getFragment(fragment *frag)
{
    ui->cmb_razdel->clear();
    ui->cmb_question->clear();
    ui->cmb_act->clear();
    ui->cmb_quality->clear();

    ui->cmb_act->addItems(ListAct);
    ui->cmb_razdel->addItems(ListRazd);
    ui->cmb_quality->addItems(ListQuality);

    QString fragAkt = frag->getAkt();
    QString fragRazdel = frag->getRazdel();
    QString fragQuality = frag->getKachestvo();
    QString fragQuestionABR = frag->getVoprosABR();

    for (int i = 0; i < ListAct.size(); i++) {
        if (fragAkt == AbbreviationAct[i]) {
            ui->cmb_act->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListRazd.size(); i++) {
        if (fragRazdel == AbbreviationRazd[i]) {
            ui->cmb_razdel->setCurrentIndex(i);
            ui->cmb_question->addItems(QuestionsAtRazdel[i]);
            for (int j = 0; j < ABRQuestionsAtRazdel[i].size(); j++) {
                if (fragQuestionABR == ABRQuestionsAtRazdel[i][j]) {
                    ui->cmb_question->setCurrentIndex(j);
                }
            }
            break;
        }
    }
    for (int i = 0; i < ListQuality.size(); i++) {
        if (fragQuality == AbbreviationQuality[i]) {
            ui->cmb_quality->setCurrentIndex(i);
            break;
        }
    }

    m_currentVoprosNumber = frag->getVoprosNumber();

    ui->te_text->setText(frag->getText());
    ui->gb_text->setTitle("Начальный фрагмент");
    originalText = frag->getText();
    _select();
}

void knowledgebase::on_pb_unlock_clicked()
{
    m_unlocked = !m_unlocked;
    if (m_unlocked) {
        ui->pb_unlock->setText("Заблокировать");
    } else {
        ui->pb_unlock->setText("Разблокировать");
    }
    ui->cmb_razdel->setEnabled(m_unlocked);
    ui->cmb_act->setEnabled(m_unlocked);
    ui->cmb_question->setEnabled(m_unlocked);
    ui->cmb_quality->setEnabled(m_unlocked);
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
    // TODO: m_allActs тут должно использоваться и накладывать фильтр, что если eAllActs то не играет роли
    //Написать алгоритм на бумаге и проверить его логику
    fragmentsForShow.clear();
    QString questionKod = QString::number(m_currentVoprosNumber);
    QSqlQuery querySelect;
    querySelect.prepare("SELECT ТФрагмент.ТекстФрагмента FROM ТФрагмент WHERE Тфрагмент.КодВопрос = :val1");
    querySelect.bindValue(":val1", questionKod);
    if (!querySelect.exec()) {
        qDebug() << querySelect.lastError().text();
    }
    while (querySelect.next()) {
        QString text = querySelect.value(0).toString();
        fragmentsForShow.append(text);
    }
    currentFragmentNumber = -1;
}

void knowledgebase::on_pb_insert_into_kd_clicked()
{
    if (ui->te_text->toPlainText().isEmpty())
        return;

    auto transportFrag = new fragment();
    transportFrag = new fragment();
    transportFrag->setText(ui->te_text->toPlainText());
    transportFrag->setKachestvo(AbbreviationQuality[ui->cmb_quality->currentIndex()]);
    transportFrag->setAkt(AbbreviationQuality[ui->cmb_act->currentIndex()]);
    transportFrag->setVoprosABR(ABRQuestionsAtRazdel[ui->cmb_razdel->currentIndex()][ui->cmb_question->currentIndex()]);
    transportFrag->setRazdel(AbbreviationRazd[ui->cmb_razdel->currentIndex()]);
    transportFrag->setChanged(true);

    emit startTransportFrag(transportFrag);
    delete transportFrag;
    close();
}

void knowledgebase::on_pb_next_clicked()
{
    //! Показать следующий фрагмент
    if (fragmentsForShow.isEmpty())
        return;
    if (currentFragmentNumber == fragmentsForShow.size() - 1) {
        currentFragmentNumber = -1;
        ui->te_text->setText(originalText);
        ui->gb_text->setTitle("Начальный фрагмент");
    } else {
        currentFragmentNumber++;
        ui->te_text->setText(fragmentsForShow[currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(currentFragmentNumber + 1) + "/"
                              + QString::number(fragmentsForShow.size()));
    }
}

void knowledgebase::on_pb_prev_clicked()
{
    //! Показать предыдущий фрагмент
    if (fragmentsForShow.isEmpty())
        return;
    if (currentFragmentNumber == 0) {
        currentFragmentNumber = -1;
        ui->te_text->setText(originalText);
        ui->gb_text->setTitle("Начальный фрагмент");
    } else if (currentFragmentNumber == -1) {
        currentFragmentNumber = fragmentsForShow.size() - 1;
        ui->te_text->setText(fragmentsForShow[currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(currentFragmentNumber + 1) + "/"
                              + QString::number(fragmentsForShow.size()));
    } else {
        currentFragmentNumber--;
        ui->te_text->setText(fragmentsForShow[currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(currentFragmentNumber + 1) + "/"
                              + QString::number(fragmentsForShow.size()));
    }
}

void knowledgebase::on_pb_showList_clicked()
{
    // TODO: [10] Показать окно со списком всех фрагментов, можно по 100 символов и полностью при наведении
    // ui->lw_fragments->insertItems(0, fragmentsForShow);
}

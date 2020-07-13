#include "knowledgebase.h"
#include "ui_knowledgebase.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>

knowledgebase::knowledgebase(QWidget *parent) : QDialog(parent), ui(new Ui::knowledgebase)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _changeViewMode(eTypicalKD);
}

knowledgebase::~knowledgebase()
{
    qDebug() << "destructor knowledgebase";
    if (ordersForShow.size() > 0) {
        for (auto order : ordersForShow) {
            delete order;
        }
        ordersForShow.clear();
    }
    delete ui;
}

void knowledgebase::prepareWindowWithoutFrag()
{
    _prepareWindow();
    ui->cmb_razdel->setCurrentIndex(0);
    ui->cmb_question->setCurrentIndex(0);
    ui->cmb_act->setCurrentIndex(0);
    ui->cmb_quality->setCurrentIndex(0);

    m_currentVoprosNumber = -1;
}

void knowledgebase::_changeViewMode(EFragmentsViewMode newViewMode)
{
    switch (newViewMode) {
    case eLaw:
        ui->lb_quality->setText("Возможность:");
        ui->cmb_quality->clear();
        ui->cmb_quality->addItems(ListVozmojnosti);
        ui->cmb_quality->setCurrentIndex(0);
        ui->frame->setVisible(true);
        ui->ch_all_acts->setVisible(false);
        break;
    case eTypicalKD:
        ui->lb_quality->setText("Качество:");
        ui->frame->setVisible(false);
        if (m_currentViewMode == eLaw) {
            ui->cmb_quality->clear();
            ui->cmb_quality->addItems(ListQuality);
            ui->cmb_quality->setCurrentIndex(0);
        }
        ui->ch_all_acts->setVisible(true);
        break;
    case eAllKD:
        ui->lb_quality->setText("Качество:");
        ui->frame->setVisible(false);
        if (m_currentViewMode == eLaw) {
            ui->cmb_quality->clear();
            ui->cmb_quality->addItems(ListQuality);
            ui->cmb_quality->setCurrentIndex(0);
        }
        ui->ch_all_acts->setVisible(true);
        break;
    }
    m_currentViewMode = newViewMode;
    if (ui->cmb_razdel->currentText() != "") {
        ui->gb_text->setTitle("Начальный фрагмент");
        ui->te_text->setText(originalText);
    }
}

void knowledgebase::_prepareWindow()
{
    ui->gb_text->setTitle("Начальный фрагмент");

    ui->cmb_razdel->clear();
    ui->cmb_question->clear();
    ui->cmb_act->clear();
    ui->cmb_quality->clear();

    ui->cmb_razdel->addItems(ListRazd);
    ui->cmb_quality->addItems(ListQuality);
    ui->cmb_act->addItems(ListAct);
}

void knowledgebase::getFragment(fragment *frag)
{
    _prepareWindow();

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
    originalText = frag->getText();
    _select();
}

void knowledgebase::on_pb_unlock_clicked()
{
    m_unlocked = !m_unlocked;
    if (m_unlocked) {
        ui->pb_unlock->setText("Применить");
    } else {
        ui->pb_unlock->setText("Разблокировать");
        _select();
    }
    ui->cmb_razdel->setEnabled(m_unlocked);
    ui->cmb_act->setEnabled(m_unlocked);
    ui->cmb_question->setEnabled(m_unlocked);
    ui->cmb_quality->setEnabled(m_unlocked);
    ui->groupBox_2->setEnabled(m_unlocked);
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
    // TODO: [ПРАВКИ] реализовать в select'е подбор по всем параметрам запроса (4 параметра)
    // m_allActs тут должно использоваться и накладывать фильтр. m_allActs устновить везде где нужно
    fragmentsForShow.clear();
    for (auto order : ordersForShow) {
        delete order;
    }
    ordersForShow.clear();
    // m_currentVoprosNumber только если есть (есть только после getFragment)
    QString questionKod;
    if (m_currentVoprosNumber != -1) {
        questionKod = QString::number(m_currentVoprosNumber);
    }
    QSqlQuery querySelect;
    switch (m_currentViewMode) {
    case eLaw: {
        querySelect.prepare("SELECT ТФрагмент.ТекстФрагмента, ТФрагмент.КодЗакона FROM ТФрагмент WHERE "
                            "Тфрагмент.КодВопрос = :val1");
        break;
    }
    case eTypicalKD:
        querySelect.prepare("SELECT Тексты.Текст FROM Тексты WHERE Тексты.Вопрос = :val1 AND ВклВСправку=true");
        break;
    case eAllKD:
        querySelect.prepare("SELECT Тексты.Текст FROM Тексты WHERE Тексты.Вопрос = :val1");
        break;
    }

    querySelect.bindValue(":val1", questionKod);
    if (!querySelect.exec()) {
        qDebug() << querySelect.lastError().text();
    }
    while (querySelect.next()) {
        QString text = querySelect.value(0).toString();
        fragmentsForShow.append(text);
        if (m_currentViewMode == eLaw) {
            qint32 kodZakona = querySelect.value(1).toInt();
            for (auto order : TOrder) {
                if (kodZakona == order->id) {
                    auto zakonInfo = new structOrder();
                    zakonInfo->name = order->name;
                    zakonInfo->dateAdoptation = order->dateAdoptation;
                    zakonInfo->dateChange = order->dateChange;
                    ordersForShow.append(zakonInfo);
                }
            }
        }
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
    if (m_currentViewMode == eLaw) {
        transportFrag->setKachestvo("Ан");
    } else {
        transportFrag->setKachestvo(AbbreviationQuality[ui->cmb_quality->currentIndex()]);
    }
    transportFrag->setAkt(AbbreviationAct[ui->cmb_act->currentIndex()]);
    transportFrag->setVoprosABR(ABRQuestionsAtRazdel[ui->cmb_razdel->currentIndex()][ui->cmb_question->currentIndex()]);
    transportFrag->setRazdel(AbbreviationRazd[ui->cmb_razdel->currentIndex()]);
    transportFrag->setNewAdded(true);

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

    if (m_currentViewMode == eLaw) {
        if (currentFragmentNumber == -1) {
            ui->ln_order->setText("-");
            ui->ln_adoption_date->setText("-");
            ui->ln_change_date->setText("-");
        } else if (ordersForShow.size() != 0 && ordersForShow.size() >= currentFragmentNumber) {
            ui->ln_order->setText(ordersForShow[currentFragmentNumber]->name);
            ui->ln_adoption_date->setText(ordersForShow[currentFragmentNumber]->dateAdoptation.toString("dd.MM.yyyy"));
            ui->ln_change_date->setText(ordersForShow[currentFragmentNumber]->dateChange.toString("dd.MM.yyyy"));
        }
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
    } else if (ordersForShow.size() != 0 && ordersForShow.size() >= currentFragmentNumber) {
        currentFragmentNumber--;
        ui->te_text->setText(fragmentsForShow[currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(currentFragmentNumber + 1) + "/"
                              + QString::number(fragmentsForShow.size()));
    }

    if (m_currentViewMode == eLaw) {
        if (currentFragmentNumber == -1) {
            ui->ln_order->setText("-");
            ui->ln_adoption_date->setText("-");
            ui->ln_change_date->setText("-");
        } else {
            ui->ln_order->setText(ordersForShow[currentFragmentNumber]->name);
            ui->ln_adoption_date->setText(ordersForShow[currentFragmentNumber]->dateAdoptation.toString("dd.MM.yyyy"));
            ui->ln_change_date->setText(ordersForShow[currentFragmentNumber]->dateChange.toString("dd.MM.yyyy"));
        }
    }
}

void knowledgebase::on_pb_showList_clicked()
{
    // TODO: [?later?] [10] Показать окно со списком всех фрагментов, можно по 100 символов и полностью при наведении
    // ui->lw_fragments->insertItems(0, fragmentsForShow);
}

void knowledgebase::on_cmb_razdel_currentTextChanged(const QString &arg1)
{
    if (arg1.isEmpty())
        return;
    ui->cmb_question->clear();
    ui->cmb_question->addItems(QuestionsAtRazdel[ui->cmb_razdel->currentIndex()]);
}

void knowledgebase::on_pb_insert_into_file_clicked()
{
    if (fileWithAdd.isEmpty()) {
        fileWithAdd = QFileDialog::getSaveFileName(this, "Текстовый файл для дополнительных фрагментов", "", "*.txt");
    }
    QFile file(fileWithAdd);
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream writeStream(&file);
    writeStream << "\n\n" << ui->te_text->toPlainText();
    file.close();
}

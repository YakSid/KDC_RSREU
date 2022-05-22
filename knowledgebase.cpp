#include "knowledgebase.h"
#include "ui_knowledgebase.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopWidget>

const QString STR_START_FRAG = "Начальный фрагмент";
const QString STR_FRAG_FROM_SELECTED = " Фрагмент из выбранных";

knowledgebase::knowledgebase(QWidget *parent) : QDialog(parent), ui(new Ui::knowledgebase)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    _changeViewMode(eTypicalKD);

    this->setStyleSheet("QPushButton:disabled {"
                        "background-color: darkGrey;"
                        "border: 2px solid darkGrey;}");
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

void knowledgebase::_changeViewMode(EFragmentsViewMode newViewMode)
{
    switch (newViewMode) {
    case eLaw:
        ui->lb_quality->setText("Возможность:");
        ui->cmb_quality->clear();
        ui->cmb_quality->addItems(ListVozmojnosti);
        ui->cmb_quality->setCurrentIndex(0);
        ui->frameLaw->setVisible(true);
        ui->ch_all_acts->setVisible(false);
        ui->lbl_kdName->setVisible(false);
        ui->ln_kdName->setVisible(false);
        ui->pb_unlock->setToolTip("Показать фрагменты законов");
        break;
    case eTypicalKD:
        ui->lb_quality->setText("Качество:");
        ui->frameLaw->setVisible(false);
        if (m_currentViewMode == eLaw) {
            ui->cmb_quality->clear();
            ui->cmb_quality->addItems(ListQuality);
            ui->cmb_quality->setCurrentIndex(0);
        }
        ui->ch_all_acts->setVisible(true);
        ui->lbl_kdName->setVisible(true);
        ui->ln_kdName->setVisible(true);
        ui->pb_unlock->setToolTip("Показать типовые фрагменты");
        break;
    case eAllKD:
        ui->lb_quality->setText("Качество:");
        ui->frameLaw->setVisible(false);
        if (m_currentViewMode == eLaw) {
            ui->cmb_quality->clear();
            ui->cmb_quality->addItems(ListQuality);
            ui->cmb_quality->setCurrentIndex(0);
        }
        ui->ch_all_acts->setVisible(true);
        ui->lbl_kdName->setVisible(true);
        ui->ln_kdName->setVisible(true);
        ui->pb_unlock->setToolTip("Показать фрагменты всех КД");
        break;
    }
    m_currentViewMode = newViewMode;
    if (ui->cmb_razdel->currentText() != "") {
        ui->gb_text->setTitle(STR_START_FRAG);
        ui->te_text->setText(originalText);
    }
}

void knowledgebase::_prepareWindow()
{
    ui->gb_text->setTitle(STR_START_FRAG);

    ui->cmb_razdel->clear();
    ui->cmb_question->clear();
    ui->cmb_act->clear();
    ui->cmb_quality->clear();

    ui->cmb_razdel->addItems(ListRazd);
    ui->cmb_quality->addItems(ListQuality);
    ui->cmb_act->addItems(ListAct);
}

void knowledgebase::_showMessage(QString text, QString title)
{
    QMessageBox msg;
    msg.setText(text);
    msg.setWindowTitle(title);
    msg.exec();
}

bool knowledgebase::_showQuestion(QString text, QString title, QString textYes, QString textNo)
{
    bool result = false;
    QMessageBox msgBox(QMessageBox::Question, title, text, QMessageBox::Yes | QMessageBox::No, this);
    msgBox.setButtonText(QMessageBox::Yes, textYes);
    msgBox.setButtonText(QMessageBox::No, textNo);

    //Центрируем окно по центру экрана
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(this);
    QPoint center = rect.center();
    int x = center.x() - (width() / 2);
    int y = center.y() - (height() / 2) + 20; //Смещение 20, чтобы было видно кнопки управления
    center.setX(x);
    center.setY(y);
    move(center);

    qint32 resMsg = msgBox.exec();
    if (resMsg == QMessageBox::Yes)
        result = true;
    return result;
}

void knowledgebase::getFragment(fragment *frag)
{
    _prepareWindow();

    m_originalFrag = frag;
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
    ui->te_text->setText(frag->getText());
    originalText = frag->getText();
    _select();
}

void knowledgebase::open()
{
    QDialog::open();
    if (m_showHelp) {
        QString info = "При первом входе будет представлен раздел БЗ «Все фрагменты из КД», и фрагменты со значениями "
                       "характеристик «Раздел», «Вопрос», «Качество», как в выделенном фрагменте в тексте проекта. Для "
                       "их просмотра нажмите «Следующий».\n\n"
                       "Для просмотра других разделов БЗ или фрагментов с другими значениями характеристик «Раздел», "
                       "«Вопрос», «Качество» нажмите «Разблокировать», отметьте нужные значения и нажмите «Применить». "
                       "Управление просмотром - кнопки «Следующий», «Предыдущий».";
        // TODO: В будущем реализовать кнопку справка для БЗ
        //" Для повторного просмотра сообщения нажмите «Справка».";
        m_showHelp = _showQuestion(info + "\n\nОтобразить эту подсказку при следующем входе в Базу знаний?");
    }
}

// TODO: Баг: через раз открывается то в одном режиме, то в другом
void knowledgebase::on_pb_unlock_clicked()
{
    m_unlocked = !m_unlocked;
    if (m_unlocked) {
        ui->pb_unlock->setText("Применить");
    } else {
        ui->pb_unlock->setText("Разблокировать");
        ui->pb_unlock->setToolTip("Изменить характеристики или тип показываемых фрагментов");
        m_originalFrag->setRazdel(AbbreviationRazd[ui->cmb_razdel->currentIndex()]);
        m_originalFrag->setVoprosABR(
                ABRQuestionsAtRazdel[ui->cmb_razdel->currentIndex()][ui->cmb_question->currentIndex()]);
        m_originalFrag->setAkt(AbbreviationAct[ui->cmb_act->currentIndex()]);
        m_originalFrag->setKachestvo(AbbreviationQuality[ui->cmb_quality->currentIndex()]);
        _select();
    }
    ui->cmb_razdel->setEnabled(m_unlocked);
    ui->cmb_act->setEnabled(m_unlocked);
    ui->cmb_question->setEnabled(m_unlocked);
    ui->cmb_quality->setEnabled(m_unlocked);
    ui->ch_all_acts->setEnabled(m_unlocked);
    ui->groupBox_2->setEnabled(m_unlocked);

    ui->gb_text->setEnabled(!m_unlocked);
    ui->pb_prev->setEnabled(!m_unlocked);
    ui->pb_next->setEnabled(!m_unlocked);
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
    ui->ln_kdName->clear();
    fragmentsForShow.clear();
    namesForShow.clear();
    idForNames.clear();
    for (auto order : ordersForShow) {
        delete order;
    }
    ordersForShow.clear();

    qint32 questionKod = m_originalFrag->getVoprosNumber();
    QSqlQuery querySelect;
    // TODO: [старое, нужно?] добавить закону норм поиск используя листВозможность
    switch (m_currentViewMode) {
    case eLaw: {
        querySelect.prepare("SELECT ТФрагмент.ТекстФрагмента, ТФрагмент.КодЗакона FROM ТФрагмент WHERE "
                            "Тфрагмент.КодВопрос = :val1");
        break;
    }
    case eTypicalKD:
        querySelect.prepare("SELECT Тексты.Текст, Тексты.Качество, Тексты.Акт, Тексты.[#Дог] FROM Тексты WHERE "
                            "Тексты.Вопрос = :val1 "
                            "AND ВклВСправку=true");
        break;
    case eAllKD:
        querySelect.prepare("SELECT Тексты.Текст, Тексты.Качество, Тексты.Акт, Тексты.[#Дог] FROM Тексты WHERE "
                            "Тексты.Вопрос = :val1");
        break;
    }

    querySelect.bindValue(":val1", questionKod);
    if (!querySelect.exec()) {
        qDebug() << querySelect.lastError().text();
    }
    while (querySelect.next()) {
        QString text = querySelect.value(0).toString();
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
            fragmentsForShow.append(text);
        } else {
            //типовые и все фрагменты
            if (m_allActs) {
                if (m_originalFrag->getKachestvo() == querySelect.value(1).toString()) {
                    fragmentsForShow.append(text);
                    idForNames.append(querySelect.value(3).toString());
                }
            } else {
                if (m_originalFrag->getAkt() == querySelect.value(2).toString()
                    && m_originalFrag->getKachestvo() == querySelect.value(1).toString()) {
                    fragmentsForShow.append(text);
                    idForNames.append(querySelect.value(3).toString());
                }
            }
        }
    }
    //Собираем названия ВУЗов КД по id полученным из текстов
    if (m_currentViewMode != eLaw) {
        for (auto kdId : idForNames) {
            QSqlQuery queryName;
            queryName.prepare("SELECT ТУчреждение.Аббр, ТУчреждение.ИмяУчреждения FROM ТУчреждение WHERE "
                              "ТУчреждение.КодУчреждения = :val1");
            queryName.bindValue(":val1", kdId);
            if (!queryName.exec()) {
                qDebug() << querySelect.lastError().text();
            }
            if (queryName.next()) {
                if (queryName.value(0).toString().isEmpty()) {
                    namesForShow.append(queryName.value(1).toString());
                } else {
                    namesForShow.append(queryName.value(0).toString());
                }
            }
        }
    }

    currentFragmentNumber = -1;
    on_pb_next_clicked();
    on_pb_prev_clicked();
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
        ui->gb_text->setTitle(STR_START_FRAG);
        ui->ln_kdName->setText("-");
    } else {
        currentFragmentNumber++;
        ui->te_text->setText(fragmentsForShow[currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(currentFragmentNumber + 1) + "/"
                              + QString::number(fragmentsForShow.size()) + STR_FRAG_FROM_SELECTED);
        if ((currentFragmentNumber < namesForShow.count() - 1) && m_currentViewMode != eLaw)
            ui->ln_kdName->setText(namesForShow[currentFragmentNumber]);
    }

    if (m_currentViewMode == eLaw) {
        if (currentFragmentNumber == -1) {
            ui->ln_order->setText("-");
            ui->ln_adoption_date->setText("-");
            ui->ln_change_date->setText("-");
            ui->ln_order->setToolTip("");
        } else if (ordersForShow.size() != 0 && ordersForShow.size() >= currentFragmentNumber) {
            ui->ln_order->setText(ordersForShow[currentFragmentNumber]->name);
            ui->ln_adoption_date->setText(ordersForShow[currentFragmentNumber]->dateAdoptation.toString("dd.MM.yyyy"));
            ui->ln_change_date->setText(ordersForShow[currentFragmentNumber]->dateChange.toString("dd.MM.yyyy"));
            ui->ln_order->setToolTip(ordersForShow[currentFragmentNumber]->name);
        }
        ui->ln_order->home(false);
    } else {
        ui->ln_kdName->home(false);
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
        ui->gb_text->setTitle(STR_START_FRAG);
        ui->ln_kdName->setText("-");
    } else if (currentFragmentNumber == -1) {
        currentFragmentNumber = fragmentsForShow.size() - 1;
        ui->te_text->setText(fragmentsForShow[currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(currentFragmentNumber + 1) + "/"
                              + QString::number(fragmentsForShow.size()) + STR_FRAG_FROM_SELECTED);
        if (m_currentViewMode != eLaw)
            ui->ln_kdName->setText(namesForShow[currentFragmentNumber]);
    } else {
        currentFragmentNumber--;
        ui->te_text->setText(fragmentsForShow[currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(currentFragmentNumber + 1) + "/"
                              + QString::number(fragmentsForShow.size()) + STR_FRAG_FROM_SELECTED);
        if (m_currentViewMode != eLaw)
            ui->ln_kdName->setText(namesForShow[currentFragmentNumber]);
    }

    if (m_currentViewMode == eLaw) {
        if (currentFragmentNumber < 0 || currentFragmentNumber < 0) {
            ui->ln_order->setText("-");
            ui->ln_adoption_date->setText("-");
            ui->ln_change_date->setText("-");
            ui->ln_order->setToolTip("");
        } else if (ordersForShow.size() != 0 && ordersForShow.size() >= currentFragmentNumber) {
            ui->ln_order->setText(ordersForShow[currentFragmentNumber]->name);
            ui->ln_adoption_date->setText(ordersForShow[currentFragmentNumber]->dateAdoptation.toString("dd.MM.yyyy"));
            ui->ln_change_date->setText(ordersForShow[currentFragmentNumber]->dateChange.toString("dd.MM.yyyy"));
            ui->ln_order->setToolTip(ordersForShow[currentFragmentNumber]->name);
        }
        ui->ln_order->home(false);
    } else {
        ui->ln_kdName->home(false);
    }
}

void knowledgebase::on_pb_showList_clicked()
{
    // TODO: [?Улучшение?] [10] Показать окно со списком всех фрагментов, можно по 100 символов и полностью при
    // наведении
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

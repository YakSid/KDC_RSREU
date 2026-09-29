#include "knowledgebase.h"
#include "ui_knowledgebase.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

const QString STR_START_FRAG = "Начальный фрагмент";
const QString STR_FRAG_FROM_SELECTED = " Фрагмент из выбранных";

const QStringList LAW_GROUPS = {"1 – Неизменяемые параметры (Ан, Ут)",
                                "2 – Устанавливаемые в КД порядке (До)",
                                "3 – Повышаемые параметры (Вы)",
                                "4 - Специфические вопросы (Св)"};

knowledgebase::knowledgebase(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::knowledgebase)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    setStyleSheet("QPushButton:disabled {"
                  "background-color: darkGrey;"
                  "border: 2px solid darkGrey;}");

    ui->wgt_laws->setStyleSheet("QWidget#wgt_laws { background-color: #87CEEB; }");
}

knowledgebase::~knowledgebase()
{
    if (m_lawHeadersForShow.size() > 0) {
        m_lawHeadersForShow.clear();
    }
    delete ui;
}

void knowledgebase::_prepareUi()
{
    ui->gb_text->setTitle(STR_START_FRAG);

    ui->cmb_razdel->clear();
    ui->cmb_question->clear();
    ui->cmb_act->clear();
    ui->cmb_quality->clear();

    ui->cmb_razdel->addItems(ListRazd);
    ui->cmb_quality->addItems(ListQuality);
    ui->cmb_act->addItems(ListAct);

    // при открытии БЗ всегда выставляем стандартный выбор
    // if (ui->rb_typical_fragments_kd->isChecked()) {
    //     ui->rb_typical_fragments_kd->setChecked(false);
    //     ui->rb_all_fragments_kd->setChecked(true);
    // }
    // ui->ch_all_acts->setChecked(true);

    _lockUi(true);
}

void knowledgebase::_lockUi(bool lock)
{
    ui->cmb_razdel->setDisabled(lock);
    ui->cmb_act->setDisabled(lock);
    ui->cmb_question->setDisabled(lock);
    ui->cmb_quality->setDisabled(lock);
    ui->ch_all_acts->setDisabled(lock);
    ui->groupBox_2->setDisabled(lock);

    ui->gb_text->setDisabled(!lock);
    ui->pb_prev->setDisabled(!lock);
    ui->pb_next->setDisabled(!lock);
    ui->pb_prevLaw->setDisabled(!lock);
    ui->pb_nextLaw->setDisabled(!lock);

    if (lock) {
        ui->pb_unlock->setText("Разблокировать параметры");
        ui->pb_unlock->setToolTip("Изменить характеристики или тип показываемых фрагментов");
    } else {
        ui->pb_unlock->setText("Искать");
        ui->pb_unlock->setToolTip("");
    }
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

    // Центрируем окно по центру экрана
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(this);
    QPoint center = rect.center();
    int x = center.x() - (width() / 2);
    int y = center.y() - (height() / 2) + 20; // Смещение 20, чтобы было видно кнопки управления
    center.setX(x);
    center.setY(y);
    move(center);

    qint32 resMsg = msgBox.exec();
    if (resMsg == QMessageBox::Yes)
        result = true;
    return result;
}

void knowledgebase::_showLaw(qint32 index)
{
    if (m_lawHeadersForShow.isEmpty()) {
        ui->ln_lawName->setText("-");
        ui->ln_dt_accept->setText("-");
        ui->ln_dt_change->setText("-");
        ui->te_law->setText("Пробел в праве");
        ui->gb_laws->setTitle("Фрагменты законов");
        ui->lb_lawParam->setText(LAW_GROUPS[3]);
    } else {
        if (0 <= index && index < m_lawHeadersForShow.count()) {
            ui->ln_lawName->setText(m_lawHeadersForShow[index]->name);
            ui->ln_dt_accept->setText(m_lawHeadersForShow[index]->dateAdoptation.toString("dd.MM.yyyy"));
            ui->ln_dt_change->setText(m_lawHeadersForShow[index]->dateChange.toString("dd.MM.yyyy"));
            ui->te_law->setToolTip(m_lawHeadersForShow[index]->name);
            ui->gb_laws->setTitle("Фрагменты законов [" + QString::number(index + 1) + "/" + QString::number(m_lawHeadersForShow.count())
                                  + "]");
        }

        if (0 <= index && index < m_lawsForShow.count())
            ui->te_law->setText(m_lawsForShow[index]);

        if (0 <= index && index < m_lawParams.count()) {
            ui->lb_lawParam->setText(LAW_GROUPS[m_lawParams[index] - 1]);
            if (m_lawParams[index] == 4) {
                ui->te_law->setText("Пробел в праве");
            }
        }
    }
}

void knowledgebase::_showFragment(qint32 index)
{
    if (m_fragmentsForShow.isEmpty() || m_currentFragmentNumber >= m_fragmentsForShow.count()) {
        ui->te_text->setText(m_originalText);
        ui->gb_text->setTitle(STR_START_FRAG);
        ui->ln_kdName->setText("-");
        return;
    }

    if (m_currentFragmentNumber == -1) {
        ui->te_text->setText(m_originalText);
        ui->gb_text->setTitle(STR_START_FRAG);
        ui->ln_kdName->setText("-");
    } else {
        ui->te_text->setText(m_fragmentsForShow[m_currentFragmentNumber]);
        ui->gb_text->setTitle(QString::number(m_currentFragmentNumber + 1) + "/" + QString::number(m_fragmentsForShow.size())
                              + STR_FRAG_FROM_SELECTED);
        if (m_currentFragmentNumber >= m_institutionNames.count() || m_currentFragmentNumber < 0) {
            ui->ln_kdName->setText("-");
        } else {
            ui->ln_kdName->setText(m_institutionNames[m_currentFragmentNumber]);
        }
    }
    ui->ln_kdName->home(false);
}

void knowledgebase::prepare(fragment *frag)
{
    _prepareUi();

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
    m_originalText = frag->getText();

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
                       "«Вопрос», «Качество» нажмите «Разблокировать параметры», отметьте нужные значения и нажмите «Искать». "
                       "Управление просмотром - кнопки «Следующий», «Предыдущий».\n\n"
                       "Для повторного просмотра сообщения наведите курсор на поле \"Справка\" в верхнем правом углу.";
        m_showHelp = _showQuestion(info + "\n\nОтобразить эту подсказку при следующем входе в Базу знаний?");
    }
}

int knowledgebase::exec()
{
    if (m_showHelp) {
        QString info = "При первом входе будет представлен раздел БЗ «Все фрагменты из КД», и фрагменты со значениями "
                       "характеристик «Раздел», «Вопрос», «Качество», как в выделенном фрагменте в тексте проекта. Для "
                       "их просмотра нажмите «Следующий».\n\n"
                       "Для просмотра других разделов БЗ или фрагментов с другими значениями характеристик «Раздел», "
                       "«Вопрос», «Качество» нажмите «Разблокировать параметры», отметьте нужные значения и нажмите «Искать». "
                       "Управление просмотром - кнопки «Следующий», «Предыдущий».\n\n"
                       "Для повторного просмотра сообщения наведите курсор на поле \"Справка\" в верхнем правом углу.";
        m_showHelp = _showQuestion(info + "\n\nОтобразить эту подсказку при следующем входе в Базу знаний?");
    }
    return QDialog::exec();
}

void knowledgebase::on_pb_unlock_clicked()
{
    ELockState prevState = ui->pb_unlock->text() == "Разблокировать параметры" ? ELockState::locked : ELockState::unlocked;
    ELockState newState = prevState == ELockState::locked ? ELockState::unlocked : ELockState::locked;

    if (newState == ELockState::locked) {
        m_originalFrag->setRazdel(AbbreviationRazd[ui->cmb_razdel->currentIndex()]);
        m_originalFrag->setVoprosABR(ABRQuestionsAtRazdel[ui->cmb_razdel->currentIndex()][ui->cmb_question->currentIndex()]);
        m_originalFrag->setAkt(AbbreviationAct[ui->cmb_act->currentIndex()]);
        m_originalFrag->setKachestvo(AbbreviationQuality[ui->cmb_quality->currentIndex()]);

        _select();
    }

    _lockUi(newState == ELockState::locked);
}

void knowledgebase::_select()
{
    ui->ln_kdName->clear();

    // Очищаем выбранные данные фрагментов
    m_fragmentsForShow.clear();
    m_institutionNames.clear();

    // Очищаем выбранные данные законов
    m_lawHeadersForShow.clear();
    m_lawsForShow.clear();
    m_lawParams.clear();
    m_currentLaw = 0;

    qint32 questionKod = m_originalFrag->requestQuestionCodeFromDB();

    // Заполняем законы
    QSqlQuery lawQuery;
    lawQuery.prepare("SELECT ТФрагмент.ТекстФрагмента, ТФрагмент.КодЗакона, ТФрагмент.КодГрПарам FROM ТФрагмент WHERE "
                     "Тфрагмент.КодВопрос = :val1");
    lawQuery.bindValue(":val1", questionKod);
    if (!lawQuery.exec()) {
        qDebug() << lawQuery.lastError().text();
    }
    while (lawQuery.next()) {
        QString text = lawQuery.value(0).toString();
        qint32 kodZakona = lawQuery.value(1).toInt();
        quint32 groupParam = lawQuery.value(2).toUInt();
        if (groupParam == 0)
            groupParam = 4;
        for (auto order : TOrder) {
            if (kodZakona == order->id) {
                m_lawHeadersForShow.append(order);
            }
        }
        m_lawsForShow.append(text);
        m_lawParams.append(groupParam);
    }

    // Готовим фрагменты КД
    QString strQuery = "SELECT Тексты.Текст, Тексты.Качество, Тексты.Акт, Тексты.[#Дог] FROM Тексты WHERE Тексты.Вопрос = :val1";
    if (ui->rb_typical_fragments_kd->isChecked())
        strQuery += " AND ВклВСправку=true";
    QSqlQuery querySelect;
    querySelect.prepare(strQuery);
    querySelect.bindValue(":val1", questionKod);
    if (!querySelect.exec()) {
        qDebug() << querySelect.lastError().text();
    }
    //! Коды учреждений
    QList<QString> vuzCodes;
    while (querySelect.next()) {
        QString text = querySelect.value(0).toString();
        QString kachestvo = querySelect.value(1).toString();
        QString akt = querySelect.value(2).toString();
        QString id = querySelect.value(3).toString();

        bool compared = m_originalFrag->getKachestvo() == kachestvo && (ui->ch_all_acts->isChecked() || m_originalFrag->getAkt() == akt);

        if (compared) {
            m_fragmentsForShow.append(text);
            vuzCodes.append(id);
        }
    }

    // Собираем названия ВУЗов КД по id полученным из текстов
    for (const QString &vuzCode : vuzCodes) {
        QSqlQuery queryName;
        queryName.prepare("SELECT ТУчреждение.Аббр, ТУчреждение.ИмяУчреждения FROM ТУчреждение WHERE "
                          "ТУчреждение.КодУчреждения = :val1");
        queryName.bindValue(":val1", vuzCode);
        if (!queryName.exec()) {
            qDebug() << querySelect.lastError().text();
        }
        if (queryName.next()) {
            if (queryName.value(0).toString().isEmpty()) {
                m_institutionNames.append(queryName.value(1).toString());
            } else {
                m_institutionNames.append(queryName.value(0).toString());
            }
        }
    }

    m_currentFragmentNumber = -1;

    _showFragment(m_currentFragmentNumber);

    //! Нельзя листать фрагменты законов - если их меньше двух
    bool noPrevNextLaw = m_lawHeadersForShow.count() <= 1;
    ui->pb_nextLaw->setDisabled(noPrevNextLaw);
    ui->pb_prevLaw->setDisabled(noPrevNextLaw);

    _showLaw(m_currentLaw);

    _lockUi(true);

    ui->lb_look->setText("Найдено фрагментов: " + QString::number(m_fragmentsForShow.count()));
}

void knowledgebase::on_pb_insert_into_kd_clicked()
{
    if (ui->te_text->toPlainText().isEmpty())
        return;

    auto transportFrag = new fragment();
    transportFrag = new fragment();
    transportFrag->setText(ui->te_text->toPlainText());
    transportFrag->setKachestvo(AbbreviationQuality[ui->cmb_quality->currentIndex()]);

    transportFrag->setAkt(AbbreviationAct[ui->cmb_act->currentIndex()]);
    transportFrag->setVoprosABR(ABRQuestionsAtRazdel[ui->cmb_razdel->currentIndex()][ui->cmb_question->currentIndex()]);
    transportFrag->setRazdel(AbbreviationRazd[ui->cmb_razdel->currentIndex()]);
    transportFrag->setNewAdded(true);

    emit s_fragmentSentFromKB(transportFrag);
    delete transportFrag;
    close();
}

void knowledgebase::on_pb_next_clicked()
{
    m_currentFragmentNumber++;
    if (m_currentFragmentNumber >= m_fragmentsForShow.size()) {
        m_currentFragmentNumber = -1;
    }
    _showFragment(m_currentFragmentNumber);
}

void knowledgebase::on_pb_prev_clicked()
{
    m_currentFragmentNumber--;
    if (m_currentFragmentNumber < -1) {
        m_currentFragmentNumber = m_fragmentsForShow.size() - 1;
    }
    _showFragment(m_currentFragmentNumber);
}

void knowledgebase::on_pb_showList_clicked()
{
    // TODO: [?Улучшение?] [10] Показать окно со списком всех фрагментов, можно по 100 символов и полностью при
    // наведении
    // ui->lw_fragments->insertItems(0, m_fragmentsForShow);
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
    if (m_savedFragmentsPath.isEmpty()) {
        m_savedFragmentsPath = QFileDialog::getSaveFileName(this, "Текстовый файл для дополнительных фрагментов", "", "*.txt");
        ui->pb_insert_into_file->setToolTip(m_savedFragmentsPath);
    }
    QFile file(m_savedFragmentsPath);
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream writeStream(&file);
    writeStream << "\n\n" << ui->te_text->toPlainText();
    file.close();
}

void knowledgebase::on_pb_nextLaw_clicked()
{
    m_currentLaw++;
    if (m_currentLaw >= m_lawHeadersForShow.count()) {
        m_currentLaw = 0;
    }
    _showLaw(m_currentLaw);
}

void knowledgebase::on_pb_prevLaw_clicked()
{
    m_currentLaw--;
    if (m_currentLaw < 0) {
        m_currentLaw = m_lawHeadersForShow.count() - 1;
    }
    _showLaw(m_currentLaw);
}

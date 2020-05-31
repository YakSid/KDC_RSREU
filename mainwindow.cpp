#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAxObject>
#include "cdatabasemanager.h"

const char PREVIOUS_SELECTION[] = "previousSelection";

//! Вопросы:
//! Эффективность и Кэф это одно и то же? Почему в БД два разных поля?
//! ОТВЕТ: в листКД можно оставить ту, что есть, главное хорошие в КД
//!
//! Можно ли не выбрав фрагмент зайти в базу знаний? (Можно ради добавления нового пункта?)
//! ОТВЕТ: можно (юзер сам введёт параметры)
//!
//! Клавиша "Очистить поле" очищает параметры фрагмента или только текстовое поле?
//! ОТВЕТ: только текстовое поле
//!
//! Для фрагментов законов переносящихся из БЗ в mw что указывать в поле КАЧЕСТВО (Было же ВОЗМОЖНОСТЬ)
//! ОТВЕТ: ставить Ан аналог
//!
//! Что писать в поле АКТ, когда выбраны "Все фрагменты" в БЗ? (ведь поле Акт скрывается(МОЖЕТ ЕГО БЛОКИРОВАТЬ, А НЕ СКРЫВАТЬ?))
//! ОТВЕТ: Да, делать disabled, а не скрывать поле (показывать все фрагменты не обращяя внимания на поле АКТ)
//!

/* Новые вопросы
 * 1. В таблице "вопросы2" код 6 (СДД ПСП Увольнение) и код 39 (СДД ПСП Гарантии), 64,68 одинаковые сокращения (вопросы2
 * вообще какая-то странная) [dev: если утвержят, то later можно сделать индексацию вопроса не по abr, а по id]
 *
 *
 */

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    lDialog = new ListKD();
    kBase = new knowledgebase();
    connect(this, &MainWindow::s_sentFragment, kBase, &knowledgebase::getFragment);
    connect(kBase, &knowledgebase::startTransportFrag, this, &MainWindow::insertFragFromKB);

    bool debugMode = true;
    if (!debugMode) {
        sDialog.setModal(true);
        sDialog.exec();
        if (sDialog.StartMode == 1) {
            lDialog->setModal(true);
            lDialog->exec();
            if (!lDialog->WantGo) {
                exit(3);
            } else {
                ui->setupUi(this);
                m_db = new CDatabaseManager();
                //Подготовка договора и mw
                _prepareMainWindow(lDialog->SelectedKD);
            }
        } else
            exit(2);
    } else {
        // DebugMode
        ui->setupUi(this);
        m_db = new CDatabaseManager();
        _prepareMainWindow("6201");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete kBase;
    delete lDialog;
    delete m_db;
    delete currentKolDog;
}

void MainWindow::setWorkMode(EWorkMode newMode)
{
    switch (newMode) {
    case eBasicMode:
        ui->BazeKnowledge->setEnabled(false);
        ui->TextRight->setEnabled(false);
        ui->pb_clearField->setEnabled(false);
        ui->pb_deleteFrag->setEnabled(false);
        ui->pb_newFrag->setEnabled(true);
        ui->GoRight->setEnabled(false);
        ui->GoLeft->setEnabled(false);
        ui->groupBox->setEnabled(false);
        ui->tw_navigator->setEnabled(true);
        ui->btn_showFullText->setEnabled(m_navigatorButtonEnabled);
        SelectedFragment = -1;
        break;
    case eItemSelectedMode:
        if (m_currentWorkMode != eItemSelectedMode) {
            ui->BazeKnowledge->setEnabled(true);
            ui->TextRight->setEnabled(false);
            ui->pb_clearField->setEnabled(false);
            ui->pb_deleteFrag->setEnabled(true);
            ui->pb_newFrag->setEnabled(true);
            ui->GoRight->setEnabled(true);
            ui->GoLeft->setEnabled(false);
            ui->groupBox->setEnabled(false);
            ui->tw_navigator->setEnabled(true);
            ui->btn_showFullText->setEnabled(m_navigatorButtonEnabled);
        }
        break;
    case eRightFrameMode:
        ui->BazeKnowledge->setEnabled(true);
        ui->TextRight->setEnabled(true);
        ui->pb_clearField->setEnabled(true);
        ui->pb_deleteFrag->setEnabled(false);
        ui->pb_newFrag->setEnabled(false);
        ui->GoRight->setEnabled(false);
        ui->GoLeft->setEnabled(true);
        ui->groupBox->setEnabled(true);
        ui->tw_navigator->setEnabled(false);
        ui->btn_showFullText->setEnabled(false);
        break;
    }
    m_currentWorkMode = newMode;
}

void MainWindow::insertFragFromKB(fragment *frag)
{
    // TODO: [9] СЕЙЧАС сделать метод вставки из БЗ в окно редактирования
    // Учитывать изменённые параметры и при разных типах фрагментов
    qint32 prevSelectedFragId = SelectedFragment;
    switch (m_currentWorkMode) {
    case eBasicMode:
        return;
    case eItemSelectedMode:
        _clearSelectionInCentral(currentKolDog->fragments[SelectedFragment]->getPositionFirst(),
                                 currentKolDog->fragments[SelectedFragment]->getPositionLast());
        break;
    case eRightFrameMode:
        on_GoLeft_clicked();
        break;
    }
    setWorkMode(eRightFrameMode);

    ui->TextRight->setText(frag->getText());
    _prepareSettingsInRight(frag->getAkt(), frag->getRazdel(), frag->getKachestvo(), frag->getVoprosABR());

    TextCenterIsBlocked = true;
    m_addNewFrag = true;

    SelectedFragment = prevSelectedFragId;
}

void MainWindow::_prepareMainWindow(QString docId)
{
    SelectedKD = docId;
    //Здесь производится заполнение данных
    //Заполнение коэффициентов
    currentKolDog = new CKolDog();
    QSqlQuery in1_query, in2_query, in3_query, in4_query;
    in1_query.prepare("SELECT * FROM Договор WHERE Договор.[#Дог] = :val1");
    in1_query.bindValue(":val1", SelectedKD);
    if (!in1_query.exec()) {
        qDebug() << in1_query.lastError().text();
    }
    if (in1_query.next()) {
        // Расчёт параметров и заполнение полей mw
        ui->startKTR->setText(in1_query.value(7).toString());
        ui->startKSC->setText(in1_query.value(16).toString());
        ui->startKGDP->setText(in1_query.value(15).toString());
        ui->startKPSP->setText(_doubleToFloatString(in1_query.value(14).toDouble()));
        double doubleKEF = 1.3 * (1.5 * ui->startKTR->text().toDouble() + ui->startKSC->text().toDouble())
                + ui->startKGDP->text().toDouble() + ui->startKPSP->text().toDouble();
        QString strKEF = _doubleToFloatString(doubleKEF);
        ui->startKEF->setText(strKEF);
        // Заполнение параметров класса договора
        currentKolDog->setMainParameters(
                in1_query.value(0).toString(), in1_query.value(1).toString(), in1_query.value(2).toDate(),
                in1_query.value(3).toUInt(), in1_query.value(4).toBool(), in1_query.value(5).toFloat(),
                in1_query.value(6).toInt(), in1_query.value(7).toInt(), _strDoubleToFloat(strKEF),
                in1_query.value(14).toFloat(), in1_query.value(15).toInt(), in1_query.value(16).toInt(),
                in1_query.value(20).toDate(), in1_query.value(21).toInt(), in1_query.value(22).toInt(),
                in1_query.value(23).toInt(), in1_query.value(24).toInt(), in1_query.value(25).toInt(),
                in1_query.value(26).toInt(), in1_query.value(28).toInt(), in1_query.value(27).toInt(),
                in1_query.value(30).toFloat());
        //Заполнение текущих коэффициентов
        currentKolDog->calculateCurrentKeffs();
        _fillCurrentKeffs(currentKolDog->getFiveCurrentKeffs());
    }
    //Нахождение полного имени учрежд. и запись в шапку
    in4_query.prepare("SELECT ИмяУчреждения FROM ТУчреждение WHERE ТУчреждение.КодУчреждения = :val1");
    in4_query.bindValue(":val1", currentKolDog->getId());
    if (!in4_query.exec()) {
        qDebug() << in4_query.lastError().text();
    }
    if (in4_query.next()) {
        ui->DogName->setText(in4_query.value(0).toString());
    }
    //
    ui->tw_navigator->setColumnWidth(0, 211);
    ui->tw_navigator->setProperty(PREVIOUS_SELECTION, -1);
    //Запрос на заполнение центрального поля
    int QuestionNum = 0;
    in2_query.prepare("SELECT * FROM Тексты WHERE Тексты.[#Дог] = :val1 ORDER BY Тексты.[#Текст]");
    in2_query.bindValue(":val1", SelectedKD);
    if (!in2_query.exec()) {
        qDebug() << in2_query.lastError().text();
    }
    while (in2_query.next()) {
        fragment *frag = new fragment();
        //Заполнение данных фрагмента и навигатора
        QuestionNum = in2_query.value(6).toInt();
        in3_query.prepare("SELECT * FROM Вопросы WHERE Вопросы.Код = :val1");
        in3_query.bindValue(":val1", QuestionNum);
        if (!in3_query.exec()) {
            qDebug() << in3_query.lastError().text();
        }
        if (in3_query.next()) {
            for (int i = 0; i < 11; i++)
                if (in3_query.value(2) == AbbreviationRazd[i]) {
                    frag->setRazdel(in3_query.value(2).toString());
                    frag->setVoprosABR(in3_query.value(1).toString());
                    break;
                }
        }
        frag->setText(in2_query.value(2).toString());
        frag->setKachestvo(in2_query.value(4).toString());
        frag->setAkt(in2_query.value(5).toString());
        currentKolDog->fragments.append(frag);
        // WARNING: [later] Решить проблему увеличения длинны фрагментов (а не подгонять костылями)
    }
    _fillCentralField(eAllSections);
    TextCenterIsBlocked = false;
}

void MainWindow::_fillCentralField(EDisplayedSection selectedSection)
{
    TextCenterIsBlocked = true;
    ui->te_textCenter->clear();
    QTextCursor cursor(ui->te_textCenter->document());

    if (selectedSection == eAllSections) {
        for (auto fragment : currentKolDog->fragments) {
            _addFragmentToCentralField(fragment, cursor);
        }
    } else {
        for (auto fragment : currentKolDog->fragments) {
            if (fragment->getRazdel() == AbbreviationRazd[selectedSection]) {
                _addFragmentToCentralField(fragment, cursor);
                fragment->setVisible(true);
            } else {
                fragment->SetPositions(0, 0);
                fragment->setVisible(false);
            }
        }
    }
    TextCenterIsBlocked = false;
    setWorkMode(eBasicMode);
}

void MainWindow::_addFragmentToCentralField(fragment *frag, QTextCursor cursor)
{
    QString argLine;
    qint32 posBegin, posEnd;
    cursor.movePosition(QTextCursor::End);
    posBegin = cursor.position();
    cursor.insertText(frag->getText());
    if (frag->getText().right(1) != '\n') {
        cursor.insertBlock();
    }
    argLine.clear();
    argLine += frag->getRazdel() + "\t" + frag->getVoprosABR() + "\t" + frag->getAkt() + "\t" + frag->getKachestvo();
    cursor.insertText(argLine);
    cursor.insertBlock();
    cursor.insertBlock();
    posEnd = cursor.position();
    frag->SetPositions(
            posBegin,
            posEnd); // NOTE: Размер включает в себя: Текст, Размер СтрокиАргументов и 2-3 символа нового параграфа
}

void MainWindow::_recountPositions(int idfrag, int delta, bool withFirstOfCurrent)
{
    for (int i = idfrag; i < currentKolDog->fragments.count(); i++) {
        if (i != idfrag || withFirstOfCurrent) {
            currentKolDog->fragments[i]->setPositionFirst(currentKolDog->fragments[i]->getPositionFirst() + delta);
            if (withFirstOfCurrent)
                withFirstOfCurrent = false;
        }
        currentKolDog->fragments[i]->setPositionLast(currentKolDog->fragments[i]->getPositionLast() + delta);
    }
}

QString MainWindow::_doubleToFloatString(double value)
{
    auto result = QString::number(value, 'f', 1);
    result = result.replace(result.indexOf('.'), 1, ',');
    return result;
}

float MainWindow::_strDoubleToFloat(QString value)
{
    value = value.replace(value.indexOf(','), 1, '.');
    return value.toFloat();
}

void MainWindow::_setSelectionInCentral(qint32 posStart, qint32 posEnd)
{
    QTextCursor cursor(ui->te_textCenter->document());
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    //Выделить
    cursor.setPosition(posStart, QTextCursor::MoveAnchor);
    cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
}

void MainWindow::_clearSelectionInCentral(qint32 posStart, qint32 posEnd)
{
    QTextCursor cursor(ui->te_textCenter->document());
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    //Убираем выделение
    cursor.setPosition(posStart, QTextCursor::MoveAnchor);
    cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
}

void MainWindow::_clearSelectionInCentral()
{
    QTextCursor cursor(ui->te_textCenter->document());
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    cursor.setPosition(ui->te_textCenter->toPlainText().length(), QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
}

void MainWindow::_deleteSelectedFrag()
{
    // TODO: [later] переделать функцию, по типу добавления, чтобы без _fillCentralField было и перемотки вверх
    currentKolDog->fragments.removeAt(SelectedFragment);
    if (ui->tw_navigator->property(PREVIOUS_SELECTION).toInt() == -1) {
        _fillCentralField(eAllSections);
    } else {
        _fillCentralField(EDisplayedSection(ui->tw_navigator->property(PREVIOUS_SELECTION).toInt()));
    }
    SelectedFragment = -1;
}

void MainWindow::_showMessage(QString text, QString title)
{
    QMessageBox msg;
    msg.setText(text);
    msg.setWindowTitle(title);
    msg.exec();
}

bool MainWindow::_showQuestion(QString text, QString title, QString textYes, QString textNo)
{
    bool result = false;
    QMessageBox msgBox(QMessageBox::Question, title, text, QMessageBox::Yes | QMessageBox::No, this);
    msgBox.setButtonText(QMessageBox::Yes, textYes);
    msgBox.setButtonText(QMessageBox::No, textNo);
    qint32 resMsg = msgBox.exec();
    if (resMsg == QMessageBox::Yes)
        result = true;
    return result;
}

void MainWindow::_fillCurrentKeffs(QVariantList keffs)
{
    ui->KTR->setText(QString::number(keffs[0].toInt()));
    ui->KSC->setText(QString::number(keffs[1].toInt()));
    ui->KGDP->setText(QString::number(keffs[2].toInt()));
    ui->KPSP->setText(_doubleToFloatString(keffs[3].toDouble()));
    ui->KEF->setText(_doubleToFloatString(keffs[4].toDouble()));
    // Сравнение и раскрашивание
    // NOTE: потом-потом оптимизировать
    QPalette greenPal, redPal, blackPal;
    greenPal.setColor(QPalette::WindowText, Qt::darkGreen);
    redPal.setColor(QPalette::WindowText, Qt::darkRed);
    blackPal.setColor(QPalette::WindowText, Qt::black);
    QVariantList startKeffs({ ui->startKTR->text().toFloat(), ui->startKSC->text().toFloat(),
                              ui->startKGDP->text().toFloat(), _strDoubleToFloat(ui->startKPSP->text()),
                              _strDoubleToFloat(ui->startKEF->text()) });
    QList<QLabel *> labels({ ui->lb_ktr, ui->lb_ksc, ui->lb_kgdp, ui->lb_kpsp, ui->lb_kef });
    for (int i = 0; i < startKeffs.size(); i++) {
        if (keffs[i].toFloat() > startKeffs[i].toFloat()) {
            labels[i]->setPalette(greenPal);
        } else if (keffs[i].toFloat() < startKeffs[i].toFloat()) {
            labels[i]->setPalette(redPal);
        } else {
            labels[i]->setPalette(blackPal);
        }
    }
}

void MainWindow::_prepareSettingsInRight(QString fragAkt, QString fragRazdel, QString fragQuality,
                                         QString fragQuestionABR)
{
    ui->Act->clear();
    ui->Razd->blockSignals(true);
    ui->Razd->clear();
    ui->Razd->blockSignals(false);
    ui->Quality->clear();
    ui->Question->clear();

    ui->Act->addItems(ListAct);
    ui->Razd->addItems(ListRazd); //От этого сработает on_Razd_currentItemChanged и вопросы добавятся сами
    ui->Quality->addItems(ListQuality);
    for (int i = 0; i < ListAct.size(); i++) {
        if (fragAkt == AbbreviationAct[i]) {
            ui->Act->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListQuality.size(); i++) {
        if (fragQuality == AbbreviationQuality[i]) {
            ui->Quality->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListRazd.size(); i++) {
        if (fragRazdel == AbbreviationRazd[i]) {
            ui->Razd->setCurrentIndex(i);
            for (int j = 0; j < ABRQuestionsAtRazdel[i].size(); j++) {
                if (fragQuestionABR == ABRQuestionsAtRazdel[i][j]) {
                    ui->Question->setCurrentIndex(j);
                }
            }
            break;
        }
    }
}

void MainWindow::on_te_textCenter_cursorPositionChanged()
{
    if (!TextCenterIsBlocked) {
        setWorkMode(eItemSelectedMode);

        if (SelectedFragment != -1) {
            _clearSelectionInCentral(currentKolDog->fragments[SelectedFragment]->getPositionFirst(),
                                     currentKolDog->fragments[SelectedFragment]->getPositionLast());
        }
        //Поиск id выделенного фрагмента
        qint32 cursorPos = ui->te_textCenter->textCursor().position();
        for (int i = 0; i < currentKolDog->fragments.count(); i++) {
            if (currentKolDog->fragments[i]->getPositionFirst() <= cursorPos
                && currentKolDog->fragments[i]->getPositionLast() > cursorPos) {
                SelectedFragment = i;
                break;
            }
        }
        if (SelectedFragment == -1) {
            qDebug() << "Сейчас всё навернётся";
        }
        qDebug() << "ID " + QString::number(SelectedFragment)
                        + " Начало: " + QString::number(currentKolDog->fragments[SelectedFragment]->getPositionFirst())
                        + " Конец: " + QString::number(currentKolDog->fragments[SelectedFragment]->getPositionLast());
        _setSelectionInCentral(currentKolDog->fragments[SelectedFragment]->getPositionFirst(),
                               currentKolDog->fragments[SelectedFragment]->getPositionLast());
    }
}

void MainWindow::on_pb_newFrag_clicked()
{
    if (m_currentWorkMode == eBasicMode) {
        m_addFirst =
                _showQuestion("Если вы хотите добавить пункт на первую позицию - нажмите \"Добавить первым\".\n"
                              "Если нет, то нажмите \"Отмена\", выберите пункт, после которого добавится новый пункт.",
                              "Master KDA - Добавление нового пункта", "Добавить первым", "Отмена");
    }
    if (m_currentWorkMode != eBasicMode || m_addFirst) {
        setWorkMode(eRightFrameMode);
        ui->Act->addItems(ListAct);
        ui->Razd->addItems(ListRazd);
        ui->Quality->addItems(ListQuality);
        TextCenterIsBlocked = true;
        m_addNewFrag = true;
    }
}

void MainWindow::on_pb_deleteFrag_clicked()
{
    if (SelectedFragment == -1)
        return;
    _deleteSelectedFrag();
}

void MainWindow::on_GoRight_clicked()
{
    setWorkMode(eRightFrameMode);

    ui->TextRight->clear();
    ui->TextRight->setText(currentKolDog->fragments[SelectedFragment]->getText());
    //Заполнение параметров
    _prepareSettingsInRight(currentKolDog->fragments[SelectedFragment]->getAkt(),
                            currentKolDog->fragments[SelectedFragment]->getRazdel(),
                            currentKolDog->fragments[SelectedFragment]->getKachestvo(),
                            currentKolDog->fragments[SelectedFragment]->getVoprosABR());

    TextCenterIsBlocked = true;
}

void MainWindow::on_GoLeft_clicked()
{
    TextCenterIsBlocked = true;
    QTextCursor cursor(ui->te_textCenter->document());

    if (m_addNewFrag) {
        //Подготовка позиции для размещения фрагмента на первое место или любое другое
        //! Первая позиция предыдущего фрагмента
        qint32 posPrevFragFirst;
        //! Последняя позиция предыдущего фрагмента
        qint32 posPrevFragLast;
        //! ID предыдущего фрагмента, после которого добавляем
        qint32 idPrevFrag;
        if (m_addFirst) {
            posPrevFragFirst = 0;
            posPrevFragLast = 0;
            idPrevFrag = -1;
        } else {
            posPrevFragFirst = currentKolDog->fragments[SelectedFragment]->getPositionFirst();
            posPrevFragLast = currentKolDog->fragments[SelectedFragment]->getPositionLast();
            idPrevFrag = SelectedFragment;
        }
        //Создание нового фрагмента
        cursor.setPosition(posPrevFragLast, QTextCursor::MoveAnchor);
        fragment *frag = new fragment();
        frag->setChanged(true);
        frag->setRazdel(AbbreviationRazd[ui->Razd->currentIndex()]);
        frag->setVoprosABR(ABRQuestionsAtRazdel[ui->Razd->currentIndex()][ui->Question->currentIndex()]);
        frag->setText(ui->TextRight->toPlainText());
        frag->setKachestvo(AbbreviationQuality[ui->Quality->currentIndex()]);
        frag->setAkt(AbbreviationAct[ui->Act->currentIndex()]);
        ArgLine = "\n" + frag->getRazdel() + "\t" + frag->getVoprosABR() + "\t" + frag->getAkt() + "\t"
                + frag->getKachestvo();
        frag->setSize(ArgLine.size() + frag->getText().size() + 2);
        cursor.insertText(frag->getText() + ArgLine + "\n\n");
        frag->setPositionFirst(posPrevFragLast);
        frag->setPositionLast(frag->getPositionFirst() + frag->getSize());
        //Убираем выделение старого и нового фрагмента
        _clearSelectionInCentral(posPrevFragFirst, frag->getPositionLast());
        //
        if (m_addFirst) {
            currentKolDog->addFragOnFirstPos(frag);
        } else {
            currentKolDog->addFragAfter(idPrevFrag, frag);
        }
        qint32 idFragAfterInserted = idPrevFrag + 2;
        _recountPositions(idFragAfterInserted, frag->getSize(), true);
        //Спуск флагов
        if (m_addFirst)
            m_addFirst = false;
        m_addNewFrag = false;
    } else {
        //Изменение старого фрагмента
        //Обновление текста и параметров в памяти
        currentKolDog->fragments[SelectedFragment]->setText(ui->TextRight->toPlainText());
        currentKolDog->fragments[SelectedFragment]->setRazdel(AbbreviationRazd[ui->Razd->currentIndex()]);
        currentKolDog->fragments[SelectedFragment]->setVoprosABR(
                ABRQuestionsAtRazdel[ui->Razd->currentIndex()][ui->Question->currentIndex()]);
        currentKolDog->fragments[SelectedFragment]->setAkt(AbbreviationAct[ui->Act->currentIndex()]);
        currentKolDog->fragments[SelectedFragment]->setKachestvo(AbbreviationQuality[ui->Quality->currentIndex()]);
        //Обновление данных в центральном поле
        cursor.setPosition(currentKolDog->fragments[SelectedFragment]->getPositionFirst(), QTextCursor::MoveAnchor);
        cursor.setPosition(currentKolDog->fragments[SelectedFragment]->getPositionLast(), QTextCursor::KeepAnchor);
        ArgLine.clear();
        if (ui->TextRight->toPlainText().right(1) != '\n') {
            ArgLine += "\n";
        }
        ArgLine += currentKolDog->fragments[SelectedFragment]->getRazdel() + "\t"
                + currentKolDog->fragments[SelectedFragment]->getVoprosABR() + "\t"
                + currentKolDog->fragments[SelectedFragment]->getAkt() + "\t"
                + currentKolDog->fragments[SelectedFragment]->getKachestvo();
        cursor.insertText(currentKolDog->fragments[SelectedFragment]->getText() + ArgLine + "\n\n");
        //Вычисление нового размера
        qint32 delta = ui->TextRight->toPlainText().size() + ArgLine.size() + 2
                - currentKolDog->fragments[SelectedFragment]->getSize();
        if (delta != 0) {
            _recountPositions(SelectedFragment, delta);
        }
        currentKolDog->fragments[SelectedFragment]->Resize();
        //Узнаём изменился ли фрагмент
        if (delta != 0) {
            currentKolDog->fragments[SelectedFragment]->setChanged(true);
        } else {
            //! TODO: [later] Подумать сохранять ли текст при переносе вправо, чтобы потом сверить или нет.
            /*if (ui->TextRight->toPlainText() != currentKolDog->fragments[SelectedFragment]->getText()) {
                currentKolDog->fragments[SelectedFragment]->setChanged(true);
            }*/
        }
        //Убираем выделение изменённого фрагмента
        _clearSelectionInCentral(currentKolDog->fragments[SelectedFragment]->getPositionFirst(),
                                 currentKolDog->fragments[SelectedFragment]->getPositionLast());
    }
    //Удаление данных
    ui->TextRight->clear();
    TextCenterIsBlocked = false;

    ui->Act->clear();
    ui->Razd->blockSignals(true);
    ui->Razd->clear();
    ui->Razd->blockSignals(false);
    ui->Quality->clear();
    ui->Question->clear();

    setWorkMode(eBasicMode);
    //! TODO: сделать перерасчёт кэффов
}

void MainWindow::on_Razd_currentIndexChanged(int index)
{
    ui->Question->clear();
    ui->Question->addItems(QuestionsAtRazdel[index]);
    ui->Question->setCurrentIndex(0);
}

void MainWindow::on_Effekt_po_razd_clicked()
{
    kef *kefDialog = new kef();
    connect(this, &MainWindow::s_sentKefs, kefDialog, &kef::getKefs);
    emit s_sentKefs(currentKolDog->getKef(), currentKolDog->getZnachimost(), currentKolDog->getKdog(),
                    currentKolDog->getKrv(), currentKolDog->getKzp(), currentKolDog->getKvo(), currentKolDog->getKot(),
                    currentKolDog->getKots(), currentKolDog->getKtsp(), currentKolDog->getKmol());
    kefDialog->setModal(true);
    kefDialog->exec();
}

void MainWindow::on_BazeKnowledge_clicked()
{
    if (TextCenterIsBlocked) {
        emit s_sentFragment(currentKolDog->fragments[SelectedFragment]);
    }
    kBase->setModal(true);
    kBase->exec();
}

void MainWindow::on_pb_clearField_clicked()
{
    ui->TextRight->clear();
}

// TODO: [later] в disabled режиме у навигатора цвет выбранных раньше отличается от остальных
void MainWindow::on_tw_navigator_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    if (ui->tw_navigator->property(PREVIOUS_SELECTION).toInt() != -1) {
        ui->tw_navigator->item(ui->tw_navigator->property(PREVIOUS_SELECTION).toInt(), 0)
                ->setBackgroundColor(Qt::white);
    }
    ui->tw_navigator->item(row, 0)->setBackgroundColor(Qt::yellow);
    ui->tw_navigator->setProperty(PREVIOUS_SELECTION, row);
    _fillCentralField(EDisplayedSection(row));
    ui->btn_showFullText->setEnabled(true);
    m_navigatorButtonEnabled = true;
}

void MainWindow::on_btn_showFullText_clicked()
{
    if (ui->tw_navigator->property(PREVIOUS_SELECTION).toInt() != -1) {
        ui->tw_navigator->item(ui->tw_navigator->property(PREVIOUS_SELECTION).toInt(), 0)
                ->setBackgroundColor(Qt::white);
    }
    ui->tw_navigator->setProperty(PREVIOUS_SELECTION, -1);
    _fillCentralField(eAllSections);
    ui->btn_showFullText->setEnabled(false);
    m_navigatorButtonEnabled = false;
}

void MainWindow::on_actionSave_triggered()
{
    // TODO: [later] [1] сделать автоматическое сохранение файла по указанному пути
    // QString pathName = QFileDialog::getSaveFileName();

    // Инициализация переменных для настройки документа
    QAxObject *word = new QAxObject("Word.Application");
    QAxObject *documents = word->querySubObject("Documents");
    QAxObject *document = documents->querySubObject("Add()");
    QAxObject *activeDocument = word->querySubObject("ActiveDocument()");
    QAxObject *rangeHead = activeDocument->querySubObject("Range()");
    QAxObject *range = activeDocument->querySubObject("Range()");
    QAxObject *paragraphs = document->querySubObject("Paragraphs");
    QAxObject *paragraph = paragraphs->querySubObject("Item(int)", 1);

    //Подготовка макета страниц
    word->setProperty("Visible", true);
    //Разметка открытого документа
    paragraph->setProperty("SpaceAfter", 0);
    QAxObject *Font = rangeHead->querySubObject("Font");
    Font->setProperty("Size", 14);
    Font->setProperty("Name", "Times New Roman");
    // Вставка данных
    QString output = "";
    for (auto frag : currentKolDog->fragments) {
        output += frag->getText() + '\n';
    }
    range->dynamicCall("SetRange(int, int)", 0, 1);
    range->setProperty("Text", output);
}

void MainWindow::on_actionStartAnotherKD_triggered()
{
    //Очистка предыдущих настроек
    delete currentKolDog;
    delete m_db;
    delete lDialog;
    lDialog = new ListKD();
    //Новый старт
    lDialog->setModal(true);
    lDialog->exec();
    if (!lDialog->WantGo) {
        exit(3);
    } else {
        //Подготовка договора и mw
        _prepareMainWindow(lDialog->SelectedKD);
    }
}

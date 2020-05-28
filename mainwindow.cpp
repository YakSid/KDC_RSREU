#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAxObject>
#include "cdatabasemanager.h"

const char PREVIOUS_SELECTION[] = "previousSelection";
const QStringList AbbreviationTreeHead = { "ПСП", "ДОГ", "РВ", "ВО", "ГДП", "ЗП", "ОТ", "ТСП", "СЦ", "ТОК", "ПР" };

//! Вопросы:
//! Эффективность и Кэф это одно и то же? Почему в БД два разных поля?
//! Можно ли не выбрав фрагмент зайти в базу знаний? (Можно ради добавления нового пункта?)
//! Клавиша "Очистить поле" очищает параметры фрагмента или только текстовое поле?

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

// TODO: [9] СЕЙЧАС сделать метод вставки из БЗ в окно редактирования
void MainWindow::insertFragFromKB(fragment *frag)
{
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
    //! 2. Заполнить поля из frag
    //

    //Подготовка к вставке нового пункта
    m_document = ui->TextRight->document();
    QTextCursor cursor(m_document);
    ui->Act->addItems(ListAct);
    ui->Razd->addItems(ListRazd);
    ui->Quality->addItems(ListQuality);
    _setUpQuestion();
    ui->Question->addItems(ListVopros);
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
                if (in3_query.value(2) == AbbreviationTreeHead[i]) {
                    frag->setRazdel(in3_query.value(2).toString());
                    frag->setVoprosABR(in3_query.value(1).toString());
                    break;
                }
        }
        frag->SetArguments(in2_query.value(2).toString(), in2_query.value(4).toString(), in2_query.value(5).toString());
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
    m_document = ui->te_textCenter->document();
    QTextCursor cursor(m_document);

    if (selectedSection == eAllSections) {
        for (auto fragment : currentKolDog->fragments) {
            _addFragmentToCentralField(fragment, cursor);
        }
    } else {
        for (auto fragment : currentKolDog->fragments) {
            if (fragment->getRazdel() == AbbreviationTreeHead[selectedSection]) {
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

void MainWindow::_setUpQuestion()
{
    if (ui->Razd->currentIndex() == 0) {
        ListVopros.append({ "Стороны и их полномочия", "Разрешение споров", "Изменение, заключение и контроль КД",
                            "Ответственность сторон", "Сроки и действие КД", "Консультация, переговоры сторон",
                            "Цель заключения, правовые основы", "Информирование и знакомство с КД",
                            "Обязательства сторон", "Другие вопросы", "Определения, содержание КД" });
        AbbreviationVopros.append({ "ПЛН", "РСП", "ИЗК", "ОТВ", "СДД", "ПЕГ", "ЦЕЗ", "ИНФ", "ОБЯ", "ДРГ", "ОПР" });
    }
    if (ui->Razd->currentIndex() == 1) {
        ListVopros.append({ "Заключение ТД", "Срок ТД", "Изменение ТД", "Повышение квалификации", "Занятость",
                            "Увольнение", "Сокращение", "Гарантия, компенсации", "Обязательства сторон" });
        AbbreviationVopros.append({ "ЗТД", "СТД", "ИТД", "ПК", "ЗАН", "УВ", "СОК", "ГАР", "ОБЯ" });
    }
    if (ui->Razd->currentIndex() == 2) {
        ListVopros.append({ "Продолжительность РВ", "Режим работы", "Сверхурочные", "Нормирование труда", "Дисциплина",
                            "Гарантии, компенсации", "Обязательства сторон" });
        AbbreviationVopros.append({ "ПР", "РЕЖ", "СВХ", "НОР", "ДИС", "ГАР", "ОБЯ" });
    }
    if (ui->Razd->currentIndex() == 3) {
        ListVopros.append({ "Перерыв в течение рабочего дня", "Выходные дни", "Ежегодный отпуск",
                            "Дополнительный отпуск", "Гарантии, компенсации", "Обязательства сторон" });
        AbbreviationVopros.append({ "ПЕР", "ВЫХ", "ОТП", "ДОВ", "ГАР", "ОБЯ" });
    }
    if (ui->Razd->currentIndex() == 4) {
        ListVopros.append({ "Права профкома, профсоюза", "Условия предоставляемые для деятельности",
                            "Не члены профсоюза", "Льготы неосвобожденному профактиву",
                            "Льготы освобожденному профактиву", "Согласования", "Льготы членам профсоюза",
                            "Обязательства сторон" });
        AbbreviationVopros.append({ "ПКМ", "УСЛ", "НЧП", "ЛГН", "ЛГО", "СГЛ", "ЛЧП", "ОБГ" });
    }
    if (ui->Razd->currentIndex() == 5) {
        ListVopros.append({ "Система оплаты", "Надбавки", "Доплаты за не нормальные условия", "Сроки выплат",
                            "Индексация", "Внебюджетные средства", "Премии", "Гарантии, компенсации",
                            "Обязательства сторон" });
        AbbreviationVopros.append({ "СИС", "НАД", "ДОП", "СРК", "ИНД", "ВНБ", "ПРМ", "ГАР", "ОБЯ" });
    }
    if (ui->Razd->currentIndex() == 6) {
        ListVopros.append({ "Средства на ОТ", "Соглашение по ОТ", "Мероприятия по улучшению ОТ",
                            "Аттестация рабочих мест", "Обеспечение средствами защиты",
                            "Страхование и проф. обследование", "Контроль за ОТ", "Гарантии, компенсации",
                            "Обязательства сторон", "Ответственность сторон" });
        AbbreviationVopros.append({ "СУМ", "СОГ", "МЕР", "АТТ", "ИСЗ", "ПРФ", "КОН", "ГАР", "ОБЯ", "ОТВ" });
    }
    if (ui->Razd->currentIndex() == 7) {
        ListVopros.append({ "Обращения, жалобы", "Индивидуальные", "Коллективные", "Комиссия по труд. спорам" });
        AbbreviationVopros.append({ "ЖЛБ", "ИНД", "КОЛ", "КОМ" });
    }
    if (ui->Razd->currentIndex() == 8) {
        ListVopros.append({ "Социальное страхование", "Медиц. обслуживание", "Жилищные вопросы",
                            "Организация отдыха работников", "Услуги (бытовые, спорт, культура и пр.)",
                            "Контроль в соцсфере", "Материальная помощь, социальная защита",
                            "Мероприятия и средства" });
        AbbreviationVopros.append({ "СТР", "МЕД", "ЖИЛ", "ОТД", "УСЛ", "КОН", "МАТ", "МСР" });
    }
    if (ui->Razd->currentIndex() == 9) {
        ListVopros.append(
                { "Совместители", "Руководители", "Женщины, семейные", "Молодые, учащиеся", "Гарантии, компенсации" });
        AbbreviationVopros.append({ "СОВ", "РУК", "ЖЕН", "МОЛ", "ГАР" });
    }
    if (ui->Razd->currentIndex() == 10) {
        ListVopros.append({ "Производственно-экономические", "Внебюджетная деятельность", "Обязательства сторон" });
        AbbreviationVopros.append({ "ПРЭ", "ВБД", "ОБЯ" });
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
    m_document = ui->te_textCenter->document();
    QTextCursor cursor(m_document);
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    //Выделить
    cursor.setPosition(posStart, QTextCursor::MoveAnchor);
    cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
}

void MainWindow::_clearSelectionInCentral(qint32 posStart, qint32 posEnd)
{
    // TODO: [min] сделать чтобы m_document и cursor один раз для всез инициализировался, а не постоянно
    m_document = ui->te_textCenter->document();
    QTextCursor cursor(m_document);
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    //Убираем выделение
    cursor.setPosition(posStart, QTextCursor::MoveAnchor);
    cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
}

void MainWindow::_clearSelectionInCentral()
{
    QTextCursor cursor(m_document);
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
        m_document = ui->TextRight->document();
        QTextCursor cursor(m_document);
        ui->Act->addItems(ListAct);
        ui->Razd->addItems(ListRazd);
        ui->Quality->addItems(ListQuality);
        _setUpQuestion();
        ui->Question->addItems(ListVopros);
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
    m_document = ui->TextRight->document();
    QTextCursor cursor(m_document);
    TextCenterIsBlocked = true;
    //Вставка текста в правое окно
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(currentKolDog->fragments[SelectedFragment]->getText());
    //Заполнение данных
    TextCenterIsBlocked = false;
    ui->Act->addItems(ListAct);
    ui->Razd->addItems(ListRazd);
    ui->Quality->addItems(ListQuality);
    for (int i = 0; i < ListAct.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->getAkt() == AbbreviationAct[i]) {
            ui->Act->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListRazd.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->getRazdel() == AbbreviationRazd[i]) {
            ui->Razd->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListQuality.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->getKachestvo() == AbbreviationQuality[i]) {
            ui->Quality->setCurrentIndex(i);
            break;
        }
    }
    _setUpQuestion();
    ui->Question->addItems(ListVopros);
    for (int i = 0; i < ListVopros.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->getVoprosABR() == AbbreviationVopros[i]) {
            ui->Question->setCurrentIndex(i);
            break;
        }
    }
    TextCenterIsBlocked = true;
}

void MainWindow::on_GoLeft_clicked()
{
    TextCenterIsBlocked = true;
    m_document = ui->te_textCenter->document();
    QTextCursor cursor(m_document);
    QString tmp = ui->TextRight->toPlainText();

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
        frag->setVoprosABR(AbbreviationVopros[ui->Question->currentIndex()]);
        frag->SetArguments(tmp, AbbreviationQuality[ui->Quality->currentIndex()],
                           AbbreviationAct[ui->Act->currentIndex()]);
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
        currentKolDog->fragments[SelectedFragment]->setText(tmp);
        cursor.setPosition(currentKolDog->fragments[SelectedFragment]->getPositionFirst(), QTextCursor::MoveAnchor);
        cursor.setPosition(currentKolDog->fragments[SelectedFragment]->getPositionLast(), QTextCursor::KeepAnchor);
        ArgLine.clear();
        if (currentKolDog->fragments[SelectedFragment]->getText().right(1) != '\n') {
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
            currentKolDog->fragments[SelectedFragment]->setChanged(true);
            _recountPositions(SelectedFragment, delta);
        }
        currentKolDog->fragments[SelectedFragment]->Resize();
        //Убираем выделение изменённого фрагмента
        _clearSelectionInCentral(currentKolDog->fragments[SelectedFragment]->getPositionFirst(),
                                 currentKolDog->fragments[SelectedFragment]->getPositionLast());
    }
    //Удаление данных
    ui->TextRight->clear();
    //Удаление данных
    TextCenterIsBlocked = false;
    ui->Act->clear();
    ui->Razd->clear();
    ui->Quality->clear();
    ui->Question->clear();
    ListVopros.clear();
    AbbreviationVopros.clear();

    setWorkMode(eBasicMode);
    //! TODO: сделать перерасчёт кэффов
}

void MainWindow::on_Razd_currentIndexChanged(int index)
{
    // BUG: [5] При смене раздела неправильно меняется вопрос, только вручную норм (и изменение параметров пункта
    // неправильное)
    //Вообще переделать этот момент: должны же данные меняться по нажатию <<, а не с каждым изменением
    //Проверить при условиях: добавление фрагмента на первую позицию
    if (TextCenterIsBlocked) {
        if (!m_addNewFrag) {
            currentKolDog->fragments[SelectedFragment]->setRazdel(AbbreviationRazd[index]);
            QuestionNotSelected = true;
            ui->Question->clear();
            ListVopros.clear();
            AbbreviationVopros.clear();
            _setUpQuestion();
            ui->Question->addItems(ListVopros);
            QuestionNotSelected = false;
        }
    }
}

void MainWindow::on_Question_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked) {
        if (!QuestionNotSelected) {
            currentKolDog->fragments[SelectedFragment]->setVoprosABR(AbbreviationVopros[index]);
        }
    }
}

void MainWindow::on_Act_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked) {
        currentKolDog->fragments[SelectedFragment]->setAkt(AbbreviationAct[index]);
    }
}

void MainWindow::on_Quality_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked) {
        currentKolDog->fragments[SelectedFragment]->setKachestvo(AbbreviationQuality[index]);
    }
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

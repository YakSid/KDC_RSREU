#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QAxObject>
#include <QTextDocumentWriter>
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
// TODO: [Улучшение] сделать автосохранение?
// TODO: [Улучшение продакшена] добавить файл ресурсов с мета-данными, как для MayProg. Справку, что не работает без
// word и по-умолч.

// TODO: Повторить вопросы
/* Новые вопросы
 * 1. В таблице "вопросы2" код 6 (СДД ПСП Увольнение) и код 39 (СДД ПСП Гарантии), 64,68 одинаковые сокращения (вопросы2
 * вообще какая-то странная) [dev: если утвердят, то later можно сделать индексацию вопроса не по abr, а по id]
 *
 * 2. Заполнить возможность закона из таблицы ТФрагмент поля КодГрПарам?
 *
 * 3. Не тормозит ли когда-нибудь сильно?
 */

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    sDialog = new StartDialog();
    kefDialog = new kef();
    kBase = new knowledgebase();
    connect(this, &MainWindow::s_sentFragment, kBase, &knowledgebase::getFragment);
    connect(kBase, &knowledgebase::startTransportFrag, this, &MainWindow::insertFragFromKB);

    bool debugMode = false;
    if (!debugMode) {
        sDialog->setModal(true);
        sDialog->exec();
        if (sDialog->startClicked) {
            if (sDialog->StartMode == StartDialog::EStartMode::startNew) {
                lDialog = new ListKD(this, sDialog->dbPath);
                lDialog->setModal(true);
                lDialog->exec();
                if (!lDialog->WantGo) {
                    exit(3);
                } else {
                    ui->setupUi(this);
                    m_db = new CDatabaseManager(sDialog->dbPath);
                    //Подготовка договора и mw
                    _prepareMainWindow(lDialog->SelectedKD);
                }
            } else if (sDialog->StartMode == StartDialog::EStartMode::contibueOld) {
                //Загрузка сохранённого проекта
                ui->setupUi(this);
                m_db = new CDatabaseManager(sDialog->dbPath);
                QJsonDocument jDoc = m_jsonManager->loadJson(sDialog->jFilename);
                _prepareMainWindowFromJson(jDoc);
            } else {
                exit(2);
            }
        } else {
            exit(2);
        }
    } else {
        // DebugMode
        ui->setupUi(this);
        m_db = new CDatabaseManager("");
        _prepareMainWindow("6201");
    }

    this->setStyleSheet("QPushButton:disabled {"
                        "background-color: darkGrey;"
                        "border: 2px solid darkGrey;}");
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
    qint32 prevSelectedFragId = SelectedFragment;
    switch (m_currentWorkMode) {
    case eBasicMode:
        return;
    case eItemSelectedMode:
        _clearSelectionInCentral(currentKolDog->fragments[SelectedFragment]->getPositionFirst(),
                                 currentKolDog->fragments[SelectedFragment]->getPositionLast());
        break;
    case eRightFrameMode:
        if (!ui->TextRight->toPlainText().isEmpty())
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

void MainWindow::setParameters()
{
    for (auto obj : sender()->parent()->children()) {
        if (obj->metaObject()->className() == QLineEdit::staticMetaObject.className()) {
            auto line = static_cast<QLineEdit *>(obj);
            if (obj->objectName() == "lineA") {
                m_paramA = line->text().toDouble();
            } else if (obj->objectName() == "lineB") {
                m_paramB = line->text().toDouble();
            }
        }
    }

    //Перерасчитываем стартовый КЭФ
    double doubleKEF = m_paramB
                    * (m_paramA * static_cast<double>(currentKolDog->getStartKtr())
                       + static_cast<double>(currentKolDog->getStartKsc()))
            + static_cast<double>(currentKolDog->getStartKgdp()) + static_cast<double>(currentKolDog->getStartKpsp());
    QString strKEF = _doubleToFloatString(doubleKEF);
    ui->startKEF->setText(strKEF);

    currentKolDog->setParameters(m_paramA, m_paramB);

    //Заполняем новые значения КЭФ
    _fillCurrentKEFOnly(currentKolDog->getKef());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool needToAsk = false;
    if (lDialog != nullptr) {
        needToAsk = lDialog->WantGo;
    }
    if (needToAsk || sDialog->StartMode != StartDialog::EStartMode::exitApp) {
        bool answer = _showQuestion("Вы сохранили проект и хотите выйти?");
        if (answer) {
            event->accept();
        } else {
            event->ignore();
        }
    }
}

void MainWindow::_prepareMainWindow(QString docId)
{
    //Центрируем окно по центру экрана
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(this);
    QPoint center = rect.center();
    int x = center.x() - (width() / 2);
    int y = center.y() - (height() / 2);
    center.setX(x);
    center.setY(y);
    move(center);
    //Настройка навигатора
    ui->tw_navigator->setColumnWidth(0, 211);
    for (int row = 0; row < ui->tw_navigator->rowCount(); row++) {
        auto item = ui->tw_navigator->item(row, 0);
        item->setBackgroundColor(Qt::white);
    }
    ui->tw_navigator->setProperty(PREVIOUS_SELECTION, -1);
    //Здесь производится заполнение данных
    //Заполнение коэффициентов
    SelectedKD = docId;
    currentKolDog = new CKolDog(m_paramA, m_paramB);
    QSqlQuery in1_query, in2_query, in3_query, in4_query;
    in1_query.prepare("SELECT * FROM Договор WHERE Договор.[#Дог] = :val1");
    in1_query.bindValue(":val1", SelectedKD);
    if (!in1_query.exec()) {
        qDebug() << in1_query.lastError().text();
    }
    if (in1_query.next()) {
        // Расчёт параметров и заполнение полей mw
        ui->startKTR->setText(in1_query.value(7).toString());
        currentKolDog->setStartKtr(in1_query.value(7).toInt());
        ui->startKSC->setText(in1_query.value(16).toString());
        currentKolDog->setStartKsc(in1_query.value(16).toInt());
        ui->startKGDP->setText(in1_query.value(15).toString());
        currentKolDog->setStartKgdp(in1_query.value(15).toInt());
        ui->startKPSP->setText(_doubleToFloatString(in1_query.value(14).toDouble()));
        currentKolDog->setStartKpsp(in1_query.value(14).toFloat());
        double doubleKEF = m_paramB
                        * (m_paramA * static_cast<double>(currentKolDog->getStartKtr())
                           + static_cast<double>(currentKolDog->getStartKsc()))
                + static_cast<double>(currentKolDog->getStartKgdp())
                + static_cast<double>(currentKolDog->getStartKpsp());
        QString strKEF = _doubleToFloatString(doubleKEF);
        ui->startKEF->setText(strKEF);
        currentKolDog->setStartKef(doubleKEF);
        // Заполнение параметров класса договора
        currentKolDog->setKef(doubleKEF);
        currentKolDog->setMainParameters(
                in1_query.value(0).toString(), in1_query.value(2).toDate(), in1_query.value(3).toInt(),
                in1_query.value(4).toBool(), in1_query.value(5).toFloat(), in1_query.value(7).toInt(),
                in1_query.value(14).toFloat(), in1_query.value(15).toInt(), in1_query.value(16).toInt(),
                in1_query.value(20).toDate(), in1_query.value(21).toInt(), in1_query.value(22).toInt(),
                in1_query.value(23).toInt(), in1_query.value(24).toInt(), in1_query.value(25).toInt(),
                in1_query.value(26).toInt(), 0, 0, in1_query.value(27).toInt(), in1_query.value(30).toFloat());
    }
    //Нахождение полного имени учрежд. и запись в шапку
    in4_query.prepare("SELECT ИмяУчреждения FROM ТУчреждение WHERE ТУчреждение.КодУчреждения = :val1");
    in4_query.bindValue(":val1", currentKolDog->getId());
    if (!in4_query.exec()) {
        qDebug() << in4_query.lastError().text();
    }
    if (in4_query.next()) {
        ui->DogName->setText(in4_query.value(0).toString());
        currentKolDog->setName(ui->DogName->text());
    }
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
        frag->updateFlagsViDoSvUt();
        frag->setAkt(in2_query.value(5).toString());
        currentKolDog->fragments.append(frag);
        // NOTE: [Улучшение] Решить проблему увеличения длинны фрагментов (а не подгонять костылями)
    }
    //Высчитывание Кпр и Кток т.к. их нет в базе
    qint32 calculatedKpr = 0, calculatedKtok = 0;
    currentKolDog->calculateKprAndKtok(calculatedKpr, calculatedKtok);
    currentKolDog->setKpr(calculatedKpr);
    currentKolDog->setKtok(calculatedKtok);
    //Заполнение стартовых дополнительных кэффов (сейчас они равны обычным)
    currentKolDog->setStartMinorKeffs(currentKolDog->getKdog(), currentKolDog->getKrv(), currentKolDog->getKvo(),
                                      currentKolDog->getKzp(), currentKolDog->getKot(), currentKolDog->getKtsp(),
                                      currentKolDog->getKpr(), currentKolDog->getKtok());
    currentKolDog->setStartZnachimost(currentKolDog->getZnachimost());
    //Заполнение окна кэф начальными данными
    kefDialog->setStartKeffs(currentKolDog->getKtr(), currentKolDog->getZnachimost(), currentKolDog->getKdog(),
                             currentKolDog->getKrv(), currentKolDog->getKzp(), currentKolDog->getKvo(),
                             currentKolDog->getKot(), currentKolDog->getKpr(), currentKolDog->getKtok(),
                             currentKolDog->getKtsp());
    //Заполнение текущих коэффициентов
    currentKolDog->calculateCurrentKeffs();
    _fillCurrentKeffs(currentKolDog->getFiveCurrentKeffs());
    // Примечание: Окно кэф заполнится текущими значениями по востребованию, чтобы зря время не тратить

    _fillCentralField(eAllSections);
    TextCenterIsBlocked = false;

    ui->te_textCenter->blockSignals(true);
    ui->te_textCenter->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
    ui->te_textCenter->moveCursor(QTextCursor::Start, QTextCursor::KeepAnchor);
    ui->te_textCenter->blockSignals(false);

    //Сокрытие недоделанных элементов
    ui->label_16->setVisible(false);
    ui->QualityWeightSpinBox->setVisible(false);
}

void MainWindow::_prepareMainWindowFromJson(QJsonDocument jDoc)
{
    //Центрируем окно по центру экрана
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(this);
    QPoint center = rect.center();
    int x = center.x() - (width() / 2);
    int y = center.y() - (height() / 2);
    center.setX(x);
    center.setY(y);
    move(center);

    QJsonObject jObjInsideDoc = jDoc.object();
    QJsonObject mainSettings = jObjInsideDoc["mainSettings"].toObject();
    //Заполнение текущего КД из сохранённого json документа
    currentKolDog = new CKolDog(m_paramA, m_paramB);
    currentKolDog->setId(mainSettings["id"].toString());
    currentKolDog->setName(mainSettings["name"].toString());
    QDate date;
    date.fromString(mainSettings["dateStr"].toString(), "dd.MM.yyyy");
    currentKolDog->setDate(date);
    currentKolDog->setValidity(mainSettings["validity"].toInt());
    currentKolDog->setComplWithReq(mainSettings["validity"].toBool());
    currentKolDog->setZnachimost(mainSettings["znachimost"].toDouble());
    currentKolDog->setStartZnachimost(mainSettings["startZnachimost"].toDouble());
    currentKolDog->setStartKtr(mainSettings["startKtr"].toInt());
    currentKolDog->setStartKef(mainSettings["startKef"].toDouble());
    currentKolDog->setStartKpsp(mainSettings["startKpsp"].toDouble());
    currentKolDog->setStartKgdp(mainSettings["startKgdp"].toInt());
    currentKolDog->setStartKsc(mainSettings["startKsc"].toInt());
    currentKolDog->setKtr(mainSettings["ktr"].toInt());
    currentKolDog->setKef(mainSettings["kef"].toDouble());
    currentKolDog->setKpsp(mainSettings["kpsp"].toDouble());
    currentKolDog->setKgdp(mainSettings["kgdp"].toInt());
    currentKolDog->setKsc(mainSettings["ksc"].toInt());
    QDate endDate;
    endDate.fromString(mainSettings["endDateStr"].toString(), "dd.MM.yyyy");
    currentKolDog->setEndDate(endDate);
    currentKolDog->setStartMinorKeffs(mainSettings["startKdog"].toInt(), mainSettings["startKrv"].toInt(),
                                      mainSettings["startKvo"].toInt(), mainSettings["startKzp"].toInt(),
                                      mainSettings["startKot"].toInt(), mainSettings["startKtsp"].toInt(),
                                      mainSettings["startKpr"].toInt(), mainSettings["startKtok"].toInt());
    currentKolDog->setKdog(mainSettings["kdog"].toInt());
    currentKolDog->setKrv(mainSettings["krv"].toInt());
    currentKolDog->setKvo(mainSettings["kvo"].toInt());
    currentKolDog->setKzp(mainSettings["kzp"].toInt());
    currentKolDog->setKot(mainSettings["kot"].toInt());
    currentKolDog->setKtsp(mainSettings["ktsp"].toInt());
    currentKolDog->setKpr(mainSettings["kpr"].toInt());
    currentKolDog->setKtok(mainSettings["ktok"].toInt());
    currentKolDog->setKmol(mainSettings["kmol"].toInt());
    currentKolDog->setSum(mainSettings["sum"].toDouble());
    //Заполнение фрагментов текущего КД из сохранённого json документа
    QJsonArray jArray = jObjInsideDoc["fragments"].toArray();
    for (QJsonValueRef jValueRef : jArray) {
        auto jFrag = jValueRef.toObject();
        auto frag = new fragment();
        frag->setText(jFrag["text"].toString());
        frag->setKachestvo(jFrag["kachestvo"].toString());
        frag->setAkt(jFrag["akt"].toString());
        frag->setVoprosABR(jFrag["voprosABR"].toString());
        frag->setRazdel(jFrag["razdel"].toString());
        frag->setSize(jFrag["size"].toInt());
        frag->setChanged(jFrag["changed"].toBool());
        frag->setNewAdded(jFrag["newAdded"].toBool());
        frag->setVisible(jFrag["visible"].toBool());
        frag->setViDoSv(jFrag["ViDoSv"].toBool());
        frag->setUt(jFrag["Ut"].toBool());
        currentKolDog->fragments.append(frag);
    }
    //Заполнение переменных и настройка mw
    SelectedKD = currentKolDog->getId();
    ui->startKTR->setText(QString::number(currentKolDog->getStartKtr()));
    ui->startKSC->setText(QString::number(currentKolDog->getStartKsc()));
    ui->startKGDP->setText(QString::number(currentKolDog->getStartKgdp()));
    ui->startKPSP->setText(_doubleToFloatString(currentKolDog->getStartKpsp()));
    ui->startKEF->setText(_doubleToFloatString(currentKolDog->getStartKef()));
    ui->DogName->setText(currentKolDog->getName());
    //Настройка навигатора
    ui->tw_navigator->setColumnWidth(0, 211);
    for (int row = 0; row < ui->tw_navigator->rowCount(); row++) {
        auto item = ui->tw_navigator->item(row, 0);
        item->setBackgroundColor(Qt::white);
    }
    ui->tw_navigator->setProperty(PREVIOUS_SELECTION, -1);
    //Заполнение окна кэф начальными данными
    kefDialog->setStartKeffs(currentKolDog->getStartKtr(), currentKolDog->getStartZnachimost(),
                             currentKolDog->getStartKdog(), currentKolDog->getStartKrv(), currentKolDog->getStartKzp(),
                             currentKolDog->getStartKvo(), currentKolDog->getStartKot(), currentKolDog->getStartKpr(),
                             currentKolDog->getStartKtok(), currentKolDog->getStartKtsp());
    //Заполнение текущих коэффициентов
    currentKolDog->calculateCurrentKeffs();
    _fillCurrentKeffs(currentKolDog->getFiveCurrentKeffs());

    _fillCentralField(eAllSections);
    TextCenterIsBlocked = false;

    ui->te_textCenter->blockSignals(true);
    ui->te_textCenter->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
    ui->te_textCenter->moveCursor(QTextCursor::Start, QTextCursor::KeepAnchor);
    ui->te_textCenter->blockSignals(false);

    //Сокрытие недоделанных элементов
    ui->label_16->setVisible(false);
    ui->QualityWeightSpinBox->setVisible(false);
}

void MainWindow::_fillCentralField(EDisplayedSection selectedSection)
{
    TextCenterIsBlocked = true;
    ui->te_textCenter->clear();

    if (selectedSection == eAllSections) {
        for (auto fragment : currentKolDog->fragments) {
            _addFragmentToCentralField(fragment);
        }
    } else {
        for (auto fragment : currentKolDog->fragments) {
            if (fragment->getRazdel() == AbbreviationRazd[selectedSection]) {
                _addFragmentToCentralField(fragment);
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

void MainWindow::_addFragmentToCentralField(fragment *frag)
{
    QTextCursor cursor(ui->te_textCenter->document());
    QTextCharFormat format;
    // format.setForeground(Qt::black);
    // format.setBackground(Qt::white);
    qint32 posBegin, posEnd;
    cursor.movePosition(QTextCursor::End);
    cursor.setCharFormat(format);
    posBegin = cursor.position();
    cursor.insertText(frag->getText());
    if (frag->getText().right(1) != "\n") {
        cursor.insertBlock();
    }
    QString argLine =
            frag->getRazdel() + "\t" + frag->getVoprosABR() + "\t" + frag->getAkt() + "\t" + frag->getKachestvo();
    cursor.insertText(argLine);
    cursor.insertBlock();
    cursor.insertBlock();
    posEnd = cursor.position();
    frag->SetPositions(
            posBegin,
            posEnd); // NOTE: Размер включает в себя: Текст, Размер СтрокиАргументов и 2-3 символа нового параграфа
    //Окраска в цвет если изменён
    if (frag->isChanged())
        _markAsChanged(posBegin, posEnd);
    if (frag->isNewAdded())
        _markAsNewAdded(posBegin, posEnd);
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
    cursor.setCharFormat(format);
}

void MainWindow::_clearSelectionInCentral()
{
    QTextCursor cursor(ui->te_textCenter->document());
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    cursor.setPosition(ui->te_textCenter->toPlainText().length(), QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);
}

void MainWindow::_markAsChanged(qint32 posStart, qint32 posEnd)
{
    QTextCursor cursor(ui->te_textCenter->document());
    cursor.setPosition(posStart, QTextCursor::MoveAnchor);
    cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
    auto format = cursor.charFormat();
    format.setForeground(Qt::darkMagenta);
    format.setBackground(Qt::white);
    cursor.setCharFormat(format);
}

void MainWindow::_markAsNewAdded(qint32 posStart, qint32 posEnd)
{
    QTextCursor cursor(ui->te_textCenter->document());
    cursor.setPosition(posStart, QTextCursor::MoveAnchor);
    cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
    auto format = cursor.charFormat();
    format.setForeground(Qt::black);
    format.setBackground(Qt::yellow);
    cursor.setCharFormat(format);
}

void MainWindow::_markAsChangedAndNewAdded(qint32 posStart, qint32 posEnd)
{
    QTextCursor cursor(ui->te_textCenter->document());
    cursor.setPosition(posStart, QTextCursor::MoveAnchor);
    cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
    auto format = cursor.charFormat();
    format.setForeground(Qt::darkMagenta);
    format.setBackground(Qt::yellow);
    cursor.setCharFormat(format);
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
    //Изменение кэффов в КД
    currentKolDog->setKtr(keffs[0].toInt());
    currentKolDog->setKsc(keffs[1].toInt());
    currentKolDog->setKgdp(keffs[2].toInt());
    currentKolDog->setKpsp(keffs[3].toDouble());
    currentKolDog->setKef(keffs[4].toDouble());
    //Изменение кэффов в mw
    ui->KTR->setText(QString::number(keffs[0].toInt()));
    ui->KSC->setText(QString::number(keffs[1].toInt()));
    ui->KGDP->setText(QString::number(keffs[2].toInt()));
    ui->KPSP->setText(_doubleToFloatString(keffs[3].toDouble()));
    ui->KEF->setText(_doubleToFloatString(keffs[4].toDouble()));
    // Сравнение и раскрашивание
    // NOTE: [Улучшение] потом-потом оптимизировать (типы) (для KEFOnly тоже)
    QPalette greenPal, redPal, blackPal;
    greenPal.setColor(QPalette::WindowText, Qt::darkGreen);
    redPal.setColor(QPalette::WindowText, Qt::darkRed);
    blackPal.setColor(QPalette::WindowText, Qt::black);
    QVariantList startKeffs({ currentKolDog->getStartKtr(), currentKolDog->getStartKsc(), currentKolDog->getStartKgdp(),
                              currentKolDog->getStartKpsp(), currentKolDog->getStartKef() });
    QList<QLabel *> labels({ ui->lb_ktr, ui->lb_ksc, ui->lb_kgdp, ui->lb_kpsp, ui->lb_kef });
    for (int i = 0; i < startKeffs.size(); i++) {

        //Исключение для kpsp и kef т.к. там есть знаки после запятой
        if (i == 3 || i == 4) {
            QString currentKString = QString::number(keffs[i].toDouble(), 'f', 2);
            QString startKString = QString::number(startKeffs[i].toDouble(), 'f', 2);
            double currentK = currentKString.toDouble();
            double startK = startKString.toDouble();
            if (currentK > startK) {
                labels[i]->setPalette(greenPal);
            } else if (currentK < startK) {
                labels[i]->setPalette(redPal);
            } else {
                labels[i]->setPalette(blackPal);
            }

        } else {

            if (keffs[i].toFloat() > startKeffs[i].toFloat()) {
                labels[i]->setPalette(greenPal);
            } else if (keffs[i].toFloat() < startKeffs[i].toFloat()) {
                labels[i]->setPalette(redPal);
            } else {
                labels[i]->setPalette(blackPal);
            }
        }
    }
}

void MainWindow::_fillCurrentKEFOnly(double kef)
{
    ui->KEF->setText(_doubleToFloatString(kef));
    //Сравнение и окраска
    QPalette greenPal, redPal, blackPal;
    greenPal.setColor(QPalette::WindowText, Qt::darkGreen);
    redPal.setColor(QPalette::WindowText, Qt::darkRed);
    blackPal.setColor(QPalette::WindowText, Qt::black);

    QString currentKString = QString::number(kef, 'f', 2);
    QString startKString = ui->startKEF->text();
    double currentK = currentKString.toDouble();
    double startK = startKString.toDouble();
    if (currentK > startK) {
        ui->KEF->setPalette(greenPal);
    } else if (currentK < startK) {
        ui->KEF->setPalette(redPal);
    } else {
        ui->KEF->setPalette(blackPal);
    }
}

QVariantList MainWindow::_calculateKeffsWithDelta(QVariantList delta)
{
    QVariantList result { 0, 0, 0, 0.0, 0.0 };
    result[0] = ui->KTR->text().toInt() + delta[0].toInt();
    result[1] = ui->KSC->text().toInt() + delta[1].toInt();
    result[2] = ui->KGDP->text().toInt() + delta[2].toInt();
    result[3] = static_cast<double>(currentKolDog->getKpsp()) + delta[3].toDouble();
    result[4] = static_cast<double>(currentKolDog->getKef()) + delta[4].toDouble();
    return result;
}

bool MainWindow::_isKeffsChanged(QVariantList delta)
{
    for (auto value : delta) {
        if (value.toInt() != 0)
            return true;
    }
    return false;
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
            // Узнаём сначала какой цвет был у фрагмента и его возвращаем после выделения жирным
            auto curFrag = currentKolDog->fragments[SelectedFragment];
            auto posFirst = curFrag->getPositionFirst();
            auto posLast = curFrag->getPositionLast();
            if (curFrag->isChanged() && curFrag->isNewAdded()) {
                _markAsChangedAndNewAdded(posFirst, posLast);
            } else if (curFrag->isChanged()) {
                _markAsChanged(posFirst, posLast);
            } else if (curFrag->isNewAdded()) {
                _markAsNewAdded(posFirst, posLast);
            } else {
                _clearSelectionInCentral(posFirst, posLast);
            }
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
            return;
        }
        auto frag = currentKolDog->fragments[SelectedFragment];
        _prepareSettingsInRight(frag->getAkt(), frag->getRazdel(), frag->getKachestvo(), frag->getVoprosABR());
        _setSelectionInCentral(frag->getPositionFirst(), frag->getPositionLast());
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
        TextCenterIsBlocked = true;
        m_addNewFrag = true;
        if (m_addFirst) {
            //Поле справа пустое, заполним его
            auto sectionId = ui->tw_navigator->property(PREVIOUS_SELECTION).toInt();
            if (sectionId == -1 || sectionId == 11) {
                //Отображаются все разделы, заполним первыми значениями
                _prepareSettingsInRight("КЗОТ", "ПСП", "Ан", "ПЛН");
            } else {
                //Выбран фильтр по разделу, заполним его значениями
                _prepareSettingsInRight("КЗОТ", AbbreviationRazd[sectionId], "Ан", ABRQuestionsAtRazdel[sectionId][1]);
            }
        }
    }
}

void MainWindow::on_pb_deleteFrag_clicked()
{
    if (SelectedFragment == -1)
        return;

    //Изменение кэффов //TODO: [улучш.] Убрать кэф из перерасчёта (ещё в создании нового и изменении старого)
    QVariantList deltaKeffs = currentKolDog->fragments[SelectedFragment]->getKeffsDeltaToZero();
    QString razdAbr = currentKolDog->fragments[SelectedFragment]->getAffectsOnMinorKeffs();
    currentKolDog->fragments.removeAt(SelectedFragment);
    QVariantList newKeffs = _calculateKeffsWithDelta(deltaKeffs);
    _fillCurrentKeffs(newKeffs);
    //! КЭФ отдельно пересчитываем т.к. она меняется нелинейно
    _fillCurrentKEFOnly(currentKolDog->calculateKef());
    //Изменение дополнительных кэффов
    if (!razdAbr.isEmpty()) {
        currentKolDog->decrementMinorKeff(razdAbr);
    }

    // TODO: [Улучшение] убрать _fillCentralField по типу функции добавления, чтобы без перемотки вверх было
    if (ui->tw_navigator->property(PREVIOUS_SELECTION).toInt() == -1) {
        _fillCentralField(eAllSections);
    } else {
        _fillCentralField(EDisplayedSection(ui->tw_navigator->property(PREVIOUS_SELECTION).toInt()));
    }
    SelectedFragment = -1;

    currentKolDog->calculateKzn();

    //Внешний вид ui
    ui->Act->clear();
    ui->Razd->blockSignals(true);
    ui->Razd->clear();
    ui->Razd->blockSignals(false);
    ui->Quality->clear();
    ui->Question->clear();
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
    bool cancel = false;
    if (ui->TextRight->toPlainText().isEmpty()) {
        if (m_addNewFrag) {
            cancel = _showQuestion("Текст пункта пустой, отменить добавление?", "Master KDA", "Да, отменить",
                                   "Нет, добавить пустой");
        } else {
            _showMessage("Текст пункта пустой, заполните его перед добавлением");
            return;
        }
    }

    TextCenterIsBlocked = true;
    QTextCursor cursor(ui->te_textCenter->document());

    if (m_addNewFrag && !cancel) {
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
            //Новый режим с добавлением в начало раздела, а не документа
            auto sectionId = ui->tw_navigator->property(PREVIOUS_SELECTION).toInt();
            if (sectionId != -1) {
                auto section = EDisplayedSection(sectionId);
                if (section != eAllSections) {
                    auto firstInRazdFragId = currentKolDog->findFirstInRazd(AbbreviationRazd[section]) - 1;
                    if (firstInRazdFragId > -1) {
                        SelectedFragment = firstInRazdFragId;
                        posPrevFragFirst = currentKolDog->fragments[SelectedFragment]->getPositionFirst();
                        posPrevFragLast = currentKolDog->fragments[SelectedFragment]->getPositionLast();
                        idPrevFrag = firstInRazdFragId;
                    } else {
                        bool insertLast = false;
                        //Раздел пустой, ищем положение по фрагментам следующих разделов
                        while (firstInRazdFragId < 0) {
                            sectionId++;
                            if (sectionId == 11) {
                                //Разделов позднее нет - можно ставить фрагмент последним
                                insertLast = true;
                                break;
                            } else {
                                section = EDisplayedSection(sectionId);
                                firstInRazdFragId = currentKolDog->findFirstInRazd(AbbreviationRazd[section]) - 1;
                            }
                        }
                        //Вставляем последним
                        if (insertLast) {
                            if (currentKolDog->fragments.isEmpty()) {
                                //Если документ пустой
                                posPrevFragFirst = 0;
                                posPrevFragLast = 0;
                                idPrevFrag = -1;
                            } else {
                                //Если есть фрагменты выше
                                idPrevFrag = currentKolDog->fragments.count() - 1;
                                posPrevFragFirst = 0;
                                posPrevFragLast = 0;
                                SelectedFragment = idPrevFrag;
                            }
                        } else {
                            //Вставляем перед этим фрагментом
                            SelectedFragment = firstInRazdFragId;
                            posPrevFragFirst = currentKolDog->fragments[SelectedFragment]->getPositionFirst();
                            posPrevFragLast = currentKolDog->fragments[SelectedFragment]->getPositionLast();
                            idPrevFrag = firstInRazdFragId;
                        }
                    }
                }
            }
        } else {
            posPrevFragFirst = currentKolDog->fragments[SelectedFragment]->getPositionFirst();
            posPrevFragLast = currentKolDog->fragments[SelectedFragment]->getPositionLast();
            idPrevFrag = SelectedFragment;
        }
        //Создание нового фрагмента
        cursor.setPosition(posPrevFragLast, QTextCursor::MoveAnchor);
        fragment *frag = new fragment();
        frag->setNewAdded(true);
        frag->setRazdel(AbbreviationRazd[ui->Razd->currentIndex()]);
        frag->setVoprosABR(ABRQuestionsAtRazdel[ui->Razd->currentIndex()][ui->Question->currentIndex()]);
        frag->setText(ui->TextRight->toPlainText());
        frag->setKachestvo(AbbreviationQuality[ui->Quality->currentIndex()]);
        frag->setAkt(AbbreviationAct[ui->Act->currentIndex()]);
        frag->updateFlagsViDoSvUt();
        QString argLine = "\n" + frag->getRazdel() + "\t" + frag->getVoprosABR() + "\t" + frag->getAkt() + "\t"
                + frag->getKachestvo();
        frag->setSize(argLine.size() + frag->getText().size() + 2);
        cursor.insertText(frag->getText() + argLine + "\n\n");
        frag->setPositionFirst(posPrevFragLast);
        frag->setPositionLast(frag->getPositionFirst() + frag->getSize());
        //Изменение главных кэффов
        QVariantList deltaKeffs = frag->getKeffsDeltaFromZero();
        QVariantList newKeffs = _calculateKeffsWithDelta(deltaKeffs);
        _fillCurrentKeffs(newKeffs); // NOTE: КЭФ посчитается  идобавиться после добавления фрага!
        //Изменение минорных кэффов
        QString razdAbr = frag->getAffectsOnMinorKeffs();
        currentKolDog->incrementMinorKeff(razdAbr);
        currentKolDog->calculateKzn();
        //Убираем выделение старого и нового фрагмента, окрашиваем старый если тот был
        _clearSelectionInCentral(posPrevFragFirst, frag->getPositionLast());
        if (idPrevFrag != -1) {
            if (currentKolDog->fragments[SelectedFragment]->isChanged()
                && currentKolDog->fragments[SelectedFragment]->isNewAdded()) {
                _markAsChangedAndNewAdded(posPrevFragFirst, posPrevFragLast);
            } else if (currentKolDog->fragments[SelectedFragment]->isChanged()) {
                _markAsChanged(posPrevFragFirst, posPrevFragLast);
            } else if (currentKolDog->fragments[SelectedFragment]->isNewAdded()) {
                _markAsNewAdded(posPrevFragFirst, posPrevFragLast);
            }
        }
        //
        if (m_addFirst) {
            // currentKolDog->addFragOnFirstPos(frag);//Новый режим
            currentKolDog->addFragAfter(idPrevFrag, frag);
        } else {
            currentKolDog->addFragAfter(idPrevFrag, frag);
        }
        //Перерасчёт кэф после вставки фрагмента
        auto fiveKeffs = currentKolDog->getFiveCurrentKeffs();
        fiveKeffs[4] = currentKolDog->calculateKef();
        _fillCurrentKeffs(fiveKeffs);
        //
        qint32 idFragAfterInserted = idPrevFrag + 2;
        _recountPositions(idFragAfterInserted, frag->getSize(), true);
        //Окраска в цвет изменённого
        _markAsNewAdded(frag->getPositionFirst(), frag->getPositionLast());
        //Спуск флагов
        if (m_addFirst)
            m_addFirst = false;
        m_addNewFrag = false;
    } else if (!cancel) {
        //Изменение старого фрагмента
        //Обновление текста и параметров в памяти
        auto fragBeforeChange = *currentKolDog->fragments[SelectedFragment];
        auto currentFrag = currentKolDog->fragments[SelectedFragment];
        currentFrag->setText(ui->TextRight->toPlainText());
        currentFrag->setRazdel(AbbreviationRazd[ui->Razd->currentIndex()]);
        currentFrag->setVoprosABR(ABRQuestionsAtRazdel[ui->Razd->currentIndex()][ui->Question->currentIndex()]);
        currentFrag->setAkt(AbbreviationAct[ui->Act->currentIndex()]);
        currentFrag->setKachestvo(AbbreviationQuality[ui->Quality->currentIndex()]);
        currentFrag->updateFlagsViDoSvUt();
        //Обновление данных в центральном поле
        cursor.setPosition(currentFrag->getPositionFirst(), QTextCursor::MoveAnchor);
        cursor.setPosition(currentFrag->getPositionLast(), QTextCursor::KeepAnchor);
        QString argLine;
        if (ui->TextRight->toPlainText().right(1) != "\n") {
            argLine += "\n";
        }
        argLine += currentFrag->getRazdel() + "\t" + currentFrag->getVoprosABR() + "\t" + currentFrag->getAkt() + "\t"
                + currentFrag->getKachestvo();
        cursor.insertText(currentFrag->getText() + argLine + "\n\n");
        //Вычисление нового размера
        qint32 deltaTextSize = ui->TextRight->toPlainText().size() + argLine.size() + 2 - currentFrag->getSize();
        if (deltaTextSize != 0) {
            _recountPositions(SelectedFragment, deltaTextSize);
        }
        currentFrag->Resize();
        //Изменение главных кэффов и узнаём изменился ли фрагмент
        QVariantList deltaKeffs = currentFrag->getKeffsDelta(&fragBeforeChange);
        if (_isKeffsChanged(deltaKeffs)) {
            QVariantList newKeffs = _calculateKeffsWithDelta(deltaKeffs);
            _fillCurrentKeffs(newKeffs); // NOTE: пересчёт КЭФ идёт далее отдельно
            currentFrag->setChanged(true);
        } else {
            if (deltaTextSize != 0)
                currentFrag->setChanged(true);
            //! TODO: [Улучшение кода] Подумать сохранять ли текст при переносе вправо, чтобы потом сверить или нет
        }
        QString razdAbr = currentFrag->getAffectsOnMinorKeffs();
        QString razdAbrBefore = fragBeforeChange.getAffectsOnMinorKeffs();
        if (razdAbr != razdAbrBefore) {
            currentKolDog->decrementMinorKeff(razdAbrBefore);
            currentKolDog->incrementMinorKeff(razdAbr);
            currentKolDog->calculateKzn();
            if (!currentFrag->isChanged())
                currentFrag->setChanged(true);
        }
        //Пересчёт КЭФ
        auto fiveKeffs = currentKolDog->getFiveCurrentKeffs();
        fiveKeffs[4] = currentKolDog->calculateKef();
        _fillCurrentKeffs(fiveKeffs);

        //Убираем выделение изменённого фрагмента
        _clearSelectionInCentral(currentFrag->getPositionFirst(), currentFrag->getPositionLast());
        //Окраска в цвет изменённого
        if (currentFrag->isChanged() && currentFrag->isNewAdded()) {
            _markAsChangedAndNewAdded(currentFrag->getPositionFirst(), currentFrag->getPositionLast());
        } else if (currentFrag->isChanged()) {
            _markAsChanged(currentFrag->getPositionFirst(), currentFrag->getPositionLast());
        } else if (currentFrag->isNewAdded()) {
            _markAsNewAdded(currentFrag->getPositionFirst(), currentFrag->getPositionLast());
        }
    } else {
        //Отмена добавления нового пустого пункта
        if (m_addFirst) {
            m_addFirst = false;
        } else {
            //Окраска выбранного пункта в исходные цвета
            auto curFrag = currentKolDog->fragments[SelectedFragment];
            auto posFirst = curFrag->getPositionFirst();
            auto posLast = curFrag->getPositionLast();
            if (curFrag->isChanged() && curFrag->isNewAdded()) {
                _markAsChangedAndNewAdded(posFirst, posLast);
            } else if (curFrag->isChanged()) {
                _markAsChanged(posFirst, posLast);
            } else if (curFrag->isNewAdded()) {
                _markAsNewAdded(posFirst, posLast);
            } else {
                _clearSelectionInCentral(posFirst, posLast);
            }
        }
        m_addNewFrag = false;
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
    //Заполнение текущих коэффициентов

    setWorkMode(eBasicMode);
}

void MainWindow::on_Razd_currentIndexChanged(int index)
{
    ui->Question->clear();
    ui->Question->addItems(QuestionsAtRazdel[index]);
    ui->Question->setCurrentIndex(0);
}

void MainWindow::on_Effekt_po_razd_clicked()
{
    currentKolDog->calculateKzn();
    kefDialog->setCurrentKeffs(currentKolDog->getKtr(), currentKolDog->getZnachimost(), currentKolDog->getKdog(),
                               currentKolDog->getKrv(), currentKolDog->getKzp(), currentKolDog->getKvo(),
                               currentKolDog->getKot(), currentKolDog->getKpr(), currentKolDog->getKtok(),
                               currentKolDog->getKtsp());
    kefDialog->setModal(true);
    kefDialog->exec();
}

void MainWindow::on_BazeKnowledge_clicked()
{
    switch (m_currentWorkMode) {
    case eBasicMode:
        return;
    case eItemSelectedMode:
        emit s_sentFragment(currentKolDog->fragments[SelectedFragment]);
        break;
    case eRightFrameMode:
        emit s_sentFragment(currentKolDog->fragments[SelectedFragment]);
        break;
    }
    kBase->setModal(true);
    kBase->open();
}

void MainWindow::on_pb_clearField_clicked()
{
    ui->TextRight->clear();
}

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

void MainWindow::on_actionMakeDoc_triggered()
{
    // TODO: [Улучшение] переделать сохранение файла текста по указанному пути
    // QString pathName = QFileDialog::getSaveFileName();
    currentKolDog->setName(ui->DogName->text()); //учесть имя КД

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
    QString argLine;
    for (auto frag : currentKolDog->fragments) {
        argLine = frag->getRazdel() + " \t" + frag->getVoprosABR() + " \t" + frag->getAkt() + " \t"
                + frag->getKachestvo() + "\n\n";
        output += frag->getText() + argLine;
    }
    range->dynamicCall("SetRange(int, int)", 0, 1);
    range->setProperty("Text", output);
}

void MainWindow::on_actionMakeOdt_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить договор"), "", tr("Open Document (*.odt)"));

    if (!fileName.isEmpty()) {
        QTextDocumentWriter *writer = new QTextDocumentWriter(fileName);
        if (writer->write(ui->te_textCenter->document())) {
            _showMessage("Договор успешно сохранён в файл:\n" + fileName);
        } else {
            _showMessage("Произошла ошибка записи, документ не сохранён");
        }
    } else {
        _showMessage("Документ не сохранён, потому что не выбран каталог сохранения");
    }
}

void MainWindow::on_actionStartAnotherKD_triggered()
{
    bool answer = _showQuestion("Вы сохранили проект и хотите выбрать другой?");
    if (!answer) {
        return;
    }
    //Очистка предыдущих настроек
    delete currentKolDog;
    delete m_db;
    if (lDialog != nullptr) {
        delete lDialog;
    }
    lDialog = nullptr;
    delete sDialog;
    ui->TextRight->clear();
    ui->Act->clear();
    ui->Razd->blockSignals(true);
    ui->Razd->clear();
    ui->Razd->blockSignals(false);
    ui->Quality->clear();
    ui->Question->clear();
    setWorkMode(eBasicMode);
    // TODO: [Улучшение] Добавить анимацию загрузки (мб на central widget) тут и в других местах
    ui->centralWidget->setHidden(true);
    //Новый старт
    sDialog = new StartDialog();
    sDialog->setModal(true);
    sDialog->exec();
    if (sDialog->startClicked) {
        if (sDialog->StartMode == StartDialog::EStartMode::startNew) {
            lDialog = new ListKD(this, sDialog->dbPath);
            lDialog->setModal(true);
            lDialog->exec();
            if (!lDialog->WantGo) {
                exit(3);
            } else {
                m_db = new CDatabaseManager(sDialog->dbPath);
                ui->centralWidget->setHidden(false);
                _prepareMainWindow(lDialog->SelectedKD);
            }
        } else if (sDialog->StartMode == StartDialog::EStartMode::contibueOld) {
            //Загрузка сохранённого проекта
            m_db = new CDatabaseManager(sDialog->dbPath);
            QJsonDocument jDoc = m_jsonManager->loadJson(sDialog->jFilename);
            ui->centralWidget->setHidden(false);
            _prepareMainWindowFromJson(jDoc);
        } else {
            exit(2);
        }
    } else {
        exit(2);
    }
}

void MainWindow::on_actionSaveProject_triggered()
{
    QString jName = QFileDialog::getSaveFileName(this, "Сохранить проект", "", "*.json");
    if (!jName.isEmpty()) {
        currentKolDog->setName(ui->DogName->text());
        currentKolDog->calculateKzn();
        auto jDocPtr = currentKolDog->packKolDogToJson();
        m_jsonManager->saveJson(jDocPtr, jName);
    }
}

void MainWindow::on_actionEditFormulas_triggered()
{
    auto dialog = new QDialog(this);
    dialog->setWindowTitle("Master KDA");

    auto mainLayout = new QVBoxLayout(dialog);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    auto lblA = new QLabel("Параметр a:", dialog);
    auto lineA = new QLineEdit(QString::number(m_paramA), dialog);
    lineA->setObjectName("lineA");
    auto lblB = new QLabel("Параметр b:", dialog);
    auto lineB = new QLineEdit(QString::number(m_paramB), dialog);
    lineB->setObjectName("lineB");
    auto topLayout = new QHBoxLayout(dialog);
    topLayout->addWidget(lblA);
    topLayout->addWidget(lineA);
    mainLayout->addLayout(topLayout);
    auto bottomLayout = new QHBoxLayout(dialog);
    bottomLayout->addWidget(lblB);
    bottomLayout->addWidget(lineB);
    mainLayout->addLayout(bottomLayout);
    auto pb_ok = new QPushButton("Принять", dialog);
    connect(pb_ok, &QPushButton::clicked, this, &MainWindow::setParameters);
    connect(pb_ok, &QPushButton::clicked, dialog, &QDialog::close);
    mainLayout->addWidget(pb_ok);
    dialog->setLayout(mainLayout);

    dialog->setModal(true);
    dialog->exec();
    delete dialog;
}

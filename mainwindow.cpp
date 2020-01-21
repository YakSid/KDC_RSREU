#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 * Master KDA
 */
/*#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QMessageBox>*/ //Уже подключены?

const char PREVIOUS_SELECTION[] = "previousSelection";
const QStringList AbbreviationTreeHead = { "ПСП", "ДОГ", "РВ", "ВО", "ГДП", "ЗП", "ОТ", "ТСП", "СЦ", "ТОК", "ПР" };

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //НУЖНО ЕСЛИ ПРОПУСКАЕТСЯ lDIALOG (Продолжается работа)
    /*QString ACC = "DRIVER={Microsoft Access Driver (*.mdb)}; FIL={MS Access}; DBQ=";
    QString tmpPath = QDir::currentPath() + "\\БД МК.mdb";
    while (tmpPath.contains('/'))
        tmpPath.replace(tmpPath.indexOf('/'),1,'\\');
    ACC += tmpPath;
    DatabaseForList = QSqlDatabase::addDatabase("QODBC");
    DatabaseForList.setDatabaseName(ACC);
    if (!Database.open())
    {
        QMessageBox::critical(this, "Master KDA - Error", Database.lastError().text());
        return;
    }*/

    // TODO: Сделать кнопку "Назад / К выбору КД"

    sDialog.setModal(true);
    sDialog.exec();
    if (sDialog.StartMode == 1) {
        lDialog.setModal(true);
        lDialog.exec();
        if (!lDialog.WantGo) {
            exit(3);
        } else {
            SelectedKD = lDialog.SelectedKD;
            ui->setupUi(this);
            //Здесь производится заполнение данных
            //Заполнение коэффициентов
            currentKolDog = new CKolDog();
            QSqlQuery in1_query, in2_query, in3_query;
            in1_query.prepare("SELECT * FROM Договор WHERE Договор.[#Дог] = :val1");
            in1_query.bindValue(":val1", SelectedKD);
            if (!in1_query.exec()) {
                qDebug() << in1_query.lastError().text();
            }
            if (in1_query.next()) {
                // Заполнение параметров класса договора
                currentKolDog->setMainParameters(
                        in1_query.value(0).toString(), in1_query.value(1).toString(), in1_query.value(2).toDate(),
                        in1_query.value(3).toUInt(), in1_query.value(4).toBool(), in1_query.value(5).toFloat(),
                        in1_query.value(6).toInt(), in1_query.value(7).toInt(), in1_query.value(13).toFloat(),
                        in1_query.value(14).toFloat(), in1_query.value(15).toInt(), in1_query.value(16).toInt(),
                        in1_query.value(20).toDate(), in1_query.value(21).toInt(), in1_query.value(22).toInt(),
                        in1_query.value(23).toInt(), in1_query.value(24).toInt(), in1_query.value(25).toInt(),
                        in1_query.value(26).toInt(), in1_query.value(28).toInt(), in1_query.value(30).toFloat());
                ui->DogName->setText(in1_query.value(1).toString());
                ui->startKTR->setText(in1_query.value(7).toString());
                ui->startKSC->setText(in1_query.value(16).toString());
                ui->startKGDP->setText(in1_query.value(15).toString());
                auto tmp = QString::number(in1_query.value(14).toDouble(), 'f', 1);
                tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
                ui->startKPSP->setText(tmp);
                double doubleKEF = 1.3 * (1.5 * ui->startKTR->text().toDouble() + ui->startKSC->text().toDouble())
                        + ui->startKGDP->text().toDouble() + ui->startKPSP->text().toDouble();
                tmp = QString::number(doubleKEF, 'f', 1);
                tmp = tmp.replace(tmp.indexOf('.'), 1, ',');
                ui->startKEF->setText(tmp);
                ui->KTR->setText(ui->startKTR->text());
                ui->KSC->setText(ui->startKSC->text());
                ui->KGDP->setText(ui->startKGDP->text());
                ui->KPSP->setText(ui->startKPSP->text());
                ui->KEF->setText(ui->startKEF->text());
            }
            _prepareView();
            //Запрос на заполнение центрального поля
            int QuestionNum = 0;
            in2_query.prepare("SELECT * FROM Тексты WHERE Тексты.[#Дог] = :val1 ORDER BY Тексты.[#Текст]");
            in2_query.bindValue(":val1", SelectedKD);
            if (!in2_query.exec()) {
                qDebug() << in2_query.lastError().text();
            }
            // int posbegin = 0, posend = 0;
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
                            frag->Razdel = in3_query.value(2).toString();
                            frag->VoprosABR = in3_query.value(1).toString();
                            break;
                        }
                }
                frag->SetArguments(in2_query.value(2).toString(), in2_query.value(4).toString(),
                                   in2_query.value(5).toString());
                currentKolDog->fragments.append(frag);
                // TODO: Решить проблему увеличения длинны фрагментов (а не подгонять костылями)
            }
            _fillCentralField(eAllSections);
            TextCenterIsBlocked = false;
        }
    } else
        exit(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_prepareView()
{
    ui->tw_navigator->setColumnWidth(0, 211);
    ui->tw_navigator->setProperty(PREVIOUS_SELECTION, -1);
}

void MainWindow::_fillCentralField(EDisplayedSection selectedSection)
{
    // FIXME: выделение слетает иногда странно и курсор ставится на небывалую позицию
    ui->te_textCenter->clear();
    QTextDocument *document = ui->te_textCenter->document();
    QTextCursor cursor(document);

    if (selectedSection == eAllSections) {
        for (auto fragment : currentKolDog->fragments) {
            // FIXME: оптимизировать - долго грузит
            _addFragmentToCentralField(fragment, cursor);
        }
    } else {
        for (auto fragment : currentKolDog->fragments) {
            if (fragment->Razdel == AbbreviationTreeHead[selectedSection]) {
                _addFragmentToCentralField(fragment, cursor);
                fragment->isVisible = true;
            } else {
                fragment->SetPositions(0, 0);
                fragment->isVisible = false;
            }
        }
    }
}

void MainWindow::_addFragmentToCentralField(fragment *frag, QTextCursor cursor)
{
    QString argLine;
    qint32 posBegin, posEnd;
    cursor.movePosition(QTextCursor::End);
    posBegin = cursor.position();
    cursor.insertText(frag->text);
    if (frag->text.right(1) != '\n') {
        cursor.insertBlock();
    }
    argLine.clear();
    argLine += frag->Razdel + "\t" + frag->VoprosABR + "\t" + frag->Akt + "\t" + frag->Kachestvo;
    cursor.insertText(argLine);
    cursor.insertBlock();
    cursor.insertBlock();
    posEnd = cursor.position();
    frag->SetPositions(posBegin,
                       posEnd); //Размер включает в себя: Текст, Размер СтрокиАргументов
                                //и 2-3 символа нового параграфа
}

void MainWindow::_recountPositions(int idfrag, int delta)
{
    for (int i = idfrag; i < currentKolDog->fragments.count(); i++) {
        if (i != idfrag)
            currentKolDog->fragments[i]->PositionOfFirst += delta;
        currentKolDog->fragments[i]->PositionOfLast += delta;
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

void MainWindow::on_te_textCenter_cursorPositionChanged()
{
    if (!TextCenterIsBlocked) {
        ui->GoRight->setEnabled(true);
        //Начать работу с текстом
        QTextDocument *document = ui->te_textCenter->document();
        QTextCursor cursor(document);
        QTextCharFormat format;
        //Снять выделение
        if (SelectedFragment != -1) {
            cursor.setPosition(currentKolDog->fragments[SelectedFragment]->PositionOfFirst, QTextCursor::MoveAnchor);
            cursor.setPosition(currentKolDog->fragments[SelectedFragment]->PositionOfLast, QTextCursor::KeepAnchor);
            format.setFontWeight(QFont::Normal);
            cursor.mergeCharFormat(format);
        }
        //
        int CursorPosition = ui->te_textCenter->textCursor().position();
        for (int i = 0; i < currentKolDog->fragments.count(); i++) {
            if (currentKolDog->fragments[i]->PositionOfFirst < CursorPosition
                && currentKolDog->fragments[i]->PositionOfLast > CursorPosition) {
                SelectedFragment = i;
                break;
            }
        }
        //Выделить
        cursor.setPosition(currentKolDog->fragments[SelectedFragment]->PositionOfFirst, QTextCursor::MoveAnchor);
        cursor.setPosition(currentKolDog->fragments[SelectedFragment]->PositionOfLast, QTextCursor::KeepAnchor);
        format.setFontWeight(QFont::Bold);
        cursor.mergeCharFormat(format);
    }
}

void MainWindow::on_DeleteClause_clicked()
{
    //
}

void MainWindow::on_GoRight_clicked()
{
    QTextDocument *document = ui->TextRight->document();
    QTextCursor cursor(document);
    TextCenterIsBlocked = true;
    ui->GoLeft->setEnabled(true);
    ui->GoRight->setDisabled(true);
    //Вставка текста в правое окно
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(currentKolDog->fragments[SelectedFragment]->text);
    OldSizeOfSelectedFragment = currentKolDog->fragments[SelectedFragment]->Size;
    //Заполнение данных
    TextCenterIsBlocked = false;
    ui->Act->addItems(ListAct);
    ui->Razd->addItems(ListRazd);
    ui->Quality->addItems(ListQuality);
    for (int i = 0; i < ListAct.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->Akt == AbbreviationAct[i]) {
            ui->Act->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListRazd.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->Razdel == AbbreviationRazd[i]) {
            ui->Razd->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < ListQuality.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->Kachestvo == AbbreviationQuality[i]) {
            ui->Quality->setCurrentIndex(i);
            break;
        }
    }
    _setUpQuestion();
    ui->Question->addItems(ListVopros);
    for (int i = 0; i < ListVopros.size(); i++) {
        if (currentKolDog->fragments[SelectedFragment]->VoprosABR == AbbreviationVopros[i]) {
            ui->Question->setCurrentIndex(i);
            break;
        }
    }
    TextCenterIsBlocked = true;
}

void MainWindow::on_GoLeft_clicked()
{
    ui->GoRight->setDisabled(true);
    ui->GoLeft->setDisabled(true);
    QTextDocument *document = ui->te_textCenter->document();
    QTextCursor cursor(document);
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    QString tmp = ui->TextRight->toPlainText();
    currentKolDog->fragments[SelectedFragment]->text = tmp;
    cursor.setPosition(currentKolDog->fragments[SelectedFragment]->PositionOfFirst, QTextCursor::MoveAnchor);
    cursor.setPosition(currentKolDog->fragments[SelectedFragment]->PositionOfLast, QTextCursor::KeepAnchor);
    ArgLine.clear();
    if (currentKolDog->fragments[SelectedFragment]->text.right(1) != '\n') {
        ArgLine += "\n";
    }
    ArgLine += currentKolDog->fragments[SelectedFragment]->Razdel + "\t"
            + currentKolDog->fragments[SelectedFragment]->VoprosABR + "\t"
            + currentKolDog->fragments[SelectedFragment]->Akt + "\t"
            + currentKolDog->fragments[SelectedFragment]->Kachestvo;
    cursor.insertText(currentKolDog->fragments[SelectedFragment]->text + ArgLine + "\n\n");
    cursor.mergeCharFormat(format);
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
    //Вычисление нового размера
    _recountPositions(SelectedFragment,
                      ui->TextRight->toPlainText().size() + ArgLine.size() + 2
                              - currentKolDog->fragments[SelectedFragment]->Size);
    currentKolDog->fragments[SelectedFragment]->Resize();
    //Удаление данных
    TextCenterIsBlocked = false;
    ui->TextRight->clear();
    ui->Act->clear();
    ui->Razd->clear();
    ui->Quality->clear();
    ui->Question->clear();
    ListVopros.clear();
    AbbreviationVopros.clear();
}

void MainWindow::on_NewClause_clicked()
{
    //
}

void MainWindow::on_Razd_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked) {
        currentKolDog->fragments[SelectedFragment]->Razdel = AbbreviationRazd[index];
        QuestionNotSelected = true;
        ui->Question->clear();
        ListVopros.clear();
        AbbreviationVopros.clear();
        _setUpQuestion();
        ui->Question->addItems(ListVopros);
        QuestionNotSelected = false;
    }
}

void MainWindow::on_Question_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked) {
        if (!QuestionNotSelected) {
            currentKolDog->fragments[SelectedFragment]->VoprosABR = AbbreviationVopros[index];
        }
    }
}

void MainWindow::on_Act_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked) {
        currentKolDog->fragments[SelectedFragment]->Akt = AbbreviationAct[index];
    }
}

void MainWindow::on_Quality_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked) {
        currentKolDog->fragments[SelectedFragment]->Kachestvo = AbbreviationQuality[index];
    }
}

void MainWindow::on_Effekt_po_razd_clicked()
{
    kef *kefDialog = new kef();
    connect(this, &MainWindow::s_sentKefs, kefDialog, &kef::getKefs);
    emit s_sentKefs(currentKolDog->getKtr(), currentKolDog->getKsc(), currentKolDog->getKgdp(),
                    currentKolDog->getKpsp(), currentKolDog->getKef(), currentKolDog->getZnachimost(),
                    currentKolDog->getKdog(), currentKolDog->getKrv(), currentKolDog->getKzp(), currentKolDog->getKvo(),
                    currentKolDog->getKot(), currentKolDog->getKots(), currentKolDog->getKtsp());
    kefDialog->setModal(true);
    kefDialog->exec();
}

void MainWindow::on_BazeKnowledge_clicked()
{
    knowledgebase *kBase = new knowledgebase();
    connect(this, &MainWindow::s_sentFragment, kBase, &knowledgebase::getFragment);
    if (TextCenterIsBlocked) {
        emit s_sentFragment(currentKolDog->fragments[SelectedFragment]);
    }
    kBase->setModal(true);
    kBase->exec();
}

void MainWindow::on_pb_clearField_clicked()
{
    // TODO: сделать очистку поля редактирования, проверить, работает ли изменение размера
}

void MainWindow::on_TextRight_textChanged()
{
    currentKolDog->fragments[SelectedFragment]->changed = true;
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
}

void MainWindow::on_btn_showFullText_clicked()
{
    if (ui->tw_navigator->property(PREVIOUS_SELECTION).toInt() != -1) {
        ui->tw_navigator->item(ui->tw_navigator->property(PREVIOUS_SELECTION).toInt(), 0)
                ->setBackgroundColor(Qt::white);
    }
    _fillCentralField(eAllSections);
    ui->btn_showFullText->setEnabled(false);
}

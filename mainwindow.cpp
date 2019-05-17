#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 * Master KDA
*/
/*#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QMessageBox>*/ //Уже подключены?

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    sDialog.setModal(true);
    sDialog.exec();
    if(sDialog.StartMode==1)
    {
        lDialog.setModal(true);
        lDialog.exec();
        if (lDialog.SelectedKD=="")
        {
            exit(3);
        }
        else
        {
            SelectedKD = lDialog.SelectedKD;
            ui->setupUi(this);
            //Здесь производится заполнение данных
            //Заполнение коэффициентов
            QSqlQuery in1_query, in2_query, in3_query;
            in1_query.prepare("SELECT * FROM Договор WHERE Договор.[#Дог] = :val1");
            in1_query.bindValue(":val1", SelectedKD);
            if (!in1_query.exec())
            {
                qDebug() << in1_query.lastError().text();
            }
            if (in1_query.next())
            {
                QString tmp;
                ui->DogName->setText(in1_query.value(1).toString());
                ui->startKTR->setText(in1_query.value(7).toString());
                ui->startKSC->setText(in1_query.value(16).toString());
                ui->startKGDP->setText(in1_query.value(15).toString());
                tmp = QString::number(in1_query.value(14).toDouble(),'f',1);
                tmp = tmp.replace(tmp.indexOf('.'),1,',');
                ui->startKPSP->setText(tmp);
                double doubleKEF = 1.3 * (1.5 * ui->startKTR->text().toDouble() + ui->startKSC->text().toDouble()) + ui->startKGDP->text().toDouble() + ui->startKPSP->text().toDouble();
                tmp = QString::number(doubleKEF,'f',1);
                tmp = tmp.replace(tmp.indexOf('.'),1,',');
                ui->startKEF->setText(tmp);

                ui->KTR->setText(ui->startKTR->text());
                ui->KSC->setText(ui->startKSC->text());
                ui->KGDP->setText(ui->startKGDP->text());
                ui->KPSP->setText(ui->startKPSP->text());
                ui->KEF->setText(ui->startKEF->text());
            }
            //Создание дерева
            ui->treeWidgetRazd->setColumnCount(1);
            Tree_currentItem = NULL;
            Tree_currentColumn = 0;
            //Заполнение заголовков дерева
            QString TreeHead[11] = {"1. Социальное партнерство (ПСП)","2. Трудовой договор, занятость (ДОГ)","3. Рабочее время (РВ)",
                                    "4. Время отдыха (ВО)","5. Гарантии деятельности профкома (ГДП)","6. Заработная плата (ЗП)","7. Охрана труда (ОТ)",
                                    "8. Трудовые споры (ТСП)","9. Социально-бытовые вопросы (СЦ)","10. Труд отдельных категорий работников (ТОК)","11. Производственные вопросы (ПР)"};
            QString AbbreviationTreeHead[11] = {"ПСП", "ДОГ", "РВ", "ВО", "ГДП", "ЗП", "ОТ", "ТСП", "СЦ", "ТОК", "ПР"};
            QVector <QTreeWidgetItem*> TreeParentsList;
            for (int i=0; i<11; i++)
            {
                QTreeWidgetItem *newItem = new QTreeWidgetItem(ui->treeWidgetRazd, NULL);
                newItem->setText(Tree_currentColumn, TreeHead[i]);
                TreeParentsList.append(newItem);
            }
            //Запрос на заполнение центрального поля
            int QuestionNum = 0;
            in2_query.prepare("SELECT * FROM Тексты WHERE Тексты.[#Дог] = :val1 ORDER BY Тексты.[#Текст]");
            in2_query.bindValue(":val1", SelectedKD);
            if (!in2_query.exec())
            {
                qDebug() << in2_query.lastError().text();
            }
            int posbegin=0, posend=0;
            while (in2_query.next())
            {
                frag = new fragment();
                //Заполнение данных фрагмента и навигатора
                QuestionNum = in2_query.value(6).toInt();
                in3_query.prepare("SELECT * FROM Вопросы WHERE Вопросы.Код = :val1");
                in3_query.bindValue(":val1", QuestionNum);
                if (!in3_query.exec())
                {
                    qDebug() << in3_query.lastError().text();
                }
                if (in3_query.next())
                {
                    for (int i=0; i<11; i++)
                        if (in3_query.value(2) == AbbreviationTreeHead[i])
                        {
                            frag->Razdel = in3_query.value(2).toString();
                            frag->VoprosABR = in3_query.value(1).toString();
                            Tree_currentItem = TreeParentsList[i];
                            break;
                        }
                    Tree_InsertItem(Tree_currentItem, in3_query.value(3).toString() + " (" + in3_query.value(1).toString() + ")");
                }
                frag->SetArguments(in2_query.value(2).toString(), in2_query.value(4).toString(), in2_query.value(5).toString());
                //Заполнение текста в центральном поле
                QTextDocument *document = ui->TextCenter->document();
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::End);
                posbegin = cursor.position();
                cursor.insertText(in2_query.value(2).toString());
                if (frag->text.right(1) != '\n')
                {
                    cursor.insertBlock();
                }
                ArgLine.clear();
                ArgLine += frag->Razdel + "\t" + frag->VoprosABR + "\t" + frag->Akt + "\t" + frag->Kachestvo;
                cursor.insertText(ArgLine);
                cursor.insertBlock();
                cursor.insertBlock();
                posend = cursor.position();
                frag->SetPositions(posbegin, posend);//Размер включает в себя: Текст, Размер СтрокиАргументов и 2-3 символа нового параграфа
                Fragments.append(frag);
                /*//Вычленение номера фрагмента
                QString Clause = in2_query.value(2).toString();
                int PosIndex=-1;
                for (int i=0; i<Clause.size(); i++)
                {
                    if (Clause[i].isLetter())
                    {
                        PosIndex = i;
                        break;
                    }
                }
                Clause.remove(PosIndex, Clause.length()-PosIndex);
                if (Clause.length()!=0)
                    if (Clause[Clause.length()-1]==" ")
                        Clause.remove(Clause.length()-1, 1);
                qDebug() << Clause;*/
            }
            TextCenterIsBlocked = false;
        }
    }
    else exit(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Tree_InsertItem(QTreeWidgetItem *parent, QString text)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem(parent, 0);
    newItem->setText(Tree_currentColumn, text);
}

void MainWindow::RecountPositions(int idfrag, int delta)
{
    for (int i=idfrag; i<Fragments.count(); i++)
    {
        if (i != idfrag) Fragments[i]->PositionOfFirst += delta;
        Fragments[i]->PositionOfLast += delta;
    }
}

void MainWindow::SetUpVopros()
{
    if (ui->Razd->currentIndex() == 0)
    {
        ListVopros.append({"Стороны и их полномочия", "Разрешение споров", "Изменение, заключение и контроль КД", "Ответственность сторон",
                           "Сроки и действие КД", "Консультация, переговоры сторон", "Цель заключения, правовые основы",
                           "Информирование и знакомство с КД", "Обязательства сторон", "Другие вопросы", "Определения, содержание КД"});
        AbbreviationVopros.append({"ПЛН", "РСП", "ИЗК", "ОТВ", "СДД", "ПЕГ", "ЦЕЗ", "ИНФ", "ОБЯ", "ДРГ", "ОПР"});
    }
    if (ui->Razd->currentIndex() == 1)
    {
        ListVopros.append({"Заключение ТД", "Срок ТД", "Изменение ТД", "Повышение квалификации", "Занятость", "Увольнение",
                           "Сокращение", "Гарантия, компенсации", "Обязательства сторон"});
        AbbreviationVopros.append({"ЗТД", "СТД", "ИТД", "ПК", "ЗАН", "УВ", "СОК", "ГАР", "ОБЯ"});
    }
    if (ui->Razd->currentIndex() == 2)
    {
        ListVopros.append({"Продолжительность РВ", "Режим работы", "Сверхурочные", "Нормирование труда", "Дисциплина",
                                                          "Гарантии, компенсации", "Обязательства сторон"});
        AbbreviationVopros.append({"ПР", "РЕЖ", "СВХ", "НОР", "ДИС", "ГАР", "ОБЯ"});
    }
    if (ui->Razd->currentIndex() == 3)
    {
        ListVopros.append({"Перерыв в течение рабочего дня", "Выходные дни", "Ежегодный отпуск", "Дополнительный отпуск",
                                                          "Гарантии, компенсации", "Обязательства сторон"});
        AbbreviationVopros.append({"ПЕР", "ВЫХ", "ОТП", "ДОВ", "ГАР", "ОБЯ"});
    }
    if (ui->Razd->currentIndex() == 4)
    {
        ListVopros.append({"Права профкома, профсоюза", "Условия предоставляемые для деятельности", "Не члены профсоюза",
                                                          "Льготы неосвобожденному профактиву", "Льготы освобожденному профактиву", "Согласования",
                                                          "Льготы членам профсоюза", "Обязательства сторон"});
        AbbreviationVopros.append({"ПКМ", "УСЛ", "НЧП", "ЛГН", "ЛГО", "СГЛ", "ЛЧП", "ОБГ"});
    }
    if (ui->Razd->currentIndex() == 5)
    {
        ListVopros.append({"Система оплаты", "Надбавки", "Доплаты за не нормальные условия", "Сроки выплат", "Индексация",
                                                          "Внебюджетные средства", "Премии", "Гарантии, компенсации", "Обязательства сторон"});
        AbbreviationVopros.append({"СИС", "НАД", "ДОП", "СРК", "ИНД", "ВНБ", "ПРМ", "ГАР", "ОБЯ"});
    }
    if (ui->Razd->currentIndex() == 6)
    {
        ListVopros.append({"Средства на ОТ", "Соглашение по ОТ", "Мероприятия по улучшению ОТ", "Аттестация рабочих мест",
                                                          "Обеспечение средствами защиты", "Страхование и проф. обследование", "Контроль за ОТ",
                                                          "Гарантии, компенсации", "Обязательства сторон", "Ответственность сторон"});
        AbbreviationVopros.append({"СУМ", "СОГ", "МЕР", "АТТ", "ИСЗ", "ПРФ", "КОН", "ГАР", "ОБЯ", "ОТВ"});
    }
    if (ui->Razd->currentIndex() == 7)
    {
        ListVopros.append({"Обращения, жалобы", "Индивидуальные", "Коллективные", "Комиссия по труд. спорам"});
        AbbreviationVopros.append({"ЖЛБ", "ИНД", "КОЛ", "КОМ"});
    }
    if (ui->Razd->currentIndex() == 8)
    {
        ListVopros.append({"Социальное страхование", "Медиц. обслуживание", "Жилищные вопросы", "Организация отдыха работников",
                                                          "Услуги (бытовые, спорт, культура и пр.)", "Контроль в соцсфере", "Материальная помощь, социальная защита",
                                                          "Мероприятия и средства"});
        AbbreviationVopros.append({"СТР", "МЕД", "ЖИЛ", "ОТД", "УСЛ", "КОН", "МАТ", "МСР"});
    }
    if (ui->Razd->currentIndex() == 9)
    {
        ListVopros.append({"Совместители", "Руководители", "Женщины, семейные", "Молодые, учащиеся", "Гарантии, компенсации"});
        AbbreviationVopros.append({"СОВ", "РУК", "ЖЕН", "МОЛ", "ГАР"});
    }
    if (ui->Razd->currentIndex() == 10)
    {
        ListVopros.append({"Производственно-экономические", "Внебюджетная деятельность", "Обязательства сторон"});
        AbbreviationVopros.append({"ПРЭ", "ВБД", "ОБЯ"});
    }
}

void MainWindow::on_treeWidgetRazd_itemClicked(QTreeWidgetItem *item, int column)
{
    //
}

void MainWindow::on_TextCenter_cursorPositionChanged()
{
    if (!TextCenterIsBlocked)
    {
        ui->GoRight->setEnabled(true);
        //Начать работу с текстом
        QTextDocument *document = ui->TextCenter->document();
        QTextCursor cursor(document);
        QTextCharFormat format;
        //Снять выделение
        if (SelectedFragment != -1)
        {
            cursor.setPosition(Fragments[SelectedFragment]->PositionOfFirst, QTextCursor::MoveAnchor);
            cursor.setPosition(Fragments[SelectedFragment]->PositionOfLast, QTextCursor::KeepAnchor);
            format.setFontWeight(QFont::Normal);
            cursor.mergeCharFormat(format);
        }
        //
        int CursorPosition = ui->TextCenter->textCursor().position();
        for (int i=0; i<Fragments.count(); i++)
        {
            if (Fragments[i]->PositionOfFirst < CursorPosition && Fragments[i]->PositionOfLast > CursorPosition)
            {
                SelectedFragment = i;
                break;
            }
        }
        //Выделить
        cursor.setPosition(Fragments[SelectedFragment]->PositionOfFirst, QTextCursor::MoveAnchor);
        cursor.setPosition(Fragments[SelectedFragment]->PositionOfLast, QTextCursor::KeepAnchor);
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
    cursor.insertText(Fragments[SelectedFragment]->text);
    OldSizeOfSelectedFragment = Fragments[SelectedFragment]->Size;
    //Заполнение данных
    TextCenterIsBlocked = false;
    ui->Act->addItems(ListAct);
    ui->Razd->addItems(ListRazd);
    ui->Quality->addItems(ListQuality);
    for (int i=0; i<ListAct.size(); i++)
    {
        if (Fragments[SelectedFragment]->Akt == AbbreviationAct[i])
        {
            ui->Act->setCurrentIndex(i);
            break;
        }
    }
    for (int i=0; i<ListRazd.size(); i++)
    {
        if (Fragments[SelectedFragment]->Razdel == AbbreviationRazd[i])
        {
            ui->Razd->setCurrentIndex(i);
            break;
        }
    }
    for (int i=0; i<ListQuality.size(); i++)
    {
        if (Fragments[SelectedFragment]->Kachestvo == AbbreviationQuality[i])
        {
            ui->Quality->setCurrentIndex(i);
            break;
        }
    }
    SetUpVopros();
    ui->Question->addItems(ListVopros);
    for (int i=0; i<ListVopros.size(); i++)
    {
        if (Fragments[SelectedFragment]->VoprosABR == AbbreviationVopros[i])
        {
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
    QTextDocument *document = ui->TextCenter->document();
    QTextCursor cursor(document);
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    QString tmp = ui->TextRight->toPlainText();
    Fragments[SelectedFragment]->text = tmp;
    cursor.setPosition(Fragments[SelectedFragment]->PositionOfFirst, QTextCursor::MoveAnchor);
    cursor.setPosition(Fragments[SelectedFragment]->PositionOfLast, QTextCursor::KeepAnchor);
    ArgLine.clear();
    if (Fragments[SelectedFragment]->text.right(1) != '\n')
    {
        ArgLine += "\n";
    }
    ArgLine += frag->Razdel + "\t" + frag->VoprosABR + "\t" + frag->Akt + "\t" + frag->Kachestvo;
    cursor.insertText(Fragments[SelectedFragment]->text + ArgLine + "\n\n");
    cursor.mergeCharFormat(format);
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
    //Вычисление нового размера
    RecountPositions(SelectedFragment, ui->TextRight->toPlainText().size() + ArgLine.size() + 2 - Fragments[SelectedFragment]->Size);
    Fragments[SelectedFragment]->Resize();
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
    if (TextCenterIsBlocked)
    {
        Fragments[SelectedFragment]->Razdel = AbbreviationRazd[index];
        QuestionNotSelected = true;
        ui->Question->clear();
        ListVopros.clear();
        AbbreviationVopros.clear();
        SetUpVopros();
        ui->Question->addItems(ListVopros);
        QuestionNotSelected = false;
    }
}

void MainWindow::on_Question_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked)
    {
        if (!QuestionNotSelected)
        {
            Fragments[SelectedFragment]->VoprosABR = AbbreviationVopros[index];
        }
    }
}

void MainWindow::on_Act_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked)
    {
        Fragments[SelectedFragment]->Akt = AbbreviationAct[index];
    }
}

void MainWindow::on_Quality_currentIndexChanged(int index)
{
    if (TextCenterIsBlocked)
    {
        Fragments[SelectedFragment]->Kachestvo = AbbreviationQuality[index];
    }
}

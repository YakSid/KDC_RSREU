#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 * Master KDA
*/
/*#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QMessageBox>*/ //Уже подключены?

#define ACCESS "DRIVER={Microsoft Access Driver (*.mdb)};" \
            "FIL={MS Access};DBQ=C:\\users\\Yakov\\desktop\\БД Диплом.mdb"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //НУЖНО ЕСЛИ ПРОПУСКАЕТСЯ lDIALOG (Продолжается работа)
    /*Database = QSqlDatabase::addDatabase("QODBC");
    Database.setDatabaseName(ACCESS);
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
            QString TreeHead[10] = {"1. Общие положения (ПСП)","2. Трудовой договор, занятость (ДОГ)","3. Рабочее время (РВ)",
                                    "4. Время отдыха (ВО)","5. Заработная плата (ЗП)","6. Охрана труда (ОТ)","7. Производственные вопросы (ПР)",
                                    "8. Трудовые споры (ТСП)", "9. Гарантии деятельности профкома (ГДП)","10. Социально-бытовые вопросы (СЦ)"};
            QString AbbreviationTreeHead[10] = {"ПСП", "ДОГ", "РВ", "ВО", "ЗП", "ОТ", "ПР", "ТСП", "ГДП", "СЦ"};
            QVector <QTreeWidgetItem*> TreeParentsList;
            for (int i=0; i<10; i++)
            {
                QTreeWidgetItem *newItem = new QTreeWidgetItem(ui->treeWidgetRazd, NULL);
                newItem->setText(Tree_currentColumn, TreeHead[i]);
                TreeParentsList.append(newItem);
            }
            //Заполнение центрального поля
            int QuestionNum = 0;
            in2_query.prepare("SELECT * FROM Тексты WHERE Тексты.[#Дог] = :val1 ORDER BY Тексты.[#Текст]");
            in2_query.bindValue(":val1", SelectedKD);
            if (!in2_query.exec())
            {
                qDebug() << in2_query.lastError().text();
            }
            while (in2_query.next())
            {
                ui->TextCenter->appendPlainText(in2_query.value(2).toString());
                ui->TextCenter->appendPlainText("");
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
                QuestionNum = in2_query.value(6).toInt();
                in3_query.prepare("SELECT * FROM Вопросы WHERE Вопросы.Код = :val1");
                in3_query.bindValue(":val1", QuestionNum);
                if (!in3_query.exec())
                {
                    qDebug() << in3_query.lastError().text();
                }
                while (in3_query.next())
                {
                    qDebug() << in3_query.value(2) << in3_query.value(1);
                    for (int i=0; i<10; i++)
                        if (in3_query.value(2) == AbbreviationTreeHead[i])
                        {
                            Tree_currentItem = TreeParentsList[i];
                            break;
                        }
                    Tree_InsertItem(Tree_currentItem, in3_query.value(3).toString() + " (" + in3_query.value(1).toString() + ")");
                }
            }

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

void MainWindow::on_treeWidgetRazd_itemClicked(QTreeWidgetItem *item, int column)
{
    //
}

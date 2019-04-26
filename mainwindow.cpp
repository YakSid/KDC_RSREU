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
            //Заполнение центрального поля
            int QuestionNum = 0;
            in2_query.prepare("SELECT * FROM Тексты WHERE Тексты.[#Дог] = :val1");
            in2_query.bindValue(":val1", SelectedKD);
            if (!in2_query.exec())
            {
                qDebug() << in2_query.lastError().text();
            }
            while (in2_query.next())
            {
                ui->TextCenter->appendPlainText(in2_query.value(2).toString());
                ui->TextCenter->appendPlainText("");
                QuestionNum = in2_query.value(6).toInt();
                in3_query.prepare("SELECT * FROM Вопросы WHERE Вопросы.Код = :val1");
                in3_query.bindValue(":val1", QuestionNum);
                if (!in3_query.exec())
                {
                    qDebug() << in3_query.lastError().text();
                }
                while (in3_query.next())
                {
                    qDebug() << in3_query.value(2);
                }
            }
            //Создание дерева
            Tree_count = 0;
            ui->treeWidgetRazd->setColumnCount(1);
            //QStringList headers;
            //headers << tr("Навигация по разделам");
            //ui->treeWidgetRazd->setHeaderLabels(headers);
            Tree_currentItem = NULL;
            Tree_currentColumn = 0;
            //Заполнение дерева

            if (Tree_currentItem)
            {
                QString word = Tree_currentItem->text(Tree_currentColumn);
                Tree_InsertItem(Tree_currentItem, word + " " + QString("%1").arg(Tree_count++));
            }
            else
            {
                QTreeWidgetItem *newItem = new QTreeWidgetItem(ui->treeWidgetRazd, ui->treeWidgetRazd->currentItem());
                //указываем 2-м параметром текущий элемент как предшествующий
                newItem->setText (Tree_currentColumn, "" + QString("%1").arg(Tree_count++));
                newItem->setExpanded(true);
            }
            Tree_currentItem = NULL;
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
    if (!parent->isExpanded())
        parent->setExpanded(true);
    QTreeWidgetItem *newItem = new QTreeWidgetItem(parent, ui->treeWidgetRazd->currentItem());
    newItem->setText(Tree_currentColumn, text);
    newItem->setExpanded(true);
}

void MainWindow::on_treeWidgetRazd_itemClicked(QTreeWidgetItem *item, int column)
{
    Tree_currentItem = item;
    Tree_currentColumn = column;
}

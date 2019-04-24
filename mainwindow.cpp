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
            QSqlQuery in1_query, in2_query;
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
            in2_query.prepare("SELECT * FROM Тексты WHERE Тексты.[#Дог] = :val1");
            in2_query.bindValue(":val1", SelectedKD);
            if (!in2_query.exec())
            {
                qDebug() << in1_query.lastError().text();
            }
            while (in2_query.next())
            {
                ui->TextCenter->appendPlainText(in2_query.value(2).toString());
                ui->TextCenter->appendPlainText("");
            }
        }
    }
    else exit(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

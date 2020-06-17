#include "startdialog.h"
#include "ui_startdialog.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
// TODO: [later] указывать имя выбранного к продолжению кд?
StartDialog::StartDialog(QWidget *parent) : QDialog(parent), ui(new Ui::StartDialog)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowMinimizeButtonHint);
    ui->stackedWidget->setCurrentWidget(ui->page_authorization);
    this->resize(this->width(), 200);

    QFile logfile("LoginInfo.txt");
    if (!logfile.exists()) {
        logfile.setFileName("LoginInfo.txt");
    } else {
        firstStart = false;
        logfile.open(QIODevice::ReadOnly);
        QTextStream in(&logfile);
        while (!in.atEnd()) {
            ui->cb_author->addItem(in.readLine());
        }
        logfile.close();
        ui->pb_deleteAuthor->setEnabled(true);
    }
}

StartDialog::~StartDialog()
{
    delete ui;
}

void StartDialog::on_Start_clicked()
{
    startClicked = true;
    StartDialog::close();
}

void StartDialog::on_pb_accept_clicked()
{
    if (ui->cb_author->currentText() != "") {
        authorName = ui->cb_author->currentText();
        ui->stackedWidget->setCurrentWidget(ui->page_modeSettings);
        ui->lb_author->setText("Сотрудник: " + authorName);
    } else {
        _showMessage("Выберите сотрудника");
    }
}

void StartDialog::on_pb_addAuthor_clicked()
{
    if (ui->ln_addAuthor->text() != "") {
        ui->cb_author->clear();
        _addAuthor(ui->ln_addAuthor->text());
        ui->ln_addAuthor->setText("");
        ui->cb_author->addItems(_getAllAuthors());
        firstStart = false;
        ui->pb_deleteAuthor->setEnabled(true);
    } else {
        _showMessage("Введите Ф.И.О. сотрудника");
    }
}

void StartDialog::on_pb_deleteAuthor_clicked()
{
    ui->cb_author->removeItem(ui->cb_author->currentIndex());
    QStringList doctors;
    if (ui->cb_author->count() != 0) {
        for (int i = 0; i < ui->cb_author->count(); i++) {
            doctors.append(ui->cb_author->itemText(i));
        }
        //Изменение логфайла
        _clearLogfile();
        _updateLogfile(doctors);

        ui->cb_author->clear();
        ui->cb_author->addItems(_getAllAuthors());
    } else {
        _clearLogfile();
        ui->cb_author->clear();
        ui->pb_deleteAuthor->setDisabled(true);
        firstStart = true;
    }
}

void StartDialog::_addAuthor(QString name)
{
    QFile logfile("LoginInfo.txt");
    logfile.open(QIODevice::Append | QIODevice::Text);
    QTextStream writeStream(&logfile);
    if (!firstStart)
        writeStream << endl;
    writeStream << name;
    logfile.close();
}

QStringList StartDialog::_getAllAuthors()
{
    QStringList result;

    QFile logfile("LoginInfo.txt");
    logfile.open(QIODevice::ReadOnly);
    QTextStream in(&logfile);
    while (!in.atEnd()) {
        result.append(in.readLine());
    }
    logfile.close();

    return result;
}

void StartDialog::_showMessage(QString text)
{
    QMessageBox msg;
    msg.setText(text);
    msg.setWindowTitle("Пожалуйста");
    msg.exec();
}

void StartDialog::_updateLogfile(QStringList authors)
{
    QFile logfile("LoginInfo.txt");
    logfile.open(QIODevice::Append | QIODevice::Text);
    QTextStream writeStream(&logfile);
    while (authors.count() != 0) {
        writeStream << authors.first();
        authors.pop_front();
        if (authors.count() != 0)
            writeStream << endl;
    }
    logfile.close();
}

void StartDialog::_clearLogfile()
{
    QFile logfile("LoginInfo.txt");
    if (logfile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        logfile.close();
}

void StartDialog::on_pb_changeAuthor_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_authorization);
}

void StartDialog::on_stackedWidget_currentChanged(int arg1)
{
    if (arg1 == 0) {
        this->resize(this->width(), 200);
    } else if (arg1 == 1) {
        this->resize(this->width(), 200);
    } else {
        this->resize(this->width(), 200);
    }
}

void StartDialog::on_pb_startNew_clicked()
{
    StartMode = EStartMode::startNew;
    ui->stackedWidget->setCurrentWidget(ui->page_start);
}

void StartDialog::on_pb_continueSaved_clicked()
{
    StartMode = EStartMode::contibueOld;
    ui->stackedWidget->setCurrentWidget(ui->page_start);
}

void StartDialog::on_pb_loadFile_clicked()
{
    jFilename = QFileDialog::getOpenFileName(this, "Выберите проект", QString(), tr("JSON (*.json)"));
    ui->pb_continueSaved->setEnabled(true);
}

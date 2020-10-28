#include "startdialog.h"
#include "ui_startdialog.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
// TODO: [later] указывать имя выбранного к продолжению кд?
// TODO: [later] сделать автосохранение?

//! TODO: [later] Заменить размещение кнопок - сделать в groupBox'ы с подписями

StartDialog::StartDialog(QWidget *parent) : QDialog(parent), ui(new Ui::StartDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    ui->stackedWidget->setCurrentWidget(ui->page_authorization);
    this->resize(this->width(), 200);

    ui->pb_dbManage->setVisible(false);
    ui->ln_db->setVisible(false);

    QFile logfile("LoginInfo.ini");
    if (!logfile.exists()) {
        logfile.setFileName("LoginInfo.ini");
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
    QFile logfile("LoginInfo.ini");
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

    QFile logfile("LoginInfo.ini");
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
    QFile logfile("LoginInfo.ini");
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
    QFile logfile("LoginInfo.ini");
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
    if (dbLocal) {
        StartMode = EStartMode::startNew;
        ui->stackedWidget->setCurrentWidget(ui->page_start);
    } else if (dbPath.isEmpty()) {
        _showMessage("Выберите базу данных");
    } else {
        StartMode = EStartMode::startNew;
        ui->stackedWidget->setCurrentWidget(ui->page_start);
    }
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

// TODO: [later] сделать сохранение пути к БД в тот же файл, что и логи с помощью qsettingsini
void StartDialog::on_pb_dbManage_clicked()
{
    dbPath = QFileDialog::getOpenFileName(this, "Выберите базу данных", QString(), tr("Microsoft Access (*.mdb)"));
    ui->ln_db->setText(dbPath);
}

void StartDialog::on_ch_dbLocal_toggled(bool checked)
{
    ui->pb_dbManage->setVisible(!checked);
    ui->ln_db->setVisible(!checked);
    dbLocal = checked;
}

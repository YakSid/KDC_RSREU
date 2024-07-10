#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

    enum EStartMode { startNew, contibueOld, exitApp };
    EStartMode StartMode { EStartMode::exitApp };
    QString authorName;
    bool startClicked { false };
    QString jFilename { "" };
    // Перменные для работы с БД
    QString dbPath { "" };

public:
    void saveDbPath(QString path);

private slots:
    void on_Start_clicked();
    void on_pb_accept_clicked();
    void on_pb_addAuthor_clicked();
    void on_pb_deleteAuthor_clicked();
    void on_pb_changeAuthor_clicked();
    void on_stackedWidget_currentChanged(int arg1);
    void on_pb_startNew_clicked();
    void on_pb_continueSaved_clicked();
    void on_pb_loadFile_clicked();
    void on_pb_dbManage_clicked();

private:
    void _showMessage(QString text);
    // Работа с файлом авторов
    void _addAuthor(QString name);
    QStringList _getAllAuthors();
    void _updateLogfile(QStringList authors);
    void _clearLogfile();
    // Работа с файлом пути к БД
    void _savePath(QString path);
    QString _getPath();

    void _showDbSettings(bool visible);

private:
    Ui::StartDialog *ui;
    bool firstStart { true };
};

#endif // STARTDIALOG_H

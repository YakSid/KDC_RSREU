#ifndef KNOWLEDGEBASE_H
#define KNOWLEDGEBASE_H

#include <QDialog>

namespace Ui {
class knowledgebase;
}

class knowledgebase : public QDialog
{
    Q_OBJECT

public:
    explicit knowledgebase(QWidget *parent = nullptr);
    ~knowledgebase();

private:
    Ui::knowledgebase *ui;
};

#endif // KNOWLEDGEBASE_H

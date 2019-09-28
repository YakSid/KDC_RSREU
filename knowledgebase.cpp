#include "knowledgebase.h"
#include "ui_knowledgebase.h"

knowledgebase::knowledgebase(QWidget *parent) : QDialog(parent), ui(new Ui::knowledgebase)
{
    ui->setupUi(this);
}

knowledgebase::~knowledgebase()
{
    delete ui;
}

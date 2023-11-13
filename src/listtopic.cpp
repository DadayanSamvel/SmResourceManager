#include "listtopic.h"
#include "ui_listtopic.h"

ListTopic::ListTopic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListTopic)
{
    ui->setupUi(this);
}

ListTopic::~ListTopic()
{
    delete ui;
}

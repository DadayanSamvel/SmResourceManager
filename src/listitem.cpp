#include "listitem.h"
#include "ui_listitem.h"

ListItem::ListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItem)
{
    ui->setupUi(this);
}

ListItem::~ListItem()
{
    delete ui;
}

void ListItem::setItemData(QString name, QString path, std::string pid, QString rss, QString utimeStime, QString priority, char state, time_t currentTime)
{
    ui->label->setText(QString(pid.c_str()));
    ui->label_2->setText(rss);
    ui->label_3->setText(name);
    ui->label_4->setText(path);
    ui->label_5->setText(utimeStime);
    switch(state) {
    case 'R':
        ui->label_6->setText("Running");
        break;
    case 'S':
        ui->label_6->setText("Sleeping");
        break;
    case 'Z':
        ui->label_6->setText("Zombie");
        break;
    case 'I':
        ui->label_6->setText("Waiting");
        break;
    default:
        ui->label_6->setText("Unknown state");
    }

    ui->label_7->setText(priority);
    this->currentTime = currentTime;
    this->pid = pid;
}

void ListItem::updateItemData(QString rss, QString utimeStime, QString priority, char state, time_t currentTime)
{
    ui->label_2->setText(rss);
    ui->label_5->setText(utimeStime);
    switch(state) {
    case 'R':
        ui->label_6->setText("Running");
        break;
    case 'S':
        ui->label_6->setText("Sleeping");
        break;
    case 'Z':
        ui->label_6->setText("Zombie");
        break;
    case 'I':
        ui->label_6->setText("Waiting");
        break;
    default:
        ui->label_6->setText("Unknown state");
    }

    ui->label_7->setText(priority);
    this->currentTime = currentTime;
}

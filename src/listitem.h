#ifndef LISTITEM_H
#define LISTITEM_H

#include <QWidget>

namespace Ui {
class ListItem;
}

class ListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ListItem(QWidget *parent = nullptr);
    ~ListItem();
    void setItemData(QString name, QString path, std::string pid, QString rss, QString utimeStime, QString priority, char state, time_t currentTime);
    void updateItemData(QString rss, QString utimeStime, QString priority, char state, time_t currentTime);
    time_t currentTime;
    std::string pid;

private:
    Ui::ListItem *ui;
};

#endif // LISTITEM_H

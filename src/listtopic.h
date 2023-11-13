#ifndef LISTTOPIC_H
#define LISTTOPIC_H

#include <QWidget>

namespace Ui {
class ListTopic;
}

class ListTopic : public QWidget
{
    Q_OBJECT

public:
    explicit ListTopic(QWidget *parent = nullptr);
    ~ListTopic();

private:
    Ui::ListTopic *ui;
};

#endif // LISTTOPIC_H

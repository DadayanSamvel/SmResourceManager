#ifndef CPUITEM_H
#define CPUITEM_H

#include <QWidget>

namespace Ui {
class CpuItem;
}

class CpuItem : public QWidget
{
    Q_OBJECT

public:
    explicit CpuItem(QWidget *parent = nullptr);
    ~CpuItem();
    void setItemData(QString cpuId, QString cpuInfo);
    void updateItemData(int load);
    QString cpuId;

private:
    Ui::CpuItem *ui;
    QString cpuInfo;

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void selectCpuSig(QString cpuId, QString cpuInfo);

};

#endif // CPUITEM_H

#include "cpuitem.h"
#include "ui_cpuitem.h"
#include <QMouseEvent>

CpuItem::CpuItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuItem)
{
    ui->setupUi(this);
}

CpuItem::~CpuItem()
{
    delete ui;
}

void CpuItem::setItemData(QString cpuId, QString cpuInfo)
{
    this->cpuId = cpuId;
    this->cpuInfo = cpuInfo;
    ui->label->setText(cpuId);
}

void CpuItem::updateItemData(int load)
{
    ui->progressBar->setValue(load);
}

void CpuItem::mousePressEvent(QMouseEvent *event)
{
    emit selectCpuSig(cpuId, cpuInfo);
}

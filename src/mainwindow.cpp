#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include "listitem.h"
#include "listtopic.h"
#include "cpuitem.h"

void MainWindow::updateUi(SystemInfoInstant* systemInfoInstant)
{
    //qDebug() << "update ui main thread id " << QThread::currentThreadId();

    bool m = true;
    time_t timeStamp = 0;

    for (const ProcessInfo& process : *systemInfoInstant->process_info) {
        bool findResult = false;
        if (m) {
            timeStamp = process.current_time;
            m = false;
        }
        for (int i = 1; i < ui->listWidget->model()->rowCount(); i++) {
            auto findItem = ui->listWidget->item(i);
            ListItem *findItemWidget = (ListItem*)ui->listWidget->itemWidget(findItem);
            if(findItemWidget->pid == process.pid) {
                //findItemWidget->currentTime = process.current_time;
                findItemWidget->updateItemData(QString::number(process.rss),
                                               QString("%1/%2").arg(process.utime).arg(process.stime),
                                               QString::number(process.priority),
                                               process.state,
                                               process.current_time
                                               );
                findResult = true;
                break;
            }
        }

        if(!findResult) {
            auto listItem = new QListWidgetItem();
            auto itemWidget = new ListItem(this);
            itemWidget->setItemData(QString(process.process_name.c_str()),
                                    QString(process.process_path.c_str()),
                                    process.pid,
                                    QString::number(process.rss),
                                    QString("%1/%2").arg(process.utime).arg(process.stime),
                                    QString::number(process.priority),
                                    process.state,
                                    process.current_time
                                    );
            listItem->setSizeHint(itemWidget->sizeHint());
            ui->listWidget->addItem(listItem);
            ui->listWidget->setItemWidget(listItem, itemWidget);
        }
    }

    // del pid closed
    for (int i = 1; i < ui->listWidget->model()->rowCount(); i++) {
        auto findItem = ui->listWidget->item(i);
        ListItem *findItemWidget = (ListItem*)ui->listWidget->itemWidget(findItem);
        if(findItemWidget->currentTime != timeStamp) {
            //qDebug() << "del pid " << findItemWidget->pid.c_str();
            delete findItem;
        }
    }

    ui->textEdit->setText(QString("Total process: %1 \nRunning process: %2").arg(systemInfoInstant->average->total_processes).arg(systemInfoInstant->average->running_processes));;

    // set cpu load
    QVBoxLayout *existingLayout = qobject_cast<QVBoxLayout*>(ui->frame_4->layout());
    for (int i = 0; i < existingLayout->count(); ++i) {
        QLayoutItem* item = existingLayout->itemAt(i);
        if (item && item->widget()) {
            CpuItem *cpuItem = (CpuItem *)item->widget();
            for (const auto& pair : *systemInfoInstant->cpu_load) {
                auto key = pair.first;
                auto load = pair.second;
                if(cpuItem->cpuId == QString(key.c_str())) {
                    //qDebug() << "Key: " << key.c_str();
                    cpuItem->updateItemData(load);
                }
                if(key == "cpu_full") {
                    ui->progressBar_4->setValue(load);
                    ui->label->setText(QString::number(load, 'f', 2) + "%");
                }
            }
        }
    }

    //set memory
    long total_memory = systemInfoInstant->memory_info->total_memory;
    //long free_memory = systemInfoInstant->memory_info->free_memory;
    long used_memory = systemInfoInstant->memory_info->used_memory;
    float percMem = (float)used_memory/total_memory;
    float usedM = (float)used_memory/(1024*1024);
    float totalM = (float)total_memory/(1024*1024);
    //qDebug() << "used_memory " << used_memory << " total_memory " << total_memory;
    //qDebug() << "percMem " << percMem;
    ui->progressBar_5->setValue(percMem*100);
    QString lblMem = QString("%1Gb/%2Gb").arg(QString::number(usedM, 'f', 2) + "%").arg(QString::number(totalM, 'f', 2) + "%");
    ui->label_2->setText(lblMem);

    delete systemInfoInstant->average;
    delete systemInfoInstant->cpu_load;
    delete systemInfoInstant->memory_info;
    delete systemInfoInstant->process_info;
    delete systemInfoInstant;
}

void MainWindow::cpuInfoSlot(QString allCpuInfo, QMap<QString, QString> cpuInfoSlises)
{
    cpuInfo = allCpuInfo;
    this->cpuInfoSlises = cpuInfoSlises;

    QVBoxLayout *existingLayout = qobject_cast<QVBoxLayout*>(ui->frame_4->layout());
    if (!existingLayout) {
        existingLayout = new QVBoxLayout(ui->frame_4);
        ui->frame_4->setLayout(existingLayout);
    }

    bool m = true;
    QMap<QString, QString>::const_iterator it;
    for (it = cpuInfoSlises.constBegin(); it != cpuInfoSlises.constEnd(); ++it) {
        QString key = it.key();
        QString value = it.value();
        if(m) {
            setSelectedCpu(key, value);
            m = false;
        }

        CpuItem *cpuItem = new CpuItem(this);
        cpuItem->setItemData(key, value);
        cpuItem->updateItemData(0);
        connect(cpuItem, SIGNAL(selectCpuSig(QString,QString)), this, SLOT(cpuSelectedSlot(QString,QString)));
        existingLayout->addWidget(cpuItem);
        //qDebug() << "Key:" << key;
    }

}

void MainWindow::errorString(QString err)
{
    //
}

void MainWindow::cpuSelectedSlot(QString cpuId, QString cpuInfo)
{
    setSelectedCpu(cpuId, cpuInfo);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QThread* thread = new QThread;
    worker = new SmWorker();
    worker->moveToThread(thread);
    connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(worker, SIGNAL(workerUpdate(SystemInfoInstant*)), this, SLOT(updateUi(SystemInfoInstant*)));
    connect(worker, SIGNAL(cpuInfoSig(QString,QMap<QString,QString>)), this, SLOT(cpuInfoSlot(QString,QMap<QString,QString>)));
    thread->start();

    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(1);
    ui->line->show();
    ui->line_2->hide();

    auto listItem = new QListWidgetItem();
    auto itemWidget = new ListTopic(this);
    listItem->setSizeHint(itemWidget->sizeHint());
    listItem->setBackground(Qt::green);
    ui->listWidget->addItem(listItem);
    ui->listWidget->setItemWidget(listItem, itemWidget);

}

MainWindow::~MainWindow()
{
    delete worker;
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->line->show();
    ui->line_2->hide();
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->line->hide();
    ui->line_2->show();
}

void MainWindow::setSelectedCpu(QString cpuId, QString cpuInfo)
{
    ui->label_6->setText(cpuId);
    ui->textEdit_2->setText(cpuInfo);
}


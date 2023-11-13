#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "smworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void updateUi(SystemInfoInstant* systemInfoInstant);
    void cpuInfoSlot(QString allCpuInfo, QMap<QString, QString> cpuInfoMap);
    void errorString(QString err);
    void cpuSelectedSlot(QString cpuId, QString cpuInfo);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    SmWorker* worker;
    QString cpuInfo;
    QMap<QString, QString> cpuInfoSlises;
    void setSelectedCpu(QString cpuId, QString cpuInfo);
};
#endif // MAINWINDOW_H

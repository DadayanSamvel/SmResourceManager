#ifndef SMWORKER_H
#define SMWORKER_H

#include <QThread>
#include <QTimer>
#include <QMap>
#include "smsysinfo.h"

class SmWorker : public QObject
{
    Q_OBJECT
public:
    SmWorker();
    ~SmWorker();

private:
    QTimer *tmr;
    SmSysInfo *sysInfo;
    std::unordered_map<std::string, CpuLoad> *cpuLoadPrevious;
    std::unordered_map<std::string, CpuLoad> *cpuLoadCurrent;
    const int INTERVAL = 1000;
    std::unordered_map<std::string, double>* getCpuLoadPercentage();
    void doWork();
    void init();

public slots:
    void process();

private slots:
    void timerUpd();
    void dispose();

signals:
    void finished();
    void error(QString err);
    void workerUpdate(SystemInfoInstant* systemInfoInstant);
    void cpuInfoSig(QString allCpuInfo, QMap<QString, QString> cpuInfoMap);
    void freeKernelObj();

};

#endif // SMWORKER_H

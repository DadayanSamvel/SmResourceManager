#include "smworker.h"
#include <QDebug>


SmWorker::SmWorker()
{
    cpuLoadPrevious = nullptr;
    cpuLoadCurrent = nullptr;
}

SmWorker::~SmWorker()
{
    emit freeKernelObj();
    qDebug("worker closed");
    emit finished();
}

std::unordered_map<std::string, double> *SmWorker::getCpuLoadPercentage()
{
    std::unordered_map<std::string, double>* cpuLoadPercentage = new std::unordered_map<std::string, double>();

    for (const auto& currentEntry : *cpuLoadCurrent) {
        const std::string& key = currentEntry.first;

        auto previousIter = cpuLoadPrevious->find(key);

        if (previousIter != cpuLoadPrevious->end()) {
            const CpuLoad& previousCpuLoad = previousIter->second;
            const CpuLoad& currentCpuLoad = currentEntry.second;

            int idleDifference = currentCpuLoad.idle_time - previousCpuLoad.idle_time;
            int totalDifference = currentCpuLoad.total_time - previousCpuLoad.total_time;
            double load_percentage = 0;
            if (totalDifference) load_percentage = 100.0 * (1.0 - static_cast<double>(idleDifference) / totalDifference);
            (*cpuLoadPercentage)[key] = load_percentage;
            //qDebug() << "Key: " << QString::fromStdString(key) <<  ", load_percentage: " << load_percentage;
        }
    }

    return cpuLoadPercentage;
}


void SmWorker::doWork()
{
    //qDebug("doWork");

    //get_memory_info();

    cpuLoadPrevious = cpuLoadCurrent;
    cpuLoadCurrent = sysInfo->getCpuLoad();

    SystemInfoInstant* systemInfoInstant = new SystemInfoInstant();
    systemInfoInstant->cpu_load = getCpuLoadPercentage();
    systemInfoInstant->process_info = sysInfo->getSystemProcesses();
    systemInfoInstant->average = sysInfo->getLoadAverage();
    systemInfoInstant->memory_info = sysInfo->getMemoryInfo();

    emit workerUpdate(systemInfoInstant);
}

void SmWorker::init()
{
    sysInfo = new SmSysInfo();
    sysInfo->init();

    cpuLoadCurrent = sysInfo->getCpuLoad();

    std::vector<CpuInfo>* cpuInfoList = sysInfo->getCpuInfo();
    QStringList qStringList;
    QMap<QString, QString> cpuInfoSlises;
    for (const CpuInfo& processorInfo : *cpuInfoList) {
        QString qStr = QString::fromStdString(processorInfo.cpu_info);
        QString cpuStr = QString("cpu-%1 \n%2").arg(processorInfo.cpu_number).arg(qStr);
        qStringList << cpuStr;
        cpuInfoSlises[QString("cpu%1").arg(processorInfo.cpu_number)] = cpuStr;
    }
    QString str = qStringList.join("\n");
    delete cpuInfoList;

    doWork();
    emit cpuInfoSig(str, cpuInfoSlises);
}

void SmWorker::process()
{
    qDebug() << "worker process start thread id " << QThread::currentThreadId();
    tmr = new QTimer();
    tmr->setInterval(INTERVAL);
    connect(tmr, SIGNAL(timeout()), this, SLOT(timerUpd()));
    connect(this, SIGNAL(freeKernelObj()), this, SLOT(dispose()), Qt::BlockingQueuedConnection);
    init();
    tmr->start();
}

void SmWorker::timerUpd()
{
    //qDebug("timer update");
    doWork();
}

void SmWorker::dispose()
{
    qDebug() << "dispose thread id " << QThread::currentThreadId();
    delete tmr;
    delete sysInfo;
}

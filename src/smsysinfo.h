#ifndef SMSYSINFO_H
#define SMSYSINFO_H

#include <dirent.h>
#include "smdatatype.h"
#include <vector>
#include <unordered_map>

class SmSysInfo
{
public:
    SmSysInfo();
    ~SmSysInfo();
    int init();
    std::vector<ProcessInfo>* getSystemProcesses();
    MemoryInfo* getMemoryInfo();
    Average* getLoadAverage();
    std::unordered_map<std::string, CpuLoad>* getCpuLoad();
    std::vector<CpuInfo>* getCpuInfo();

private:
    DIR *dir;

};

#endif // SMSYSINFO_H

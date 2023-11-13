#ifndef SMDATATYPE_H
#define SMDATATYPE_H

#include <string>
#include <vector>
#include<unordered_map>

typedef struct MemoryInfo {
    long total_memory;
    long free_memory;
    long used_memory;
    int used_mem_percentage;
} MemoryInfo;

typedef struct Average {
    double avg_1min;
    double avg_5min;
    double avg_15min;
    int running_processes;
    int total_processes;
    //int last_pid;
} Average;

typedef struct CpuInfo {
    int cpu_number;
    std::string cpu_info;
} CpuInfo;

typedef struct CpuLoad {
    std::string cpu_label;
    long user;
    int nice;
    int system;
    int idle;
    long total_time;
    long idle_time;
    time_t current_time;
} CpuLoad;

typedef struct ProcessInfo {
    std::string process_name;
    std::string process_path;
    char state;
    long utime;
    long stime;
    long rss;
    int priority;
    std::string pid;
    time_t current_time;
} ProcessInfo;

typedef struct SystemInfoInstant {
    std::vector<ProcessInfo>* process_info;
    std::unordered_map<std::string, double>* cpu_load;
    Average* average;
    MemoryInfo* memory_info;
} SystemInfoInstant;

#endif // SMDATATYPE_H

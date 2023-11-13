#include "smsysinfo.h"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <QThread>
#include <unordered_map>

using namespace std;


string get_process_path(const string& pid) {
    string exe_path = "/proc/" + pid + "/exe";
    char buf[4096];
    ssize_t len = readlink(exe_path.c_str(), buf, sizeof(buf) - 1);

    if (len != -1) {
        buf[len] = '\0';
        return string(buf);
    } else {
        //cerr << "Error getting process path for PID " << pid << endl;
        return "N/A";
    }
}

string read_value_from_proc(const string& file_path) {
    ifstream file(file_path);
    if (file) {
        string value;
        getline(file, value);
        return value;
    } else {
        cerr << "Error reading " << file_path << endl;
        return "N/A";
    }
}

void print_cpu_info(int cpu_number, const string& cpu_info) {
    cout << "CPU " << cpu_number << ":" << endl;

    size_t model_name_pos = cpu_info.find("model name");
    size_t cpu_mhz_pos = cpu_info.find("cpu MHz");

    if (model_name_pos != string::npos) {
        size_t model_name_end = cpu_info.find("\n", model_name_pos);
        cout << cpu_info.substr(model_name_pos, model_name_end - model_name_pos) << endl;
    }

    if (cpu_mhz_pos != string::npos) {
        size_t cpu_mhz_end = cpu_info.find("\n", cpu_mhz_pos);
        cout << cpu_info.substr(cpu_mhz_pos, cpu_mhz_end - cpu_mhz_pos) << endl;
    }

    cout << "--------------------------" << endl;
}

void get_process_cpu_time(string pid, long *utime, long *stime) {
    std::string stat_path = "/proc/" + pid + "/stat";

    std::ifstream stat_file(stat_path);
    if (!stat_file) {
        std::cerr << "Unable to open " << stat_path << std::endl;
        return;
    }

    std::string line;
    getline(stat_file, line);
    //std::cout << "Process " << line << std::endl;

    std::istringstream iss(line);
    std::string token;
    // Пропускаем первые 13 полей
    for (int i = 0; i < 13; ++i) {
        iss >> token;
    }

    // utime - время в пользовательском пространстве
    // stime - время в пространстве ядра
    //unsigned long utime, stime;
    iss >> *utime >> *stime;

    //std::cout << "Process " << pid << " CPU Time:" << std::endl;
    //std::cout << "User Time: " << utime << " jiffies" << std::endl;
    //std::cout << "Kernel Time: " << stime << " jiffies" << std::endl;

    stat_file.close();
}


SmSysInfo::SmSysInfo()
{
    cout << "SmSysInfo constructor thread id " << QThread::currentThreadId() << endl;
}

SmSysInfo::~SmSysInfo()
{
    cout << "SmSysInfo destructor thread id " << QThread::currentThreadId() << endl;
    closedir(dir);
}

int SmSysInfo::init()
{
    dir = opendir("/proc");
    return 0;
}


MemoryInfo *SmSysInfo::getMemoryInfo() {
    string mem_total_path = "/proc/meminfo";
    string mem_total_data = read_value_from_proc(mem_total_path);

    MemoryInfo* memoryInfo = new MemoryInfo();

    std::ifstream meminfo("/proc/meminfo");

    if (!meminfo.is_open()) {
        std::cerr << "Failed to open /proc/meminfo" << std::endl;
        delete memoryInfo;
        return nullptr;
    }

    std::string line;
    long total_memory = 0;
    long free_memory = 0;
    long buffered_memory = 0;
    long cached_memory = 0;

    while (getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;

        if (iss >> key >> value) {
            if (key == "MemTotal:") {
                total_memory = value;
            } else if (key == "MemFree:") {
                free_memory = value;
            } else if (key == "Buffers:") {
                buffered_memory = value;
            } else if (key == "Cached:") {
                cached_memory = value;
            }
        }
    }

    meminfo.close();

    long used_memory = total_memory - free_memory - buffered_memory - cached_memory;
    double used_memory_percentage = static_cast<double>(used_memory) / total_memory * 100;

    //std::cout << "Total Memory: " << total_memory << " kB" << std::endl;
    //std::cout << "Free Memory: " << free_memory << " kB" << std::endl;
    //std::cout << "Buffered Memory: " << buffered_memory << " kB" << std::endl;
    //std::cout << "Cached Memory: " << cached_memory << " kB" << std::endl;
    //std::cout << "Used Memory: " << used_memory << " kB (" << static_cast<int>(used_memory_percentage) << "%)" << std::endl;


    memoryInfo->free_memory = free_memory;
    memoryInfo->total_memory = total_memory;
    memoryInfo->used_memory = used_memory;
    memoryInfo->used_mem_percentage = static_cast<int>(used_memory_percentage);

    return memoryInfo;
}


Average *SmSysInfo::getLoadAverage() {
    //cout << "Load Average:" << endl;
    //cout << "--------------" << endl;

    Average* loadAverage = new Average();

    string loadavg = read_value_from_proc("/proc/loadavg");
    size_t pos = loadavg.find('/');
    if (pos != string::npos) {
        replace(loadavg.begin() + pos, loadavg.begin() + pos + 1, '/', ' ');
    }
    //cout << "Load Average: " << loadavg << endl;

    istringstream iss(loadavg);
    double avg_1min, avg_5min, avg_15min;
    int running_processes, total_processes, last_pid;

    iss >> avg_1min >> avg_5min >> avg_15min >> running_processes >> total_processes >> last_pid;

    loadAverage->avg_15min = avg_15min;
    loadAverage->avg_5min = avg_5min;
    loadAverage->avg_1min = avg_1min;
    loadAverage->running_processes = running_processes;
    loadAverage->total_processes = total_processes;

    /*
    cout << "1 Minute Load Average: " << avg_1min << endl;
    cout << "5 Minutes Load Average: " << avg_5min << endl;
    cout << "15 Minutes Load Average: " << avg_15min << endl;
    cout << "Running Processes: " << running_processes << endl;
    cout << "Total Processes: " << total_processes << endl;
    cout << "Last Process ID: " << last_pid << endl;
    */

    return loadAverage;
}


std::vector<CpuInfo> *SmSysInfo::getCpuInfo() {
    //cout << "CPU Info:" << endl;
    //cout << "----------" << endl;

    vector<CpuInfo> *allCpuInfo = new vector<CpuInfo>();
    int cpu_number = 0;
    string cpu_info;
    string cpu_path = "/proc/cpuinfo";

    ifstream cpu_info_file(cpu_path);

    while (cpu_info_file) {
        string line;
        getline(cpu_info_file, line);
        if (line.empty()) {
            if (!cpu_info.empty()) {
                /*
                cout << "CPU " << cpu_number << ":" << endl;
                cout << cpu_info << endl;
                cout << "--------------------------" << endl;
                */
                //print_cpu_info(cpu_number, cpu_info);

                CpuInfo cpuInfo;
                cpuInfo.cpu_number = cpu_number;
                cpuInfo.cpu_info = cpu_info;
                allCpuInfo->push_back(cpuInfo);
            }
            cpu_info = "";
            cpu_number++;
        } else {
            cpu_info += line + "\n";
        }
    }

    cpu_info_file.close();
    return allCpuInfo;
}

unordered_map<string, CpuLoad> *SmSysInfo::getCpuLoad() {
    //cout << "CPU Load for Each Processor:" << endl;
    //cout << "-----------------------------" << endl;

    unordered_map<string, CpuLoad>* cpuLoadList = new unordered_map<string, CpuLoad>();

    string cpu_path = "/proc/stat";
    ifstream cpu_stat_file(cpu_path);


    if (!cpu_stat_file) {
        cerr << "Error opening " << cpu_path << endl;
        delete cpuLoadList;
        return nullptr;
    }

    string line;

    while (getline(cpu_stat_file, line)) {
        if (line.find("cpu") == 0) {

            istringstream iss(line);
            string cpu_label;
            //cout << "line - " << line << endl;
            iss >> cpu_label;

            if(line.find("cpu ") != 0) {
                //
            } else {
                cpu_label = "cpu_full";
            }

            long user, nice, system, idle;
            iss >> user >> nice >> system >> idle;

            long total_time = user + nice + system + idle;
            long idle_time = idle;

            time_t current_time;
            time(&current_time);

            CpuLoad cpuLoad;
            cpuLoad.idle = idle;
            cpuLoad.nice = nice;
            cpuLoad.system = system;
            cpuLoad.idle_time = idle_time;
            cpuLoad.total_time = total_time;
            cpuLoad.current_time = current_time;
            cpuLoad.cpu_label = cpu_label;
            (*cpuLoadList)[cpu_label] = cpuLoad;

        }
    }

    cpu_stat_file.close();
    return cpuLoadList;
}


std::vector<ProcessInfo> *SmSysInfo::getSystemProcesses() {
    rewinddir(dir);
    struct dirent *entry;

    //const int col_width = 15;
    std::vector<ProcessInfo>* sysProcesses = new std::vector<ProcessInfo>();

    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            string pid = entry->d_name;

            string status_path = "/proc/" + pid + "/status";
            ifstream status_file(status_path);

            time_t current_time;
            time(&current_time);

            long utime;
            long stime;

            string process_path = get_process_path(pid);
            get_process_cpu_time(pid, &utime, &stime);

            ProcessInfo processInfo;
            processInfo.process_path = process_path;
            processInfo.pid = pid;
            processInfo.utime = utime;
            processInfo.stime = stime;
            processInfo.current_time = current_time;

            string line;
            while (getline(status_file, line)) {
                istringstream iss(line);
                string label;
                iss >> label;

                if (label == "Name:") {
                    iss >> processInfo.process_name;
                } else if (label == "State:") {
                    iss >> processInfo.state;
                } else if (label == "VmRSS:") {
                    iss >> processInfo.rss;
                } else if (label == "Priority:") {
                    iss >> processInfo.priority;
                }
            }


            sysProcesses->push_back(processInfo);

        }
    }

    return sysProcesses;
}

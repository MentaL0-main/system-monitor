#include "monitor.hpp"
#include <fstream>
#include <sstream>

Monitor::Monitor() : lastUpdate_(std::chrono::steady_clock::now()) {
    updateMemory();
    updateCpu();
    updateDisk();
}

void Monitor::update() {
    updateCpu();
    updateMemory();
    updateDisk();
    lastUpdate_ = std::chrono::steady_clock::now();
}

void Monitor::updateCpu() {
    std::ifstream file("/proc/stat");
    std::string label;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

    if (!(file >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal)) {
        return;
    }

    unsigned long long currentIdle = idle + iowait;
    unsigned long long currentNonIdle = user + nice + system + irq + softirq + steal;
    unsigned long long currentTotal = currentIdle + currentNonIdle;

    unsigned long long totalDiff = currentTotal - prevCpuTotal_;
    unsigned long long idleDiff = currentIdle - prevCpuIdle_;

    if (totalDiff > 0) {
        cpuUsage_ = static_cast<short>((totalDiff - idleDiff) * 100 / totalDiff);
    }

    prevCpuTotal_ = currentTotal;
    prevCpuIdle_ = currentIdle;
}

void Monitor::updateMemory() {
    std::ifstream file("/proc/meminfo");
    std::string line, key;
    unsigned long value;
    unsigned long memTotal = 0, memAvailable = 0;

    while (std::getline(file, line) && (memTotal == 0 || memAvailable == 0)) {
        std::istringstream iss(line);
        iss >> key >> value;
        if (key == "MemTotal:") memTotal = value;
        else if (key == "MemAvailable:") memAvailable = value;
    }

    if (memTotal > 0) {
        totalMem_ = static_cast<int>(memTotal);
        memUsage_ = static_cast<int>(memTotal - memAvailable);
    }
}
void Monitor::updateDisk() {
    std::ifstream file("/proc/diskstats");
    if (!file.is_open()) return;

    std::string line;
    unsigned long long totalSectorsWritten = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string major, minor, device;
        unsigned long long r_ios, r_merges, r_sec, r_ticks, 
                           w_ios, w_merges, w_sec;

        if (iss >> major >> minor >> device >> r_ios >> r_merges >> r_sec >> r_ticks >> w_ios >> w_merges >> w_sec) {
            if (std::isdigit(device.back()) || device.find("loop") != std::string::npos || device.find("ram") != std::string::npos) {
                continue;
            }

            totalSectorsWritten += w_sec;
        }
    }

    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate_).count();

    if (prevDiskSectors_ > 0 && duration > 0) {
        unsigned long long deltaSectors = (totalSectorsWritten >= prevDiskSectors_) 
                                         ? (totalSectorsWritten - prevDiskSectors_) : 0;
        
        double bytesWritten = static_cast<double>(deltaSectors) * SECTOR_SIZE_BYTES;
        double seconds = duration / 1000000.0;
        
        diskWriteSpeed_ = static_cast<int>((bytesWritten / 1024.0) / seconds);
    }

    prevDiskSectors_ = totalSectorsWritten;
}

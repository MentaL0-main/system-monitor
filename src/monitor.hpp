#pragma once

#include <cstddef>
#include <chrono>

class Monitor {
public:
    Monitor();
    void update();
    
    short getCpuUsage() const noexcept { return cpuUsage_; }
    int getMemUsage() const noexcept { return memUsage_ / 1024; }
    int getTotalMem() const noexcept { return totalMem_ / 1024; }
    int getDiskWriteSpeed() const noexcept { return diskWriteSpeed_; }

private:
    short cpuUsage_ = 0;
    int memUsage_ = 0;
    int totalMem_ = 0;
    int diskWriteSpeed_ = 0;

    unsigned long long prevCpuTotal_ = 0;
    unsigned long long prevCpuIdle_ = 0;
    unsigned long long prevDiskSectors_ = 0;
    std::chrono::steady_clock::time_point lastUpdate_;

    static constexpr std::size_t SECTOR_SIZE_BYTES = 512;
    
    void updateCpu();
    void updateMemory();
    void updateDisk();
};

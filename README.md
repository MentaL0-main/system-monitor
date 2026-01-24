# System Monitor

System Monitor is a lightweight system monitor for Linux written in C++17. It provides real-time statistics on CPU load, RAM usage and disk write speed by working directly with/proc kernel file system.
<img src=". /screenshots/screenshot.png" alt="Description" width="1200" height="900">

## Features

- Zero Dependencies: Does not require external libraries. Only the standard C++ library.
- Accurate CPU Usage: Calculation of processor load through delta states (takes into account user, system, iowait, steal etc.).
- Smart Memory: Displays real occupied memory based on MemAvailable, not just MemFree.
- Auto-Disk Discovery: Automatically detects active physical disks (sda, sdb, nvme) and adds up their write speed, ignoring virtual devices and partitions.
-Modern C++: Use of RAII, std::chrono, steady_clock and type-safe parsing.
- Beautiful UI: Intuitive console interface with color display and Progress Bars.

-----------------------------------------------------------------------
Architecture

The project is based on the principles of separation of responsibilities (SRP):
-Monitor: The core of the system. Responsible for parsing /proc/stat, /proc/meminfo and /proc/diskstats.
-Main: View layer. Responsible for ANSI interface rendering and update cycle management.

----------------------------------------------------------------------------
Quick start

Requirements
- Compiler supporting C++17 (g++ 7+ or clang 5+).
- Linux operating system.
- CMake
- Make

Build with CMake and run

`
git clone https://github.com/MentaL0-main/system-monitor.git
cd system-monitor
mkdir build && cd build
cmake .
make -j3 && . /system-monitor
`

Note: some distributions may require root to read /proc/diskstats correctly.

-------------------------------------------------------------------------------
Interface

The program displays data in the following format:
- CPU Usage: Graphic strip (green red) + percent.
-Memory: Volume in MB + percentage of usage of the total volume.
- Disk Write: The current write speed in MB/C, calculated for the refresh interval.

How it works

1. CPU: Calculate the difference between the active time of the processor and the idle time between two iterations.
2. RAM: Takes MemTotal - MemAvailable, which is the most accurate representation of the memory used in modern Linux.
3. Disk: The number of sectors from /proc/diskstats is read, the difference is calculated, multiplied by 512 bytes and divided by the exact time passed between measurements (via steady_clock).

-------------------------------------------------------------------------------

License

The project is distributed under MIT license. Use freely for your purposes.

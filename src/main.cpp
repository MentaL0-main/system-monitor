#include "monitor.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <string>
#include <csignal>

static volatile sig_atomic_t g_keepRunning = 1;

void signalHandler(int signum) {
    g_keepRunning = 0;
}

namespace Ansi {
    const std::string RESET = "\033[0m";
    const std::string CLEAR_SCREEN = "\033[2J";
    const std::string CURSOR_HOME = "\033[H";
    const std::string BOLD = "\033[1m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string GRAY = "\033[90m";
}

std::string progressBar(int current, int max, int width, const std::string& color = Ansi::BRIGHT_GREEN) {
    if (max == 0) max = 1;
    int filled = (static_cast<double>(current) / max) * width;
    
    std::string bar = color;
    for (int i = 0; i < width; ++i) {
        bar += (i < filled) ? "█" : "░";
    }
    bar += Ansi::RESET;
    return bar;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signalHandler);

    std::chrono::milliseconds updateInterval = std::chrono::seconds(1);

    if (argc > 1) {
        try {
            int interval_sec = std::stoi(argv[1]);
            if (interval_sec <= 0) {
                std::cerr << Ansi::BRIGHT_RED << "Error: Update interval must be a positive integer." << Ansi::RESET << "\n";
                std::cerr << "Usage: " << argv[0] << " [update_interval_seconds]" << "\n";
                return 1;
            }
            updateInterval = std::chrono::seconds(interval_sec);
        } catch (const std::invalid_argument& e) {
            std::cerr << Ansi::BRIGHT_RED << "Error: Invalid update interval provided." << Ansi::RESET << "\n";
            std::cerr << "Usage: " << argv[0] << " [update_interval_seconds]" << "\n";
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << Ansi::BRIGHT_RED << "Error: Update interval out of range." << Ansi::RESET << "\n";
            std::cerr << "Usage: " << argv[0] << " [update_interval_seconds]" << "\n";
            return 1;
        }
    }

    Monitor monitor;

    std::cout << Ansi::GRAY << "Press Ctrl+C to exit." << Ansi::RESET << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    while (g_keepRunning) {
        std::cout << Ansi::CLEAR_SCREEN << Ansi::CURSOR_HOME;

        monitor.update();

        short cpuUsage = monitor.getCpuUsage();
        int memUsageMB = monitor.getMemUsage();
        int totalMemMB = monitor.getTotalMem();
        int diskWriteSpeedKBps = monitor.getDiskWriteSpeed();

        std::cout << Ansi::BOLD << Ansi::BRIGHT_BLUE << "=== System Monitor ===" << Ansi::RESET << "\n";
        std::cout << "----------------------\n";

        std::cout << Ansi::BRIGHT_CYAN << "CPU Usage: " << Ansi::RESET
                  << std::setw(3) << cpuUsage << "% "
                  << progressBar(cpuUsage, 100, 20, (cpuUsage > 80 ? Ansi::BRIGHT_RED : Ansi::BRIGHT_GREEN))
                  << "\n";

        double memUsagePct = 0.0;
        if (totalMemMB > 0) {
            memUsagePct = (static_cast<double>(memUsageMB) / totalMemMB) * 100.0;
        }
        std::cout << Ansi::BRIGHT_MAGENTA << "Mem Usage: " << Ansi::RESET
                  << std::setw(4) << memUsageMB << "MB / "
                  << totalMemMB << "MB "
                  << "(" << std::fixed << std::setprecision(1) << memUsagePct << "%) "
                  << progressBar(memUsageMB, totalMemMB, 20, (memUsagePct > 80 ? Ansi::BRIGHT_RED : Ansi::BRIGHT_YELLOW))
                  << "\n";

        std::cout << Ansi::BRIGHT_YELLOW << "Disk Write: " << Ansi::RESET
                  << std::setw(7) << std::fixed << std::setprecision(2) << static_cast<double>(diskWriteSpeedKBps) / 1024.0 << " MB/s"
                  << "\n";
        
        std::cout << "----------------------\n";

        std::this_thread::sleep_for(updateInterval);
    }

    std::cout << Ansi::CURSOR_HOME << Ansi::CLEAR_SCREEN;
    std::cout << Ansi::BRIGHT_GREEN << "Monitor stopped. Goodbye!" << Ansi::RESET << "\n";

    return 0;
}

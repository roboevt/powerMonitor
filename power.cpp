#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <thread>

int main() {
    std::cout << "Hello" << std::endl;
    uint64_t msr_value;
    double energy_units = pow(0.5, 14);
    int fd = open("/dev/cpu/0/msr", O_RDONLY);
    auto lastTime = std::chrono::high_resolution_clock::now();
    double lastJoules = 0;
    while(true) {
        pread(fd, &msr_value, 8, 0x611);
        double currentJoules = msr_value * energy_units;
        double joules = currentJoules - lastJoules;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime).count();
        auto watts = joules / (micros / 1000000.0);
        std::cout << "\e[A\r\e[0K" << watts << "watts" << std::endl;
        lastTime = currentTime;
        lastJoules = currentJoules;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}
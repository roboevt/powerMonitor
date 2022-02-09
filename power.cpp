#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#define PKG_ENERGY_STATUS 0x611
#define PP0_ENERGY_STATUS 0x639
#define PP1_ENERGY_STATUS 0x641
#define MSR_DRAM_ENERGY_STATUS 0x619

class Power {
    std::chrono::time_point<std::chrono::high_resolution_clock> 
    lastTime = std::chrono::high_resolution_clock::now();
    double lastJoules;
    
    double energy_units = pow(0.5, 14);

    double getWatts(off_t offset) {
        uint64_t msr_value;
        int fd = open("/dev/cpu/0/msr", O_RDONLY);
        pread(fd, &msr_value, 8, offset);
        double currentJoules = msr_value * energy_units;
        double deltaJoules = currentJoules - lastJoules;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime).count();
        auto watts = deltaJoules / (deltaTime / 1e6);
        lastTime = currentTime;
        lastJoules = currentJoules;
        return watts;
    }

    public:
    Power() {
        //int fd = open("/dev/cpu/0/msr", O_RDONLY);
        //pread(fd, &msr_value, 8, PKG_ENERGY_STATUS);
        //double currentJoules = msr_value * energy_units;
    }

    
    double getPKGWatts() {
        return getWatts(PKG_ENERGY_STATUS);
    }
    double getPP0Watts() {
        return getWatts(PP0_ENERGY_STATUS);
    }
    double getPP1Watts() {
        return getWatts(PP1_ENERGY_STATUS);
    }
    double getDRAMWatts() {
        return getWatts(MSR_DRAM_ENERGY_STATUS);
    }
};

int main() {
    Power power;
    std::cout << std::endl;
    while(true) {
        std::cout << "\e[A\r\e[0K" << "PKG: " << power.getPKGWatts() << "w\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}

//Sources:
//Register names + addresses:
//https://github.com/razvanlupusoru/Intel-RAPL-via-Sysfs/blob/master/intel_rapl_power.c
//
//Accessing mode:
//https://dl.acm.org/doi/pdf/10.1145/3177754
//https://cdrdv2.intel.com/v1/dl/getContent/671200

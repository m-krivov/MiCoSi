
#pragma once

#include "Defs.h"

class HardwareScanner;

// Container with the basic information about CPU and GPU models
class HardwareInfo
{
  public:
    class DeviceInfo
    {
      private:
        std::string _name;
        size_t _cores;
        double _freq;

      public:
        inline DeviceInfo()
          : _name("N/A"), _cores(0), _freq(0.0)
        { /*nothing*/ }

        inline DeviceInfo(const std::string name, size_t cores, double freq)
          : _name(name), _cores(cores), _freq(freq)
        { /*nothing*/ }

        const std::string &Name() const { return _name; }
        size_t Cores() const { return _cores; }
        double FrequencyMHz() const { return _freq; }
    };

  private:
    DeviceInfo _cpu;
    std::vector<DeviceInfo> _gpus;

    inline HardwareInfo(const DeviceInfo &cpu, const std::vector<DeviceInfo> &gpus)
      : _cpu(cpu), _gpus(gpus)
    { /*nothing*/ }

  public:
    const DeviceInfo &CPU() const { return _cpu; }
    const std::vector<DeviceInfo> &GPUs() const { return _gpus; }

  friend class HardwareScanner;
};

// Scans the hardware and tries to detect its parameters
class HardwareScanner
{
  private:
    HardwareScanner() = delete;
    HardwareScanner(const HardwareScanner &) = delete;
    void operator =(const HardwareScanner &) = delete;

  public:
    static HardwareInfo Scan();
};

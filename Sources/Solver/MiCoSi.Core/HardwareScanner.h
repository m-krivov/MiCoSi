#pragma once
#include "Defs.h"

class HardwareScanner;

// Container with the basic information about CPU and GPU models
class HardwareInfo
{
  public:
    class DeviceInfo
    {
      public:
        DeviceInfo()
          : _name("N/A"), _cores(0), _freq(0.0)
        { /*nothing*/ }

        DeviceInfo(const std::string name, size_t cores, double freq)
          : _name(name), _cores(cores), _freq(freq)
        { /*nothing*/ }

        DeviceInfo(const DeviceInfo &) = default;
        DeviceInfo &operator =(const DeviceInfo &) = default;

        const std::string &Name() const { return _name; }
        size_t Cores() const { return _cores; }
        double FrequencyMHz() const { return _freq; }

      private:
        std::string _name;
        size_t _cores;
        double _freq;
    };

    const DeviceInfo &CPU() const { return _cpu; }
    const std::vector<DeviceInfo> &GPUs() const { return _gpus; }

  private:
    DeviceInfo _cpu;
    std::vector<DeviceInfo> _gpus;

    HardwareInfo(const DeviceInfo &cpu, const std::vector<DeviceInfo> &gpus)
      : _cpu(cpu), _gpus(gpus)
    { /*nothing*/ }

  friend class HardwareScanner;
};

// Scans the hardware and tries to detect its parameters
class HardwareScanner
{
  public:
    HardwareScanner() = delete;
    HardwareScanner(const HardwareScanner &) = delete;
    void operator =(const HardwareScanner &) = delete;

    static HardwareInfo Scan();
};

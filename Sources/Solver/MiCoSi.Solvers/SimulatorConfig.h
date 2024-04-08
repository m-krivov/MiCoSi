#pragma once
#include "MiCoSi.Core/Defs.h"

// Describes, which implementation must be used and on which device (if possible)
class SimulatorConfig
{
  public:
    enum SimulatorType
    {
      CPU        = 1,
      CUDA       = 2,
    };

    SimulatorConfig()
    { _type = Default()._type; _devNumber = Default()._devNumber; }

    SimulatorConfig(SimulatorType type, int deviceNumber = -1)
      : _type(type), _devNumber(std::max(-1, deviceNumber))
    { /*nothing*/ }

    SimulatorConfig(const SimulatorConfig &) = default;
    SimulatorConfig &operator =(const SimulatorConfig &) = default;

    inline SimulatorType Type() const
    { return _type; }
    
    inline bool HasDeviceNumber(int &number) const
    { number = _devNumber; return _devNumber >= 0; }

    static SimulatorConfig Parse(const std::string &str);

    static std::string Serialize(SimulatorConfig config);

    static SimulatorConfig Default();

  private:
    SimulatorType _type;
    int _devNumber;
};

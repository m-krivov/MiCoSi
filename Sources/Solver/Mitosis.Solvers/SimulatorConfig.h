
#pragma once

#include "Mitosis.Core/Defs.h"

// Describes, which implementation must be used and on which device (if possible).
class SimulatorConfig
{
  public:
    enum SimulatorType
    {
      CPU        = 1,
      CUDA      = 2,
      EXPERIMENTAL  = 3
    };

  private:
    SimulatorType _type;
    int _devNumber;

  public:
    inline SimulatorConfig()
    { _type = Default()._type; _devNumber = Default()._devNumber; }

    inline SimulatorConfig(SimulatorType type, int deviceNumber = -1)
      : _type(type), _devNumber(deviceNumber)
    { /*nothing*/ }

    inline SimulatorType Type() const
    { return _type; }
    
    inline bool HasDeviceNumber(int &number) const
    { number = _devNumber; return _devNumber >= 0; }

    static SimulatorConfig Parse(const char *str);
    static std::string Serialize(SimulatorConfig config);
    static SimulatorConfig Default();
};

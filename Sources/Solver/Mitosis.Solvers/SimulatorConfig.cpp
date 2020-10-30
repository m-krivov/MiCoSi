
#include "SimulatorConfig.h"

namespace
{
constexpr const char *CPU_STR = "cpu";
constexpr const char *CUDA_STR = "cuda";
}

//-----------------------
//--- SimulatorConfig ---
//-----------------------

SimulatorConfig SimulatorConfig::Parse(const std::string &str)
{
  int deviceNumber = -1;
  std::string deviceType;

  auto delimiter = str.find(':');
  if (delimiter != std::string::npos)
  {
    if (delimiter + 1 == str.size())
    { throw std::runtime_error("wrong config string, no device number"); }

    std::istringstream iss(str.substr(delimiter + 1, str.size() - delimiter - 1));
    if (!(iss >> deviceNumber) || !iss.eof() || deviceNumber < 0)
    { throw std::runtime_error("wrong config string, bad device number"); }

    deviceType = str.substr(0, delimiter);
  }
  else {
    deviceType = str;
  }

  if (deviceType != CPU_STR && deviceType != CUDA_STR)
  { throw std::runtime_error("wrong config string, unknown solver"); }

  return SimulatorConfig(deviceType == CPU_STR ? SimulatorConfig::CPU : SimulatorConfig::CUDA,
                         deviceNumber);
}

std::string SimulatorConfig::Serialize(SimulatorConfig config)
{
  std::stringstream res;

  if (config.Type() == SimulatorConfig::CPU)
  { res << CPU_STR; }
  else if (config.Type() == SimulatorConfig::CUDA)
  { res << CUDA_STR; }
  else
  { throw std::runtime_error("internal error, wrong simulator type"); }

  int deviceNumber = 0;
  if (config.HasDeviceNumber(deviceNumber))
  { res << ':' << deviceNumber; }

  return res.str();
}

SimulatorConfig SimulatorConfig::Default()
{
  return SimulatorConfig(CPU);
}

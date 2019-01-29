
#include "SimulatorConfig.h"

//-----------------------
//--- SimulatorConfig ---
//-----------------------

SimulatorConfig SimulatorConfig::Parse(const char *str)
{
  std::string sStr(str);

  //Expermental check.
  if (sStr == "experimental")
    return SimulatorConfig(SimulatorConfig::EXPERIMENTAL);
  else if (sStr == "cpu")
    return SimulatorConfig(SimulatorConfig::CPU);

  //Splitting.
  if (sStr.size() < 4)
    throw std::runtime_error("config string is too short");

  std::string solverStr = sStr.substr(0, 4);
  std::string devStr;
  if (sStr.size() >= 5)
  {
    if (sStr[4] != ':')
      throw std::runtime_error("wrong config string, separator ':' is expeted");
    if (sStr.size() == 5)
      throw std::runtime_error("wrong config string, device number not found");
    devStr = sStr.substr(5, sStr.size() - 5);
  }

  //Casting.
  int deviceNumber = -1;

  if (solverStr != "cuda")
    throw std::runtime_error("wrong config string, unknown solver");

  if (!devStr.empty())
  {
    std::istringstream iss(devStr);
    if (!(iss >> deviceNumber) || !iss.eof() || deviceNumber < 0)
      throw std::runtime_error("wrong config string, bad device number");
  }

  return SimulatorConfig(SimulatorConfig::CUDA, deviceNumber);
}

std::string SimulatorConfig::Serialize(SimulatorConfig config)
{
  if (config.Type() == SimulatorConfig::EXPERIMENTAL)
    return "experimental";
  else if (config.Type() == SimulatorConfig::CPU)
    return "cpu";
  else if (config.Type() == SimulatorConfig::CUDA)
  {
    std::stringstream ss;
    ss << "cuda";
    int devNum = 0;
    if (config.HasDeviceNumber(devNum))
      ss << ":" << devNum;
    return ss.str();
  }
  else
    throw std::runtime_error("Internal error - wrong simulator type");
}

SimulatorConfig SimulatorConfig::Default()
{
  return SimulatorConfig(CPU);
}

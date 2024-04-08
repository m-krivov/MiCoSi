#include "Versions.h"
#include <sstream>

//---------------
//--- Version ---
//---------------

std::string Version::ToString() const
{
  std::stringstream ss;
  ss << _major << "." << _minor << "." << _build;
  return ss.str();
}

//----------------------
//--- CurrentVersion ---
//----------------------

Version *CurrentVersion::_programVersion = new Version(0, 9, 2, "January, 2021");
int CurrentVersion::_fileFormatVersion = 2;

std::string CurrentVersion::CompilationFlags()
{
  std::vector<std::string> flags;

#if defined(MICOSI_PRECISION_FP32)
  flags.emplace_back("MICOSI_PRECISION_FP32");
#elif defined(MICOSI_PRECISION_FP64)
  flags.emplace_back("MICOSI_PRECISION_FP64");
#endif

#if defined(MICOSI_RNG_LCG)
  flags.emplace_back("MICOSI_RNG_LCG");
#elif defined(MICOSI_RNG_MTG)
  flags.emplace_back("MICOSI_RNG_MTG");
#endif

  std::ostringstream ss;
  for (size_t i = 0; i < flags.size(); i++)
  {
    if (i > 0)
    { ss << "; "; }
    ss << flags[i];
  }

  return ss.str();
}


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

Version *CurrentVersion::_programVersion = new Version(0, 9, 0, "January, 2019");

int CurrentVersion::_fileFormatVersion = 0;

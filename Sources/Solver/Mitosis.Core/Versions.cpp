
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

int CurrentVersion::_fileFormatVersion = 1;

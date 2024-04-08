#include "Converter.h"
#include <cstdio>
#include <stdexcept>
#include <sstream>

#ifndef WIN32
#define sprintf_s sprintf
#define sscanf_s sscanf
#endif

//-----------------
//--- Converter ---
//-----------------

template <class T>
std::string ToStringAux(T val, const char *format)
{
  char buf[1024];
  sprintf_s(buf, format, val);
  return std::string(buf);
}

std::string Converter::ToString(int val)
{ return ToStringAux(val, "%d"); }

std::string Converter::ToString(double val)
{ return ToStringAux(val, "%e"); }

std::string Converter::ToString(offset_t val)
{ return ToStringAux(val, "%lld"); }

template <class T>
static inline bool TryParseAux(const std::string &str, T &val)
{
  std::istringstream ss(str);
  return (ss >> val) && ss.eof();
}

template <class T>
static inline void ParseAux(const std::string &str, T &val)
{
  if (!TryParseAux(str, val))
    throw std::runtime_error("Internal error at Converter::Parse(): cannot convert value");
}

void Converter::Parse(const std::string &str, int &val)
{ ParseAux(str, val); }

bool Converter::TryParse(const std::string &str, int &val)
{ return TryParseAux(str, val); }

void Converter::Parse(const std::string &str, double &val)
{ ParseAux(str, val); }

bool Converter::TryParse(const std::string &str, double &val)
{ return TryParseAux(str, val); }

void Converter::Parse(const std::string &str, offset_t &val)
{ ParseAux(str, val); }

bool Converter::TryParse(const std::string &str, offset_t &val)
{ return TryParseAux(str, val); }

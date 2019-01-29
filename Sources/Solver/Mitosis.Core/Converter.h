
#pragma once

#include "Defs.h"

// Some syntactic sugar, to avoid the awful C++ casting.
class Converter
{
  public:
    Converter() = delete;
    Converter(const Converter &) = delete;
    void operator =(const Converter &) = delete;

    static std::string ToString(int val);
    static std::string ToString(double val);
    static std::string ToString(offset_t val);

    static void Parse(const std::string &str, int &val);
    static bool TryParse(const std::string &str, int &val);
    static void Parse(const std::string &str, double &val);
    static bool TryParse(const std::string &str, double &val);
    static void Parse(const std::string &str, offset_t &val);
    static bool TryParse(const std::string &str, offset_t &val);
};

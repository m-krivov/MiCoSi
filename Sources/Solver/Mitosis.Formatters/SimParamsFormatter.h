
#pragma once

#include "Mitosis.Core/Defs.h"
#include "Mitosis.Core/SimParams.h"

// Static class that exports/imports simulation parameters.
// The can be stored in user friendly "name=value" file format or as XML-node.
class SimParamsFormatter
{
  private:
    inline SimParamsFormatter() = delete;
    inline SimParamsFormatter(const SimParamsFormatter &) = delete;
    inline void operator=(const SimParamsFormatter &) = delete;

  public:
    static std::string ExportAsProps(SimParams *params);
    static void ImportAsProps(SimParams *params, const std::string &props);
};

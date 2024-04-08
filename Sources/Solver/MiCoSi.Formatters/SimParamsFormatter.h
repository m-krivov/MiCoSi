#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Core/SimParams.h"

// Static class that exports/imports simulation parameters.
// The can be stored in user friendly "name=value" file format or as XML-node.
class SimParamsFormatter
{
  public:
    SimParamsFormatter() = delete;
    SimParamsFormatter(const SimParamsFormatter &) = delete;
    SimParamsFormatter &operator=(const SimParamsFormatter &) = delete;

    static std::string ExportAsProps(SimParams *params);
    static void ImportAsProps(SimParams *params, const std::string &props);
};

#pragma once
#include "MiCoSi.Core/Defs.h"

class IErrorFormatter
{
  public:
    enum ErrorType
    {
      WrongParams,
      RuntimeError,
      CorruptedFiles,
      FailedToRepair
    };

    IErrorFormatter() = default;
    IErrorFormatter(const IErrorFormatter &) = delete;
    IErrorFormatter &operator =(const IErrorFormatter &) = delete;
    virtual ~IErrorFormatter() = default;

    virtual void PrintError(ErrorType type, const char *errDesc) abstract;
};

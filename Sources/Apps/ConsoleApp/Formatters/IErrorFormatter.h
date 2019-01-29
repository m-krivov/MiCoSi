
#pragma once

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

  public:
    virtual void PrintError(ErrorType type, const char *errDesc) = 0;

    virtual ~IErrorFormatter() { /*nothing*/ }
};

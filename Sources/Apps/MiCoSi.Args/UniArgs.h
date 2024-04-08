#pragma once
#include "MiCoSi.Core/Defs.h"

// Base class for CLI argument parsers
class UniArgs
{
  public:
    class UniRecord;

    UniArgs(const UniArgs &) = delete;
    UniArgs &operator =(UniArgs) = delete;
    virtual ~UniArgs() = default;

    void Verify();

    void Import(std::vector<std::string> &args);
    void Import(std::string args);
    void Import(int argc, char **argv);
    void Export(std::vector<std::string> &args);
    void Export(std::string &args);
    
    virtual std::string HelpMessage() = 0;

  protected:
    UniArgs() = default;

    void Register(const std::string &id, bool &value);
    void Register(const std::string &id, int &value, bool (*validator)(int) = nullptr);
    void Register(const std::string &id, double &value, bool (*validator)(double) = nullptr);
    void Register(const std::string &id, std::string &value, bool (*validator)(std::string) = nullptr);
    void Reset(const std::string &id);

    void SingleOption(const std::string &id);
    void IncompatibleWith(const std::string &id, const std::string &opt, const char *value = nullptr);  // magic!

  private:
    std::map<std::string, std::shared_ptr<UniRecord> > _records;
    std::vector<std::string> _singleOptions;
    std::vector<std::pair<std::string, std::pair<std::string, std::string> > > _incompatibleOptions;
};

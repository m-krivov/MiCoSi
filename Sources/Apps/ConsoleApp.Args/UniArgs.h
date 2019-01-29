
#pragma once

#include "Mitosis.Core/Defs.h"

// Base class for CLI argument parsers.
class UniArgs
{
  private:
    class UniRecord;

  private:
    std::map<std::string, std::shared_ptr<UniRecord> > _records;
    std::vector<std::string> _singleOptions;
    std::vector<std::pair<std::string, std::pair<std::string, std::string> > > _incompatibleOptions;

  protected:
    void Register(const std::string &id, bool &value);
    void Register(const std::string &id, int &value, bool (*validator)(int) = NULL);
    void Register(const std::string &id, double &value, bool (*validator)(double) = NULL);
    void Register(const std::string &id, std::string &value, bool (*validator)(std::string) = NULL);
    void Reset(const std::string &id);

    void SingleOption(const std::string &id);
    void IncompatibleWith(const std::string &id, const std::string &opt, const char *value = NULL);  //magic!

    inline UniArgs() { /*nothing*/ };

  public:
    void Verify();

    void Import(std::vector<std::string> &args);
    void Import(std::string args);
    void Import(int argc, char **argv);
    void Export(std::vector<std::string> &args);
    void Export(std::string &args);
    
    virtual std::string HelpMessage() = 0;

    virtual ~UniArgs() { /*nothing*/ }

  // Some auxiliary classes.
  private:
    class UniRecord
    {
      private:
        std::string _id;
        bool _isFlag;
        std::string _defaultValue;

      protected:
        UniRecord(const std::string &id, bool isFlag, const std::string &defaultValue)
          : _id(id), _isFlag(isFlag), _defaultValue(defaultValue)
        { /*nothing*/ }

      public:
        inline std::string ID() { return _id; }
        inline bool IsFlag() { return _isFlag; }
        
        inline std::string DefaultValue() { return _defaultValue; }
        virtual void SetValue(const std::string &value) = 0;
        virtual std::string GetValue() = 0;
        virtual void Verify() = 0;
        virtual ~UniRecord() { /*nothing*/ }
    };

    template <class T>
    class TemplatedRecord : public UniRecord
    {
      private:
        static std::string ToString(T value)
        {
          std::stringstream ss;
          ss << value;
          return ss.str();
        }

        static T FromString(const std::string &value)
        {
          T tvalue = T();
          std::istringstream stream(value);
          if (!(stream >> tvalue) || !stream.eof())
          {
            std::stringstream err;
            err << "Cannot parse value \"" << value << "\"";
            throw std::runtime_error(err.str());
          }
          return tvalue;
        }

      private:
        bool (*_validator)(T);
        T *_value;

      public:
        inline TemplatedRecord(const std::string &id, bool isFlag, bool (*validator)(T), T *value)
          : UniRecord(id, isFlag, ToString(*value)), _validator(validator), _value(value)
        { /*nothing*/ }

        virtual void SetValue(const std::string &value)
        { *_value = FromString(value); }

        virtual std::string GetValue()
        { return ToString(*_value); }

        virtual void Verify()
        {
          if (_validator != NULL && !_validator(*_value))
          {
            std::stringstream err;
            err << "Unsupported value for \"" << ID() << "\" option (\"" << *_value << "\" provided)";
            throw std::runtime_error(err.str());
          }
        }
    };
};

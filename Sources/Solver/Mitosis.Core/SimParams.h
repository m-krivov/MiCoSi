
#pragma once

#include "Defs.h"


// Class with information (without values) about all model parameters.
class SimParameter
{
  private:
    SimParameter() = delete;
    SimParameter(const SimParameter &) = delete;
    void operator =(const SimParameter &) = delete;

  public:
    // Definition of two static classes.
    class Int;
    class Double;

    // An universal descriptor for any parameter.
    template <class T>
    class Descriptor
    {
      private:
        T _default, _multiplier, _upper, _lower;
        bool _constant, _hasUpper, _hasLower;
        std::string _name;

      private:
        Descriptor(const std::string &name, bool constant,
               T defaultVal, T multiplierVal,
               bool hasLower, T lowerVal,
               bool hasUpper, T upperVal)
          : _name(name), _constant(constant),
            _default(defaultVal), _multiplier(multiplierVal),
            _upper(upperVal), _lower(lowerVal),
            _hasUpper(hasUpper), _hasLower(hasLower)
        { /*nothing*/ }

      public:
        Descriptor()
          : _name("not_set"), _constant(false),
            _default(0), _multiplier(0),
            _upper(false), _lower(0),
            _hasUpper(false), _hasLower(0)
        { /*nothing*/ }

        // The reference name with lower letters. Like "n_mt_total".
        const std::string &Name() const
        { return _name; }

        // Can parameter be updated during simulation? If so, returns false.
        bool Constant() const
        { return _constant; }

        // The value used by default.
        T DefaultValue() const
        { return _default; }

        // The value needed to be multiplied by in order to convert parameter to SI.
        T SiMultiplier() const
        { return _multiplier; }

        // Returns true and sets upper boundary (if applicable, false otherwise).
        bool HasUpperBoundary(T &upper) const
        { upper = _upper; return _hasUpper; }

        // Returns true and sets lower boundary (if applicable, false otherwise).
        bool HasLowerBoundary(T &lower) const
        { _lower = lower; return _hasLower; }

      friend class Int;
      friend class Double;
    };

  private:
    // Base class with common logic for Int and Double sub-classes.
    template <class T1, class T2>
    class BaseType
    {
      private:
        std::vector<T1> _all;
        std::map<T1, Descriptor<T2> > _descs;
        std::map<std::string, T1> _names;

      public:
        void Register(T1 type, class Descriptor<T2> &desc)
        {
          _all.push_back(type);
          _descs[type] = desc;
          _names[desc.Name()] = type;
        }

        const std::vector<T1> &All() const
        { return _all; }

        bool TryParse(std::string str, T1 &res) const
        {
          for (size_t i = 0; i < str.size(); i++)
            str[i] = (char)tolower(str[i]);

          auto iter = _names.find(str);
          if (iter == _names.end())
            return false;
          else
          { res = iter->second; return true; }
        }

        const Descriptor<T2> &Info(T1 param) const
        {
          auto iter = _descs.find(param);
          if (iter == _descs.end())
            throw std::runtime_error("Such parameter is not known");
          return iter->second;
        }
    };

  public:

    // Integer-based parameters - counts, flags, etc.
    class Int
    {
      private:
        Int() = delete;
        Int(const Int &) = delete;
        void operator =(const Int &) = delete;

      public:
        enum Type
        {
          N_MT_Total                = 0,
          N_Cr_Total                = 1,
          Spring_Brake_Type         = 2,
          Spring_Brake_MTs          = 3,
          N_Nods                    = 4,
          Spring_Type               = 5,
          Frozen_Coords             = 6,
          MT_Wrapping               = 7,
          MT_Lateral_Attachments    = 8,
          N_KMT_Max                 = 9
        };

      private:
        static BaseType<Type, int> *_ref;
        static void SafeInit();

      public:
        // Returns all available int-based parameters.
        static const std::vector<Type> &All()
        { SafeInit(); return _ref->All(); }

        // Tries to parse name and return the corresponding parameter.
        static bool TryParse(std::string str, Type &res)
        { SafeInit(); return _ref->TryParse(str, res); }

        // Returns information about parameter.
        static const Descriptor<int> &Info(Type param)
        { SafeInit(); return _ref->Info(param); }
    };

    // Double-based parameters - values, lengthes, coefficients, etc.
    class Double
    {
      private:
        Double() = delete;
        Double(const Double &) = delete;
        void operator =(const Double &) = delete;

      public:
        enum Type
        {
          L_Poles              = 0,
          R_Cell               = 1,
          Spring_Brake_Force   = 2,
          V_Pol                = 3,
          V_Dep                = 4,
          F_Cat                = 5,
          F_Res                = 6,
          Gamma                = 7,
          Ieta                 = 8,
          L1                   = 9,
          L3                   = 10,
          Eps                  = 11,
          K_On                 = 12,
          K_Off                = 13,
          Dt                   = 14,
          T_End                = 15,
          Save_Freq_Micro      = 16,
          Save_Freq_Macro      = 17,
          Cr_L                 = 18,
          Cr_Kin_L             = 19,
          Cr_Kin_D             = 20,
          Cr_Kin_Angle         = 21,
          Cr_Hand_D            = 22,
          Const_A              = 23,
          Const_B              = 24,
          D_Trans              = 25,
          D_Rot                = 26,
          Spring_Length        = 27,
          Spring_K             = 28
        };

      private:
        static BaseType<Type, double> *_ref;
        static void SafeInit();

      public:
        // Returns all available double-based parameters.
        static const std::vector<Type> &All()
        { SafeInit(); return _ref->All(); }

        // Tries to parse name and return the corresponding parameter.
        static bool TryParse(std::string str, Type &res)
        { SafeInit(); return _ref->TryParse(str, res); }

        // Returns information about parameter.
        static const Descriptor<double> &Info(Type param)
        { SafeInit(); return _ref->Info(param); }
    };
};

// Class that provides values of the simulation parameters.
class SimParams
{
  public:
    // Mode for accessing container with simulation parameters.
    // Access should be set by object's owner according to usage scenario.
    class Access
    {
      public:
        enum Type
        {
          Initialize  = 1,  // Value of any parameter can be changed.
          Update      = 2,  // Values of the the constant parameters cannot be updated.
          ReadOnly    = 3   // The "set" operation cannot be applyed to any parameter.
        };
    };

  private:
    Access::Type _access;
    std::map<SimParameter::Int::Type, int> _intValues;
    std::map<SimParameter::Double::Type, double> _doubleValues;

    SimParams(const SimParams &) = delete;
    void operator =(const SimParams &) = delete;

  public:
    // Creates instance with default values.
    SimParams();

    int GetParameter(SimParameter::Int::Type type) const;
    void SetParameter(SimParameter::Int::Type type, int value);
    double GetParameter(SimParameter::Double::Type type, bool convertToSI = false) const;
    void SetParameter(SimParameter::Double::Type type, double value, bool isSI = false);
    std::string GetParameter(const std::string &name, bool convertToSI = false) const;
    void SetParameter(const std::string &name, const std::string &value, bool isSI = false);

    // Sets new mode for container.
    void SetAccess(Access::Type access)
    { _access = access; }

    // Returns current access mode.
    Access::Type GetAccess()
    { return _access; }

    // Assings default values to all parameters.
    void SetDefault();

    // Exports values as set of name + value records.
    std::vector<std::pair<std::string, std::string> > ExportValues() const;

    // Imports values stores as name + value records.
    void ImportValues(const std::vector<std::pair<std::string, std::string> > &values);
};

// Static class that provides references for parameters that will be used for simulation.
class GlobalSimParams
{
  private:
    static SimParams *_ref;

  public:
    GlobalSimParams() = delete;
    GlobalSimParams(const GlobalSimParams &) = delete;
    void operator =(const GlobalSimParams &) = delete;

    static SimParams *GetRef()
    { return _ref; }
};

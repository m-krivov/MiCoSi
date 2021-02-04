
#pragma once

#include "ConsoleApp.Args/MitosisArgs.h"

#include <string>

namespace Mitosis
{
  public enum class LaunchMode
  {
    New        = ::LaunchMode::New,
    Restart    = ::LaunchMode::Restart,
    Continue   = ::LaunchMode::Continue,
    Fix        = ::LaunchMode::Fix,
    Info       = ::LaunchMode::Info,
    Help       = ::LaunchMode::Help
  };

  public enum class SimulatorType
  {
    CPU           = ::SimulatorConfig::CPU,
    CUDA          = ::SimulatorConfig::CUDA,
  };

  public ref class SimulatorConfig : System::IDisposable
  {
    private:
      ::SimulatorConfig *_obj;

    private:
      void SafeRelease()
      {
        if (_obj != NULL) delete _obj;
        _obj = NULL;
      }

    internal:
      SimulatorConfig(::SimulatorConfig config)
      { _obj = new ::SimulatorConfig(config); }

      ::SimulatorConfig *GetObject()
      { return _obj; }

    public:
      SimulatorConfig()
      { _obj = new ::SimulatorConfig(); }

      SimulatorConfig(SimulatorType type)
      { _obj = new ::SimulatorConfig((::SimulatorConfig::SimulatorType)type); }

      SimulatorConfig(SimulatorType type, int deviceNumber)
      { _obj = new ::SimulatorConfig((::SimulatorConfig::SimulatorType)type, deviceNumber); }

      property SimulatorType Type
      { SimulatorType get() { return (SimulatorType)_obj->Type(); } }

      bool HasDeviceNumber(int %number)
      {
        int n = 0;
        bool res = _obj->HasDeviceNumber(n);
        number = n;
        return res;
      }

      ~SimulatorConfig()
      { SafeRelease(); }

      !SimulatorConfig()
      { SafeRelease(); }
  };

  public ref class CliArgs : System::IDisposable, System::ICloneable
  {
    public:
      enum class Option
      {
        Mode                   = ::MitosisArgs::Option::Mode,
        CellFile               = ::MitosisArgs::Option::CellFile,
        ConfigFile             = ::MitosisArgs::Option::ConfigFile,
        InitialConditionsFile  = ::MitosisArgs::Option::InitialConditionsFile,
        PoleCoordsFile         = ::MitosisArgs::Option::PoleCoordsFile,
        RngSeed                = ::MitosisArgs::Option::RngSeed,
        CellCount              = ::MitosisArgs::Option::CellCount,
        Solver                 = ::MitosisArgs::Option::Solver,
        CsvOutput              = ::MitosisArgs::Option::CsvOutput,
        PrintDelay             = ::MitosisArgs::Option::PrintDelay
      };

      static System::String ^OptionName(Option opt)
      {
        try
        { return gcnew System::String(MitosisArgs::Option::ToString((MitosisArgs::Option::Type)opt)); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      static System::String ^ModeName(LaunchMode mode)
      {
        try
        { return gcnew System::String(::LaunchMode::ToString((::LaunchMode::Type)mode)); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

    private:
      ::MitosisArgs *_obj;

    private:
      void SafeRelease()
      {
        if (_obj != NULL) delete _obj;
        _obj = NULL;
      }

      static std::string StrToStr(System::String ^str)
      {
        std::string res;
        res.resize(str->Length);
        for (int i = 0; i < str->Length; i++)
          res[i] = (char)str[i];
        return res;
      }

    public:
      CliArgs()
      { _obj = new ::MitosisArgs(); }

      ~CliArgs()
      { SafeRelease(); }

      !CliArgs()
      { SafeRelease(); }

      virtual System::Object ^Clone()
      {
        CliArgs ^res = gcnew CliArgs();
        res->SafeRelease();
        res->_obj = _obj->Clone();
        return res;
      }

      void Verify()
      {
        try
        { _obj->Verify(); }
        catch (std::exception &err)
        { throw gcnew System::ApplicationException(gcnew System::String(err.what())); }
      }

      void Reset(Option opt)
      {
        try
        { _obj->Reset((MitosisArgs::Option::Type)opt); }
        catch (std::exception &err)
        { throw gcnew System::ApplicationException(gcnew System::String(err.what())); }
      }

      System::String ^Export()
      {
        try
        {
          std::string res;
          _obj->Export(res);
          return gcnew System::String(res.c_str());
        }
        catch (std::exception &err)
        { throw gcnew System::ApplicationException(gcnew System::String(err.what())); }
      }

      void Import(System::String ^args)
      {
        try
        { _obj->Import(StrToStr(args)); }
        catch (std::exception &err)
        { throw gcnew System::ApplicationException(gcnew System::String(err.what())); }
      }

    public:
      property LaunchMode Mode
      {
        LaunchMode get() { return (LaunchMode)_obj->GetMode(); }
        void set(LaunchMode value) { _obj->SetMode((::LaunchMode::Type)value); }
      }

      property System::String ^CellFile
      {
        System::String ^get() { return gcnew System::String(_obj->GetCellFile().c_str()); }
        void set(System::String ^value) { _obj->SetCellFile(StrToStr(value)); }
      }

      property System::String ^ConfigFile
      {
        System::String ^get() { return gcnew System::String(_obj->GetConfigFile().c_str()); }
        void set(System::String ^value) { _obj->SetConfigFile(StrToStr(value)); }
      }

      property System::String ^InitialConditionsFile
      {
        System::String ^get()
        {
          const char *res = _obj->GetInitialConditionsFile();
          return res == NULL ? nullptr : gcnew System::String(res);
        }
        void set(System::String ^value)
        { _obj->SetInitialConditionsFile(System::String::IsNullOrEmpty(value) ? NULL : StrToStr(value).c_str()); }
      }

      property System::String ^PoleCoordsFile
      {
        System::String ^get()
        {
          const char *res = _obj->GetPoleCoordsFile();
          return res == NULL ? nullptr : gcnew System::String(res);
        }
        void set(System::String ^value)
        { _obj->SetPoleCoordsFile(System::String::IsNullOrEmpty(value) ? NULL : StrToStr(value).c_str()); }
      }

      property System::UInt32 UserSeed
      {
        System::UInt32 get() { return _obj->GetUserSeed(); }
        void set(System::UInt32 value) { _obj->SetUserSeed(value); }
      }

      property int CellCount
      {
        int get() { return _obj->GetCellCount(); }
        void set(int value) { _obj->SetCellCount(value); }
      }

      property SimulatorConfig ^Solver
      {
        SimulatorConfig ^get() { return gcnew SimulatorConfig(_obj->GetSolver()); }
        void set(SimulatorConfig ^value) { _obj->SetSolver(*(value->GetObject())); }
      }

      property bool CsvOutput
      {
        bool get() { return _obj->GetCsvOutput(); }
        void set(bool value) { _obj->SetCsvOutput(value); }
      }

      property double PrintDelay
      {
        double get() { return _obj->GetPrintDelay(); }
        void set(double value) { _obj->SetPrintDelay(value); }
      }

      static array<System::String ^> ^MultiplyCells(System::String ^filenameTemplate, int cellCount)
      {
        auto res = ::MitosisArgs::MultiplyCells(StrToStr(filenameTemplate).c_str(), cellCount);

        array<System::String ^> ^mres = gcnew array<System::String ^>((int)res.size());
        for (int i = 0; i < mres->Length; i++)
          mres[i] = gcnew System::String(res[i].c_str());

        return mres;
      }
  };
}

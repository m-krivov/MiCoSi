
#pragma once

#include "Mitosis.Core/SimParams.h"
#include "Mitosis.Formatters/SimParamsFormatter.h"

namespace Mitosis
{
  public ref class SimParameter abstract sealed
  {
    public:
      enum class Int
      {
        N_MT_Total                = ::SimParameter::Int::N_MT_Total,
        N_Cr_Total                = ::SimParameter::Int::N_Cr_Total,
        Spring_Brake_Type         = ::SimParameter::Int::Spring_Brake_Type,
        Spring_Brake_MTs          = ::SimParameter::Int::Spring_Brake_MTs,
        N_Nods                    = ::SimParameter::Int::N_Nods,
        Spring_Type               = ::SimParameter::Int::Spring_Type,
        Frozen_Coords             = ::SimParameter::Int::Frozen_Coords,
        MT_Wrapping               = ::SimParameter::Int::MT_Wrapping,
        MT_Lateral_Attachments    = ::SimParameter::Int::MT_Lateral_Attachments,
        N_KMT_Max                 = ::SimParameter::Int::N_KMT_Max
      };

      enum class Double
      {
        L_Poles              = ::SimParameter::Double::L_Poles,
        R_Cell               = ::SimParameter::Double::R_Cell,
        Spring_Brake_Force   = ::SimParameter::Double::Spring_Brake_Force,
        V_Pol                = ::SimParameter::Double::V_Pol,
        V_Dep                = ::SimParameter::Double::V_Dep,
        F_Cat                = ::SimParameter::Double::F_Cat,
        F_Res                = ::SimParameter::Double::F_Res,
        Gamma                = ::SimParameter::Double::Gamma,
        Ieta                 = ::SimParameter::Double::Ieta,
        L1                   = ::SimParameter::Double::L1,
        L3                   = ::SimParameter::Double::L3,
        Eps                  = ::SimParameter::Double::Eps,
        K_On                 = ::SimParameter::Double::K_On,
        K_Off                = ::SimParameter::Double::K_Off,
        Dt                   = ::SimParameter::Double::Dt,
        T_End                = ::SimParameter::Double::T_End,
        Save_Freq_Micro      = ::SimParameter::Double::Save_Freq_Micro,
        Save_Freq_Macro      = ::SimParameter::Double::Save_Freq_Macro,
        Cr_L                 = ::SimParameter::Double::Cr_L,
        Cr_Kin_L             = ::SimParameter::Double::Cr_Kin_L,
        Cr_Kin_D             = ::SimParameter::Double::Cr_Kin_D,
        Cr_Kin_Angle         = ::SimParameter::Double::Cr_Kin_Angle,
        Cr_Hand_D            = ::SimParameter::Double::Cr_Hand_D,
        Const_A              = ::SimParameter::Double::Const_A,
        Const_B              = ::SimParameter::Double::Const_B,
        D_Trans              = ::SimParameter::Double::D_Trans,
        D_Rot                = ::SimParameter::Double::D_Rot,
        Spring_Length        = ::SimParameter::Double::Spring_Length,
        Spring_K             = ::SimParameter::Double::Spring_K
      };

      template <class T>
      ref class Descriptor
      {
        private:
          const ::SimParameter::Descriptor<T> *_obj;

        internal:
          Descriptor(const ::SimParameter::Descriptor<T> *obj)
          { _obj = obj; }

        public:
          property System::String ^Name
          {
            System::String ^get()
            {
              try { return gcnew System::String(_obj->Name().c_str()); }
              catch (std::exception &ex)
              { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
            }
          }

          property bool Constant
          {
            bool get()
            {
              try { return _obj->Constant(); }
              catch (std::exception &ex)
              { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
            }
          }
          
          property T DefaultValue
          {
            T get()
            {
              try { return _obj->DefaultValue(); }
              catch (std::exception &ex)
              { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
            }
          }
          
          property T SiMultiplier
          {
            T get()
            {
              try { return _obj->SiMultiplier(); }
              catch (std::exception &ex)
              { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
            }
          }
          
          bool HasUpperBoundary(T &upper)
          {
            try { return _obj->HasUpperBoundary(upper); }
            catch (std::exception &ex)
            { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
          }
          
          bool HasLowerBoundary(T &lower)
          {
            try { return _obj->HasLowerBoundary(lower); }
            catch (std::exception &ex)
            { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
          }
      };

    private:
      static System::Collections::Generic::Dictionary<Int, Descriptor<int> ^> ^_intDescs;
      static System::Collections::Generic::Dictionary<Double, Descriptor<double> ^> ^_doubleDescs;

    public:
      static SimParameter()
      {
        try
        {
          {
            auto allInt = ::SimParameter::Int::All();
            _intDescs = gcnew System::Collections::Generic::Dictionary<Int, Descriptor<int> ^>();
            for (size_t i = 0; i < allInt.size(); i++)
              _intDescs->Add((Int)allInt[i],
                       gcnew Descriptor<int>(&::SimParameter::Int::Info(allInt[i])));
          }

          {
            auto allDouble = ::SimParameter::Double::All();
            _doubleDescs = gcnew System::Collections::Generic::Dictionary<Double, Descriptor<double> ^>();
            for (size_t i = 0; i < allDouble.size(); i++)
              _doubleDescs->Add((Double)allDouble[i],
                        gcnew Descriptor<double>(&::SimParameter::Double::Info(allDouble[i])));
          }
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      static Descriptor<int> ^GetInfo(Int type)
      {
        Descriptor<int> ^value;
        if (!_intDescs->TryGetValue(type, value))
          throw gcnew System::ApplicationException("No such parameter");
        return value;
      }

      static Descriptor<double> ^GetInfo(Double type)
      {
        Descriptor<double> ^value;
        if (!_doubleDescs->TryGetValue(type, value))
          throw gcnew System::ApplicationException("No such parameter");
        return value;
      }
  };

  public ref class SimParams : public System::ICloneable, public System::IDisposable
  {
    private:
      ::SimParams *_params;

    internal:
      SimParams(::SimParams *params)
      { _params = params; }

      SimParams(const ::SimParams &params)
      {
        try
        {
          auto values = params.ExportValues();
          _params = new ::SimParams();
          _params->SetAccess(::SimParams::Access::Initialize);
          _params->ImportValues(values);
          _params->SetAccess(::SimParams::Access::ReadOnly);
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      ::SimParams *GetObject()
      { return _params; }

      void SafeRelease()
      {
        try
        {
          if (_params != nullptr)
          {
            delete _params;
            _params = nullptr;
          }
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

    public:
      SimParams()
      {
        try
        {
          _params = new ::SimParams();
          _params->SetAccess(::SimParams::Access::Initialize);
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      double GetValue(SimParameter::Double type)
      {
        try { return _params->GetParameter((::SimParameter::Double::Type)type); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      void SetValue(SimParameter::Double type, double value)
      {
        try { _params->SetParameter((::SimParameter::Double::Type)type, value); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      double GetValue(SimParameter::Double type, bool convertToSI)
      {
        try { return _params->GetParameter((::SimParameter::Double::Type)type, convertToSI); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      void SetValue(SimParameter::Double type, double value, bool convertToSI)
      {
        try { _params->SetParameter((::SimParameter::Double::Type)type, value, convertToSI); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      int GetValue(SimParameter::Int type)
      {
        try { return _params->GetParameter((::SimParameter::Int::Type)type); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      void SetValue(SimParameter::Int type, int value)
      {
        try { _params->SetParameter((::SimParameter::Int::Type)type, value); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      property int default[SimParameter::Int]
      {
        int get(SimParameter::Int type) { return GetValue(type); }
        void set(SimParameter::Int type, int value) { SetValue(type, value); }
      }

      property double default[SimParameter::Double]
      {
        double get(SimParameter::Double type) { return GetValue(type); }
        void set(SimParameter::Double type, double value) { SetValue(type, value); }
      }

      System::String ^ExportAsProps()
      {
        try
        { return gcnew System::String(::SimParamsFormatter::ExportAsProps(_params).c_str()); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      void ImportAsProps(System::String ^props)
      {
        try
        {
          std::string _props;
          for (int i = 0; i < props->Length; i++)
            _props.push_back((char)props[i]);

          SimParamsFormatter::ImportAsProps(_params, _props);
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      virtual System::Object ^Clone()
      {
        ::SimParams *params = NULL;
        try
        {
          params = new ::SimParams();
          params->SetAccess(::SimParams::Access::Initialize);
          std::string props = ::SimParamsFormatter::ExportAsProps(_params);
          ::SimParamsFormatter::ImportAsProps(params, props);
          params->SetAccess(_params->GetAccess());
          return gcnew SimParams(params);
        }
        catch (std::exception &ex)
        {
          if (params != NULL)
          {
            try { delete params; }
            catch (std::exception &) { }
          }
          throw gcnew System::ApplicationException(gcnew System::String(ex.what()));
        }
      }

      ~SimParams()
      { SafeRelease(); }

      !SimParams()
      { SafeRelease(); }

    public:
      // Obsolete member. Use SimParameter::Int::GetInfo().
      static int GetDefaultValue(SimParameter::Int type)
      { return SimParameter::GetInfo(type)->DefaultValue; }

      // Obsolete member. Use SimParameter::Double::GetInfo()
      static double GetDefaultValue(SimParameter::Double type, bool SI)
      {
        auto info = SimParameter::GetInfo(type);
        return info->DefaultValue * (SI ? info->SiMultiplier : 1.0);
      }
  };
}

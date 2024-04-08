#pragma once

#include "MiCoSi.Core/Versions.h"

namespace MiCoSi
{
  public ref class VersionConflictException : System::ApplicationException
  {
    public:
      property System::String ^Current
      {
        System::String ^get()
        { return _current; }
      }

      property System::String ^Required
      {
        System::String ^get()
        { return _required; }
      }

      virtual property System::String ^Message
      {
        virtual System::String ^get() override
        {
          return System::String::Format("Program versions conflict! Current: \"MiCoSi v{0}\". Required: \"MiCoSi v{1}\"",
                                        _current, _required);
        }
      }

    internal:
      VersionConflictException(const Version *current, const Version *required)
      {
        _current = gcnew System::String(current->ToString().c_str());
        _required = gcnew System::String(required->ToString().c_str());
      }

    private:
      System::String ^_current;
      System::String ^_required;
  };

  public ref class CompilationConflictException : System::ApplicationException
  {
    public:
      property System::String ^Current
      {
        System::String ^get()
        { return _current; }
      }

      property System::String ^Required
      {
        System::String ^get()
        { return _required; }
      }

      virtual property System::String ^Message
      {
        virtual System::String ^get() override
        {
          return System::String::Format("Compilation flags conflict! Current: \"{0}\". Required: \"{1}\"",
                                        _current, _required);
        }
      }

    internal:
      CompilationConflictException(const std::string &current, const std::string &required)
      {
        _current = gcnew System::String(current.c_str());
        _required = gcnew System::String(required.c_str());
      }

    private:
      System::String ^_current, ^_required;
  };

  public ref class CurrentVersion abstract
  {
    public:
      static property System::UInt32 Major
      {
        System::UInt32 get()
        { return ::CurrentVersion::ProgramVersion().Major(); }
      }

      static property System::UInt32 Minor
      {
        System::UInt32 get()
        { return ::CurrentVersion::ProgramVersion().Minor(); }
      }

      static property System::UInt32 Build
      {
        System::UInt32 get()
        { return ::CurrentVersion::ProgramVersion().Build(); }
      }

      static property System::String ^CompilationFlags
      {
        System::String ^get()
        { return gcnew System::String(::CurrentVersion::CompilationFlags().c_str()); }
      }

      static property System::UInt32 FileFormat
      {
        System::UInt32 get()
        { return ::CurrentVersion::FileFormatVersion(); }
      }

      static property System::String ^PublishDate
      {
        System::String ^get()
        { return gcnew System::String(::CurrentVersion::ProgramVersion().PublishDate().c_str()); }
      }

      static System::String ^ToString()
      { return gcnew System::String(::CurrentVersion::ProgramVersion().ToString().c_str()); }
  };
}

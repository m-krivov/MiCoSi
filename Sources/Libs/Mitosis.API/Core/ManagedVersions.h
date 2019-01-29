
#pragma once

#include "Mitosis.Core/Versions.h"

namespace Mitosis
{
  public ref class VersionConflictException : System::ApplicationException
  {
    private:
      System::String ^_used;
      System::String ^_required;

    internal:
      VersionConflictException(const Version *used, const Version *required)
      {
        _used = gcnew System::String(used->ToString().c_str());
        _required = gcnew System::String(required->ToString().c_str());
      }

    public:
      property System::String ^UsedVersion
      {
        System::String ^get()
        { return _used; }
      }

      property System::String ^RequiredVersion
      {
        System::String ^get()
        { return _required; }
      }

      virtual property System::String ^Message
      {
        virtual System::String ^get() override
        { return System::String::Format("Version conflict! Used - \"Mitosis v{0}\", required - \"Mitosis v{1}\"",
                        _used, _required); }
      }
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

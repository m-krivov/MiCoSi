#pragma once

#include "MiCoSi.Streams/TimeStream.h"
#include "../Core/ManagedVersions.h"
#include "../Objects/ManagedCell.h"
#include "ManagedTimeLayer.h"

namespace MiCoSi
{
  public ref class TimeStream : public System::IDisposable,
                     public System::Collections::Generic::IEnumerator<TimeLayer ^>,
                     public System::Collections::IEnumerator
  {
    private:
      std::unique_ptr<::TimeStream> *_stream;
      Cell ^_cachedCell;

      void Release()
      {
        try
        {
          if (_stream != NULL)
          {
            delete _stream;
            _stream = NULL;
          }
          _cachedCell = nullptr;
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

    internal:
      TimeStream(std::unique_ptr<::TimeStream> *stream)
      {
        _stream = stream;
        _cachedCell = nullptr;
      }

      ::TimeStream *GetObject()
      { return _stream->get(); }

    public:
      static TimeStream ^Open(System::String ^cellFile)
      {
        try
        {
          System::IntPtr wd = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(cellFile);
          auto *ptr = new std::unique_ptr<::TimeStream>();
          *ptr = ::TimeStream::Open((const char *)(void *)wd);
          System::Runtime::InteropServices::Marshal::FreeHGlobal(wd);
          return gcnew TimeStream(ptr);
        }
        catch (::VersionConflictException &ex)
        { throw gcnew VersionConflictException(&ex.CurrentVersion(), &ex.RequiredVersion()); }

        catch (::CompilationConflictException &ex)
        { throw gcnew CompilationConflictException(ex.CurrentFlags(), ex.RequiredFlags()); }

        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      property int LayerCount
      {
        int get()
        {
          try
          { return (int)(*_stream)->LayerCount(); }
          catch (std::exception &ex)
          { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
        }
      }

      property unsigned __int64 UserSeed
      {
        unsigned __int64 get()
        {
          try
          { return (*_stream)->UserSeed(); }
          catch (std::exception &ex)
          { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
        }
      }

      property TimeLayer ^Current
      {
        virtual TimeLayer ^get() = System::Collections::Generic::IEnumerator<TimeLayer ^>::Current::get
        {
          try
          {
            auto layer = (*_stream)->Current();
            if (_cachedCell == nullptr)
            { _cachedCell = gcnew Cell(&layer.GetCell()); }

            return gcnew TimeLayer(_cachedCell,
                                   gcnew SimParams(layer.GetSimParams()),
                                   layer.GetTime());
          }
          catch (std::exception &ex)
          { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
        }
      }

      property System::Object ^Current2
      {
        virtual System::Object ^get() = System::Collections::IEnumerator::Current::get
        { return Current; }
      }

      virtual void Reset() = System::Collections::Generic::IEnumerator<TimeLayer ^>::Reset
      {
        try
        {
          _cachedCell = nullptr;
          (*_stream)->Reset();
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      void MoveTo(int layer)
      {
        try
        {
          _cachedCell = nullptr;
          (*_stream)->MoveTo((size_t)layer);
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      virtual bool MoveNext() = System::Collections::Generic::IEnumerator<TimeLayer ^>::MoveNext
      {
        try
        {
          _cachedCell = nullptr;
          return (*_stream)->MoveNext();
        }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      double GetLayerTime(int layerIndex)
      {
        try
        { return (*_stream)->GetLayerTime((size_t)layerIndex); }
        catch (std::exception &ex)
        { throw gcnew System::ApplicationException(gcnew System::String(ex.what())); }
      }

      ~TimeStream()
      { Release(); }

      !TimeStream()
      { Release(); }
  };
}

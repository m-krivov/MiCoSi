
#pragma once

#include "IObjectWithID.h"
#include "Mitosis.Objects/Pole.h"
#include "../Geometry/Converter.h"

namespace Mitosis
{
  public enum class PoleType
  {
    Left   = (int)::PoleType::Left,
    Right  = (int)::PoleType::Right
  };

  public ref class Pole : public IObjectWithID
  {
    private:
      ::Pole *_pole;

    internal:
      Pole(::Pole *pole)
      { _pole = pole; }

      ::Pole *GetObject()
      { return _pole; }

    public:
      property System::UInt32 ID
      {
        virtual System::UInt32 get()
        { return _pole->Type() == ::PoleType::Left ? 0 : 1; }
      }

      virtual bool Equals(System::Object ^obj) override
      {
        Pole ^pole = dynamic_cast<Pole ^>(obj);
        return pole != nullptr && pole->GetObject() == GetObject();
      }

      property PoleType Type
      {
        PoleType get()
        { return _pole->Type() == ::PoleType::Left
              ? PoleType::Left
              : PoleType::Right;
        }
      }

      property Vector3 Position
      {
        Vector3 get()
        { return Converter::ToVector3((vec3r)_pole->Position()); }
      }

  };
}

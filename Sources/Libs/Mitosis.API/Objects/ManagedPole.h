
#pragma once

#include "IObjectWithID.h"
#include "Mitosis.Objects/Pole.h"
#include "../Geometry/ManagedVec3D.h"

namespace Mitosis
{
  public enum class PoleType
  {
    Left  = ::PoleType::Left,
    Right  = ::PoleType::Right
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

      property Vec3D ^Position
      {
        Vec3D ^get()
        { return gcnew Vec3D((vec3r)_pole->Position()); }
      }

  };
}
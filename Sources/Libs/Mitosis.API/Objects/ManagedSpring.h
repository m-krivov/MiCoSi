
#pragma once

#include "IObjectWithID.h"
#include "Mitosis.Objects/Spring.h"
#include "../Geometry/Converter.h"

namespace Mitosis
{
  public ref class Spring : public IObjectWithID
  {
    private:
      ::Spring *_spring;

    internal:
      Spring(::Spring *spring)
      {
        _spring = spring;
      }

      ::Spring *GetObject()
      { return _spring; }

    public:
      property System::UInt32 ID
      {
        virtual System::UInt32 get()
        { return _spring->GetID(); }
      }

      virtual bool Equals(System::Object ^obj) override
      {
        Spring ^spring = dynamic_cast<Spring ^>(obj);
        return spring != nullptr && spring->GetObject() == GetObject();
      }

      property double Length
      {
        double get()
        { return _spring->Length(); }
      }

      property Vector3 LeftJoint
      {
        Vector3 get()
        { return Converter::ToVector3(_spring->LeftJoint()); }
      }

      property Vector3 RightJoint
      {
        Vector3 get()
        { return Converter::ToVector3(_spring->RightJoint()); }
      }
  };
}


#pragma once

#include "Defs.h"

//Defines object that can be clonned.
class IClonnable
{
  public:
    virtual IClonnable *Clone() const = 0;

    template <class T>
    T *CloneTemplated() const
    {
      IClonnable *res = Clone();
      T *tRes = dynamic_cast<T *>(res);
      if (tRes == NULL)
      {
        delete res;
        throw std::runtime_error("Cannot cast the clonned object to the required type");
      }
      return tRes;
    }

    virtual ~IClonnable()
    { /*nothing*/ }
};

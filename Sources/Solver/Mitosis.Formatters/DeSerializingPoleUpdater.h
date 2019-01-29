
#pragma once

#include "Mitosis.Objects/Pole.h"

class DeSerializingPoleUpdater : public IPoleUpdater
{
  public:
    virtual IClonnable *Clone()
    { return new DeSerializingPoleUpdater(); }

    virtual void SetInitial(Pole *left, Pole *right, uint32_t &seed)
    { /*nothing*/ }

    virtual void MovePoles(Pole *left, Pole *right, real time, uint32_t &seed)
    { /*nothing*/ }
};

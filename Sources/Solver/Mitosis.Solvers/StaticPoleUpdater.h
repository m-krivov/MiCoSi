
#pragma once

#include "Mitosis.Objects/Interfaces.h"

// Don't moves poles, sets initial position according to config file.
class StaticPoleUpdater : public IPoleUpdater
{
  public:
    virtual IClonnable *Clone()
    { return new StaticPoleUpdater(); }

    virtual void SetInitial(Pole *left, Pole *right, uint32_t &seed);

    virtual void MovePoles(Pole *left, Pole *right, real time, uint32_t &seed)
    { /*nothing*/ }
};

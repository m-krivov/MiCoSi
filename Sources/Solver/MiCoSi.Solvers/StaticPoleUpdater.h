#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Objects/Interfaces.h"

// Don't moves poles, sets initial position according to config file
class StaticPoleUpdater : public IPoleUpdater
{
  public:
    virtual void SetInitial(Pole *left, Pole *right, Random::State &state) override;

    virtual void MovePoles(Pole *left, Pole *right, real time, Random::State &state) override
    { /*nothing*/ }

    virtual IClonnable *Clone() const override
    { return new StaticPoleUpdater(); }
};

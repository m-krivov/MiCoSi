#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Objects/Pole.h"
#include "MiCoSi.Objects/Interfaces.h"

class DeSerializingPoleUpdater : public IPoleUpdater
{
  public:
    DeSerializingPoleUpdater() = default;
    DeSerializingPoleUpdater(const DeSerializingPoleUpdater &) = default;
    DeSerializingPoleUpdater &operator =(const DeSerializingPoleUpdater &) = default;

    virtual IClonnable *Clone() const override
    { return new DeSerializingPoleUpdater(); }

    virtual void SetInitial(Pole *left, Pole *right, Random::State &state) override
    { /*nothing*/ }

    virtual void MovePoles(Pole *left, Pole *right, real time, Random::State &state) override
    { /*nothing*/ }
};

#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Objects/Interfaces.h"

// Updates pole positions using coordinates from XML-file
class XmlPoleUpdater : public IPoleUpdater
{
  public:
    // Loads from provided file coordinates.
    XmlPoleUpdater(const char *xmlFile);

    // IClonnable member
    virtual IClonnable *Clone() const override;

    // IPoleUpdater member
    virtual void SetInitial(Pole *left, Pole *right, Random::State &state)
    { MovePoles(left, right, 0.0, state); }

    // IPoleUpdater member
    virtual void MovePoles(Pole *left, Pole *right, real time, Random::State &);

  private:
    std::vector<std::pair<real, std::pair<vec3r, vec3r> > > _coords;
    size_t _lastAccessedRecord;
};

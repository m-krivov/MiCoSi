
#pragma once

#include "Mitosis.Objects/Interfaces.h"

// Updates pole positions using coordinates from XML-file.
class XmlPoleUpdater : public IPoleUpdater
{
  private:
    std::vector<std::pair<real, std::pair<vec3r, vec3r> > > _coords;
    size_t _lastAccessedRecord;

  public:
    // Loads from provided file coordinates.
    XmlPoleUpdater(const char *xmlFile);

    //IClonnable member.
    virtual IClonnable *Clone();

    // IPoleUpdater member. Sets initial position of the poles.
    virtual void SetInitial(Pole *left, Pole *right, uint32_t &seed)
    { MovePoles(left, right, 0.0, seed); }

    // IPoleUpdater member. Changes position of the poles.
    virtual void MovePoles(Pole *left, Pole *right, real time, uint32_t &seed);
};

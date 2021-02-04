
#include "XmlPoleUpdater.h"

#include "tinyxml.h"

//----------------------
//--- XmlPoleUpdater ---
//----------------------

XmlPoleUpdater::XmlPoleUpdater(const char *xmlFile)
{
  _lastAccessedRecord = 0;
  TiXmlDocument doc;

  if (!doc.LoadFile(xmlFile))
  { throw std::runtime_error(std::string("Cannot load coordinates for poles: ") + doc.ErrorDesc()); }

  for (TiXmlNode *n = doc.RootElement()->FirstChild();
     n != NULL;
     n = n->NextSibling())
  {
    TiXmlElement *el = n->ToElement();

    if (el != NULL && std::string("Coords") == el->Value())
    {
      double time = 0.0;
      double lx = 0.0, ly = 0.0, lz = 0.0;
      double rx = 0.0, ry = 0.0, rz = 0.0;
      if (el->QueryDoubleAttribute("time", &time) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("lx", &lx) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("ly", &ly) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("lz", &lz) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("rx", &rx) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("ry", &ry) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("rz", &rz) != TIXML_SUCCESS)
      { throw std::runtime_error("Cannot load coordinates for poles, wrong file format"); }

      if (_coords.size() > 0 && _coords[_coords.size() - 1].first >= time)
      { throw std::runtime_error("Cannot load coordinates for poles, records were not sorted"); }

      _coords.push_back(std::make_pair((real)time,
                       std::make_pair(vec3r((real)lx, (real)ly, (real)lz),
                              vec3r((real)rx, (real)ry, (real)rz))));
    }
  }

  if (_coords.size() == 0)
  { throw std::runtime_error("Cannot load coordinates for poles, there is no any records"); }
}

IClonnable *XmlPoleUpdater::Clone() const
{
  XmlPoleUpdater *res = new XmlPoleUpdater(*this);
  res->_lastAccessedRecord = 0;
  return res;
}

void XmlPoleUpdater::MovePoles(Pole *left, Pole *right, real time, Random::State &)
{
  if (_coords[_lastAccessedRecord].first > time)
    _lastAccessedRecord = 0;

  while (_lastAccessedRecord < _coords.size() &&
       _coords[_lastAccessedRecord].first < time)
  {
    _lastAccessedRecord += 1;
  }
  _lastAccessedRecord = std::min(_lastAccessedRecord, (size_t)(_coords.size() - 1));

  std::pair<real, std::pair<vec3r, vec3r> > &prev
    = _lastAccessedRecord == 0 ? _coords[0] : _coords[_lastAccessedRecord - 1];
  std::pair<real, std::pair<vec3r, vec3r> > &next = _coords[_lastAccessedRecord];

  if (prev.first == next.first)
  {
    left->Position() = prev.second.first;
    right->Position() = prev.second.second;
  }
  else
  {
    real coeff = (std::min(time, next.first) - prev.first) / (next.first - prev.first);
    left->Position() = next.second.first * (real)coeff + prev.second.first * (real)(1.0 - coeff);
    right->Position() = next.second.second * (real)coeff + prev.second.second * (real)(1.0 - coeff);
  }
}

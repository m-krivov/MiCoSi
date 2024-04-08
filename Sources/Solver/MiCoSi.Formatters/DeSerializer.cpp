#include "DeSerializer.h"

#include "MiCoSi.Core/All.h"
#include "MiCoSi.Geometry/All.h"
#include "MiCoSi.Objects/All.h"

#include "DeSerializingCellInitializer.h"
#include "DeSerializingPoleUpdater.h"

namespace
{

class Loader
{
  public:
    Loader() = delete;
    Loader(const void *data, size_t sizeInBytes)
      : _data(data), _sizeInBytes(sizeInBytes)
    { /*nothing*/ }
    Loader &operator =(const Loader &) = delete;

    void LoadArray(size_t offset, size_t size, void *dst) const
    {
      if (offset + size > _sizeInBytes)
      { throw std::runtime_error("Cannot load array"); }
      memcpy(dst, (uint8_t *)_data + offset, size);
    }

  private:
    const void *_data;
    size_t _sizeInBytes;
};

template <class T>
static void DeserializeArray(const TiXmlElement *node, const char *name,
                             const Loader &bin, std::vector<T> &arr)
{
  std::string sName;
  offset_t offset = -1;
  int size = 0;
  if (node->QueryStringAttribute(name, &sName) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot get offset and size for array"); }

  size_t delim = sName.find(':');
  size_t len = sName.length();
  offset_t t[2];
  if (delim != std::string::npos)
  { Converter::Parse(sName.substr(0, delim), t[0]); }
  else
  { throw std::runtime_error("Internal error at DeSerializer::DeSerializeArray(): cannot find delim"); }
  Converter::Parse(sName.substr(delim+1, len - delim), t[1]);

  if (t[0] >= 0 && t[1] > 0)
  {
    arr.resize((size_t)(t[1] / sizeof(T)));
    bin.LoadArray((size_t)t[0], (size_t)t[1], &arr[0]);
  }
  else
  { arr.resize(0); }
}

double UintToDoubleConverter(const int* buf)
{
  double res;
  int *p_res = (int *)&res;
  p_res[0] = buf[0];
  p_res[1] = buf[1];
  return res;
}

} // unnamed namespace

//--------------------
//--- DeSerializer ---
//--------------------

std::tuple<Version, std::string, int> DeSerializer::DeserializeVersion(uint64_t version)
{
  uint64_t major = 0, minor = 0, build = 0, iflags = 0, fileFormatVersion = 0;
  fileFormatVersion = version % 8;
  version >>= 8;
  iflags = version % 8;
  version >>= 8;
  build = version % 16;
  version >>= 16;
  minor = version % 16;
  version >>= 16;
  major = version % 16;

  // A temporary fix, will be replaced after introducing new containers
  std::string flags;
  switch (iflags)
  {
    case 0: flags = "MICOSI_PRECISION_FP32; MICOSI_RNG_LCG"; break;
    case 1: flags = "MICOSI_PRECISION_FP64; MICOSI_RNG_LCG"; break;
    case 2: flags = "MICOSI_PRECISION_FP32; MICOSI_RNG_MTG"; break;
    case 3: flags = "MICOSI_PRECISION_FP64; MICOSI_RNG_MTG"; break;
  }

  return std::make_tuple(Version((int)major, (int)minor, (int)build),
                         flags,
                         (int)fileFormatVersion);
}

std::pair<Random::State, int64_t> DeSerializer::DeserializeRng(const TiXmlElement *configuration,
                                                               const void *data, size_t sizeInBytes) {
  const TiXmlNode *node = nullptr;
  const TiXmlElement *elem = nullptr;
  if ((node = configuration->FirstChild("Parameters")) == nullptr ||
      (elem = node->ToElement()) == nullptr)
  { throw std::runtime_error("cannot open section with cell's parameters"); }

  std::string seedString, stateString;
  if (elem->QueryStringAttribute("rng_seed",  &seedString)  != TIXML_SUCCESS ||
      elem->QueryStringAttribute("rng_state", &stateString) != TIXML_SUCCESS)
  { throw std::runtime_error("cannot get record with initial RNG"); }

  int64_t seed = 0;
  {
    std::istringstream ss(seedString);
    if (!(ss >> seed) || !ss.eof())
    { throw std::runtime_error("failed to parse initial RNG seed"); }
  }

  Random::State state;
  Random::Deserialize(stateString, state);

  return std::make_pair(state, seed);
}

std::unique_ptr<Cell> DeSerializer::DeserializeCellConfiguration(const TiXmlElement *configuration,
                                                                 const void *data, size_t sizeInBytes)
{
  // Load parameters
  const TiXmlNode *node = nullptr;
  const TiXmlElement *params = nullptr;
  if ((node = configuration->FirstChild("Parameters")) == nullptr ||
      (params = node->ToElement()) == nullptr)
  { throw std::runtime_error("Cannot open section with cell's parameters"); }

  int mtCount, chCount;
  if (params->QueryIntAttribute("n_mt_total", &mtCount) != TIXML_SUCCESS ||
      params->QueryIntAttribute("n_cr_total", &chCount) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot get some cell's parameters"); }

  // Load MT parameters
  const TiXmlElement *mts = nullptr;
  if (configuration->FirstChild("MTs") == nullptr ||
      (mts = configuration->FirstChild("MTs")->ToElement()) == nullptr)
  { throw std::runtime_error("Cannot open section with MTs"); }

  // Create cell
  std::unique_ptr<Cell> res;
  DeSerializingCellInitializer cellInitializer((size_t)chCount / 2, (size_t)mtCount / 2);
  DeSerializingPoleUpdater poleUpdater;
  Random::State fake_state;
  res = std::make_unique<Cell>(&cellInitializer, &poleUpdater, fake_state);

  return res;
}

std::pair<double, Random::State> DeSerializer::DeserializeTimeLayer(const TiXmlElement *timeLayer, Cell &cell,
                                                                    const void *data, size_t sizeInBytes)
{
  // Load RNG state and time
  int doubleBuf[2] = { 0, 0 };
  std::string rngString;
  if (timeLayer->QueryIntAttribute("t0", &doubleBuf[0]) != TIXML_SUCCESS ||
      timeLayer->QueryIntAttribute("t1", &doubleBuf[1]) != TIXML_SUCCESS ||
      timeLayer->QueryStringAttribute("rand", &rngString) != TIXML_SUCCESS)
  { throw std::runtime_error("File with cell is corrupted. Cannot get time or RNG state attribute"); }
  
  double time = UintToDoubleConverter(doubleBuf);
  if (time < 0)
  { throw std::runtime_error("File with cell is corrupted. Cannot get time"); }
  
  // Load cell's parameters
  vec3d leftPole;
  vec3d rightPole;
  int springBroken;
  const TiXmlElement *cellSect = nullptr;
  if (timeLayer->FirstChild("Cell") == nullptr ||
      (cellSect = timeLayer->FirstChild("Cell")->ToElement()) == nullptr)
  { throw std::runtime_error("Cannot open section with cell configuration"); }

  if (cellSect->QueryIntAttribute("SprBrkn", &springBroken) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration"); }
  if (cellSect->QueryIntAttribute("LPX0", doubleBuf) != TIXML_SUCCESS || 
      cellSect->QueryIntAttribute("LPX1", doubleBuf + 1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration0"); }
  leftPole.x = UintToDoubleConverter(doubleBuf);
  if (cellSect->QueryIntAttribute("LPY0", doubleBuf) != TIXML_SUCCESS || 
      cellSect->QueryIntAttribute("LPY1", doubleBuf + 1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration1"); }
  leftPole.y = UintToDoubleConverter(doubleBuf);
  if (cellSect->QueryIntAttribute("LPZ0", doubleBuf) != TIXML_SUCCESS || 
      cellSect->QueryIntAttribute("LPZ1", doubleBuf + 1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration2"); }
  leftPole.z = UintToDoubleConverter(doubleBuf);
  if (cellSect->QueryIntAttribute("RPX0", doubleBuf) != TIXML_SUCCESS || 
      cellSect->QueryIntAttribute("RPX1", doubleBuf+1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration3"); }
  rightPole.x = UintToDoubleConverter(doubleBuf);
  if (cellSect->QueryIntAttribute("RPY0", doubleBuf) != TIXML_SUCCESS || 
      cellSect->QueryIntAttribute("RPY1", doubleBuf + 1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration4"); }
  rightPole.y = UintToDoubleConverter(doubleBuf);
  if (cellSect->QueryIntAttribute("RPZ0", doubleBuf) != TIXML_SUCCESS || 
      cellSect->QueryIntAttribute("RPZ1", doubleBuf + 1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration5"); }
  rightPole.z = UintToDoubleConverter(doubleBuf);

  cell.SetSpringFlag(springBroken != 0);
  cell.GetPole(PoleType::Left)->Position() = vec3r((real)leftPole.x, (real)leftPole.y, (real)leftPole.z);
  cell.GetPole(PoleType::Right)->Position() = vec3r((real)rightPole.x, (real)rightPole.y, (real)rightPole.z);

  // Load MT's parameters
  int offset = -1, size = 0;
  const TiXmlElement *mts = nullptr;
  if (timeLayer->FirstChild("MTs") == nullptr ||
      (mts = timeLayer->FirstChild("MTs")->ToElement()) == nullptr)
  { throw std::runtime_error("File with cell is corrupted. Cannot open section with MTs"); }
  
  std::vector<real> lengthes;
  std::vector<real> mtDirs_x;
  std::vector<real> mtDirs_y;
  std::vector<real> mtDirs_z;
  std::vector<real> mtForces_x;
  std::vector<real> mtForces_y;
  std::vector<real> mtForces_z;
  std::vector<int> states;
  std::vector<int> boundIDs;

  Loader bin(data, sizeInBytes);
  DeserializeArray<real>(mts, "Len", bin, lengthes);
  DeserializeArray<real>(mts, "DX", bin, mtDirs_x);
  DeserializeArray<real>(mts, "DY", bin, mtDirs_y);
  DeserializeArray<real>(mts, "DZ", bin, mtDirs_z);
  DeserializeArray<real>(mts, "FX", bin, mtForces_x);
  DeserializeArray<real>(mts, "FY", bin, mtForces_y);
  DeserializeArray<real>(mts, "FZ", bin, mtForces_z);
  DeserializeArray<int>(mts, "St", bin, states);
  DeserializeArray<int>(mts, "Bnd", bin, boundIDs);

  if (cell.MTs().size() != lengthes.size() ||
      cell.MTs().size() != mtDirs_x.size() ||
      cell.MTs().size() != mtDirs_y.size() ||
      cell.MTs().size() != mtDirs_z.size() ||
      cell.MTs().size() != mtForces_x.size() ||
      cell.MTs().size() != mtForces_y.size() ||
      cell.MTs().size() != mtForces_z.size() ||
      cell.MTs().size() != states.size() ||
      cell.MTs().size() != boundIDs.size())
  {
    throw std::runtime_error(
        "File with cell is corrupted. Count of MTs differs in initial configuration and time layer"
    );
  }

  int chrSize = (int)cell.Chromosomes().size();
  for (size_t i = 0; i < lengthes.size(); i++)
  {
    if (boundIDs[i] >= chrSize)
    { throw std::runtime_error("File with cell is corrupted. Wrong indices of the bound MTs"); }
  }

  for (size_t i = 0; i < lengthes.size(); i++)
  {
    MT *mt = cell.MTs()[i];
    mt->Length() = lengthes[i];
    mt->Direction() = vec3r(mtDirs_x[i], mtDirs_y[i], mtDirs_z[i]);
    mt->ForceOffset() = vec3r(mtForces_x[i], mtForces_y[i], mtForces_z[i]);
    mt->State() = states[i] == 0 ? MTState::Polymerization : MTState::Depolymerization;
    if (mt->BoundChromosome() != nullptr)
    { mt->UnBind(); }
    if (boundIDs[i] >= 0)
    { mt->Bind(cell.Chromosomes()[boundIDs[i]]); }
  }

  // Load chromosomes
  const TiXmlElement *chrs = nullptr;
  if (timeLayer->FirstChild("Chrms") == nullptr ||
      (chrs = timeLayer->FirstChild("Chrms")->ToElement()) == nullptr)
  { throw std::runtime_error("File with cell is corrupted. Cannot open section with chromosomes"); }

  std::vector<real> x;
  std::vector<real> y;
  std::vector<real> z;
  std::vector<real> mat;

  DeserializeArray<real>(chrs, "X", bin, x);
  DeserializeArray<real>(chrs, "Y", bin, y);
  DeserializeArray<real>(chrs, "Z", bin, z);
  DeserializeArray<real>(chrs, "Mat", bin, mat);

  if (cell.Chromosomes().size() != x.size() ||
      cell.Chromosomes().size() != y.size() ||
      cell.Chromosomes().size() != z.size() ||
      cell.Chromosomes().size() != mat.size() / 9)
  {
    throw std::runtime_error(
        "File with cell is corrupted. Count of Chromosomes differs in initial configuration and time layer"
    );
  }

  const std::vector<Chromosome *> &chrsRef = cell.Chromosomes();
  for (size_t i = 0; i < cell.Chromosomes().size(); i++)
  {
    Chromosome *chrRef = chrsRef[i];
    chrRef->Position() = vec3r(x[i], y[i], z[i]);
    chrRef->Orientation() = mat3x3r(mat[i * 9 + 0], mat[i * 9 + 1], mat[i * 9 + 2],
                    mat[i * 9 + 3], mat[i * 9 + 4], mat[i * 9 + 5],
                    mat[i * 9 + 6], mat[i * 9 + 7], mat[i * 9 + 8]);
  }

  //Returning results.
  Random::State rngState;
  Random::Deserialize(rngString, rngState);
  return std::make_pair(time, rngState);
}

double DeSerializer::DeserializeTime(const TiXmlElement *timeLayer)
{
  int doubleBuf[2] = { 0, 0 };
  if (timeLayer->QueryIntAttribute("t0", &doubleBuf[0]) != TIXML_SUCCESS || 
      timeLayer->QueryIntAttribute("t1", &doubleBuf[1]) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot get time attribute"); }
  
  double time = UintToDoubleConverter(doubleBuf);
  if (time < 0.0)
  {
    std::stringstream str;
    str << "Unexpected value of time record: " << time;
    throw std::runtime_error(str.str().c_str());
  }

  return time;
}

std::unique_ptr<SimParams> DeSerializer::DeserializeSimParams(const TiXmlElement *params,
                                                              const void *data, size_t sizeInBytes)
{
  std::unique_ptr<SimParams> res = std::make_unique<SimParams>();
  res->SetAccess(SimParams::Access::Initialize);

  for (const TiXmlAttribute *attr = params->FirstAttribute();
       attr != nullptr;
       attr = attr->Next())
  {
    auto name = attr->NameTStr();
    if (name[name.size() - 1] == '0')
    {
      int doubleBuf[2] = { 0, 0 };
      Converter::Parse(attr->Value(), doubleBuf[0]);
      if ((attr = attr->Next()) == nullptr)
      { throw new std::runtime_error("DeSerializer::DeserializeSimParams - damaged file"); }
      name = attr->NameTStr();
      if(name[name.size() - 1] != '1')
      { throw new std::runtime_error("DeSerializer::DeserializeSimParams - damaged file"); }
      Converter::Parse(attr->Value(), doubleBuf[1]);
      res->SetParameter(name.substr(0, name.size() - 1),
                        Converter::ToString(UintToDoubleConverter(doubleBuf)));
    }
    else
    { res->SetParameter(attr->Name(), attr->Value()); }
  }
  res->SetAccess(SimParams::Access::ReadOnly);

  return res;
}


#include "DeSerializer.h"

#include "Mitosis.Core/All.h"
#include "Mitosis.Geometry/All.h"
#include "Mitosis.Objects/All.h"

#include "DeSerializingCellInitializer.h"
#include "DeSerializingPoleUpdater.h"

//--------------------
//--- DeSerializer ---
//--------------------

std::pair<Version, int> DeSerializer::DeserializeVersion(uint64_t version)
{
  uint64_t major = 0, minor = 0, build = 0, fileFormatVersion = 0;
  uint64_t max = 1 << 16;
  fileFormatVersion = version % max;
  version >>= 16;
  build = version % max;
  version >>= 16;
  minor = version % max;
  version >>= 16;
  major = version % max;
  return std::make_pair(Version((int)major, (int)minor, (int)build), (int)fileFormatVersion);
}

std::pair<Cell *, uint32_t> DeSerializer::DeserializeCellConfiguration(TiXmlElement *configuration, void* data, size_t sizeInBytes)
{
  //Loading parameters.
  int randSeed;
  int mtCount;
  int chCount;

  std::unique_ptr<Loader> bin(new Loader(data, sizeInBytes));

  TiXmlElement *params = NULL;
  if (configuration->FirstChild("Parameters") == NULL ||
    (params = configuration->FirstChild("Parameters")->ToElement()) == NULL)
  { throw std::runtime_error("Cannot open section with cell's parameters"); }

  if (params->QueryIntAttribute("init_rand_seed", &randSeed) != TIXML_SUCCESS ||
    params->QueryIntAttribute("n_mt_total", &mtCount) != TIXML_SUCCESS ||
    params->QueryIntAttribute("n_cr_total", &chCount) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot get some cell's parameters"); }

  //Loading MT parameters.
  TiXmlElement *mts = NULL;
  if (configuration->FirstChild("MTs") == NULL ||
    (mts = configuration->FirstChild("MTs")->ToElement()) == NULL)
  { throw std::runtime_error("Cannot open section with MTs"); }

  //Creating cell.
  Cell *res = NULL;
  try
  {
    DeSerializingCellInitializer cellInitializer((size_t)chCount / 2, (size_t)mtCount / 2);
    DeSerializingPoleUpdater poleUpdater;

    uint32_t seed = 0;  //not used during deserialization
    res = new Cell(&cellInitializer, &poleUpdater, seed);
  }
  catch (std::exception &)
  {
    if (res != NULL)
      delete res;
    throw;
  }

  //Returning result.
  return std::make_pair(res, (uint32_t)randSeed);

}

double UintToDoubleConverter(const int* buf)
{
  double res;
  int* p_res = (int*)&res;
  p_res[0] = buf[0];
  p_res[1] = buf[1];
  return res;
}

std::pair<double, uint32_t> DeSerializer::DeserializeTimeLayer(TiXmlElement *timeLayer, Cell *cell, void* data, size_t sizeInBytes)
{
  std::unique_ptr<Loader> _bin(new Loader(data, sizeInBytes));
  Loader *bin = _bin.get();

  //Loading rand-seed and time.
  double time = 0.0;
  int randSeed = 0;
  int doubleBuf[2];
  doubleBuf[0] = 0;
  doubleBuf[1] = 0;
  if (timeLayer->QueryIntAttribute("t0", &doubleBuf[0]) != TIXML_SUCCESS || 
    timeLayer->QueryIntAttribute("t1", &doubleBuf[1]) != TIXML_SUCCESS ||
    timeLayer->QueryIntAttribute("rand", &randSeed) != TIXML_SUCCESS)
  { throw std::runtime_error("File with cell is corrupted. Cannot get time or random seed attribute"); }
  time = UintToDoubleConverter(doubleBuf);
  if(time < 0)
  { throw std::runtime_error("File with cell is corrupted. Cannot get time"); }
  //Loading cell's parameters.
  vec3d leftPole;
  vec3d rightPole;
  int springBroken;
  TiXmlElement *cellSect = NULL;
  if (timeLayer->FirstChild("Cell") == NULL ||
    (cellSect = timeLayer->FirstChild("Cell")->ToElement()) == NULL)
  { throw std::runtime_error("Cannot open section with cell configuration"); }

  if (cellSect->QueryIntAttribute("SprBrkn", &springBroken) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration"); }
  if(cellSect->QueryIntAttribute("LPX0", doubleBuf) != TIXML_SUCCESS || 
    cellSect->QueryIntAttribute("LPX1", doubleBuf+1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration0"); }
  leftPole.x = UintToDoubleConverter(doubleBuf);
  if(cellSect->QueryIntAttribute("LPY0", doubleBuf) != TIXML_SUCCESS || 
    cellSect->QueryIntAttribute("LPY1", doubleBuf+1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration1"); }
  leftPole.y = UintToDoubleConverter(doubleBuf);
  if(cellSect->QueryIntAttribute("LPZ0", doubleBuf) != TIXML_SUCCESS || 
    cellSect->QueryIntAttribute("LPZ1", doubleBuf+1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration2"); }
  leftPole.z = UintToDoubleConverter(doubleBuf);
  if(cellSect->QueryIntAttribute("RPX0", doubleBuf) != TIXML_SUCCESS || 
    cellSect->QueryIntAttribute("RPX1", doubleBuf+1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration3"); }
  rightPole.x = UintToDoubleConverter(doubleBuf);
  if(cellSect->QueryIntAttribute("RPY0", doubleBuf) != TIXML_SUCCESS || 
    cellSect->QueryIntAttribute("RPY1", doubleBuf+1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration4"); }
  rightPole.y = UintToDoubleConverter(doubleBuf);
  if(cellSect->QueryIntAttribute("RPZ0", doubleBuf) != TIXML_SUCCESS || 
    cellSect->QueryIntAttribute("RPZ1", doubleBuf+1) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot load cell configuration5"); }
  rightPole.z = UintToDoubleConverter(doubleBuf);

  cell->SetSpringFlag(springBroken != 0);
  cell->GetPole(PoleType::Left)->Position() = vec3r((real)leftPole.x, (real)leftPole.y, (real)leftPole.z);
  cell->GetPole(PoleType::Right)->Position() = vec3r((real)rightPole.x, (real)rightPole.y, (real)rightPole.z);

  //Loading MT's parameters.
  int offset = -1, size = 0;
  TiXmlElement *mts = NULL;
  if (timeLayer->FirstChild("MTs") == NULL ||
    (mts = timeLayer->FirstChild("MTs")->ToElement()) == NULL)
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

  DeserializeArray<real>(mts, "Len", bin, lengthes);
  DeserializeArray<real>(mts, "DX", bin, mtDirs_x);
  DeserializeArray<real>(mts, "DY", bin, mtDirs_y);
  DeserializeArray<real>(mts, "DZ", bin, mtDirs_z);
  DeserializeArray<real>(mts, "FX", bin, mtForces_x);
  DeserializeArray<real>(mts, "FY", bin, mtForces_y);
  DeserializeArray<real>(mts, "FZ", bin, mtForces_z);
  DeserializeArray<int>(mts, "St", bin, states);
  DeserializeArray<int>(mts, "Bnd", bin, boundIDs);

  if (cell->MTs().size() != lengthes.size() ||
    cell->MTs().size() != mtDirs_x.size() ||
    cell->MTs().size() != mtDirs_y.size() ||
    cell->MTs().size() != mtDirs_z.size() ||
    cell->MTs().size() != mtForces_x.size() ||
    cell->MTs().size() != mtForces_y.size() ||
    cell->MTs().size() != mtForces_z.size() ||
    cell->MTs().size() != states.size() ||
    cell->MTs().size() != boundIDs.size())
  { throw std::runtime_error("File with cell is corrupted. Count of MTs differs in initial configuration and time layer"); }

  size_t chrSize = cell->Chromosomes().size();
  for (size_t i = 0; i < lengthes.size(); i++)
    if (boundIDs[i] >= (int)chrSize)
      throw std::runtime_error("File with cell is corrupted. Wrong indices of the bound MTs");

  for (size_t i = 0; i < lengthes.size(); i++)
  {
    MT *mt = cell->MTs()[i];
    mt->Length() = lengthes[i];
    mt->Direction() = vec3r(mtDirs_x[i], mtDirs_y[i], mtDirs_z[i]);
    mt->ForceOffset() = vec3r(mtForces_x[i], mtForces_y[i], mtForces_z[i]);
    mt->State() = states[i] == 0 ? MTState::Polymerization : MTState::Depolymerization;
    if (mt->BoundChromosome() != NULL)
      mt->UnBind();
    if (boundIDs[i] >= 0)
      mt->Bind(cell->Chromosomes()[boundIDs[i]]);
  }

  //Loading chromosomes.
  TiXmlElement *chrs = NULL;
  if (timeLayer->FirstChild("Chrms") == NULL ||
    (chrs = timeLayer->FirstChild("Chrms")->ToElement()) == NULL)
  { throw std::runtime_error("File with cell is corrupted. Cannot open section with chromosomes"); }

  std::vector<real> x;
  std::vector<real> y;
  std::vector<real> z;
  std::vector<real> mat;

  DeserializeArray<real>(chrs, "X", bin, x);
  DeserializeArray<real>(chrs, "Y", bin, y);
  DeserializeArray<real>(chrs, "Z", bin, z);
  DeserializeArray<real>(chrs, "Mat", bin, mat);

  if (cell->Chromosomes().size() != x.size() ||
    cell->Chromosomes().size() != y.size() ||
    cell->Chromosomes().size() != z.size() ||
    cell->Chromosomes().size() != mat.size() / 9)
  { throw std::runtime_error("File with cell is corrupted. Count of Chromosomes differs in initial configuration and time layer"); }

  const std::vector<Chromosome *> &chrsRef = cell->Chromosomes();

  for (size_t i = 0; i < cell->Chromosomes().size(); i++)
  {
    Chromosome *chrRef = chrsRef[i];
    chrRef->Position() = vec3r(x[i], y[i], z[i]);
    chrRef->Orientation() = mat3x3r(mat[i * 9 + 0], mat[i * 9 + 1], mat[i * 9 + 2],
                    mat[i * 9 + 3], mat[i * 9 + 4], mat[i * 9 + 5],
                    mat[i * 9 + 6], mat[i * 9 + 7], mat[i * 9 + 8]);
  }

  //Returning results.
  return std::make_pair(time, (uint32_t)randSeed);
}

double DeSerializer::DeserializeTime(TiXmlElement *timeLayer)
{
  double time = 0.0;
  int doubleBuf[2];
  doubleBuf[0] = 0;
  doubleBuf[1] = 0;
  if (timeLayer->QueryIntAttribute("t0", &doubleBuf[0]) != TIXML_SUCCESS || 
    timeLayer->QueryIntAttribute("t1", &doubleBuf[1]) != TIXML_SUCCESS)
  { throw std::runtime_error("Cannot get time attribute"); }
  time = UintToDoubleConverter(doubleBuf);
  if(time < 0)
  {
    std::stringstream str;
    str << "Unexpected value of time record" << time;
    throw std::runtime_error(str.str().c_str());
  }
  return time;
}

SimParams *DeSerializer::DeserializeSimParams(TiXmlElement *params, void* data, size_t sizeInBytes)
{
  SimParams *res = NULL;

  try
  {
    res = new SimParams();
    res->SetAccess(SimParams::Access::Initialize);
    for (TiXmlAttribute *attr = params->FirstAttribute();
       attr != NULL;
       attr = attr->Next())
    {
      auto name = attr->NameTStr();
      if(name[name.size() - 1] == '0')
      {
        int doubleBuf[2];
        Converter::Parse(attr->Value(), doubleBuf[0]);
        attr = attr->Next();
        if(attr == NULL)
          throw new std::runtime_error("DeSerializer::DeserializeSimParams - damaged file");
        name = attr->NameTStr();
        if(name[name.size() - 1] != '1')
          throw new std::runtime_error("DeSerializer::DeserializeSimParams - damaged file");
        Converter::Parse(attr->Value(), doubleBuf[1]);
        res->SetParameter(name.substr(0, name.size()-1), Converter::ToString(UintToDoubleConverter(doubleBuf)));
      }
      else
        res->SetParameter(attr->Name(), attr->Value());
    }
    res->SetAccess(SimParams::Access::ReadOnly);
  }
  catch (std::runtime_error &)
  {
    if (res != NULL)
      delete res;
    throw;
  }

  return res;
}

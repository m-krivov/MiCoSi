
#include "Serializer.h"

#include "Mitosis.Core/All.h"
#include "Mitosis.Objects/All.h"

#include "ttgUtils.h"

//------------------
//--- Serializer ---
//------------------

uint64_t Serializer::SerializeVersion(const Version &programVersion, int fileFormatVersion)
{
  uint64_t res = 0;
  uint16_t major = (uint16_t)programVersion.Major();
  uint16_t minor = (uint16_t)programVersion.Minor();
  uint16_t build = (uint16_t)programVersion.Build();
  uint16_t fileFormat = (uint16_t)fileFormatVersion;
  uint64_t step = (uint64_t)1 << 48;
  res = step * major;
  step >>= 16;
  res += step * minor;
  step >>= 16;
  res += step * build;
  res += fileFormatVersion;
  return res;
}

namespace
{

std::string HelperJoin(offset_t offset, offset_t size)
{
  std::stringstream ss;
  ss << Converter::ToString(offset) << ":" << Converter::ToString(size);
  return ss.str();
}

} // unnamed namespace

TiXmlElement *Serializer::SerializeCellConfiguration(const Cell &cell,
                                                     const Random::State &rngState,
                                                     int64_t rngSeed,
                                                     MemoryStream &stream)
{
  TiXmlElement *res = nullptr;
  try
  {
    res = new TiXmlElement("Configuration");

    // Adding base parameters
    TiXmlElement *params = new TiXmlElement("Parameters");
    if (res->LinkEndChild(params) == nullptr)
    { throw std::runtime_error("Internal error at Serializer::SerializeCellConfiguration() #1"); }

    std::ostringstream ss;
    ss << rngSeed;
    params->SetAttribute("rng_seed", ss.str());
    params->SetAttribute("rng_state", Random::Serialize(rngState));
    params->SetAttribute("n_mt_total", (int)cell.MTs().size());
    params->SetAttribute("n_cr_total", (int)cell.Chromosomes().size());

    // Adding static information about MTs
    TiXmlElement *mts = new TiXmlElement("MTs");
    if (res->LinkEndChild(mts) == nullptr)
    { throw std::runtime_error("Internal error at Serializer::SerializeCellConfiguration() #2"); }

    return res;
  }
  catch (std::runtime_error &)
  {
    if (res != nullptr) delete res;
    throw;
  }
}

namespace
{

void DoubleToUIntConverter(double value, int* buf)
{
  int* res = (int*)&value;
  buf[0] = res[0];
  buf[1] = res[1];
}

} // unnamed namespace

TiXmlElement *Serializer::SerializeTimeLayer(const Cell &cell, double time,
                                             const Random::State &rng, MemoryStream &stream)
{
  TiXmlElement *res = NULL;
  try
  {
    ttg_time_stamp("Serializer::SerializeTimeLayer");
    res = new TiXmlElement("Time_layer");

    int doubleBuf[2];
    DoubleToUIntConverter(time, doubleBuf);
    res->SetAttribute("t0", doubleBuf[0]);
    res->SetAttribute("t1", doubleBuf[1]);
    res->SetAttribute("rand", Random::Serialize(rng));

    // Add the cell state
    vec3r leftPole = (vec3r)cell.GetPole(PoleType::Left)->Position();
    vec3r rightPole = (vec3r)cell.GetPole(PoleType::Right)->Position();
    TiXmlElement *cellSect = new TiXmlElement("Cell");
    if (res->LinkEndChild(cellSect) == NULL)
    { throw std::runtime_error("Internal error at Serializer::SerializeTimeLayer() #0"); }

    DoubleToUIntConverter(leftPole.x, doubleBuf);
    cellSect->SetAttribute("LPX0", doubleBuf[0]);
    cellSect->SetAttribute("LPX1", doubleBuf[1]);

    DoubleToUIntConverter(leftPole.y, doubleBuf);
    cellSect->SetAttribute("LPY0", doubleBuf[0]);
    cellSect->SetAttribute("LPY1", doubleBuf[1]);

    DoubleToUIntConverter(leftPole.z, doubleBuf);
    cellSect->SetAttribute("LPZ0", doubleBuf[0]);
    cellSect->SetAttribute("LPZ1", doubleBuf[1]);

    DoubleToUIntConverter(rightPole.x, doubleBuf);
    cellSect->SetAttribute("RPX0", doubleBuf[0]);
    cellSect->SetAttribute("RPX1", doubleBuf[1]);

    DoubleToUIntConverter(rightPole.y, doubleBuf);
    cellSect->SetAttribute("RPY0", doubleBuf[0]);
    cellSect->SetAttribute("RPY1", doubleBuf[1]);

    DoubleToUIntConverter(rightPole.z, doubleBuf);
    cellSect->SetAttribute("RPZ0", doubleBuf[0]);
    cellSect->SetAttribute("RPZ1", doubleBuf[1]);
    cellSect->SetAttribute("SprBrkn", cell.AreSpringsBroken() ? 1 : 0);

    // Add states of MTs
    const std::vector<MT *> &mtsRefs = cell.MTs();

    std::vector<real> lengthes(mtsRefs.size());
    std::vector<real> dir_x(mtsRefs.size());
    std::vector<real> dir_y(mtsRefs.size());
    std::vector<real> dir_z(mtsRefs.size());
    std::vector<real> force_x(mtsRefs.size());
    std::vector<real> force_y(mtsRefs.size());
    std::vector<real> force_z(mtsRefs.size());
    std::vector<int> states(mtsRefs.size());
    std::vector<int> boundIDs(mtsRefs.size());
    for (size_t i = 0; i < lengthes.size(); i++)
    {
      lengthes[i] = cell.MTs()[i]->Length();
      vec3r dir = (vec3r)mtsRefs[i]->Direction();
      dir_x[i] = dir.x;
      dir_y[i] = dir.y;
      dir_z[i] = dir.z;
      vec3r force = (vec3r)mtsRefs[i]->ForceOffset();
      force_x[i] = force.x;
      force_y[i] = force.y;
      force_z[i] = force.z;
      states[i] = mtsRefs[i]->State() == MTState::Polymerization ? 0 : 1;
      boundIDs[i] = mtsRefs[i]->BoundChromosome() == NULL ? -1 : mtsRefs[i]->BoundChromosome()->ID();
    }

    TiXmlElement *mts = new TiXmlElement("MTs");
    if (res->LinkEndChild(mts) == NULL)
        throw std::runtime_error("Internal error at Serializer::SerializeTimeLayer() #1");

    if (lengthes.size() != 0)
    {
      mts->SetAttribute("Len", HelperJoin(stream.Position(), lengthes.size() * sizeof(real)));
      stream.Write(&lengthes[0], lengthes.size() * sizeof(real));
      mts->SetAttribute("DX", HelperJoin(stream.Position(), dir_x.size() * sizeof(real)));
      stream.Write(&dir_x[0], dir_x.size() * sizeof(real));
      mts->SetAttribute("DY", HelperJoin(stream.Position(), dir_y.size() * sizeof(real)));
      stream.Write(&dir_y[0], dir_y.size() * sizeof(real));
      mts->SetAttribute("DZ", HelperJoin(stream.Position(), dir_z.size() * sizeof(real)));
      stream.Write(&dir_z[0], dir_z.size() * sizeof(real));
      
      mts->SetAttribute("FX", HelperJoin(stream.Position(), force_x.size() * sizeof(real)));
      stream.Write(&force_x[0], force_x.size() * sizeof(real));
      mts->SetAttribute("FY", HelperJoin(stream.Position(), force_y.size() * sizeof(real)));
      stream.Write(&force_y[0], force_y.size() * sizeof(real));
      mts->SetAttribute("FZ", HelperJoin(stream.Position(), force_z.size() * sizeof(real)));
      stream.Write(&force_z[0], force_z.size() * sizeof(real));

      mts->SetAttribute("St", HelperJoin(stream.Position(), states.size() * sizeof(int)));
      stream.Write(&states[0], states.size() * sizeof(int));
      mts->SetAttribute("Bnd", HelperJoin(stream.Position(), boundIDs.size() * sizeof(int)));
      stream.Write(&boundIDs[0], boundIDs.size() * sizeof(int));
    }
    else
    {
      mts->SetAttribute("Len", HelperJoin(-1, 0));
      mts->SetAttribute("DX", HelperJoin(-1, 0));
      mts->SetAttribute("DY", HelperJoin(-1, 0));
      mts->SetAttribute("DZ", HelperJoin(-1, 0));
      mts->SetAttribute("FX", HelperJoin(-1, 0));
      mts->SetAttribute("FY", HelperJoin(-1, 0));
      mts->SetAttribute("FZ", HelperJoin(-1, 0));
      mts->SetAttribute("St", HelperJoin(-1, 0));
      mts->SetAttribute("Bnd", HelperJoin(-1, 0));
    }

    // Add states of chromosomes
    TiXmlElement *chrs = new TiXmlElement("Chrms");
    if (res->LinkEndChild(chrs) == NULL)
      throw std::runtime_error("Internal error at Serializer::SerializeTimeLayer() #2");

    std::vector<real> x(cell.Chromosomes().size());
    std::vector<real> y(cell.Chromosomes().size());
    std::vector<real> z(cell.Chromosomes().size());
    std::vector<real> mat(cell.Chromosomes().size() * 9);
    std::vector<int> cnt(cell.Chromosomes().size());
    std::vector<int> coff(cell.Chromosomes().size());

    for (size_t i = 0; i < cell.Chromosomes().size(); i++)
    {
      Chromosome *chrRef = cell.Chromosomes()[i];
      vec3r pos = (vec3r)chrRef->Position();
      x[i] = pos.x;
      y[i] = pos.y;
      z[i] = pos.z;
      mat3x3r orient = (mat3x3r)chrRef->Orientation();
      mat[i * 9 + 0] = orient.a[0];
      mat[i * 9 + 1] = orient.a[1];
      mat[i * 9 + 2] = orient.a[2];
      mat[i * 9 + 3] = orient.a[3];
      mat[i * 9 + 4] = orient.a[4];
      mat[i * 9 + 5] = orient.a[5];
      mat[i * 9 + 6] = orient.a[6];
      mat[i * 9 + 7] = orient.a[7];
      mat[i * 9 + 8] = orient.a[8];
    }
    if (mat.size() != 0)
    {
      chrs->SetAttribute("X", HelperJoin(stream.Position(), x.size() * sizeof(real)));
      stream.Write(&x[0], x.size() * sizeof(real));
      chrs->SetAttribute("Y", HelperJoin(stream.Position(), y.size() * sizeof(real)));
      stream.Write(&y[0], y.size() * sizeof(real));
      chrs->SetAttribute("Z", HelperJoin(stream.Position(), z.size() * sizeof(real)));
      stream.Write(&z[0], z.size() * sizeof(real));
      chrs->SetAttribute("Mat", HelperJoin(stream.Position(), mat.size() * sizeof(real)));
      stream.Write(&mat[0], mat.size() * sizeof(real));
    }
    else
    {
      chrs->SetAttribute("X", HelperJoin(-1, 0));
      chrs->SetAttribute("Y", HelperJoin(-1, 0));
      chrs->SetAttribute("Z", HelperJoin(-1, 0));
      chrs->SetAttribute("Mat", HelperJoin(-1, 0));
    }

    // Return result
    ttg_time_stamp("Serializer::SerializeTimeLayer");
    return res;
  }
  catch (std::runtime_error &)
  {
    if (res != NULL) delete res;
    ttg_time_stamp("Serializer::SerializeTimeLayer");
    throw;
  }
}

TiXmlElement *Serializer::SerializeSimParams(const SimParams &params, MemoryStream &stream)
{
  TiXmlElement *res = NULL;

  try
  {
    res = new TiXmlElement("Sim_params");

    auto iParams = SimParameter::Int::All();
    for (size_t i = 0; i < iParams.size(); i++)
    { res->SetAttribute(SimParameter::Int::Info(iParams[i]).Name(), params.GetParameter(iParams[i])); }

    auto dParams = SimParameter::Double::All();
    for (size_t i = 0; i < dParams.size(); i++)
    {
      double val = params.GetParameter(dParams[i], false);
      auto name = SimParameter::Double::Info(dParams[i]).Name();
      res->SetAttribute(name + std::string("0"), ((int *)&val)[0]);
      res->SetAttribute(name + std::string("1"), ((int *)&val)[1]);
    }
  }
  catch (std::runtime_error &)
  {
    if (res != nullptr) delete res;
    throw;
  }

  return res;
}


#include "XmlCellInitializer.h"

#include "Mitosis.Core/SimParams.h"
#include "Mitosis.Objects/All.h"

//--------------------------
//--- XmlCellInitializer ---
//--------------------------

XmlCellInitializer::XmlCellInitializer(const char *xmlFile)
{
  _filename = xmlFile;
  _doc = NULL;
  _MTs = NULL;
  _chrs = NULL;

  _doc = new TiXmlDocument();
  if (!_doc->LoadFile(xmlFile))
  {
    std::string errDesc = _doc->ErrorDesc();
    delete _doc;
    throw std::runtime_error(std::string("Cannot load XML-file with initial states: ") + errDesc);
  }

  for (TiXmlNode *n = _doc->RootElement()->FirstChild();
     n != NULL;
     n = n->NextSibling())
  {
    TiXmlElement *el = n->ToElement();
    if (el != NULL)
    {
      if (std::string("MTs") == el->Value())
        _MTs = el;
      else if (std::string("Chromosomes") == el->Value())
        _chrs = el;
      else
      {
        std::string elValue = el->Value();
        delete _doc;
        throw std::runtime_error(
              std::string("XML-file with initial states contains unknown section: ") + elValue);
      }
    }
  }

  if (_MTs == NULL || _chrs == NULL)
  {
    delete _doc;
    throw std::runtime_error("Some sections were not found in XML-file with initial states");
  }
}

void XmlCellInitializer::GetCellConfig(size_t &chrPairs, size_t &mtsPerPole)
{
  //Tubes.
  int mtCount1 = 0, mtCount2 = 0;
  mtCount1 = GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::N_MT_Total) * 2;

  for (TiXmlNode *n = _MTs->FirstChild();
     n != NULL;
     n = n->NextSibling())
  {
    TiXmlElement *el = n->ToElement();
    if (el != NULL && std::string("MT") == el->Value())
      mtCount2 += 1;
  }

  if (mtCount1 != mtCount2)
    throw std::runtime_error(
        "Number of MTs from config-file is not equal to number from XML-file with initial states");

  mtsPerPole = (size_t)mtCount1 / 2;

  //Chromosomes.
  int chrPairCount1 = 0, chrPairCount2 = 0;

  chrPairCount1 = GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::N_Cr_Total);

  for (TiXmlNode *n = _chrs->FirstChild();
     n != NULL;
     n = n->NextSibling())
  {
    TiXmlElement *el = n->ToElement();
    if (el != NULL && std::string("ChromosomePair") == el->Value())
      chrPairCount2 += 1;
  }

  if (chrPairCount1 != chrPairCount2)
    throw std::runtime_error(
        "Number of chromosomes from config-file is not equal to number from XML-file with initial states");

  chrPairs = (size_t)chrPairCount1;
}

void XmlCellInitializer::InitializeCell(ICell *cell, Random::State &)
{
  real r_cell = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::R_Cell, true);

  //Setting flag.
  cell->SetSpringFlag(false);

  //Initializing poles.
  real l_poles = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::L_Poles, true);
  vec3r pos = vec3r::DEFAULT_LEFT * (l_poles / 2);
  cell->GetPole(PoleType::Left)->Position() = pos;
  cell->GetPole(PoleType::Right)->Position() = -pos;

  //Initializing micro tubes.
  const std::vector<MT *> &MTs = cell->MTs();
  std::string polymerization = "polymerization";
  std::string depolymerization = "depolymerization";
  std::string left = "left";
  std::string right = "right";
  int curLeftMT = 0, curRightMT = (int)MTs.size() / 2;

  for (TiXmlNode *n = _MTs->FirstChild();
     n != NULL;
     n = n->NextSibling())
  {
    TiXmlElement *el = n->ToElement();
    if (el != NULL && std::string("MT") == el->Value())
    {
      double direction_x = 0.0, direction_y = 0.0, direction_z = 0.0;
      double length = 0.0;
      std::string state;
      std::string pole;

      if (el->QueryDoubleAttribute("direction_x", &direction_x) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("direction_y", &direction_y) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("direction_z", &direction_z) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("length", &length) != TIXML_SUCCESS ||
        el->QueryStringAttribute("pole", &pole) != TIXML_SUCCESS ||
        el->QueryStringAttribute("state", &state) != TIXML_SUCCESS)
        throw std::runtime_error("Cannot get some values from MT section from XML-file with initial states");

      vec3r direction((real)direction_x,
              (real)direction_y,
              (real)direction_z);
      if ((pole != right && pole != left) ||
        (state != polymerization && state != depolymerization) ||
        abs((float)direction.GetLength()) <= 1e-2)
        throw std::runtime_error("Some attributes from MT section from XML-file with initial states have wrong values");
      direction = direction.Normalize();

      int idx = 0;
      if (pole == right)
        idx = curRightMT++;
      else
        idx = curLeftMT++;

      if (curLeftMT > (int)MTs.size() / 2 || curRightMT > (int)MTs.size())
        throw std::runtime_error("Wrong XML-file with initial states - too many sections with MTs");

      MTs[idx]->Direction() = direction;
      MTs[idx]->Length() = (real)length;
      MTs[idx]->State() = state == polymerization ? MTState::Polymerization : MTState::Depolymerization;

      if (MTs[idx]->EndPoint().GetLength() > r_cell * 1.01)
        throw std::runtime_error("Wrong XML-file with initial states - MT's end is out of cell");
    }
  }

  if (curLeftMT != (int)MTs.size() / 2 || curRightMT != (int)MTs.size())
    throw std::runtime_error("Not enough MT sections in XML-file with initial states");

  //Initializing chromosomes.
  int curChrPair = 0;

  double cr_spring_l = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Spring_Length, true);
  const std::vector<Chromosome *> &chrs = cell->Chromosomes();
  for (TiXmlNode *n = _chrs->FirstChild();
     n != NULL;
     n = n->NextSibling())
  {
    TiXmlElement *el = n->ToElement();
    if (el != NULL && std::string("ChromosomePair") == el->Value())
    {
      if (curChrPair >= (int)chrs.size() / 2)
        throw std::runtime_error("Wrong XML-file with initial states - too many sections with chromosomes");

      double position_x = 0.0, position_y = 0.0, position_z = 0.0;
      double orientation_x = 0.0, orientation_y = 0.0, orientation_z = 0.0;

      if (el->QueryDoubleAttribute("position_x", &position_x) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("position_y", &position_y) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("position_z", &position_z) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("orientation_x", &orientation_x) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("orientation_y", &orientation_y) != TIXML_SUCCESS ||
        el->QueryDoubleAttribute("orientation_z", &orientation_z) != TIXML_SUCCESS)
        throw std::runtime_error("Cannot get some values from Chromosome section from XML-file with initial states");

      vec3r position((real)position_x, (real)position_y, (real)position_z);
      if (position.GetLength() > r_cell * 1.01)
        throw std::runtime_error("Wrong XML-file with initial states - chromosome is out of cell");

      //Right
      mat3x3r mat1 = MatrixRotationXYZ<real>((real)orientation_x,
                           (real)orientation_y, 
                           (real)orientation_z);
      chrs[2 * curChrPair + 1]->Position() = position + mat1 * vec3r(real(cr_spring_l / 2), real(0), real(0));
      chrs[2 * curChrPair + 1]->Orientation() = mat1;
      
      //Left
      mat3x3r mat2 = mat1 * MatrixRotationY<real>((real)PI);
      chrs[2 * curChrPair + 0]->Position() = position + mat2 * vec3r(real(cr_spring_l / 2), real(0), real(0));
      chrs[2 * curChrPair + 0]->Orientation() = mat2;

      curChrPair += 1;
    }
  }

  if (curChrPair != (int)chrs.size() / 2)
    throw std::runtime_error("Not enough MT sections in XML-file with initial states");
}

XmlCellInitializer::~XmlCellInitializer()
{
  if (_doc != NULL)
  {
    delete _doc;
    _doc = NULL;
  }
}

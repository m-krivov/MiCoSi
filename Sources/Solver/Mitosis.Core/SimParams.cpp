
#include "SimParams.h"
#include "Converter.h"

//-------------------------
//--- SimParameter::Int ---
//-------------------------

SimParameter::BaseType<SimParameter::Int::Type, int> *SimParameter::Int::_ref = NULL;

void SimParameter::Int::SafeInit()
{
  if (_ref != NULL)
    return;

  _ref = new BaseType<Type, int>();
  _ref->Register(Int::N_MT_Total, Descriptor<int>("n_mt_total", true, 750, 1, true, 0, false, 0));
  _ref->Register(Int::N_Cr_Total, Descriptor<int>("n_cr_total", true, 23, 1, true, 0, false, 0));
  _ref->Register(Int::Spring_Brake_Type, Descriptor<int>("spring_brake_type", false, 0, 1, true, 0, true, 1));
  _ref->Register(Int::Spring_Brake_MTs, Descriptor<int>("spring_brake_mts", false, 25, 1, true, 0, false, 0));
  _ref->Register(Int::N_Nods, Descriptor<int>("n_nods", true, 676, 1, true, 0, false, 0));
  _ref->Register(Int::Spring_Type, Descriptor<int>("spring_type", false, 0, 1, true, 0, true, 1));
  _ref->Register(Int::Frozen_Coords, Descriptor<int>("frozen_coords", false, 0, 1, true, 0, true, 1));
  _ref->Register(Int::MT_Wrapping, Descriptor<int>("mt_wrapping", false, 1, 1, true, 0, true, 1));
}

//----------------------------
//--- SimParameter::Double ---
//----------------------------

SimParameter::BaseType<SimParameter::Double::Type, double> *SimParameter::Double::_ref = NULL;

void SimParameter::Double::SafeInit()
{
  if (_ref != NULL)
    return;

  _ref = new BaseType<Type, double>();
  _ref->Register(Double::L_Poles, Descriptor<double>("l_poles", true, 14.0, 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::R_Cell, Descriptor<double>("r_cell", true, 8.0, 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::Spring_Brake_Force, Descriptor<double>("spring_brake_force", false, 700.0, 1e-12, true, 0.0, false, 0.0));
  _ref->Register(Double::V_Pol, Descriptor<double>("v_pol", false, 12.8, 1e-6 / 60.0, true, 0.0, false, 0.0));
  _ref->Register(Double::V_Dep, Descriptor<double>("v_dep", false, 14.1, 1e-6 / 60.0, true, 0.0, false, 0.0));
  _ref->Register(Double::F_Cat, Descriptor<double>("f_cat", false, 0.058, 1.0, true, 0.0, true, 1.0));
  _ref->Register(Double::F_Res, Descriptor<double>("f_res", false, 0.045, 1.0, true, 0.0, true, 1.0));
  _ref->Register(Double::Gamma, Descriptor<double>("gamma", false, 0.006, 1e-12 / 1e-9, true, 0.0, false, 0.0));
  _ref->Register(Double::Ieta, Descriptor<double>("ieta", false, 5700.0, 1e-12 * 1e-9, true, 0.0, false, 0.0));
  _ref->Register(Double::L1, Descriptor<double>("l1", true, 100.0, 1e-9, true, 0.0, false, 0.0));
  _ref->Register(Double::L3, Descriptor<double>("l3", true, 60.0, 1e-9, true, 0.0, false, 0.0));
  _ref->Register(Double::Eps, Descriptor<double>("eps", false, 0.05, 1.0, true, 0.0, true, 1.0));
  _ref->Register(Double::K_On, Descriptor<double>("k_on", false, 1.0, 1.0, true, 0.0, false, 0.0));
  _ref->Register(Double::K_Off, Descriptor<double>("k_off", false, 4.0, 1.0, true, 0.0, false, 0.0));
  _ref->Register(Double::Dt, Descriptor<double>("dt", false, 0.1, 1.0, true, 1e-3, false, 0.0));
  _ref->Register(Double::T_End, Descriptor<double>("t_end", false, 100.0, 1.0, true, 0.0, false, 0.0));
  _ref->Register(Double::Save_Freq_Micro, Descriptor<double>("save_freq_micro", false, 0.1, 1.0, true, 0.0, false, 0.0));
  _ref->Register(Double::Save_Freq_Macro, Descriptor<double>("save_freq_macro", false, 0.1, 1.0, true, 0.0, false, 0.0));
  _ref->Register(Double::Cr_L, Descriptor<double>("cr_l", true, 5.0, 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::Cr_Kin_L, Descriptor<double>("cr_kin_l", true, 0.5, 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::Cr_Kin_D, Descriptor<double>("cr_kin_d", true, 0.3, 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::Cr_Kin_Angle, Descriptor<double>("cr_kin_angle", true, 115.0, PI / 180.0, true, 0.0, true, 180.0));
  _ref->Register(Double::Cr_Hand_D, Descriptor<double>("cr_hand_d", true, 0.5, 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::Const_A, Descriptor<double>("const_a", false, 45, 1e-12, true, 0.0, false, 0.0));
  _ref->Register(Double::Const_B, Descriptor<double>("const_b", false, 45 / 12.8, 1e-12 * 60.0 / 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::D_Trans, Descriptor<double>("d_trans", false, 0.01, 1e-9 * 1e-9, true, 0.0, false, 0.0));
  _ref->Register(Double::D_Rot, Descriptor<double>("d_rot", false, 1.5 * 1e-3, 1.0, true, 0.0, false, 0.0));
  _ref->Register(Double::Spring_Length, Descriptor<double>("spring_length", true, 0.0, 1e-6, true, 0.0, false, 0.0));
  _ref->Register(Double::Spring_K, Descriptor<double>("spring_k", false, 500.0, 1e-6, true, 20.0, false, 0.0));
}

//-----------------
//--- SimParams ---
//-----------------

SimParams::SimParams()
  : _access(Access::Initialize)
{
  //Cannot call SetDefault() because we need to initialize maps.
  {
    const std::vector<SimParameter::Int::Type> &intParams = SimParameter::Int::All();
    for (size_t i = 0; i < intParams.size(); i++)
      _intValues[intParams[i]] = SimParameter::Int::Info(intParams[i]).DefaultValue();
  }

  {
    const std::vector<SimParameter::Double::Type> &doubleParams = SimParameter::Double::All();
    for (size_t i = 0; i < doubleParams.size(); i++)
      _doubleValues[doubleParams[i]] = SimParameter::Double::Info(doubleParams[i]).DefaultValue();
  }

  SetAccess(Access::ReadOnly);
}

int SimParams::GetParameter(SimParameter::Int::Type type) const
{
  auto iter = _intValues.find(type);
  if (iter == _intValues.end())
    throw std::runtime_error("Wrong type of parameter");
  return iter->second;
}

void SimParams::SetParameter(SimParameter::Int::Type type, int value)
{
  if (_access == Access::ReadOnly)
    throw std::runtime_error("The value of parameter cannot be changed due to read-only restriction");

  auto iter = _intValues.find(type);
  if (iter == _intValues.end())
    throw std::runtime_error("Wrong type of parameter");
  if (_access == Access::Update && SimParameter::Int::Info(type).Constant() && _intValues[type] != value)
    throw std::runtime_error("Cannot set value for constant (not changeable) parameter");
  _intValues[type] = value;
}

double SimParams::GetParameter(SimParameter::Double::Type type, bool convertToSI) const
{
  auto iter = _doubleValues.find(type);
  if (iter == _doubleValues.end())
    throw std::runtime_error("Wrong type of parameter");
  return convertToSI ? iter->second * SimParameter::Double::Info(type).SiMultiplier() : iter->second;
}

void SimParams::SetParameter(SimParameter::Double::Type type, double value, bool isSI)
{
  if (_access == Access::ReadOnly)
    throw std::runtime_error("The value of parameter cannot be changed due to read-only restriction");

  auto iter = _doubleValues.find(type);
  if (iter == _doubleValues.end())
    throw std::runtime_error("Wrong type of parameter");
  value = isSI ? value / SimParameter::Double::Info(type).SiMultiplier() : value;
  if (_access == Access::Update && SimParameter::Double::Info(type).Constant() && _doubleValues[type] != value)
    throw std::runtime_error("Cannot set value for constant (not changeable) parameter");
  _doubleValues[type] = value;
}

std::string SimParams::GetParameter(const std::string &name, bool convertToSI) const
{
  SimParameter::Int::Type intType;
  bool isInt = SimParameter::Int::TryParse(name, intType);
  if (isInt)
    return Converter::ToString(GetParameter(intType));

  SimParameter::Double::Type doubleType;
  bool isDouble = SimParameter::Double::TryParse(name, doubleType);
  if (isDouble)
    return Converter::ToString(GetParameter(doubleType, convertToSI));

  throw std::runtime_error("Parameter with such name is not known");
}

void SimParams::SetParameter(const std::string &name, const std::string &value, bool isSI)
{
  SimParameter::Int::Type intType;
  bool isInt = SimParameter::Int::TryParse(name, intType);
  if (isInt)
  {
    int intValue = 0;
    if (!Converter::TryParse(value, intValue))
      throw std::runtime_error("Failed to parse parameter's value");
    SetParameter(intType, intValue);
    return;
  }

  SimParameter::Double::Type doubleType;
  bool isDouble = SimParameter::Double::TryParse(name, doubleType);
  if (isDouble)
  {
    double doubleValue = 0;
    if (!Converter::TryParse(value, doubleValue))
      throw std::runtime_error("Failed to parse parameter's value");
    SetParameter(doubleType, doubleValue, isSI);
    return;
  }

  throw std::runtime_error("Parameter with such name is not known");
}

void SimParams::SetDefault()
{
  {
    const std::vector<SimParameter::Int::Type> &intParams = SimParameter::Int::All();
    for (size_t i = 0; i < intParams.size(); i++)
      SetParameter(intParams[i], SimParameter::Int::Info(intParams[i]).DefaultValue());
  }

  {
    const std::vector<SimParameter::Double::Type> &doubleParams = SimParameter::Double::All();
    for (size_t i = 0; i < doubleParams.size(); i++)
      SetParameter(doubleParams[i], SimParameter::Double::Info(doubleParams[i]).DefaultValue(), false);
  }
}

std::vector<std::pair<std::string, std::string> > SimParams::ExportValues() const
{
  std::vector<std::pair<std::string, std::string> > res;
  {
    for (auto iter = _intValues.begin(); iter != _intValues.end(); iter++)
      res.push_back(std::make_pair(SimParameter::Int::Info(iter->first).Name(),
                     Converter::ToString(iter->second)));
  }

  {
    for (auto iter = _doubleValues.begin(); iter != _doubleValues.end(); iter++)
      res.push_back(std::make_pair(SimParameter::Double::Info(iter->first).Name(),
                       Converter::ToString(iter->second)));
  }

  return res;
}

void SimParams::ImportValues(const std::vector<std::pair<std::string, std::string> > &values)
{
  for (size_t i = 0; i < values.size(); i++)
    SetParameter(values[i].first, values[i].second);
}

//-----------------------
//--- GlobalSimParams ---
//-----------------------

SimParams *GlobalSimParams::_ref = new SimParams();

#pragma once
#include "Defs.h"
#include "Helpers.h"

void SpellingTest(String ^props, bool isCorrect)
{
  bool hasError = false;

  try
  {
    auto config = gcnew SimParams();
    config->ImportAsProps(props);
  }
  catch (Exception ^)
  { hasError = true; }

  if (hasError ^ !isCorrect)
  {
    FAIL() << StringToString((isCorrect ? "Failed to parse correct props: \""
                                        : "Successfully processed wrong props: \"") +
                              props + "\"");
  }
}

TEST(Props, Spelling)
{
  cli::array<String ^> ^correctProps = gcnew cli::array<String ^>
  {
    //Spaces at the start, commented lines.
    "  N_MT_Total=150" + Environment::NewLine +
    "   Spring_Brake_Type=1" + Environment::NewLine +
    "   #N_Cr_Total=-1" + Environment::NewLine +
    "#Spring_Brake_MTs=-1" + Environment::NewLine,
        
    //Spaces everywhere.
    "  K_On   =  2.0  " + Environment::NewLine +
    "   K_Off  = 2.0  " + Environment::NewLine +
    "Dt  =0.2 ",

    //Capital and lower letters.
    " k_on = 2.0  " + Environment::NewLine +
    " K_OFF  = 2.0  " + Environment::NewLine +
    " dT = 0.2 " + Environment::NewLine,

    //Different numeric styles.
    "T_End=1e1" + Environment::NewLine +
    "Dt=1e-1" + Environment::NewLine + 
    "K_On=0.2e1"
  };

  for each (auto props in correctProps)
  { SpellingTest(props, true); }

  cli::array<String ^> ^wrongProps = gcnew cli::array<String ^>
  {
    //No '='.
    "N_MT_Total 150" + Environment::NewLine,

    //Commented '='.
    "N_MT_Total#=150" + Environment::NewLine,

    //Wrong symbols after parameter's value.
    "N_MT_Total=150 //setting 150 MTs for each pole",

    //Wrong value.
    "N_MT_Total=150.0" + Environment::NewLine,

    //Another wrong value (comma instead of dot).
    "T_End=150,0" + Environment::NewLine,

    //Unknown parameter.
    "T_Start=10" + Environment::NewLine
  };

  for each (auto props in wrongProps)
  { SpellingTest(props, false); }
}

TEST(Props, ParameterImpact)
{
  //Changing values - multiplying them by two.
  auto config = gcnew SimParams();
  for each (auto param in Enum::GetValues(SimParameter::Double::typeid))
  {
    auto p = safe_cast<SimParameter::Double>(param);
    config[p] = SimParams::GetDefaultValue(p, false) * 2.0;
  }
  config[SimParameter::Int::N_MT_Total] = 1;
  config[SimParameter::Int::N_Cr_Total] = 15;
  config[SimParameter::Int::Spring_Brake_Type] = SimParams::GetDefaultValue(SimParameter::Int::Spring_Brake_Type) == 0 ? 1 : 0;
  config[SimParameter::Int::Spring_Brake_MTs] = SimParams::GetDefaultValue(SimParameter::Int::Spring_Brake_MTs) * 2;
  config[SimParameter::Int::N_Nods] = SimParams::GetDefaultValue(SimParameter::Int::N_Nods) * 2;
  config[SimParameter::Int::Spring_Type] = SimParams::GetDefaultValue(SimParameter::Int::Spring_Type) == 0 ? 1 : 0;
  config[SimParameter::Int::Frozen_Coords] = SimParams::GetDefaultValue(SimParameter::Int::Frozen_Coords) == 0 ? 1 : 0;
  config[SimParameter::Int::MT_Wrapping] = SimParams::GetDefaultValue(SimParameter::Int::MT_Wrapping) == 0 ? 1 : 0;

  //Launching simulation, getting copy of the used parameters.
  SimParams ^usedConfig = nullptr;
  TimeStream ^ts = nullptr;
  try
  {
    Helper::PrepareTestDirectory();
    auto parameters = gcnew LaunchParameters();
    parameters->Config = config;
    ts = Helper::LaunchAndOpen(parameters);
    {
      ts->MoveTo(ts->LayerCount - 1);
      usedConfig = safe_cast<SimParams ^>(ts->Current->SimParams->Clone());
    }
  }
  catch (Exception ^ex)
  { FAIL() << StringToString(ex->Message); }
  finally
  {
    if (ts != nullptr) { delete ts; }
    Helper::ClearUpTestDirectory();
  }

  //Comparing all values. Expecting the 100% matching.
  for each (auto param in Enum::GetValues(SimParameter::Double::typeid))
  {
    auto p = safe_cast<SimParameter::Double>(param);
    if (config[p] != usedConfig[p])
    {
      FAIL() << StringToString(String::Format("Value of the \"{0}\" parameter ignored: \"{1}\" specified, but \"{2}\" used",
                                              p.ToString(), config[p], usedConfig[p]));
    }
  }
  for each (auto param in Enum::GetValues(SimParameter::Int::typeid))
  {
    auto p = safe_cast<SimParameter::Int>(param);
    if (config[p] != usedConfig[p])
    {
      FAIL() << StringToString(String::Format("Value of the \"{0}\" parameter ignored: \"{1}\" specified, but \"{2}\" used",
                                              p.ToString(), config[p], usedConfig[p]));
    }
  }
}

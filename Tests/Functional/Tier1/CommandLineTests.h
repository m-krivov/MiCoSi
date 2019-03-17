#pragma once
#include "Defs.h"
#include "Helpers.h"

using namespace Mitosis;


static void COMMAND_LINE_TEST(IEnumerable<String ^> ^cliArgs, bool isWrong)
{
  for each (auto args in cliArgs)
  {
    bool errorDetected = false;
    try
    {
      Helper::PrepareTestDirectory();
      auto parameters = gcnew LaunchParameters();
      parameters->Args->Import(args);
      if (Helper::Launch(parameters)->ExitedWithError)
      { throw gcnew ApplicationException("Failed to launch"); }
    }
    catch (Exception ^ex)
    {
      if (!isWrong)
      { FAIL() << StringToString(String::Format("Correct arguments \"{0}\", but error detected: {1}", 
                                 args, ex->Message)); }
      errorDetected = true;
    }
    finally
    { Helper::ClearUpTestDirectory(); }

    if (isWrong && !errorDetected)
    { FAIL() << StringToString(String::Format("Wrong arguments \"{0}\", but no error detected",
                               args)); };
  }
}

TEST(CommandLine, Redefinition)
{
  cli::array<String ^> ^argSet = { "--mode new --mode new",
                                   "--mode new --mode restart",
                                   "--mode continue --help",
                                   "--csv --print_delay 1 --csv",
                                   "--cell my_cell.cell --cell another_my_cell.cell" };

  COMMAND_LINE_TEST(argSet, true);
}

TEST(CommandLine, UnknownOptions)
{
  cli::array<String ^> ^argSet = { "--mode new --CPUs 1 --GPUs 0",
                                   "--mode new --random 100500",
                                   "--mode new --force",
                                   "--mode default",
                                   "--NeW",
                                   "--mode reset",
                                   "-continue" };
  COMMAND_LINE_TEST(argSet, true);
}

TEST(CommandLine, WrongSolvers)
{
  cli::array<String ^> ^argSet = { "--solver CPU:1.2",
                                   "--solver APU",
                                   "--solver CUDA:-1",
                                   "--solver CUDA:GF580",
                                   "--solver gold",
                                   "--solver experimental:2" };
  COMMAND_LINE_TEST(argSet, true);
}

TEST(CommandLine, NoCellForUpdate)
{
  cli::array<String ^> ^argSet = { "--mode fix",
                                   "--mode continue",
                                   "--mode restart" };
  COMMAND_LINE_TEST(argSet, true);
}

TEST(CommandLine, CorrectArgs)
{
  cli::array<String ^> ^argSet = { "--mode info", "--help" };   //no --new due to huge time with default config
  COMMAND_LINE_TEST(argSet, false);
}

TEST(CommandLine, Help)
{
  try
  {
    Helper::PrepareTestDirectory();
    auto parameters = gcnew LaunchParameters();
    parameters->Args->Mode = LaunchMode::Help;
    auto res = Helper::Launch(parameters);
    if (res->ExitedWithError) FAIL() << StringToString("Failed to get help");

    auto output = res->Output;
    ASSERT_TRUE(output->Contains("-h") || output->Contains("--help")) << StringToString("Have no info about \"-h\"/\"--help\"");
    ASSERT_TRUE(output->Contains("--mode")) << StringToString("Have no info about \"--mode\"");
        
    ASSERT_TRUE(output->Contains("--cell")) << StringToString("Have no info about \"--cell\"");
    ASSERT_TRUE(output->Contains("--config")) << StringToString("Have no info about \"--config\"");
    ASSERT_TRUE(output->Contains("--initial")) << StringToString("Have no info about \"--initial\"");
    ASSERT_TRUE(output->Contains("--poles")) << StringToString("Have no info about \"--poles\"");

    ASSERT_TRUE(output->Contains("--seed")) << StringToString("Have no info about \"--seed\"");
    ASSERT_TRUE(output->Contains("--series")) << StringToString("Have no info about \"--series\"");
    ASSERT_TRUE(output->Contains("--solver")) << StringToString("Have no info about \"--solver\"");

    ASSERT_TRUE(output->Contains("--csv")) << StringToString("Have no info about \"--csv\"");
    ASSERT_TRUE(output->Contains("--print_delay")) << StringToString("Have no info about \"--print_delay\"");
  }
  catch (Exception ^ex) { FAIL() << StringToString(ex->Message); }
  finally { Helper::ClearUpTestDirectory(); }
}

TEST(CommandLine, AllArgs)
{
  try
  {
    Helper::PrepareTestDirectory();

    auto parameters = gcnew LaunchParameters();
    parameters->Args->CellFile = "cell.cell";
    parameters->Args->CellCount = 2;
    auto argsForFix = safe_cast<CliArgs ^>(parameters->Args->Clone());
    auto argsForContinue = safe_cast<CliArgs ^>(parameters->Args->Clone());
    parameters->Args->RngSeed = 42;
    auto argsForNew = safe_cast<CliArgs ^>(parameters->Args->Clone());

    parameters->Config = gcnew SimParams();
    parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
    parameters->Config[SimParameter::Int::N_MT_Total] = 1;
    parameters->Config[SimParameter::Double::Dt] = 0.5;
    parameters->Config[SimParameter::Double::T_End] = 2.0;
    parameters->Config[SimParameter::Int::Frozen_Coords] = 1;

    parameters->InitialStates = gcnew InitialStates();
    parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, 1.0, 0.0, 0.0, 0.0, MTState::Polymerization);

    parameters->PoleCoords = gcnew PoleCoordinates();
    parameters->PoleCoords->AddRecord(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    parameters->PoleCoords->AddRecord(2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    {
      parameters->Args->Mode = LaunchMode::New;
      ASSERT_FALSE(Helper::Launch(parameters)->ExitedWithError) << StringToString("Failed to launch with \"new\" mode");
    }

    {
      parameters->Args = argsForContinue;
      parameters->Args->Mode = LaunchMode::Restart;
      ASSERT_FALSE(Helper::Launch(parameters)->ExitedWithError) << StringToString("Failed to launch with \"restart\" mode");
    }

    {
      parameters->Args = argsForContinue;
      auto initial = parameters->InitialStates;
      parameters->InitialStates = nullptr;
      parameters->Args->Mode = LaunchMode::Continue;
      ASSERT_FALSE(Helper::Launch(parameters)->ExitedWithError) << StringToString("Failed to launch with \"continue\" mode");
      parameters->InitialStates = initial;
    }

    {
      parameters->Args = argsForNew;
      parameters->Args->Mode = LaunchMode::New;
      parameters->Args->CellCount = 1;
      ASSERT_FALSE(Helper::Launch(parameters)->ExitedWithError) << StringToString("Failed to launch with \"new\" mode #2");
    }

    {
      parameters->Args = argsForFix;
      parameters->Args->CellCount = 1;
      parameters->Args->Mode = LaunchMode::Fix;
      auto res = Helper::Launch(parameters);
      ASSERT_FALSE(res->ExitedWithError || res->Output->ToLower()->Contains("error"))
        << StringToString("Failed to launch with \"fix\" mode");
    }
  }
  catch (Exception ^ex) { FAIL() << StringToString(ex->Message); }
  finally { Helper::ClearUpTestDirectory(); }
}

TEST(CommandLine, ResettingArg)
{
  try
  {
    auto args = gcnew CliArgs();
    auto seedOpt = CliArgs::OptionName(CliArgs::Option::RngSeed);
        
    //Seed option not defined by default.
    ASSERT_FALSE(args->Export()->Contains(seedOpt)) << StringToString("Some options are defined by default");

    //Now it's defined.
    args->RngSeed = 100500;
    ASSERT_TRUE(args->Export()->Contains(seedOpt)) << StringToString("Defined option was ignored during export");

    //And now not.
    args->Reset(CliArgs::Option::RngSeed);
    ASSERT_FALSE(args->Export()->Contains(seedOpt)) << StringToString("Resetted option is still defined");
  }
  catch (Exception ^ex)
  { FAIL() << StringToString(ex->Message); }
}

#include "MiCoSi.Core/All.h"
#include "MiCoSi.Args/MitosisArgs.h"
#include "WorkingDirUtility.h"
#include "Formatters/ConsoleFormatter.h"
#include "Formatters/CsvFormatter.h"


int main(int argc, char *argv[])
{
  // First of all, load args
  MitosisArgs args;
  try
  { args.Import(argc, argv); }
  catch (std::exception &ex)
  {
    ConsoleFormatter formatter(1.0);    // failed to get the right formatter, use the default
    formatter.PrintError(IErrorFormatter::WrongParams, ex.what());
    return 42;
  }

  // Check for the "--help" option
  if (args.GetMode() == LaunchMode::Help)
  {
    printf(args.HelpMessage().c_str());
    return 0;
  }

  std::unique_ptr<IOutputFormatter> out_formatter(args.GetCsvOutput()
                          ? (IOutputFormatter *)(new CsvFormatter(args.GetPrintDelay()))
                          : (IOutputFormatter *)(new ConsoleFormatter(args.GetPrintDelay())));
  IErrorFormatter *err_formatter = dynamic_cast<IErrorFormatter *>(out_formatter.get());

  // Check for the "--info" option
  if (args.GetMode() == LaunchMode::Info)
  {
#ifdef NO_CUDA
    bool cudaSupport = false;
#else
    bool cudaSupport = true;
#endif
    out_formatter->PrintSystemInfo(HardwareScanner::Scan(),
                                   CurrentVersion::ProgramVersion(),
                                   CurrentVersion::FileFormatVersion(),
                                   cudaSupport);
    return 0;
  }

  // Check for the "--fix" option
  if (args.GetMode() == LaunchMode::Fix)
  {
    try
    {
      out_formatter->PrintRepairStarted(args.GetCellFile().c_str());
      auto res = WorkingDirUtility::Fix(args.GetCellFile().c_str());
      out_formatter->PrintRepairCompleted(res.first, res.second);
      return 0;
    }
    catch (std::exception &ex)
    {
      err_formatter->PrintError(IErrorFormatter::FailedToRepair, ex.what());
      return 42;
    }
  }

  // Check for "--new", "--restart" and "--continue" options
  // Create the simulator and time streams
  std::unique_ptr<Simulation> simulation;
  try
  {
    switch (args.GetMode())
    {
      case LaunchMode::New:
      {
        simulation = WorkingDirUtility::Start(args.GetCellFile().c_str(),
                                              args.GetConfigFile().c_str(),
                                              args.GetInitialConditionsFile(),
                                              args.GetPoleCoordsFile(),
                                              args.GetCellCount(),
                                              args.GetUserSeed(),
                                              args.GetSolver());
        break;
      }

      case LaunchMode::Restart:
        simulation = WorkingDirUtility::Restart(args.GetCellFile().c_str(),
                                                args.GetConfigFile().c_str(),
                                                args.GetInitialConditionsFile(),
                                                args.GetPoleCoordsFile(),
                                                args.GetCellCount(),
                                                args.GetSolver());
        break;

      case LaunchMode::Continue:
        simulation = WorkingDirUtility::Continue(args.GetCellFile().c_str(),
                                                 args.GetConfigFile().c_str(),
                                                 args.GetInitialConditionsFile(),
                                                 args.GetPoleCoordsFile(),
                                                 args.GetCellCount(),
                                                 args.GetSolver());
        break;

      default:
        throw std::runtime_error("ønternal error: unknown mode, did someone forget to update Main.cpp?");
    }
  }
  catch (std::exception &ex)
  {
    err_formatter->PrintError(IErrorFormatter::RuntimeError, ex.what());
    return 42;
  }

  // Perform simulation
  try
  {
    double t_end = (double)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::T_End, true);
    double saveFreq = (double)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Save_Freq_Macro, true);
    out_formatter->PrintOnStart(simulation->Cells());
    
    double lastSavedTime = simulation->Time();
    while (!simulation->IsFinished())
    {
      simulation->DoIteration();
      out_formatter->PrintIterationInfo(simulation.get(), simulation->Time(), t_end);
      if (lastSavedTime + saveFreq <= simulation->Time() + 1e-5)
      {
        lastSavedTime = simulation->Time();
        simulation->SaveStates();
      }
    }

    out_formatter->PrintOnFinish(simulation->Cells());
    simulation.reset(nullptr);
  }
  catch (std::exception &ex)
  {
    err_formatter->PrintError(IErrorFormatter::RuntimeError, ex.what());
    return 42;
  }

  return 0;
}

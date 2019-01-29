
#include "ttgUtils.h"

#include "Mitosis.Core/All.h"
#include "ConsoleApp.Args/MitosisArgs.h"
#include "WorkingDirUtility.h"
#include "Formatters/ConsoleFormatter.h"
#include "Formatters/CsvFormatter.h"


void HelperDump(Cell* cell)
{
#if defined(TTG_UTILS) && defined(TTG_UTILS_DUMPS)
  vec3r leftPole = (vec3r)cell->GetPole(PoleType::Left)->Position();
  vec3r rightPole = (vec3r)cell->GetPole(PoleType::Right)->Position();
  real vecArray[3];
  vecArray[0] = leftPole.x; vecArray[1] = leftPole.y; vecArray[2] = leftPole.z;
  ttg_array_dump("leftPole", vecArray, 3);
  vecArray[0] = rightPole.x; vecArray[1] = rightPole.y; vecArray[2] = rightPole.z;
  ttg_array_dump("rightPole", vecArray, 3);
  const std::vector<MT *> &mtsRefs = cell->MTs();
  std::vector<real> lengthes(mtsRefs.size());
  std::vector<real> dir_x(mtsRefs.size());
  std::vector<real> dir_y(mtsRefs.size());
  std::vector<real> dir_z(mtsRefs.size());
  std::vector<int> states(mtsRefs.size());
  std::vector<int> boundIDs(mtsRefs.size());
  for (size_t i = 0; i < lengthes.size(); i++)
  {
    lengthes[i] = cell->MTs()[i]->Length();
    vec3r dir = (vec3r)mtsRefs[i]->Direction();
    dir_x[i] = dir.x;
    dir_y[i] = dir.y;
    dir_z[i] = dir.z;
    states[i] = mtsRefs[i]->State() == MTState::Polymerization ? 0 : 1;
    boundIDs[i] = mtsRefs[i]->BoundChromosome() == NULL ? -1 : mtsRefs[i]->BoundChromosome()->ID();
  }
  ttg_array_dump("length", &lengthes[0], lengthes.size());
  ttg_array_dump("dir_x", &dir_x[0], dir_x.size());
  ttg_array_dump("dir_y", &dir_y[0], dir_y.size());
  ttg_array_dump("dir_z", &dir_z[0], dir_z.size());
  ttg_array_dump("states", &states[0], states.size());
  ttg_array_dump("boundIDs", &boundIDs[0], boundIDs.size());
  
  std::vector<real> x(cell->Chromosomes().size());
  std::vector<real> y(cell->Chromosomes().size());
  std::vector<real> z(cell->Chromosomes().size());
  std::vector<real> mat(cell->Chromosomes().size() * 9);
  std::vector<int> cnt(cell->Chromosomes().size());
  std::vector<int> coff(cell->Chromosomes().size());
  std::list<uint32> ids;
  size_t ind = 0;
  for (size_t i = 0; i < cell->Chromosomes().size(); i++)
  {
    Chromosome *chrRef = cell->Chromosomes()[i];
    x[i] = chrRef->Position().x;
    y[i] = chrRef->Position().y;
    z[i] = chrRef->Position().z;
    mat3x3r chrOrient = (mat3x3r)chrRef->Orientation();
    mat[i * 9 + 0] = chrOrient.a[0];
    mat[i * 9 + 1] = chrOrient.a[1];
    mat[i * 9 + 2] = chrOrient.a[2];
    mat[i * 9 + 3] = chrOrient.a[3];
    mat[i * 9 + 4] = chrOrient.a[4];
    mat[i * 9 + 5] = chrOrient.a[5];
    mat[i * 9 + 6] = chrOrient.a[6];
    mat[i * 9 + 7] = chrOrient.a[7];
    mat[i * 9 + 8] = chrOrient.a[8];
    auto boundMTs = chrRef->BoundMTs();
    cnt[i] = boundMTs.size();
    coff[i] = ind;
    for (auto it = boundMTs.begin();
      it != boundMTs.end();
      it++)
    {
      ids.push_back((*it)->ID());
      ind++;
    }
  }
  std::vector<long> v(ids.size());
  int i = 0;
  for(auto it = ids.begin(); it != ids.end(); it++)
  {
    v[i] = *it;
    i++;
  }
  ttg_array_dump("x", &x[0], x.size());
  ttg_array_dump("y", &y[0], y.size());
  ttg_array_dump("z", &z[0], z.size());
  ttg_array_dump("matrix", &mat[0], mat.size());
  ttg_array_dump("cnt", &cnt[0], cnt.size());
  ttg_array_dump("coff", &coff[0], coff.size());
  if(ind != 0)
    ttg_array_dump("ids", &v[0], v.size());
  const std::vector<ChromosomePair *> &pairs = cell->ChromosomePairs();
  std::vector<int> leftChrID(pairs.size());
  std::vector<int> rightChrID(pairs.size());
  for (size_t i = 0; i < pairs.size(); i++)
  {
    leftChrID[i] = pairs[i]->LeftChromosome()->ID();
    rightChrID[i] = pairs[i]->RightChromosome()->ID();
  }
  if(pairs.size() > 0)
  {
    ttg_array_dump("leftChrID", &leftChrID[0], leftChrID.size());
    ttg_array_dump("rightChrID", &rightChrID[0], rightChrID.size());
  }
#endif
}

int main(int argc, char *argv[])
{
  MitosisArgs args;

  //Loading args.
  try
  { args.Import(argc, argv); }
  catch (std::exception &ex)
  {
    ConsoleFormatter formatter(1.0);    //failed to get right formatter, using default
    formatter.PrintError(IErrorFormatter::WrongParams, ex.what());
    return 42;
  }

  //Checking "--help" option.
  if (args.GetMode() == LaunchMode::Help)
  {
    printf(args.HelpMessage().c_str());
    return 0;
  }

  std::unique_ptr<IOutputFormatter> out_formatter(args.GetCsvOutput()
                          ? (IOutputFormatter *)(new CsvFormatter(args.GetPrintDelay()))
                          : (IOutputFormatter *)(new ConsoleFormatter(args.GetPrintDelay())));
  IErrorFormatter *err_formatter = dynamic_cast<IErrorFormatter *>(out_formatter.get());

  //Checking "--info" option.
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

  //Checking "--fix" option.
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

  ttg_init();

  //Checking "--new", "--restart" and "--continue" options, creating simulator and time stream.
  std::unique_ptr<Simulation> simulation;
  try
  {
    ttg_time_stamp("Total");
    ttg_time_stamp("InitSimulation");
    switch (args.GetMode())
    {
      case LaunchMode::New:
      {
        simulation.reset(WorkingDirUtility::Start(args.GetCellFile().c_str(),
                              args.GetConfigFile().c_str(),
                              args.GetInitialConditionsFile(),
                              args.GetPoleCoordsFile(),
                              args.GetCellCount(),
                              args.GetRngSeed(),
                              args.GetSolver()));
        break;
      }

      case LaunchMode::Restart:
        simulation.reset(WorkingDirUtility::Restart(args.GetCellFile().c_str(),
                              args.GetConfigFile().c_str(),
                              args.GetInitialConditionsFile(),
                              args.GetPoleCoordsFile(),
                              args.GetCellCount(),
                              args.GetSolver()));
        break;

      case LaunchMode::Continue:
        simulation.reset(WorkingDirUtility::Continue(args.GetCellFile().c_str(),
                               args.GetConfigFile().c_str(),
                               args.GetInitialConditionsFile(),
                               args.GetPoleCoordsFile(),
                               args.GetCellCount(),
                               args.GetSolver()));
        break;

      default:
        throw std::runtime_error("Internal error - unknown mode, did someone forget to update Main.cpp?");
    }
    ttg_time_stamp("InitSimulation");
  }
  catch (std::exception &ex)
  {
    err_formatter->PrintError(IErrorFormatter::RuntimeError, ex.what());
    ttg_time_stamp("InitSimulation");
    ttg_time_stamp("Total");
    ttg_close();
    return 42;
  }

  //Performing simulation.
  try
  {
    double t_end = (double)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::T_End, true);
    double saveFreq = (double)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Save_Freq_Macro, true);
    out_formatter->PrintOnStart(simulation->Cells());
    
    double lastSavedTime = simulation->Time();
    while (!simulation->IsFinished())
    {
      simulation->DoIteration();
      HelperDump(simulation->CellsAndSeeds()[0].first);
      out_formatter->PrintIterationInfo(simulation.get(), simulation->Time(), t_end);
      if (lastSavedTime + saveFreq <= simulation->Time() + 1e-5)
      {
        lastSavedTime = simulation->Time();
        simulation->SaveStates();
      }
    }

    out_formatter->PrintOnFinish(simulation->Cells());
    simulation.reset(NULL);
  }
  catch (std::exception &ex)
  {
    err_formatter->PrintError(IErrorFormatter::RuntimeError, ex.what());
    ttg_time_stamp("Total");
    ttg_close();
    return 42;
  }

  ttg_time_stamp("Total");
  ttg_close();

  return 0;
}

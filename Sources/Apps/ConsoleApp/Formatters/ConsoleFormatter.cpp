
#include "ConsoleFormatter.h"

#include "Mitosis.Core/All.h"
#include "Mitosis.Solvers/CellStats.h"

//------------------------
//--- ConsoleFormatter ---
//------------------------

void ConsoleFormatter::PrintSystemInfo(const HardwareInfo &info,
                                       const Version &programVersion,
                                       int fileFormatVersion,
                                       bool cudaSupport)
{
  printf("Mitosis Computer Simulator (aka MiCoSi)\n");
  printf("\n");
  printf("     Version:      v%d.%d.%d\n", programVersion.Major(), programVersion.Minor(), programVersion.Build());
  printf("     File format:  v%d\n", (int)fileFormatVersion);
  printf("     Release date: %s\n", programVersion.PublishDate().c_str());
  printf("     CUDA support: %s\n", cudaSupport ? "yes" : "no");
  printf("\n");
  printf("Available computing units (presets for \"--solver\" option)\n");
  printf("\n");
  printf("     \"cpu\":     %s, %d cores @ %0.1lf MHz\n", info.CPU().Name().c_str(),
                              (int)info.CPU().Cores(),
                              info.CPU().FrequencyMHz());
  for (size_t i = 0; i < info.GPUs().size(); i++)
  {
    auto gpu = info.GPUs()[i];
    printf("     \"cuda:%d\":  %s, %d cores @ %0.1lf MHz\n", (int)i,
                                 gpu.Name().c_str(),
                                 (int)gpu.Cores(),
                                 gpu.FrequencyMHz());
  }
  printf("\n");
}

void ConsoleFormatter::PrintTimeInfo(double curSimTime, double curRealTime, double totalSimTime)
{
  printf("Time, seconds: model - %3.1lf/%3.1lf, real - %3.1lf",
      curSimTime, totalSimTime, _predictor->ElapsedTime());
  double totalRealTime;
  if (_predictor->CanPredictTotalTime(curSimTime, totalRealTime))
    printf("/%3.1lf\n", totalRealTime);
  else
    printf("/unknown\n");
}

void ConsoleFormatter::PrintSpringInfo(CellStats cell)
{
  printf("%s\n", cell.Springs().Broken() ? "Springs: broken" : "Springs: not broken");
}

void ConsoleFormatter::PrintMtInfo(CellStats cell)
{
  if (cell.MTsPerPole() == 0)
    printf("MTs: none\n");
  else
  {
    size_t MTs = cell.MTsPerPole() * 2;
    printf("MTs: polymerizing - %1.1lf(%d%%), depolymerizing - %1.1lf(%d%%), bound - %1.1lf(%d%%)\n",
         cell.MTs().Polymerizing(), (int)(100.0 * cell.MTs().Polymerizing() / MTs),
         cell.MTs().DePolymerizing(), (int)(100.0 * cell.MTs().DePolymerizing() / MTs),
         cell.MTs().Bound(), (int)(100.0 * cell.MTs().Bound() / MTs));
  }
}

void ConsoleFormatter::PrintChromosomeInfo(CellStats cell)
{
  if (cell.ChromosomePairs() == 0)
    printf("Chromosomes: none\n");
  else
  {
    size_t maxBoundMTsPerChr = cell.Chromosomes().MaxBoundMTsPerChr();
    size_t minBoundMTsPerChr = cell.Chromosomes().MinBoundMTsPerChr();
    double totalBoundMTs = cell.MTs().Bound();

    if (maxBoundMTsPerChr == minBoundMTsPerChr)
      printf("Chromosomes: %d bound MTs per chromosome\n", (int)minBoundMTsPerChr);
    else
      printf("Chromosomes: %d-%d bound MTs per chromosome (average value - %4.2lf)\n",
             (int)minBoundMTsPerChr,
             (int)maxBoundMTsPerChr,
             totalBoundMTs / (cell.ChromosomePairs() * 2));
  }
}

ConsoleFormatter::ConsoleFormatter(double printDelay)
  : _delay(printDelay)
{
  _prevRealTime = -1.0;
}

void ConsoleFormatter::PrintError(IErrorFormatter::ErrorType type, const char *errDesc)
{
  const char *errType = NULL;
  const char *recommendedAction = NULL;
  switch (type)
  {
    case IErrorFormatter::WrongParams:
      errType = "simulator was launched with wrong parameters";
      recommendedAction = "Try to launch program with \"-h\" option for help\n";
      break;

    case IErrorFormatter::CorruptedFiles:
      errType = "some files was corrupted or has wrong format";
      recommendedAction = "Try to fix them manually or use \"--fix\" option";
      break;

    case IErrorFormatter::RuntimeError:
      errType = "internal problem was detected";
      recommendedAction = "Try to restart program using different configuration and/or options";
      break;

    case IErrorFormatter::FailedToRepair:
      errType = "failed to repair file";
      recommendedAction = "You can start new simulation with the same configuration and construct a new cell file";
      break;

    default:
      errType = "unknown problem was detected";
      recommendedAction = "Write about it to developers";
      break;
  }

  fprintf(stderr, "\n");
  if (errDesc != NULL)
    fprintf(stderr, "Following error has occured: %s (%s).\n", errType, errDesc);
  else
    fprintf(stderr, "Following error has occured: %s.\n", errType);
  fprintf(stderr, "\n");
  fprintf(stderr, "%s.\n", recommendedAction);
  fflush(stderr);
}

void ConsoleFormatter::PrintRepairStarted(const char *filename)
{
  printf("\n");
  printf("Repairing file \"%s\", it can take some time ...\n", filename);
  fflush(stdout);
}

void ConsoleFormatter::PrintRepairCompleted(size_t layers, double lastTime)
{
  printf("Completed! Recovered %d time layers and %.2lf seconds of simulation\n", (int)layers, lastTime);
  fflush(stdout);
}

void ConsoleFormatter::PrintOnStart(const std::vector<Cell *> &cells)
{
  printf("Simulation started\n");
  printf("Cell radius: %e meters\n", GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::R_Cell, true));
  printf("MTs: %ld per each pole\n", GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::N_MT_Total));
  printf("Chromosomes: %ld pairs\n", GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::N_Cr_Total));
  printf("\n");
  fflush(stdout);
}

void ConsoleFormatter::PrintIterationInfo(ICellStatsProvider *stats, double curSimTime, double totalSimTime)
{
  //Initializing.
  if (_predictor.get() == NULL)
    _predictor.reset(new TimePredictor(curSimTime, totalSimTime));

  //Checking required delay.
  double curRealTime = _predictor->ElapsedTime();
  if (_prevRealTime > 0.0 && curRealTime < _prevRealTime + _delay)
    return;

  //Formatting information.
  auto cell = CellStats::Aggregate(stats->Stats());
  PrintTimeInfo(curSimTime, curRealTime, totalSimTime);
  PrintSpringInfo(cell);
  PrintMtInfo(cell);
  PrintChromosomeInfo(cell);
  printf("\n");

  //Updating times.
  _prevRealTime = curRealTime;
  fflush(stdout);
}

void ConsoleFormatter::PrintOnFinish(const std::vector<Cell *> &cells)
{
  printf("Simulation finished!\n");
  std::vector<CellStats> stats;
  for (size_t i = 0; i < cells.size(); i++)
    stats.push_back(CellStats::Create(cells[i]));
  
  CellStats astats = CellStats::Aggregate(stats);
  PrintSpringInfo(astats);
  PrintMtInfo(astats);
  PrintChromosomeInfo(astats);
  printf("\n");
  fflush(stdout);
}

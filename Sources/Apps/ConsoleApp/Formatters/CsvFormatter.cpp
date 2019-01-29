
#include "CsvFormatter.h"

#include "Mitosis.Core/All.h"
#include "TimePredictor.h"
#include "Mitosis.Solvers/CellStats.h"

//--------------------
//--- CsvFormatter ---
//--------------------

void CsvFormatter::PrintSystemInfo(const HardwareInfo &info,
                   const Version &programVersion,
                   int fileFormatVersion,
                   bool cudaSupport)
{
  printf("preset,name,cores,frequency\n");
  
  auto cpu = info.CPU();
  printf("%s,%s,%d,%0.1lf\n", "cpu", cpu.Name().c_str(), (int)cpu.Cores(), cpu.FrequencyMHz());
  
  auto gpus = info.GPUs();
  for (size_t i = 0; i < gpus.size(); i++)
  {
    printf("%s:%d,%s,%d,%0.1lf\n", "cuda", (int)i, gpus[i].Name().c_str(), (int)gpus[i].Cores(), gpus[i].FrequencyMHz());
  }
  fflush(stdout);
}

void CsvFormatter::PrintError(IErrorFormatter::ErrorType type, const char *errDesc)
{
  fprintf(stderr, "Error #%d: %s", (int)type, errDesc);
  fflush(stderr);
}

void CsvFormatter::PrintOnStart(const std::vector<Cell *> &cells)
{
  printf("Current model time,Total model time,Current real time,Total real time,");
  printf("Springs broken,Bound MTs,Free MTs,Polymerizing MTs,Depolymerizing MTs,");
  printf("Min bound MTs per chromosome,Max bound MTs per chromosome\n");
  fflush(stdout);
}

void CsvFormatter::PrintIterationInfo(ICellStatsProvider *stats, double curSimTime, double totalSimTime)
{
  if (_predictor.get() == NULL)
    _predictor.reset(new TimePredictor(0.0, totalSimTime));

  double curRealTime = _predictor->ElapsedTime();
  if (_prevTime + _delay <= curRealTime)
  {
    _prevTime = curRealTime;
    auto astats = CellStats::Aggregate(stats->Stats());
    printf("%le,%le,%le,", curSimTime, totalSimTime, _predictor->ElapsedTime());
    double predictedTotalTime = -1.0;
    if (_predictor->CanPredictTotalTime(curSimTime, predictedTotalTime))
      printf("%le,", predictedTotalTime);
    else
      printf("N/A,");
    printf("%s,%le,%le,%le,%le,", astats.Springs().Broken() ? "true" : "false",
                    astats.MTs().Bound(), astats.MTs().Free(),
                    astats.MTs().Polymerizing(), astats.MTs().DePolymerizing());
    printf("%d,%d\n", (int)astats.Chromosomes().MinBoundMTsPerChr(), (int)astats.Chromosomes().MaxBoundMTsPerChr());
    fflush(stdout);
  }
}

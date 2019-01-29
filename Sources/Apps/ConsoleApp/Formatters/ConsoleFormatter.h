
#include "IErrorFormatter.h"
#include "IOutputFormatter.h"
#include "TimePredictor.h"

class ConsoleFormatter : public IErrorFormatter, public IOutputFormatter
{
  private:
    std::unique_ptr<TimePredictor> _predictor;
    double _prevRealTime;
    double _delay;

  private:
    void PrintTimeInfo(double curSimTime, double curRealTime, double totalSimTime);

    void PrintSpringInfo(CellStats cell);

    void PrintMtInfo(CellStats cell);

    void PrintChromosomeInfo(CellStats cell);

  public:
    ConsoleFormatter(double printDelay);

    virtual void PrintSystemInfo(const HardwareInfo &info,
                                 const Version &programVersion,
                                 int fileFormatVersion,
                                 bool cudaSupport);

    virtual void PrintError(IErrorFormatter::ErrorType type, const char *errDesc);

    virtual void PrintRepairStarted(const char *filename);

    virtual void PrintRepairCompleted(size_t layers, double lastTime);

    virtual void PrintOnStart(const std::vector<Cell *> &cells);

    virtual void PrintIterationInfo(ICellStatsProvider *stats, double curSimTime, double totalSimTime);

    virtual void PrintOnFinish(const std::vector<Cell *> &cells);
};

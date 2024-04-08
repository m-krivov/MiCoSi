#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Core/Random.h"
#include "CellStats.h"
#include "CellWithRng.h"

// Definition of the used class
class SimulatorFactory;

// Base class for all simulators that provide different implementations of the same algorithm
class Simulator : public ICellStatsProvider
{
  public:
    typedef std::vector<std::unique_ptr<CellWithRng> > CellEnsemble;

    // Packs all data into cells and returns their objects paired with RNG
    // This operation may be slow, so don't call it without need
    // The returned instances may be updated (or be not) by 'DoIteration()'
    const CellEnsemble &Cells();

    // Returns count of the concurrent cells
    inline size_t CellCount()
    { return _cellCount; }

    // Formats and returns statistics, an averaged information about the simulation process
    // This operation is assumed to be fast
    virtual const std::vector<CellStats> &Stats();

    // Returns the current simulation time
    inline double Time() const
    { return _time; }

    // True, if simlation is finished
    bool IsFinished();

    // Does one iteration with predefined time step (according to GlobalSimParams)
    void DoIteration();

    virtual ~Simulator() = default;

  protected:
    Simulator() : _cellCount(0), _statsAreValid(false), _time(0.0)
    { /*nothing*/ }

    virtual void Import(CellEnsemble &cells) = 0;

    virtual void IterationStarted() { }

    virtual void DoMacroStep(double time) = 0;

    virtual void DoMicroStep(double time) = 0;

    virtual void DoPoleUpdatingStep(double time) = 0;

    virtual void DoSpringBreakingStep(double time) = 0;

    virtual void IterationFinished() { }

    virtual const CellEnsemble &SynchronizeCells() = 0;

    virtual void FormatStats(std::vector<CellStats> &stats) = 0;

  private:
    Simulator(const Simulator &) = delete;
    Simulator &operator =(const Simulator &) = delete;

    // To be used by SimulatorFactory
    void Init(CellEnsemble &cells, double startTime);

    size_t _cellCount;
    std::vector<CellStats> _stats;
    bool _statsAreValid;

    double _time;

  friend class SimulatorFactory;
};

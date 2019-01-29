
#pragma once

#include "Mitosis.Core/Defs.h"

#include "CellStats.h"

// Definition of the used class.
class SimulatorFactory;

// Base class for all simulators - different implementations of the same algorithm.
class Simulator : public ICellStatsProvider
{
  protected:
    virtual void Bind(const std::vector<std::pair<Cell *, uint32_t *> > &cells) = 0;

    virtual void IterationStarted() { }

    virtual void DoMacroStep(double time) = 0;

    virtual void DoMicroStep(double time) = 0;

    virtual void DoPoleUpdatingStep(double time) = 0;

    virtual void DoSpringBreakingStep(double time) = 0;

    virtual void IterationFinished() { }

    virtual void SynchronizeCells() = 0;

    virtual void FormatStats(std::vector<CellStats> &stats) = 0;

    inline Simulator()
      : _cellsAreValid(false), _statsAreValid(false), _time(0.0)
    { /*nothing*/ }

  private:
    inline Simulator(const Simulator &) = delete;
    inline void operator =(const Simulator &) = delete;

    void Init(std::vector<std::pair<Cell *, uint32_t *> > &cells, double startTime);

  private:
    std::vector<std::pair<Cell *, uint32_t *> > _cells;
    bool _cellsAreValid;

    std::vector<CellStats> _stats;
    bool _statsAreValid;

    double _time;

  public:
    // Packs all data into cells and returns their objects. Paired with RNG seeds.
    // This operation can be slow, so don't call it without need.
    const std::vector<std::pair<Cell *, uint32_t *> > &Cells();

    // Returns count of the concurrent cells.
    inline size_t CellCount()
    { return _cells.size(); }

    // Formats and returns statistics - averaged information about the simulation process.
    // This operation is fast.
    virtual const std::vector<CellStats> &Stats();

    // Returns current simulation time.
    inline double Time() const
    { return _time; }

    // True, if simlation is finished.
    bool IsFinished();

    // Does one iteration with predefined time step (according to GlobalSimParams).
    void DoIteration();

    virtual ~Simulator();

  friend class SimulatorFactory;
};

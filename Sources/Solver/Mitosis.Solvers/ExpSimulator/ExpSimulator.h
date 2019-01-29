
#pragma once

#include "../Simulator.h"

#include "Mitosis.Objects/Interfaces.h"

// Experimetal version of the algorithm that may contain some new features.
class ExpSimulator : public Simulator
{
  private:
    std::vector<std::pair<Cell *, uint32_t *> > _cells;
    std::unique_ptr<IPoleUpdater> _updater;

  private:
    virtual void Bind(const std::vector<std::pair<Cell *, uint32_t *> > &cells);

    virtual void DoMacroStep(double time);

    virtual void DoMicroStep(double time);

    virtual void DoPoleUpdatingStep(double time);

    virtual void DoSpringBreakingStep(double time);

    virtual void SynchronizeCells();

    virtual void FormatStats(std::vector<CellStats> &stats);

  public:
    inline ExpSimulator(IPoleUpdater &updater)
      : _updater(updater.CloneTemplated<IPoleUpdater>())
    { /*nothing*/ }

    // Versions for debugging - can be called from other simulators.

    static void DoMacroStep(Cell *cell, uint32_t &seed);

    static void DoMicroStep(Cell *cell, uint32_t &seed);

    static void DoPoleUpdatingStep(Cell *cell, uint32_t &seed, IPoleUpdater *updater, double time);

    static void DoSpringBreakingStep(Cell *cell, uint32_t &seed);
};

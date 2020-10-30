
#pragma once

#include "../Simulator.h"

#include "Mitosis.Objects/Interfaces.h"

// Parallel gold version of the algorithm that supports all available features
class CpuSimulator : public Simulator
{
  public:
    CpuSimulator() = delete;
    CpuSimulator(IPoleUpdater &updater, size_t num_threads);
    CpuSimulator &operator =(const CpuSimulator &) = delete;

    // Versions for debugging - can be called from other simulators

    static void DoMacroStep(Cell *cell, uint32_t &seed);

    static void DoMicroStep(Cell *cell, uint32_t &seed);

    static void DoPoleUpdatingStep(Cell *cell, uint32_t &seed, IPoleUpdater *updater, double time);

    static void DoSpringBreakingStep(Cell *cell, uint32_t &seed);

  private:
    virtual void Bind(const std::vector<std::pair<Cell *, uint32_t *> > &cells);

    virtual void DoMacroStep(double time);

    virtual void DoMicroStep(double time);

    virtual void DoPoleUpdatingStep(double time);

    virtual void DoSpringBreakingStep(double time);

    virtual void SynchronizeCells();

    virtual void FormatStats(std::vector<CellStats> &stats);

    int NumThreads() const { return std::max(1, std::min((int)_cells.size(), _omp_num_threads)); }

    int _omp_num_threads;
    std::vector<std::pair<Cell *, uint32_t *> > _cells;
    std::unique_ptr<IPoleUpdater> _updater;
};

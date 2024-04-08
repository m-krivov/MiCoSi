#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Objects/Interfaces.h"
#include "../Simulator.h"

// Parallel gold version of the algorithm that supports all available features
class CpuSimulator : public Simulator
{
  public:
    CpuSimulator() = delete;
    CpuSimulator(IPoleUpdater &updater, size_t num_threads);
    CpuSimulator &operator =(const CpuSimulator &) = delete;

    // Versions for debugging - can be called from other simulators

    static void DoMacroStep(Cell &cell, Random::State &state);

    static void DoMicroStep(Cell &cell, Random::State &state);

    static void DoPoleUpdatingStep(Cell &cell, Random::State &state, IPoleUpdater *updater, double time);

    static void DoSpringBreakingStep(Cell &cell, Random::State &state);

  private:
    virtual void Import(CellEnsemble &cells) override;

    virtual void DoMacroStep(double time) override;

    virtual void DoMicroStep(double time) override;

    virtual void DoPoleUpdatingStep(double time) override;

    virtual void DoSpringBreakingStep(double time) override;

    virtual const CellEnsemble &SynchronizeCells() override;

    virtual void FormatStats(std::vector<CellStats> &stats) override;

    int NumThreads() const { return std::max(1, std::min((int)_cells.size(), _omp_num_threads)); }

    int _omp_num_threads;
    CellEnsemble _cells;
    std::unique_ptr<IPoleUpdater> _updater;
};

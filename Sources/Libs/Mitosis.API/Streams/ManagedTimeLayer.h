
#pragma once

#include "../Objects/ManagedCell.h"
#include "../Core/ManagedSimParams.h"

namespace Mitosis
{
  public ref class TimeLayer
  {
    private:
      Cell ^_cell;
      SimParams ^_simParams;
      double _time;

    internal:
      TimeLayer(Cell ^cell,
            SimParams ^simParams,
            double time)
      {
        _cell = cell;
        _simParams = simParams;
        _time = time;
      }

    public:
      property Cell ^Cell
      {
        Mitosis::Cell ^get()
        { return _cell; }
      }

      property SimParams ^SimParams
      {
        Mitosis::SimParams ^get()
        { return _simParams; }
      }

      property double Time
      {
        double get()
        { return _time; }
      }
  };
}

#pragma once

#include "../Objects/ManagedCell.h"
#include "../Core/ManagedSimParams.h"

namespace MiCoSi
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
        MiCoSi::Cell ^get()
        { return _cell; }
      }

      property SimParams ^SimParams
      {
        MiCoSi::SimParams ^get()
        { return _simParams; }
      }

      property double Time
      {
        double get()
        { return _time; }
      }
  };
}

#pragma once

#include "MiCoSi.Objects/CellData.h"

namespace MiCoSi
{
  public enum class CellArray
  {
    POLE_POSITION              = ::CellArray::POLE_POSITION,
    POLE_TYPE                  = ::CellArray::POLE_TYPE,
    MT_POLE                    = ::CellArray::MT_POLE,
    MT_DIRECTION_X             = ::CellArray::MT_DIRECTION_X,
    MT_DIRECTION_Y             = ::CellArray::MT_DIRECTION_Y,
    MT_DIRECTION_Z             = ::CellArray::MT_DIRECTION_Z,
    MT_LENGTH                  = ::CellArray::MT_LENGTH,
    MT_STATE                   = ::CellArray::MT_STATE,
    MT_BOUND_CHROMOSOME        = ::CellArray::MT_BOUND_CHROMOSOME,
    CHR_POSITION               = ::CellArray::CHR_POSITION,
    CHR_ORIENTATION            = ::CellArray::CHR_ORIENTATION,
    CHR_PAIR_LEFF_CHROMOSOME   = ::CellArray::CHR_PAIR_LEFF_CHROMOSOME,
    CHR_PAIR_RIGHT_CHROMOSOME  = ::CellArray::CHR_PAIR_RIGHT_CHROMOSOME,
    SPRINGS_BROKEN             = ::CellArray::SPRINGS_BROKEN
  };

  public ref class CellData
  {
    private:
      const ::CellData *_obj;

    internal:
      CellData(const ::CellData *obj)
      { _obj = obj; }

      const ::CellData *GetObject()
      { return _obj; }

    public:
      property int ChromosomePairs
      { int get() { return (int)_obj->ChromosomePairs(); } }

      property int MTsPerPole
      { int get() { return (int)_obj->MTsPerPole(); } }

      property System::IntPtr DataPointer
      { System::IntPtr get() { return (System::IntPtr)(void *)_obj->DataPointer(); } }

      property int DataSize
      { int get() { return (int)_obj->DataSize(); } }

      property System::IntPtr OffsetPointer
      { System::IntPtr get() { return (System::IntPtr)(void *)_obj->OffsetPointer(); } }

      property int OffsetSize
      { int get() { return (int)_obj->OffsetSize(); } }
  };
}


#pragma once

#include "TimeLayer.h"
#include "FileExplorer.h"

// Stream that retrieves time layers from files.
class TimeStream
{
  private:
    std::string _file;
    uint32_t _initialRandSeed;
    FileExplorer* _fe;
    MemoryStream* _stream;
    SimParams *_params;
    Cell *_cell;
    int _curLayerIndex;
    TimeLayer _timeLayer;
    bool _wasChanged;

  private:
    static bool TimeLayerExtractor(void* metaData, size_t metaDataSize, void* binData, size_t binDataSize,
                                   Cell* cell, double &time, size_t &layerCount);
    static bool SimParamsExtractor(void* metaData, size_t metaDataSize, void* binData, size_t binDataSize);
    static bool CellConfigurationExtractor(void* metaData, size_t metaDataSize, void* binData, size_t binDataSize, Cell* &cell);

  private:
    static void LockFile(const char *file);

    static bool IsFileLocked(const char *file);

    static void UnLockFile(const char *file);

    // Creates TimeStream.
    TimeStream(const std::string &file, FileExplorer *fe, MemoryStream* stream, uint32_t initialRandSeed);

  public:
    // Returns initial randsom seed.
    inline uint32_t InitialRandSeed()
    { return _initialRandSeed; }

    // Moves to next time layer. If such exists returns "true" and "false" otherwise.
    bool MoveNext();

    // Moves to required layer.
    void MoveTo(size_t layerIndex);

    // Returns total count of time layers.
    size_t LayerCount()
    { return _fe->TimeLayerCount(); }

    // Returns current time layer.
    // Provided object can be invalid after calling "MoveNext()" method.
    inline TimeLayer &Current()
    { return _timeLayer; }

    // Resets built-in iterator.
    void Reset();

    // Returns time for required layer.
    double GetLayerTime(size_t layerIndex);

    // Writes simulation parameters into stream.
    // All following layers will use these parameters.
    // Resets built-in iterator.
    void Append(SimParams *params);

    // Writes provided time layer into end of the stream.
    // Resets built-in iterator.
    void Append(Cell *cell, double time, uint32_t randSeed);

    // Stores all changes into files.
    void Flush();

    ~TimeStream();

  private:
    static TimeStream *_UniOpen(const char *file, bool repair);

  public:

    // Creates new files for time stream. If previous files exist then rewrites them.
    // In case of error throws std::exception.
    static TimeStream *Create(const char *file, Cell *cell, uint32_t initialRandSeed);

    //Tries to open stored simulation results.
    //In case of error throws std::exception.
    static inline TimeStream *Open(const char *file)
    { return _UniOpen(file, false); }

    // Tries to open stored simulation results and validate all records.
    // In case of errors restores as much as possible. Or throws std::exception.
    static TimeStream *Repair(const char *file)
    { return _UniOpen(file, true); }
};

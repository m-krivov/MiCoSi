#pragma once
#include "MiCoSi.Core/Defs.h"

#include "FileExplorer.h"

// Stream that retrieves time layers from a file
class TimeStream
{
  public:
    // Light-weight wrapper that contains information about the current iteration
    class TimeLayer
    {
      public:
        TimeLayer(const TimeLayer &) = default;
        TimeLayer &operator =(const TimeLayer &) = default;

        const SimParams &GetSimParams() const
        { return *_simParams; }

        const Cell &GetCell() const
        { return *_cell; }

        double GetTime() const
        { return _time; }

        const Random::State &GetRng() const
        { return *_rng; }

      private:
        TimeLayer() : _cell(nullptr), _simParams(nullptr), _time(0.0), _rng(nullptr) { }
        TimeLayer(const Cell *cell, const SimParams *simParams, double time, const Random::State *rng)
          : _cell(cell), _simParams(simParams), _time(time), _rng(rng)
        { /*nothing*/ }

        const Cell *_cell;
        const SimParams *_simParams;
        double _time;
        const Random::State *_rng;

      friend class TimeStream;
    };

    TimeStream() = delete;
    TimeStream(const TimeStream &) = delete;
    TimeStream &operator =(const TimeStream &) = delete;
    ~TimeStream();

    // Returns the initial seed of RNG that may be used to reproduce the results
    // Negative values mean that RNG has no seed (e.g. time-based initialization)
    int64_t UserSeed() const
    { return _userSeed; }

    // Like 'UserSeed()', but returns the initial RNG state that is always valid
    const Random::State &InitialRNG() const
    { return _initialRng; }

    // Returns the total count of time layers
    size_t LayerCount() const
    { return _fe->TimeLayerCount(); }

    // Returns the current time layer
    // Beware: the provided object may become invalid after calling 'MoveNext()' method
    const TimeLayer Current() const;

    // Moves to next time layer
    // If such layer exists, returns 'true'
    bool MoveNext();

    // Moves to the required layer
    void MoveTo(size_t layerIndex);

    // Resets built-in iterator
    void Reset();

    // Returns time for the required layer
    double GetLayerTime(size_t layerIndex);

    // Writes simulation parameters to the stream
    // All following layers will use these parameters
    // Resets the built-in iterator
    void Append(const SimParams &params);

    // Writes the given time layer to the end of the stream
    // Resets built-in iterator
    void Append(const Cell &cell, double time, const Random::State &rng);

    // Stores all changes
    void Flush();

    // Creates new file for time stream
    // If the previous file exist, rewrites it
    static std::unique_ptr<TimeStream> Create(const std::string &file,
                                              const Cell &cell,
                                              const Random::State &rng,
                                              int64_t userSeed = -1);

    //Tries to open some stored simulation results
    static std::unique_ptr<TimeStream> Open(const std::string &file);

    // Tries to open stored simulation results and validate all records
    static std::unique_ptr<TimeStream> Repair(const std::string &file);

  private:
    TimeStream(const std::string &file,
               std::unique_ptr<FileExplorer> &fe,
               const Random::State &initialRng,
               int64_t userSeed);

    static std::unique_ptr<TimeStream> OpenFile(const std::string &file, bool repair);

    std::string _file;
    int64_t _userSeed;
    Random::State _initialRng;
    int _curLayerIndex;
    bool _needToFlush;
    std::unique_ptr<FileExplorer> _fe;

    std::unique_ptr<SimParams> _params;
    std::unique_ptr<Cell> _cell;
    double _time;
    Random::State _rng;
};

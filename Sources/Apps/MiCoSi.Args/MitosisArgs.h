#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Core/Random.h"
#include "MiCoSi.Solvers/SimulatorConfig.h"
#include "UniArgs.h"

class LaunchMode
{
  public:
    enum Type
    {
      New        = 0,
      Restart    = 1,
      Continue   = 2,
      Fix        = 3,
      Info       = 4,
      Help       = 5
    };

    static const char *ToString(Type t);
    static bool TryParse(const std::string &str, Type &t);
    static Type Parse(const std::string &str);
};

class MitosisArgs : public UniArgs
{
  // Enumeration with options
  public:
    class Option
    {
      public:
        enum Type
        {
          Mode                   = 0,
          CellFile               = 1,
          ConfigFile             = 2,
          InitialConditionsFile  = 3,
          PoleCoordsFile         = 4,
          RngSeed                = 5,
          CellCount              = 6,
          Solver                 = 7,
          CsvOutput              = 8,
          PrintDelay             = 9
        };
      
        // Returns string-based name (like "--do_something").
        static const char *ToString(Type type);
    };

    MitosisArgs();
    MitosisArgs(const MitosisArgs &) = delete;
    MitosisArgs &operator =(const MitosisArgs &) = delete;
    MitosisArgs *Clone();

    virtual std::string HelpMessage();

    // Sets default value for the required option
    void Reset(Option::Type type)
    { UniArgs::Reset(Option::ToString(type)); }

    // The required mode
    LaunchMode::Type GetMode() const
    {
      if (_help) return LaunchMode::Help;
      else return LaunchMode::Parse(_mode);
    }
    
    void SetMode(LaunchMode::Type mode)
    {
      _help = mode == LaunchMode::Help;
      if (!_help) _mode = LaunchMode::ToString(mode);
    }

    // Path to file with results
    const std::string &GetCellFile() const { return _cell; }
    void SetCellFile(const std::string &value) { _cell = value; }

    // Path to file with config
    const std::string &GetConfigFile() const { return _config; }
    void SetConfigFile(const std::string &value) { _config = value; }

    // Path to XML-file with initial conditions. Can be equal to nullptr!
    const char *GetInitialConditionsFile() const { return _initialConditions.empty() ? nullptr : _initialConditions.c_str(); }
    void SetInitialConditionsFile(const char *value) { _initialConditions = value == nullptr ? "" : value; }

    // Path to XML-file with pole coordinates. Can be equal to nullptr!
    const char *GetPoleCoordsFile() const { return _poleCoords.empty() ? nullptr : _poleCoords.c_str(); }
    void SetPoleCoordsFile(const char *value) { _poleCoords = value == nullptr ? "" : value; }

    // Randomizer's seed
    int GetUserSeed() const { return _seed; }
    void SetUserSeed(int value) { _seed = value; }

    // Count of cells that must be processed simultaniously
    int GetCellCount() const { return _cellCount; }
    void SetCellCount(int value) { _cellCount = value; }

    // Type of solver that must be used for computations
    SimulatorConfig GetSolver() const { return SimulatorConfig::Parse(_solver.c_str()); }
    void SetSolver(SimulatorConfig solver) { _solver = SimulatorConfig::Serialize(solver); }

    // True if output should be formatted as csv-table
    bool GetCsvOutput() const { return _csvOutput; }
    void SetCsvOutput(bool value) { _csvOutput = value; }

    // Delay between printing of two sequential output records (in seconds)
    double GetPrintDelay() const { return _printDelay; }
    void SetPrintDelay(double value) { _printDelay = value; }

    static std::vector<std::string> MultiplyCells(const char *filenameTemplate, size_t cellCount);

  private:
    bool _help;           // separating "--help" from other modes in order to use SingleOption restriction
    std::string _mode;
    std::string _cell;
    std::string _config;
    std::string _initialConditions;
    std::string _poleCoords;
    int _seed;
    int _cellCount;
    std::string _solver;
    bool _csvOutput;
    double _printDelay;
};

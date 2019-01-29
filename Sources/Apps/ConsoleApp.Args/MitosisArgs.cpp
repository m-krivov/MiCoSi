
#include "MitosisArgs.h"

//------------------
//--- LaunchMode ---
//------------------

const char *LaunchMode::ToString(LaunchMode::Type t)
{
  switch (t)
  {
    case New: return "new";
    case Restart: return "restart";
    case Continue: return "continue";
    case Fix: return "fix";
    case Info: return "info";
    case Help: return "help";
    default: throw std::runtime_error("Unknown type for LaunchMode");
  }
}

bool LaunchMode::TryParse(const std::string &str, LaunchMode::Type &t)
{
  if (str == "new") { t = New; return true; }
  else if (str == "restart") { t = Restart; return true; }
  else if (str == "continue") { t = Continue; return true; }
  else if (str == "fix") { t = Fix; return true; }
  else if (str == "info") { t = Info; return true; }
  else if (str == "help") { t = Help; return true; }
  else return false;
}

LaunchMode::Type LaunchMode::Parse(const std::string &str)
{
  Type res;
  if (!TryParse(str, res))
  {
    std::stringstream ss;
    ss << "Failed to parse launch mode \"" << str << "\"";
    throw std::runtime_error(ss.str());
  }

  return res;
}

//--------------
//--- Helper ---
//--------------

class MitosisArgsHelper
{
  public:
    static bool IsPositive(int n) { return n > 0; }
    static bool IsPositive(double n) { return n > 0.0; }
    static bool IsNonNegative(double n) { return n >= 0.0; }

    static bool IsSeed(int n) { return n >= -1; }

    static bool IsSolverString(std::string str)
    {
      try { SimulatorConfig::Parse(str.c_str()); return true; }
      catch (std::exception &) { return false; }
    }

    static bool IsLaunchModeButNotHelpString(std::string str)
    {
      LaunchMode::Type t;
      return LaunchMode::TryParse(str, t) && t != LaunchMode::Help;
    }

    static bool IsFileExist(std::string str)
    {
      FILE *f = fopen(str.c_str(), "r");
      if (f != NULL) fclose(f);
      return f != NULL;
    }

    static bool IsFileExistOrNotSet(std::string str)
    { return str.empty() ? true : IsFileExist(str); }
};

//---------------------------
//--- MitosisArgs::Option ---
//---------------------------

const char *MitosisArgs::Option::ToString(MitosisArgs::Option::Type type)
{
  switch (type)
  {
    case Mode:          return "--mode";
    case CellFile:        return "--cell";
    case ConfigFile:      return "--config";
    case InitialConditionsFile: return "--initial";
    case PoleCoordsFile:    return "--poles";
    case RngSeed:        return "--seed";
    case CellCount:        return "--series";
    case Solver:        return "--solver";
    case CsvOutput:        return "--csv";
    case PrintDelay:      return "--print_delay";
    default: throw std::runtime_error("Internal error - wrong value for MitosisArgs::Option");
  }
}

//-------------------
//--- MitosisArgs ---
//-------------------

MitosisArgs::MitosisArgs()
{
  //Modes.
  {
    _help = false;
    Register("--help", _help);
    SingleOption("--help");

    _mode = LaunchMode::ToString(LaunchMode::New);
    Register(Option::ToString(Option::Mode), _mode, MitosisArgsHelper::IsLaunchModeButNotHelpString);
  }

  //Files.
  {
    _cell = "results.cell";
    _config = "mitosis.conf";
    _initialConditions = "";
    _poleCoords = "";

    Register(Option::ToString(Option::CellFile), _cell);
    Register(Option::ToString(Option::ConfigFile), _config, MitosisArgsHelper::IsFileExist);
    Register(Option::ToString(Option::InitialConditionsFile), _initialConditions, MitosisArgsHelper::IsFileExistOrNotSet);
    Register(Option::ToString(Option::PoleCoordsFile), _poleCoords, MitosisArgsHelper::IsFileExistOrNotSet);

    IncompatibleWith(Option::ToString(Option::ConfigFile),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Fix));
    IncompatibleWith(Option::ToString(Option::InitialConditionsFile),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Fix));
    IncompatibleWith(Option::ToString(Option::PoleCoordsFile),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Fix));

    IncompatibleWith(Option::ToString(Option::CellFile),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Info));
    IncompatibleWith(Option::ToString(Option::ConfigFile),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Info));
    IncompatibleWith(Option::ToString(Option::InitialConditionsFile),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Info));
    IncompatibleWith(Option::ToString(Option::PoleCoordsFile),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Info));
  }

  //Parameters.
  {
    _seed = -1;
    _solver = SimulatorConfig::Serialize(SimulatorConfig::Default());
    _cellCount = 1;
    _csvOutput = false;
    _printDelay = 1.0;

    Register(Option::ToString(Option::RngSeed), _seed, MitosisArgsHelper::IsSeed);
    Register(Option::ToString(Option::Solver), _solver, MitosisArgsHelper::IsSolverString);
    Register(Option::ToString(Option::CellCount), _cellCount, MitosisArgsHelper::IsPositive);
    Register(Option::ToString(Option::CsvOutput), _csvOutput);
    Register(Option::ToString(Option::PrintDelay), _printDelay, MitosisArgsHelper::IsNonNegative);

    IncompatibleWith(Option::ToString(Option::RngSeed),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Fix));
    IncompatibleWith(Option::ToString(Option::RngSeed),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Continue));
    IncompatibleWith(Option::ToString(Option::RngSeed),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Restart));
    IncompatibleWith(Option::ToString(Option::RngSeed),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Info));
    IncompatibleWith(Option::ToString(Option::Solver),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Fix));
    IncompatibleWith(Option::ToString(Option::Solver),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Info));
    IncompatibleWith(Option::ToString(Option::CsvOutput),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Fix));
    IncompatibleWith(Option::ToString(Option::PrintDelay),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Fix));
    IncompatibleWith(Option::ToString(Option::PrintDelay),
             Option::ToString(Option::Mode), LaunchMode::ToString(LaunchMode::Info));
  }
}

MitosisArgs *MitosisArgs::Clone()
{
  MitosisArgs *res = new MitosisArgs();

  res->_help = _help;
  res->_mode = _mode;
  res->_cell = _cell;
  res->_config = _config;
  res->_initialConditions = _initialConditions;
  res->_poleCoords = _poleCoords;
  res->_seed = _seed;
  res->_cellCount = _cellCount;
  res->_solver = _solver;
  res->_csvOutput = _csvOutput;
  res->_printDelay = _printDelay;

  return res;
}

std::string MitosisArgs::HelpMessage()
{
    std::stringstream ss;

  ss << std::endl;
  ss << "The following arguments are supported:" << std::endl;
  ss << std::endl;
  ss << "     Mitosis --help" << std::endl;
  ss << "     Mitosis [--mode <MODE>] [--cell <RESULTS>.cell]" << std::endl;
  ss << "             [--config <FILE>.conf] [--initial <FILE>.xml] [--poles <FILE>.xml]" << std::endl;
  ss << "             [--seed <SEED_VALUE>] [--series <SERIES>] [--solver <SOLVER>]" << std::endl;
  ss << "             [--csv] [--print_delay <DELAY>]" << std::endl;
  ss << "     Mitosis --mode fix [--cell <RESULTS>.cell]" << std::endl;
  ss << "     Mitosis --mode info [--csv]" << std::endl;
  ss << std::endl;
  ss << "Where:" << std::endl;
  ss << std::endl;
  ss << "     --help           - Prints current help message and exits" << std::endl;
  ss << "     " << Option::ToString(Option::Mode) << " <MODE>" << std::endl;
  ss << "                        " << LaunchMode::ToString(LaunchMode::New) <<
                   ": starts new simulation. If there are any results" << std::endl;
  ss << "                        from previous launches then they will be deleted." << std::endl;
  ss << "                        " << LaunchMode::ToString(LaunchMode::Continue) <<
                   ": tryies to continue simulation using" << std::endl;
  ss << "                        previously stored results. Some values from" << std::endl;
  ss << "                        \"Mitosis.conf\" file can differ from one used" << std::endl;
  ss << "                        in previous simulation." << std::endl;
  ss << "                        " << LaunchMode::ToString(LaunchMode::Restart) <<
                   ": deletes all previously stored results and" << std::endl;
  ss << "                        starts new simulation, but uses the same random" << std::endl;
  ss << "                        number generator." << std::endl;
  ss << "                        " << LaunchMode::ToString(LaunchMode::Fix) <<
                   ": tryies to repair broken files with results." << std::endl;
  ss << "                        Should be used after program crashes and termination" << std::endl;
  ss << "                        by user (e.g. Ctrl+C)." << std::endl;
  ss << "                        " << LaunchMode::ToString(LaunchMode::Info) <<
                   ": prints information about current build and found" << std::endl;
  ss << "                        computing units" << std::endl;
  ss << "     " << Option::ToString(Option::CellFile) << " <RESULTS>.cell" << std::endl;
  ss << "                      - File, which results will be stored to or taken from." << std::endl;
  ss << "                        If argument is not specified \"results.cell\" will be used" << std::endl;
  ss << "                        as default." << std::endl;
  ss << "     " << Option::ToString(Option::ConfigFile) << " <FILE>.conf" << std::endl;
  ss << "                      - File with the actual cell configuration. If argument" << std::endl;
  ss << "                        is not specified \"mitosis.conf\" will be used as default." << std::endl;
  ss << "     " << Option::ToString(Option::InitialConditionsFile) << " <FILE>.xml" << std::endl;
  ss << "                      - The XML-file with initial state description of the" << std::endl;
  ss << "                        cell. If not specified, cell will be initialized" << std::endl;
  ss << "                        randomly." << std::endl;
  ss << "     " << Option::ToString(Option::PoleCoordsFile) << " <FILE>.xml" << std::endl;
  ss << "                      - The XML-file with coordinates for poles. If not" << std::endl;
  ss << "                        specified, poles will be static." << std::endl;;
  ss << "     " << Option::ToString(Option::RngSeed) << " <SEED_VALUE>" << std::endl;
  ss << "                      - defines seed for Random Number Generator. SEED_VALUE" << std::endl;
  ss << "                        must be declared as a positive number. The default" << std::endl;
  ss << "                        seed's value is being created using current time." << std::endl;
  ss << "     " << Option::ToString(Option::CellCount) << " <SERIES>" << std::endl;
  ss << "                      - defines how much cells must be simulated" << std::endl;
  ss << "                        simultaniously. SERIES must be declared as a positive" << std::endl;
  ss << "                        number. The default value is one." << std::endl;
  ss << "     " << Option::ToString(Option::Solver) << " <SOLVER>" << std::endl;
  ss << "                      - defines implementation of the simulation algorithm." << std::endl;
  ss << "                        SOLVER can be set by \"gold\", \"cuda\" or \"experimental\"" << std::endl;
  ss << "                        values. The gold version is used by default." << std::endl;
  ss << "     " << Option::ToString(Option::CsvOutput) << std::endl;
  ss << "                      - forces to use csv table for output printing. Useful for" << std::endl;
  ss << "                        parsing automatization." << std::endl;
  ss << "     " << Option::ToString(Option::PrintDelay) << " <DELAY>" << std::endl;
  ss << "                      - sets delay between printing of two output records, in" << std::endl;
  ss << "                        seconds. Default value - 1.0." << std::endl;
  ss << std::endl;

  return ss.str();
}

std::vector<std::string> MitosisArgs::MultiplyCells(const char *filenameTemplate, size_t cellCount)
{
  std::string name(filenameTemplate);
  std::vector<std::string> res;

  if (cellCount <= 1)
  {
    res.push_back(filenameTemplate);
  }
  else
  {
    //Searching for extension.
    std::string ext;
    size_t dotPos = name.find_last_of('.');
    if (dotPos != std::string::npos)
    {
      ext = name.substr(dotPos, name.size() - dotPos);
      name = name.substr(0, dotPos);
    }

    //Generating names.
    for (size_t i = 0; i < cellCount; i++)
    {
      std::stringstream ss;
      ss << "_" << i;
      res.push_back(name + ss.str() + (ext.empty() ? "" : ext));
    }
  }

  return res;
}

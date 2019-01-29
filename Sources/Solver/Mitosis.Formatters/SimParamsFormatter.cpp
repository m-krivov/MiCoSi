
#include "SimParamsFormatter.h"
#include <sstream>

//--------------------------
//--- SimParamsFormatter ---
//--------------------------

std::string SimParamsFormatter::ExportAsProps(SimParams *params)
{
  std::stringstream ss;

  {
    const std::vector<SimParameter::Int::Type> &intParams = SimParameter::Int::All();
    for (size_t i = 0; i < intParams.size(); i++)
      ss << SimParameter::Int::Info(intParams[i]).Name() << "=" << params->GetParameter(intParams[i]) << std::endl;
  }

  {
    const std::vector<SimParameter::Double::Type> &doubleParams = SimParameter::Double::All();
    for (size_t i = 0; i < doubleParams.size(); i++)
      ss << SimParameter::Double::Info(doubleParams[i]).Name() << "=" << params->GetParameter(doubleParams[i]) << std::endl;
  }

  return ss.str();
}

void SimParamsFormatter::ImportAsProps(SimParams *params, const std::string &props)
{
  // Parsing our properties. Storing them as name+value pairs.
  std::vector<std::pair<std::string, std::string> > records;
  std::stringstream ss(props);
  if (ss.eof()) return;

  do
  {
    std::string line;
    std::getline(ss, line);
    size_t pos = 0;
    
    // Step 1. Making some clear up - removing multiple spaces, asm-styled comments and '\n' plus '\r' symbols.
    std::string str;
    str.reserve(line.size());
    bool afterSpace = true;
    for (size_t i = 0 ; i < line.size(); i++)
    {
      if (line[i] == '#' || line[i] == '\n' || line[i] == '\r') break;

      if (isspace(line[i]))
      {
        if (!afterSpace)
        { str.push_back(' '); afterSpace = true; }
      }
      else
      { str.push_back(line[i]); afterSpace = false; }
    }
    if (str.empty()) { continue; }

    // Step 2. String "str" contains some useful record, parsing it.
    if (str.length() < 3) throw std::runtime_error(std::string("Wrong config record \"") + line + "\"");
    size_t start = 0, end = 0;
    while (end < str.size() && str[end] != '=')
      end += 1;
    if (end == 0 || end >= str.size() - 1) throw std::runtime_error(std::string("Wrong config record \"") + line + "\"");
    std::string param = str.substr(start, end - start);
    std::string value = str.substr(end + 1, str.size() - (end + 1));

    // Step 3. Cutting optional spaces.
    std::string *ref[] = { &param, &value };
    for (size_t i = 0; i < 2; i++)
    {
      std::string &s = *ref[i];
      if (isspace(s[0]))
        s = s.substr(1, s.size() - 1);
      if (!s.empty() && isspace(s[s.size() - 1]))
        s = s.substr(0, s.size() - 1);
    }
    if (param.empty() || value.empty()) throw std::runtime_error(std::string("Wrong config record \"") + line + "\"");

    // Step 4. Adding records to vector for future import operation.
    records.push_back(std::make_pair(param, value));
  }
  while (!ss.eof());

  // Importing parameters. Correctness will be checked there.
  try
  { params->ImportValues(records); }
  catch (std::exception &ex)
  { throw std::runtime_error(std::string("Failed to import config. ") + ex.what()); }
}

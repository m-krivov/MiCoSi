
#include "UniArgs.h"

template <>
std::string UniArgs::TemplatedRecord<std::string>::FromString(const std::string &value)
{ return value; }

template <>
bool UniArgs::TemplatedRecord<bool>::FromString(const std::string &value)
{
  bool is_true = value == "true";
  if (!is_true && value != "false") throw std::runtime_error(std::string("Failed to parse value \"") + value + "\"");
  return is_true;
}

template <>
std::string UniArgs::TemplatedRecord<bool>::ToString(bool value)
{ return value ? "true" : "false"; }

//---------------
//--- UniArgs ---
//---------------

void UniArgs::Register(const std::string &id, bool &value)
{
  if (_records.find(id) != _records.end())
    throw std::runtime_error("Internal error - record with such name already registered");

  _records[id].reset(new TemplatedRecord<bool>(id, true, NULL, &value));
}

void UniArgs::Register(const std::string &id, int &value, bool (*validator)(int))
{
  if (_records.find(id) != _records.end())
    throw std::runtime_error("Internal error - record with such name already registered");

  _records[id].reset(new TemplatedRecord<int>(id, false, validator, &value));
}

void UniArgs::Register(const std::string &id, double &value, bool (*validator)(double))
{
  if (_records.find(id) != _records.end())
    throw std::runtime_error("Internal error - record with such name already registered");

  _records[id].reset(new TemplatedRecord<double>(id, false, validator, &value));
}

void UniArgs::Register(const std::string &id, std::string &value, bool (*validator)(std::string))
{
  if (_records.find(id) != _records.end())
    throw std::runtime_error("Internal error - record with such name already registered");

  _records[id].reset(new TemplatedRecord<std::string>(id, false, validator, &value));
}

void UniArgs::Reset(const std::string &id)
{
  auto iter = _records.find(id);
  if (iter == _records.end())
    throw std::runtime_error("Internal error - such ID was not registered");

  iter->second->SetValue(iter->second->DefaultValue());
}

void UniArgs::SingleOption(const std::string &id)
{
  if (_records.find(id) == _records.end())
    throw std::runtime_error(std::string("Internal error - option \"") + id + "\" is not registered yet");

  _singleOptions.push_back(id);
}
    
void UniArgs::IncompatibleWith(const std::string &id, const std::string &opt, const char *value)
{
  if (_records.find(id) == _records.end() ||
    _records.find(opt) == _records.end())
    throw std::runtime_error("Internal error - options that are being marked as incompatible not registered yet");

  std::string svalue = value == NULL ? std::string() : value;
  _incompatibleOptions.push_back(
    std::pair<std::string, std::pair<std::string, std::string> >(id, std::make_pair(opt, svalue)));
}

void UniArgs::Verify()
{
  for (auto it = _records.begin(); it != _records.end(); it++)
    it->second->Verify();
}

void UniArgs::Import(std::vector<std::string> &args)
{
  //Resetting the previous values.
  for (auto it = _records.begin(); it != _records.end(); it++)
  {
    UniRecord *rec = it->second.get();
    rec->SetValue(rec->DefaultValue());
  }

  //Settings new values.
  std::map<std::string, bool> definedOptions;
  for (size_t i = 0; i < args.size(); i++)
  {
    std::string option = args[i];

    auto it = _records.find(option);
    if (it == _records.end())
      throw std::runtime_error(std::string("Unknown option \"") + option + "\"");

    UniRecord *rec = it->second.get();
    if (!rec->IsFlag() && i == args.size() - 1)
      throw std::runtime_error("No value for \"" + option + "\" option");

    if (definedOptions[option] == true)
      throw std::runtime_error(std::string("The \"") + std::string(args[i]) + "\" option already defined");

    if (!rec->IsFlag())
    { rec->SetValue(args[i + 1]); i += 1; }
    else
    { rec->SetValue("true"); }

    rec->Verify();

    definedOptions[option] = true;
  }

  //Checking for restrictions.
  if (definedOptions.size() > 1)
  {
    for (auto it = definedOptions.begin(); it != definedOptions.end(); it++)
    {
      const std::string &id = it->first;
      for (size_t i = 0; i < _singleOptions.size(); i++)
        if (_singleOptions[i] == id)
        {
          std::stringstream ss;
          ss << "Option \"" + id << "\" cannot be defined with any other";
          throw std::runtime_error(ss.str());
        }
    }
  }

  for (size_t i = 0; i < _incompatibleOptions.size(); i++)
  {
    auto pair = _incompatibleOptions[i];
    if (definedOptions.find(pair.first) != definedOptions.end() &&
      definedOptions.find(pair.second.first) != definedOptions.end())
    {
      std::string &val = pair.second.second;
      if (val.empty() || val == _records[pair.second.first]->GetValue())
      {
        std::stringstream ss;
        ss <<"Option \"" << pair.first << "\" cannot be defined together with \"" << pair.second.first;
        if (!val.empty())
          ss << " " << val;
        ss << "\"";
        throw std::runtime_error(ss.str());
      }
    }
  }
}

void UniArgs::Import(std::string args)
{
  std::vector<std::string> params;

  //Using simple handmade parser. Not universal, but good for correct variants.
  bool hasQuotes = false;
  size_t start = 0;
  for (size_t i = 0; i < args.size(); i++)
  {
    if (args[i] == '"') hasQuotes = !hasQuotes;

    if (!hasQuotes && args[i] == ' ')
    {
      if (i != start)
      {
        int shift = args[start] == '"' ? 1 : 0;
        params.push_back(args.substr(start + shift, i - start - 2 * shift));
      }
      start = i + 1;
    }
  }
  
  if (hasQuotes)
    throw std::runtime_error("Wrong CLI string - closing quotes not found");
  if (start != args.size())
    params.push_back(args.substr(start, args.size() - start));

  Import(params);
}

void UniArgs::Import(int argc, char **argv)
{
  std::vector<std::string> params (argc - 1);
  for (int i = 1; i < argc; i++)
    params[i - 1] = std::string(argv[i]);
  Import(params);
}

void UniArgs::Export(std::vector<std::string> &args)
{
  for (auto it = _records.begin(); it != _records.end(); it++)
  {
    auto rec = it->second.get();
    if (rec->GetValue() != rec->DefaultValue())
    {
      args.push_back(rec->ID());
      if (!rec->IsFlag())
      {
        std::string val = rec->GetValue();
        if (val.find(' ') != std::string::npos)
          val = std::string("\"") + val + "\"";  //wrapping some filename
        args.push_back(val);
      }
    }
  }
}

void UniArgs::Export(std::string &args)
{
  std::vector<std::string> params;
  Export(params);

  args.resize(0);
  args.reserve(params.size() * 10);
  for (size_t i = 0; i < params.size(); i++)
  {
    if (i != 0) args += " ";
    args += params[i];
  }
}

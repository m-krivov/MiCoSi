#define _CRT_SECURE_NO_WARNINGS

#include "TimeStream.h"

#include "ttgUtils.h"
#include "Mitosis.Formatters/Serializer.h"
#include "Mitosis.Formatters/DeSerializer.h"

#ifdef WIN32
#include <Windows.h>
#endif

namespace
{

bool IsFileLocked(const std::string &file)
{
  bool locked = false;
  std::string lockfile = std::string(file) + ".lock";
  FILE *f = fopen(lockfile.c_str(), "r");
  if (f != NULL)
  {
#ifdef WIN32
    // Is the lock creator still alive?
    uint64_t pid = 0;
    bool hasReadPID = fscanf(f, "%llu", &pid) == 1;
    fclose(f);
    if (hasReadPID)
    {
      HANDLE handle = NULL;
      if ((handle = OpenProcess(PROCESS_QUERY_INFORMATION, false, (DWORD)pid)) != NULL)
      {
        CloseHandle(handle);
        locked = true;
      }
      else
      { locked = remove(lockfile.c_str()) != 0; }
    }
    else
    { locked = true; }
#else
    fclose(f);
    locked = true;
#endif
  }
  return locked;
}

void LockFile(const char *file)
{
  if (IsFileLocked(file))
  { throw std::runtime_error("cannot lock already locked file"); }

  FILE *f = fopen((std::string(file) + ".lock").c_str(), "w");
  if (f != NULL)
  {
#ifdef WIN32
    // Write PID of the current process - to allow other programs remove obsolete lock
    // If PID can be omitted, it's not a problem
    fprintf(f, "%llu", (uint64_t)GetCurrentProcessId());
#endif
    fclose(f);
  }
}

void UnLockFile(const char *file)
{
  if (!IsFileLocked(file))
  { throw std::runtime_error("cannot unlock already unlocked file"); }

  if (remove((std::string(file) + ".lock").c_str()) != 0)
  { throw std::runtime_error("cannot unlock file"); }
}

bool TimeLayerExtractor(const void *metaData, size_t metaDataSize,
                        const void *binData, size_t binDataSize,
                        Cell &cell, double &time, size_t &layerCount)
{
  try
  {
    std::unique_ptr<TiXmlDocument> doc(new TiXmlDocument());
    std::string metaString((char*)metaData);
    std::stringstream ss(metaString);
    ss >> *doc;

    int _layerCount = 0;
    double _time = 0.0;
    uint64_t maxElemPerChunk = ((uint64_t*)binData)[0];
    if (maxElemPerChunk <= 0)
    { return false; }
    uint64_t *offsets = (uint64_t *)((uint8_t *)binData + sizeof(uint64_t));
    uint64_t *sizes = (uint64_t *)((uint8_t *)binData + sizeof(uint64_t) + maxElemPerChunk * sizeof(uint64_t));
    uint64_t offset = sizeof(uint64_t) + maxElemPerChunk * (sizeof(uint64_t) + sizeof(uint64_t));
    for (TiXmlNode *n = doc->RootElement()->FirstChild();
         n != nullptr;
         n = n->NextSibling())
    {
      if (offset != offsets[_layerCount])
      { return false; }

      std::pair<double, Random::State> tl = DeSerializer::DeserializeTimeLayer(
          n->ToElement(),
          cell,
          (uint8_t*)binData + offsets[_layerCount],
          (size_t)sizes[_layerCount]
      );

      if (_layerCount == 0)
      { _time = tl.first; }
      offset += sizes[layerCount];
      _layerCount++;
    }
    layerCount = _layerCount;
    time = _time;

    return true;
  }
  catch (std::exception &)
  { return false; }
}

bool SimParamsExtractor(const void *metaData, size_t metaDataSize,
                        const void *binData, size_t binDataSize)
{
  try
  {
    std::unique_ptr<TiXmlDocument> doc(new TiXmlDocument());
    std::string metaString((char*)metaData);
    std::stringstream ss(metaString);
    ss >> *doc;

    std::unique_ptr<SimParams> simParams(
        DeSerializer::DeserializeSimParams(doc->RootElement()->FirstChild()->ToElement(),
                                           binData, binDataSize)
    );
    return true;
  }
  catch(std::exception &)
  {
    return false;
  }
}

bool CellConfigurationExtractor(const void *metaData, size_t metaDataSize,
                                const void *binData, size_t binDataSize,
                                std::unique_ptr<Cell> &cell)
{
  try
  {
    std::unique_ptr<TiXmlDocument> doc(new TiXmlDocument());
    std::string metaString((char*)metaData);
    std::stringstream ss(metaString);
    ss >> *doc;
    cell = DeSerializer::DeserializeCellConfiguration(
        doc->RootElement()->FirstChild()->ToElement(),
        binData, binDataSize
    );
    return true;
  }
  catch (std::exception &)
  {
    cell = nullptr;
    return false;
  }
}

} // unnamed namespace

//------------------
//--- TimeStream ---
//------------------

std::unique_ptr<TimeStream> TimeStream::OpenFile(const std::string &file, bool repair)
{
  // Check lock
  if (IsFileLocked(file))
  { throw std::runtime_error("file with results is locked"); }

  std::unique_ptr<FileExplorer> fe(
    repair
      ? FileExplorer::Repair(file, TimeLayerExtractor, SimParamsExtractor, CellConfigurationExtractor)
      : FileExplorer::Open(file)
  );

  // Get the initial random seed and check version
  std::pair<Version, int> ver = DeSerializer::DeserializeVersion(fe->Version());
  if (ver.second != CurrentVersion::FileFormatVersion())
  { throw VersionConflictException(CurrentVersion::ProgramVersion(), ver.first); }
  auto conf = fe->Configuration();
  auto rng = DeSerializer::DeserializeRng(conf->XmlElement(),
                                          conf->BinDataPointer(),
                                          (size_t)conf->SizeInBytes());

  //Done! Return the time stream
  return std::unique_ptr<TimeStream>(new TimeStream(file, fe, rng.first, rng.second));
}

TimeStream::TimeStream(const std::string &file,
                       std::unique_ptr<FileExplorer> &fe,
                       const Random::State &initialRng,
                       int64_t userSeed)
  : _file(file), _initialRng(initialRng), _userSeed(userSeed),
    _fe(std::move(fe)), _curLayerIndex(-1), _needToFlush(false), _time(0.0)
{
  LockFile(_file.c_str());
}

const TimeStream::TimeLayer TimeStream::Current() const
{
  if (_curLayerIndex < 0)
  { throw std::runtime_error("built-in iterator is not set up"); }

  return TimeLayer(_cell.get(), _params.get(), _time, &_rng);
}

bool TimeStream::MoveNext()
{
  if (_curLayerIndex >= (int)_fe->TimeLayerCount() - 1)
  { return false; }
  else
  {
    MoveTo(_curLayerIndex + 1);
    return true;
  }
}

void TimeStream::MoveTo(size_t layerIndex)
{
  
  // Checks
  if (layerIndex >= _fe->TimeLayerCount())
  { throw std::runtime_error("layer's index is out of range"); }
  if (layerIndex == _curLayerIndex)
  { return; }

  if (_curLayerIndex < 0)
  {
    // Load the first time layer
    _curLayerIndex = (int)layerIndex;
    auto conf = _fe->Configuration();
    _cell = DeSerializer::DeserializeCellConfiguration(
        conf->XmlElement(), conf->BinDataPointer(), (size_t)conf->SizeInBytes()
    );

    auto tp = _fe->TimeLayer(_curLayerIndex);
    std::pair<double, Random::State> tr = DeSerializer::DeserializeTimeLayer(
        tp.first->XmlElement(), *_cell,
        tp.first->BinDataPointer(), (size_t)tp.first->SizeInBytes()
    );
    _params = DeSerializer::DeserializeSimParams(tp.second->XmlElement(), 
                                                 tp.second->BinDataPointer(),
                                                 (size_t)tp.second->SizeInBytes());
    _time = tr.first;
    _rng = std::move(tr.second);
  }
  else
  {
    // Load non-first time layer
    _curLayerIndex = (int)layerIndex;
    _params.reset();

    auto tp = _fe->TimeLayer(_curLayerIndex);
    std::pair<double, Random::State> tr = DeSerializer::DeserializeTimeLayer(
        tp.first->XmlElement(), *_cell,
        tp.first->BinDataPointer(), (size_t)tp.first->SizeInBytes()
    );
    _params = DeSerializer::DeserializeSimParams(tp.second->XmlElement(), 
                                                 tp.second->BinDataPointer(),
                                                 (size_t)tp.second->SizeInBytes());
    _time = tr.first;
    _rng = tr.second;
  }
}

void TimeStream::Reset()
{
  if (_curLayerIndex >= 0)
  {
    _cell.reset();
    _params.reset();
    _time = 0.0;
    _curLayerIndex = -1;
  }
}

double TimeStream::GetLayerTime(size_t layerIndex)
{  
  if (layerIndex >= _fe->TimeLayerCount())
  { throw std::runtime_error("layer's index is out of range"); }

  auto tp = _fe->TimeLayer(layerIndex);
  return DeSerializer::DeserializeTime(tp.first->XmlElement());
}

void TimeStream::Append(const SimParams &params)
{
  Reset();
  MemoryStream stream;
  TiXmlElement *elem = Serializer::SerializeSimParams(params, stream);
  _fe->AppendServiceLayer(elem, &stream);

  _needToFlush = true;
}

void TimeStream::Append(const Cell &cell, double time, const Random::State &rng)
{
  ttg_time_stamp("TimeStream::Append");

  Reset();
  MemoryStream stream;
  TiXmlElement *elem = Serializer::SerializeTimeLayer(cell, time, rng, stream);
  _fe->AppendFrameLayer(time, elem, &stream);
  _needToFlush = true;
  ttg_time_stamp("TimeStream::Append");

}

void TimeStream::Flush()
{

  if (_needToFlush)
  {
    if (_fe != nullptr)
    { _fe->Flush(); }
  }
}

TimeStream::~TimeStream()
{
  try
  {
    Flush();
    UnLockFile(_file.c_str());
  }
  catch (std::exception &) { }

  _curLayerIndex = -1;

}

std::unique_ptr<TimeStream> TimeStream::Create(const std::string &file,
                                               const Cell &cell,
                                               const Random::State &rng,
                                               int64_t userSeed)
{
  if (IsFileLocked(file))
  { throw std::runtime_error("file with results is locked"); }

  MemoryStream stream;
  std::unique_ptr<FileExplorer> fe(
      FileExplorer::Create(file, 10,
                           Serializer::SerializeVersion(CurrentVersion::ProgramVersion(),
                                                        CurrentVersion::FileFormatVersion()))
  );
  TiXmlElement* elem = Serializer::SerializeCellConfiguration(cell, rng, userSeed, stream);
  fe->AppendCellConfiguration(elem, &stream);

  return std::unique_ptr<TimeStream>(new TimeStream(file, fe, rng, userSeed));
}

std::unique_ptr<TimeStream> TimeStream::Open(const std::string &file)
{
  return OpenFile(file, false);
}

std::unique_ptr<TimeStream> TimeStream::Repair(const std::string &file)
{
  return OpenFile(file, true);
}

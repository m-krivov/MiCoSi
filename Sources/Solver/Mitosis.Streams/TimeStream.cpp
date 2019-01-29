
#include "TimeStream.h"

#include "ttgUtils.h"

#include "Mitosis.Formatters/Serializer.h"
#include "Mitosis.Formatters/DeSerializer.h"

#ifdef WIN32
#include <Windows.h>
#endif

//------------------
//--- TimeStream ---
//------------------

bool TimeStream::TimeLayerExtractor(void* metaData, size_t metaDataSize, void* binData, size_t binDataSize,
                                    Cell* cell, double &time, size_t &layerCount)
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
    if(maxElemPerChunk <= 0)
      return false;
    uint64_t* offsets = (uint64_t*)((uint8_t*)binData + sizeof(uint64_t));
    uint64_t* sizes = (uint64_t*)((uint8_t*)binData + sizeof(uint64_t) + maxElemPerChunk * sizeof(uint64_t));
    uint64_t offset = sizeof(uint64_t) + maxElemPerChunk * (sizeof(uint64_t) + sizeof(uint64_t));
    for (TiXmlNode *n = doc->RootElement()->FirstChild();
        n != NULL;
        n = n->NextSibling())
    {
      if (offset != offsets[_layerCount])
        return false;
      std::pair<double, uint32_t> tl = DeSerializer::DeserializeTimeLayer(n->ToElement(), cell,
        (uint8_t*)binData + offsets[_layerCount], (size_t)sizes[_layerCount]);
      if (_layerCount == 0)
        _time = tl.first;
      offset += sizes[layerCount];
      _layerCount++;
    }
    layerCount = _layerCount;
    time = _time;

    return true;
  }
  catch(std::exception &)
  {
    return false;
  }
}

bool TimeStream::SimParamsExtractor(void* metaData, size_t metaDataSize, void* binData, size_t binDataSize)
{
  try
  {
    std::unique_ptr<TiXmlDocument> doc(new TiXmlDocument());
    std::string metaString((char*)metaData);
    std::stringstream ss(metaString);
    ss >> *doc;

    std::unique_ptr<SimParams> simParams(DeSerializer::DeserializeSimParams(doc->RootElement()->FirstChild()->ToElement(),
                                        binData, binDataSize));
    return true;
  }
  catch(std::exception&)
  {
    return false;
  }
}

bool TimeStream::CellConfigurationExtractor(void* metaData, size_t metaDataSize, void* binData, size_t binDataSize, Cell* &cell)
{
  try
  {
    std::unique_ptr<TiXmlDocument> doc(new TiXmlDocument());
    std::string metaString((char*)metaData);
    std::stringstream ss(metaString);
    ss >> *doc;
    std::pair<Cell*, uint32_t> cl = DeSerializer::DeserializeCellConfiguration(doc->RootElement()->FirstChild()->ToElement(),
                                         binData, binDataSize);
    cell = cl.first;
    return true;
  }
  catch(std::exception &)
  {
    cell = NULL;
    return false;
  }
}

void TimeStream::LockFile(const char *file)
{
  if (IsFileLocked(file))
  { throw std::runtime_error("Cannot lock already locked file"); }

  FILE *f = fopen((std::string(file) + ".lock").c_str(), "w");
  if (f != NULL)
  {
#ifdef WIN32
    // Writing PID of the current process - to allow other programs remove obsolete lock.
    // PID can be omitted, it's not a problem.
    fprintf(f, "%llu", (uint64_t)GetCurrentProcessId());
#endif
    fclose(f);
  }
}

bool TimeStream::IsFileLocked(const char *file)
{
  bool locked = false;
  std::string lockfile = std::string(file) + ".lock";
  FILE *f = fopen(lockfile.c_str(), "r");
  if (f != NULL)
  {
#ifdef WIN32
    // Checking, is the lock creator still alive.
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

void TimeStream::UnLockFile(const char *file)
{
  if (!IsFileLocked(file))
    throw std::runtime_error("Cannot unlock already unlocked file");

  if (remove((std::string(file) + ".lock").c_str()) != 0)
    throw std::runtime_error("Cannot unlock file");
}

TimeStream::TimeStream(const std::string &file, FileExplorer *fe, MemoryStream* stream, uint32_t initialRandSeed)
  :_timeLayer(NULL, NULL, 0.0, 0)
{
  _file = file;
  _fe = fe;
  _stream = stream;
  _initialRandSeed = initialRandSeed;
  _params = NULL;
  _cell = NULL;
  _curLayerIndex = -1;
  _wasChanged = false;

  LockFile(_file.c_str());
}

bool TimeStream::MoveNext()
{
  // Checking.
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
  
  // Checking.
  if (layerIndex >= _fe->TimeLayerCount())
    throw std::runtime_error("Layer's index is out of range");
  if (layerIndex == _curLayerIndex)
    return;

  if (_curLayerIndex < 0)
  {
    // Loading the first time layer.
    _curLayerIndex = (int)layerIndex;
    auto conf = _fe->Configuration();
    std::pair<Cell *, uint32_t> cr
      = DeSerializer::DeserializeCellConfiguration(conf->XmlElement(), conf->BinDataPointer(), (size_t)conf->SizeInBytes());
    _cell = cr.first;
    auto tp = _fe->TimeLayer(_curLayerIndex);
    std::pair<double, uint32_t> tr = DeSerializer::DeserializeTimeLayer(tp.first->XmlElement(), 
      _cell, tp.first->BinDataPointer(), (size_t)tp.first->SizeInBytes());
    _params = DeSerializer::DeserializeSimParams(tp.second->XmlElement(), 
                                                 tp.second->BinDataPointer(), (size_t)tp.second->SizeInBytes());
    _timeLayer = TimeLayer(_cell, _params, tr.first, tr.second);
  }
  else
  {
    // Loading non-first time layer.
    _curLayerIndex = (int)layerIndex;
    delete _params;
    _params = NULL;
    auto tp = _fe->TimeLayer(_curLayerIndex);
    std::pair<double, uint32_t> tr = DeSerializer::DeserializeTimeLayer(tp.first->XmlElement(), _cell,
      tp.first->BinDataPointer(), (size_t)tp.first->SizeInBytes());
    _params = DeSerializer::DeserializeSimParams(tp.second->XmlElement(), 
      tp.second->BinDataPointer(), (size_t)tp.second->SizeInBytes());
    _timeLayer = TimeLayer(_cell, _params, tr.first, tr.second);
  }
}

void TimeStream::Reset()
{
  if (_curLayerIndex >= 0)
  {
    if (_cell != NULL)
    {
      delete _cell;
      _cell = NULL;
    }

    if (_params != NULL)
    {
      delete _params;
      _params = NULL;
    }

    _curLayerIndex = -1;
    _timeLayer = TimeLayer(NULL, NULL, 0.0, 0);
  }
}

double TimeStream::GetLayerTime(size_t layerIndex)
{  
  if (layerIndex >= _fe->TimeLayerCount())
    throw std::runtime_error("Layer's index is out of range");

  auto tp = _fe->TimeLayer(layerIndex);
  return DeSerializer::DeserializeTime(tp.first->XmlElement());
}

void TimeStream::Append(SimParams *params)
{
  // Reseting.
  Reset();

  // Adding node with simulation parameters.
  _stream->Reset();
  TiXmlElement* elem = Serializer::SerializeSimParams(params, _stream);
  _fe->AppendServiceLayer(elem, _stream);

  _wasChanged = true;
}

void TimeStream::Append(Cell *cell, double time, uint32_t randSeed)
{
  ttg_time_stamp("TimeStream::Append");
  // Reseting enumerator.
  Reset();

  // Adding new time layer.
  _stream->Reset();
  TiXmlElement* elem = Serializer::SerializeTimeLayer(cell,
                             time,
                             randSeed,
                             _stream);
  _fe->AppendFrameLayer(time, elem, _stream);
  _wasChanged = true;
  ttg_time_stamp("TimeStream::Append");

}

void TimeStream::Flush()
{

  if (_wasChanged)
  {
    if (_fe != NULL)
    { _fe->Flush(); }
  }
}

TimeStream::~TimeStream()
{
  Flush();

  if (_fe != NULL)
  {
    delete _fe;
    _fe = NULL;
  }
  if (_stream != NULL)
  {
    delete _stream;
    _stream = NULL;
  }
  if (_cell != NULL)
  {
    delete _cell;
    _cell = NULL;
  }
  if (_params != NULL)
  {
    delete _params;
    _params = NULL;
  }
  _curLayerIndex = -1;

  UnLockFile(_file.c_str());
}

TimeStream *TimeStream::Create(const char *file, Cell *cell, uint32_t initialRandSeed)
{
  // Checking lock.
  if (IsFileLocked(file))
    throw std::runtime_error("File with results is locked");

  MemoryStream* stream = NULL;
  FileExplorer *fe = NULL; 
  try
  {
    fe = FileExplorer::Create(file, 10,
                  Serializer::SerializeVersion(&CurrentVersion::ProgramVersion(),
                                 CurrentVersion::FileFormatVersion()));
    stream = new MemoryStream();
    TiXmlElement* elem = Serializer::SerializeCellConfiguration(cell, initialRandSeed, stream);
    fe->AppendCellConfiguration(elem, stream);
  }
  catch (std::exception &)
  {
    if (fe != NULL)
      delete fe;
    if(stream != NULL)
      delete stream;
    throw;
  }

  // Creating and returning time stream.
  return new TimeStream(file, fe, stream, initialRandSeed);
}

TimeStream *TimeStream::_UniOpen(const char *file, bool repair)
{
  // Checking lock.
  if (IsFileLocked(file))
    throw std::runtime_error("File with results is locked");

  FileExplorer *fe = NULL;
  MemoryStream* stream = new MemoryStream();
  try
  {
    fe = repair
      ? FileExplorer::Repair(file, &TimeLayerExtractor, &SimParamsExtractor, &CellConfigurationExtractor)
      : FileExplorer::Open(file);
  }
  catch (std::exception &)
  {
    if (fe != NULL)
      delete fe;
    if (stream != NULL)
      delete stream;
    throw;
  }

  // Getting initial random seed and checking version.
  uint32_t initialRandSeed = 0;
  try
  {
    std::pair<Version, int> ver = DeSerializer::DeserializeVersion(fe->Version());
    if (ver.second != CurrentVersion::FileFormatVersion())
      throw VersionConflictException(CurrentVersion::ProgramVersion(), ver.first);
    auto conf = fe->Configuration();
    std::pair<Cell *, uint32_t> cr = DeSerializer::DeserializeCellConfiguration(conf->XmlElement(),
      conf->BinDataPointer(), (size_t)conf->SizeInBytes());
    initialRandSeed = cr.second;
    delete cr.first;
  }
  catch (std::exception &)
  {
    if (fe != NULL)
      delete fe;
    if (stream != NULL)
      delete stream;
    throw;
  }

  //Creating and returning time stream.
  return new TimeStream(file, fe, stream, initialRandSeed);
}

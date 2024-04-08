#include "FileContainer.h"

#include "MiCoSi.Core/Versions.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef _MSC_VER
#define _fseeki64 fseeko
#define _ftelli64 ftello
#endif

#ifdef _WIN32
#define FREAD(buf, dstSize, elemSize, count, f) fread_s(buf, dstSize, elemSize, count, f);
#else
#define FREAD(buf, dstSize, elemSize, count, f) fread(buf, elemSize, count, f);
#endif


//--------------------------------
//--- FileContainer::Formatter ---
//--------------------------------

size_t FileContainer::Formatter::WriteHeader(FILE *f, uint64_t version)
{
  size_t size = 0;
  size_t res;

  char name[] = "?header";
  res = fprintf(f, "%s", name);
  if(res != std::strlen(name))
    throw std::runtime_error("Error at FileContainer::Formatter::WriteHeader() - cannot write ?header");
  size += res;

  uint64_t offsetToFirstChunk = std::strlen(name) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t);
  res = fwrite(&offsetToFirstChunk, 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::WriteHeader() - cannot write offset to first chunk");
  size += res;

  res = fwrite(&version, 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::WriteHeader() - cannot write version");
  size += res;

  uint64_t offsetToTable = -1;
  res = fwrite(&offsetToTable, 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::WriteHeader() - cannot write offset to table");
  size += res;

  return size;
}

size_t FileContainer::Formatter::WriteChunk(FILE *f, ChunkType::Type type, double time,
                                            void *metaData, size_t metaDataSize,
                                            void *binData, size_t binDataSize)
{
  size_t size = 0;
  size_t res;

  if (type == ChunkType::Frames)
  {
    const char name[] = "?frame";
    res = fprintf(f, name);
    if (res != strlen(name))
    { throw std::runtime_error("Error at FileContainer::Formatter::WriteChunk() - cannot write ?frame"); }
    size += res;

    res = fwrite(&time, 1, sizeof(double), f);
    if (res != sizeof(double))
    { throw std::runtime_error("Error at FileContainer::Formatter::WriteChunk() - cannot write chunk time"); }
    size += res;
  }
  else 
  {
    const char name[] = "?service";
    res = fprintf(f, name);
    if (res != strlen(name))
    { throw std::runtime_error("Error at FileContainer::Formatter::WriteChunk() - cannot write ?service"); }
    size += res;
  }

  uint64_t dsize = metaDataSize;
  res = fwrite(&dsize, 1, sizeof(uint64_t), f);
  if (res != sizeof(uint64_t))
  { throw std::runtime_error("Error at FileContainer::Formatter::WriteChunk() - cannot write metadata size"); }
  size += res;

  dsize = binDataSize;
  res = fwrite(&dsize, 1, sizeof(uint64_t), f);
  if (res != sizeof(uint64_t))
  { throw std::runtime_error("Error at FileContainer::Formatter::WriteChunk() - cannot write bin data size"); }
  size += res;

  if (metaDataSize > 0)
  {
    res = fwrite(metaData, 1, metaDataSize, f);
    if(res != metaDataSize)
    { throw std::runtime_error("Error at FileContainer::Formatter::WriteChunk() - cannot write meta data"); }
    size += res;
  }
  if (binDataSize > 0)
  {
    res = fwrite(binData, 1, binDataSize, f);
    if(res != binDataSize)
    { throw std::runtime_error("Error at FileContainer::Formatter::WriteChunk() - cannot write bin data"); }
    size += res;
  }
  return size;
}

size_t FileContainer::Formatter::WriteTable(FILE *f, std::vector<ChunkHeader> table)
{
  size_t size = 0;
  size_t res;

  char name[] = "?table";
  res = fprintf(f, "%s", name);
  if(res != strlen(name))
  { throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write ?table"); }
  size += res;

  uint64_t chunkCount = table.size();
  res = fwrite(&chunkCount, 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
  { throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write chunk count"); }
  size += res;

  for(auto it = table.begin(); it != table.end(); it++)
  {
    ChunkHeader chunk = *it;
    double chunkTime = chunk.StartTime();
    res = fwrite(&chunkTime, 1, sizeof(double), f);
    if(res != sizeof(double))
      throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write chunk start time");
    size += res;

    uint32_t count = (uint32_t)chunk.Count();
    res = fwrite(&count, 1, sizeof(uint32_t), f);
    if(res != sizeof(uint32_t))
      throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write elem per chunk");
    size += res;

    uint32_t chunkType = chunk.Type() == ChunkType::Frames ? 1 : 2;
    res = fwrite(&chunkType, 1, sizeof(uint32_t), f);
    if(res != sizeof(uint32_t))
      throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write chunk type");
    size += res;

    uint64_t offsetToChunk = chunk.ChunkOffset();
    res = fwrite(&offsetToChunk, 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write offset to chunk");
    size += res;

    uint64_t dsize = chunk.ChunkSize();
    res = fwrite(&dsize, 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write chunk size");
    size += res;

    dsize = chunk.MetaDataSize();
    res = fwrite(&dsize, 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write metadata size");
    size += res;

    dsize = chunk.BinDataSize();
    res = fwrite(&dsize, 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::WriteTable() - cannot write bin data size");
    size += res;
  }
  return size;
}

void FileContainer::Formatter::UpdateHeader(FILE *f, offset_t tablePosition)
{
  size_t res;
  size_t size = 0;
  char name[256];
  memset(name, 0, sizeof(name));
  res = FREAD(name, 256, 1, std::strlen("?header"), f);
  if(res != std::strlen("?header") || std::strcmp("?header", name) != 0)
    throw std::runtime_error("Error at FileContainer::Formatter::UpdateHeader() - cannot read ?header");
  size += res;

  uint64_t offsetToFirstChunk;
  res = FREAD(&offsetToFirstChunk, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::UpdateHeader() - cannot read offset to first chunk");
  size += res;

  uint64_t version;
  res = FREAD(&version, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::UpdateHeader() - cannot read version");
  size += res;
  uint64_t _tablePosition = tablePosition;
  uint64_t current = _ftelli64(f);
  uint64_t currentOffset;

  res = FREAD(&currentOffset, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::UpdateHeader() - cannot read offset to table");
  size += res;
  if(size != offsetToFirstChunk)
    throw std::runtime_error("Error at FileContainer::Formatter::UpdateHeader() - bad offset to first chunk");

  _fseeki64(f, current, SEEK_SET);
  res = fwrite(&_tablePosition, 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::UpdateHeader() - cannot update offset to table");
}

size_t FileContainer::Formatter::ReadHeader(FILE *f, uint64_t &version, offset_t &tableOffset)
{
  size_t res;
  size_t size = 0;
  char name[256];
  memset(name, 0, sizeof(name));
  res = FREAD(name, sizeof(name), 1, std::strlen("?header"), f);
  if(res != std::strlen("?header") || std::strcmp(name, "?header") != 0)
    throw std::runtime_error("Error at FileContainer::Formatter::ReadHeader() - cannot read ?header");
  size += res;

  uint64_t offsetToFirstChunk;
  res = FREAD(&offsetToFirstChunk, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::ReadHeader() - cannot read offset to first chunk");
  size += res;
  uint64_t _version;
  res = FREAD(&_version, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::ReadHeader() - cannot read version");
  size += res;
  uint64_t _tableOffset;
  res = FREAD(&_tableOffset, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::ReadHeader() - cannot read offset to table");
  size += res;

  if(offsetToFirstChunk != size)
    throw std::runtime_error("Error at FileContainer::Formatter::ReadHeader() - bad offset to first chunk");
  version = _version;
  tableOffset = _tableOffset;
  return size;
}

size_t FileContainer::Formatter::ScanChunk(FILE *f, ChunkType::Type &type, size_t &metaDataSize, size_t &binDataSize)
{
  size_t size = 0;
  size_t res;

  char name[256];
  memset(name, 0, sizeof(name));
  res = FREAD(name, sizeof(name), 1, 2*sizeof(char), f);
  if(res != 2 * sizeof(char))
    throw std::runtime_error("Error at FileContainer::Formatter::ScanChunk() - cannot read type");
  size += res;
  if(name[1] == 'f')
  {
    res = FREAD(name+2, sizeof(name)-2, 1, strlen("rame"), f);
    if(res != std::strlen("rame") || std::strcmp(name, "?frame") != 0)
      throw std::runtime_error("Error at FileContainer::Formatter::ScanChunk() - problem with type detect");
    size += res;
    double _time;
    res = FREAD(&_time, sizeof(double), 1, sizeof(double), f);
    if(res != sizeof(double))
      throw std::runtime_error("Error at FileContainer::Formatter::ScanChunk() - cannot read time");
    size += res;
    type = ChunkType::Frames;
  }
  else if(name[1] == 's')
  {
    res = FREAD(name+2, sizeof(name)-2, 1, strlen("ervice"), f);
    if(res != std::strlen("ervice") || std::strcmp(name, "?service") != 0)
      throw std::runtime_error("Error at FileContainer::Formatter::ScanChunk() - problem with type detect");
    size += res;
    type = ChunkType::Service;
  } else
    throw std::runtime_error("Error at FileContainer::Formatter::ScanChunk() - bad type");

  uint64_t _metaDataSize;
  res = FREAD(&_metaDataSize, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::ScanChunk() - read write metadata size");
  size += res;

  uint64_t _binDataSize;
  res = FREAD(&_binDataSize, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::ScanChunk() - read write bin data size");
  size += res;

  metaDataSize = (size_t)_metaDataSize;
  binDataSize = (size_t)_binDataSize;

  return size;
}

size_t FileContainer::Formatter::LoadChunk(FILE *f, void *metaData, size_t metaDataSize, void *binData, size_t binDataSize)
{
  size_t size = 0;
  size_t res;

  char name[256];
  memset(name, 0, sizeof(name));
  res = FREAD(name, sizeof(name), 1, 2*sizeof(char), f);
  if(res != 2 * sizeof(char))
    throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - cannot read type");
  size += res;

  if(name[1] == 'f')
  {
    res = FREAD(name+2, sizeof(name)-2, 1, std::strlen("rame"), f);
    if(res != std::strlen("rame") || std::strcmp(name, "?frame") != 0)
      throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - problem with type detect");
    size += res;
    double time;
    res = FREAD(&time, sizeof(double), 1, sizeof(double), f);
    if(res != sizeof(double))
      throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - cannot read time");
    size += res;
  }
  else if(name[1] == 's')
  {
    res = FREAD(name+2, sizeof(name)-2, 1, strlen("ervice"), f);
    if(res != std::strlen("ervice") || std::strcmp(name, "?service") != 0)
      throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - problem with type detect");
    size += res;
  }
  else
    throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - bad type");

  uint64_t _metaDataSize;
  res = FREAD(&_metaDataSize, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - cannot read metadata size");
  size += res;

  uint64_t _binDataSize;
  res = FREAD(&_binDataSize, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - cannot read bin data size");
  size += res;

  if(_metaDataSize != metaDataSize || _binDataSize != binDataSize)
    throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - different data sizes");

  if(_metaDataSize > 0)
  {
    res = FREAD(metaData, metaDataSize, 1, metaDataSize, f);
    if(res != _metaDataSize)
      throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - cannot read meta data");
    size += res;
  }

  if(_binDataSize > 0)
  {
    res = FREAD(binData, binDataSize, 1, binDataSize, f);
    if(res != _binDataSize)
      throw std::runtime_error("Error at FileContainer::Formatter::LoadChunk() - cannot read bin data");
    size += res;
  }

  return size;
}

size_t FileContainer::Formatter::ReadTable(FILE *f, std::vector<ChunkHeader> &table)
{
  size_t size = 0;
  size_t res;

  char name[256];
  memset(name, 0, sizeof(name));
  res = FREAD(name, sizeof(name), 1, std::strlen("?table"), f);
  if(res != std::strlen("?table") || std::strcmp("?table", name) != 0)
    throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read ?table");
  size += res;

  uint64_t chunkCount;
  res = FREAD(&chunkCount, sizeof(uint64_t), 1, sizeof(uint64_t), f);
  if(res != sizeof(uint64_t))
    throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read chunk count");
  size += res;

  table.clear();

  for(uint64_t i = 0; i < chunkCount; i++)
  {
    double chunkTime;
    res = FREAD(&chunkTime, sizeof(double), 1, sizeof(double), f);
    if(res != sizeof(double))
      throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read chunk start time");
    size += res;

    uint32_t count;
    res = FREAD(&count, sizeof(uint32_t), 1, sizeof(uint32_t), f);
    if(res != sizeof(uint32_t))
      throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read elem per chunk");
    size += res;

    uint32_t chunkType;
    res = FREAD(&chunkType, sizeof(uint32_t), 1, sizeof(uint32_t), f);
    if(res != sizeof(uint32_t))
      throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read chunk type");
    size += res;
    ChunkType::Type type;
    switch (chunkType)
    {
      case 1:
        type = ChunkType::Frames;
        break;
      case 2:
        type = ChunkType::Service;
        break;
      default:
        throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - bad chunk type");
    }

    uint64_t offsetToChunk;
    res = FREAD(&offsetToChunk, sizeof(uint64_t), 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read offset to chunk");
    size += res;

    uint64_t chunkSize;
    res = FREAD(&chunkSize, sizeof(uint64_t), 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read chunk size");
    size += res;

    uint64_t metaDataSize;
    res = FREAD(&metaDataSize, sizeof(uint64_t), 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read metadata size");
    size += res;

    uint64_t binDataSize;
    res = FREAD(&binDataSize, sizeof(uint64_t), 1, sizeof(uint64_t), f);
    if(res != sizeof(uint64_t))
      throw std::runtime_error("Error at FileContainer::Formatter::ReadTable() - cannot read bin data size");
    size += res;
    
    table.push_back(ChunkHeader(type, chunkTime, count, offsetToChunk, chunkSize, metaDataSize, binDataSize));
  }
  return size;
}

//---------------------
//--- FileContainer ---
//---------------------

FILE *FileContainer::OpenFile(const std::string &filename, const char *opt)
{
  FILE *f = nullptr;

#ifdef WIN32
  if (fopen_s(&f, filename.c_str(), opt) != 0)
  {
    std::stringstream ss;
    ss << "Error at FileContainer::OpenFile() - cannot open \"";
    ss << filename;
    ss << "\"";
    throw std::runtime_error(ss.str());
  }
#else
  f = fopen(filename, opt);
  if (f == nullptr)
  {
    std::stringstream ss;
    ss << "Error at FileContainer::OpenFile() - cannot open \"";
    ss << filename;
    ss << "\"";
    throw std::runtime_error(ss.str());
  }
#endif
  return f;
}

void FileContainer::AppendChunk(ChunkType::Type type, double time, size_t count,
                void *metaData, size_t metaDataSize,
                void *binData, size_t binDataSize)
{
  if (_fseeki64(_file, _newChunkStart, SEEK_SET) != 0)
    throw std::runtime_error("Error at FileContainer::AppendChunk() - failed to locate chunk position");

  size_t size = (size_t)Formatter::WriteChunk(_file, type, time, metaData, metaDataSize, binData, binDataSize);
  _table.push_back(ChunkHeader(type, time, count, _newChunkStart, size, metaDataSize, binDataSize));
  _newChunkStart += size;
  _rewriteTable = true;
}

std::shared_ptr<Chunk> FileContainer::LoadChunk(size_t chunkNumber)
{
  if (chunkNumber >= _table.size())
    throw std::runtime_error("Error at FileContainer::LoadChunk() - no such chunk");
  
  auto header = _table[chunkNumber];
  std::shared_ptr<Chunk> res(new Chunk(header));
  if (_fseeki64(_file, header.ChunkOffset(), SEEK_SET) != 0)
    throw std::runtime_error("Error at FileContainer::LoadChunk() - failed to locate required chunk");
  Formatter::LoadChunk(_file, res->MetaDataPointer(), (size_t)header.MetaDataSize(),
                res->BinDataPointer(), (size_t)header.BinDataSize());

  return res;
}

FileContainer::~FileContainer()
{
  if (_rewriteTable)
  {
    offset_t tablePosition = _newChunkStart;
    if (_fseeki64(_file, tablePosition, SEEK_SET) != 0)
      throw std::runtime_error("Error at FileContainer::~FileContainer() - failed to locate table position");
    Formatter::WriteTable(_file, _table);

    if (_fseeki64(_file, 0, SEEK_SET) != 0)
      throw std::runtime_error("Error at FileContainer::~FileContainer() - failed to locate header position");
    Formatter::UpdateHeader(_file, tablePosition);
  }
  fclose(_file);
}

std::shared_ptr<FileContainer> FileContainer::Open(const std::string &filename)
{
  FILE* f = OpenFile(filename, "r+b");
  try
  {
    std::vector<ChunkHeader> table;
    offset_t offset;
    uint64_t version;
    Formatter::ReadHeader(f, version, offset);
    if (_fseeki64(f, offset, SEEK_SET) != 0)
      throw std::runtime_error("Error at FileContainer::Open() - failed to locate table position");
    Formatter::ReadTable(f, table);
    return std::shared_ptr<FileContainer>(new FileContainer(f, offset, table, version, false));
  }
  catch (std::exception &)
  {
    if (f != nullptr)
      fclose(f);
    throw;
  }
}

bool FileContainer::ValidateChunk(FILE *f, uint64_t &newChunkStart, std::vector<ChunkHeader> &table,
                                  std::unique_ptr<Cell> &cell,
                                  FrameExtractor fextr, ServiceExtractor sextr, CellExtractor cextr)
{
  try
  {
    if (_fseeki64(f, newChunkStart, SEEK_SET) != 0)
      return false;
    ChunkType::Type chunkType;
    size_t metaDataSize, binDataSize;
    uint64_t scanChunkSize = Formatter::ScanChunk(f, chunkType, metaDataSize, binDataSize);
    if (_fseeki64(f, newChunkStart, SEEK_SET) != 0)
      return false;
    std::unique_ptr<uint8_t> metaData;
    std::unique_ptr<uint8_t> binData;
    if(metaDataSize > 0)
      metaData.reset(new uint8_t[metaDataSize]);
    if(binDataSize > 0)
      binData.reset(new uint8_t[binDataSize]);
    uint64_t loadChunkSize = Formatter::LoadChunk(f, metaData.get(), metaDataSize, binData.get(), binDataSize);
    if(scanChunkSize + metaDataSize + binDataSize != loadChunkSize)
      return false;
    double time = 0.0;
    size_t count = 1;
    if(!table.size() && chunkType == ChunkType::Service && !cextr(metaData.get(), metaDataSize, binData.get(), binDataSize, cell))
      return false;
    if(table.size() && chunkType == ChunkType::Frames && !fextr(metaData.get(), metaDataSize, binData.get(), binDataSize, *cell, time, count))
      return false;
    if(table.size() && chunkType == ChunkType::Service && !sextr(metaData.get(), metaDataSize, binData.get(), binDataSize))
      return false;
    table.push_back(ChunkHeader(chunkType, time, count, newChunkStart, loadChunkSize, metaDataSize, binDataSize));
    newChunkStart += loadChunkSize;
    return true;
  }
  catch(std::exception&)
  {
    return false;
  }
}

std::shared_ptr<FileContainer> FileContainer::Repair(const std::string &filename,
                                                     FrameExtractor fextr,
                                                     ServiceExtractor sextr,
                                                     CellExtractor cextr)
{
  FILE* f = nullptr;
  std::unique_ptr<Cell> cell;
  try
  {
    f = OpenFile(filename, "r+b");
    offset_t offsetToTable;
    uint64_t version;
    uint64_t newChunkStart = Formatter::ReadHeader(f, version, offsetToTable);
    std::vector<ChunkHeader> table;
    bool res = true;
    table.clear();
    while (res)
    { res = ValidateChunk(f, newChunkStart, table, cell, fextr, sextr, cextr); }
#ifdef _WIN32
    errno_t err = _chsize_s(_fileno(f), newChunkStart);
    if(err != 0)
      throw std::runtime_error("FileContainer::Repair - Cannot change file size");
#else
    if (ftruncate(fileno(f), newChunkStart) != 0)
      throw std::runtime_error("FileContainer::Repair - Cannot change file size");
#endif
    return std::shared_ptr<FileContainer>(new FileContainer(f, newChunkStart, table, version, true));
  }
  catch (std::exception &)
  {
    if (f != nullptr)
      fclose(f);
    throw;
  }
}

std::shared_ptr<FileContainer> FileContainer::Create(const std::string &filename, uint64_t version)
{
  FILE *f = OpenFile(filename, "w+b");
  try
  {
    std::vector<ChunkHeader> empty_table;
    offset_t startChunk = Formatter::WriteHeader(f, version);
    return std::shared_ptr<FileContainer>(new FileContainer(f, startChunk, empty_table, version, true));
  }
  catch (std::exception &)
  {
    if (f != nullptr)
    { fclose(f); }
    throw;
  }
}


#include "FileExplorer.h"
//#include <sstream>

FileExplorer::ChunkElement::ChunkElement(TiXmlElement* elem, void* data, uint64_t sizeInBytes, bool copy)
{
  if(copy)
  {
    _data = malloc((size_t)sizeInBytes);
    memcpy(_data, data, (size_t)sizeInBytes);
    _elem = (TiXmlElement*)elem->Clone();
  } else
  {
    _data = data;
    _elem = elem;
  }
  _isCopy = copy;
  _sizeInBytes = sizeInBytes;
}

FileExplorer::ChunkElement::~ChunkElement()
{
  if(_isCopy)
  {
    delete _elem;
    free(_data);
  }
}

FileExplorer::WritingChunk::WritingChunk(std::shared_ptr<FileContainer> fc, size_t maxElemPerChunk)
{
  _time = 0;
  _maxElemPerChunk = maxElemPerChunk;
  _currentIdx = 0;
  _doc = new TiXmlDocument();
  _stream = new MemoryStream();
  _offsets = new uint64_t[maxElemPerChunk];
  _sizes = new uint64_t[maxElemPerChunk];
  _fc = fc;
  _elements.clear();
}

void FileExplorer::WritingChunk::Flush()
{
  if(_currentIdx != 0)
  {
    _stream->Write(&_maxElemPerChunk, 0, sizeof(uint64_t));
    _stream->Write(_offsets, sizeof(uint64_t), (size_t)_maxElemPerChunk * sizeof(uint64_t));
    _stream->Write(_sizes, (size_t)(sizeof(uint64_t) + _maxElemPerChunk * sizeof(uint64_t)), (size_t)_maxElemPerChunk * sizeof(uint64_t));
    std::stringstream ss;
    ss << *_doc;
    std::string str = ss.str();
    _fc->AppendFrameChunk(_time, _currentIdx, (void*)str.c_str(), str.size(), _stream->GetBuffer(), _stream->Length());
    _stream->Reset();
    _doc->Clear();
    _currentIdx = 0;
    for(auto p = _elements.begin(); p != _elements.end(); p++)
      delete *p;
    _elements.clear();
  }
}

void FileExplorer::WritingChunk::Add(TiXmlElement* elem, double time, MemoryStream* stream)
{
  if(_currentIdx >= _maxElemPerChunk)
    throw std::runtime_error("Too many elements");
  uint64_t offset;
  if(_currentIdx == 0)
  {
    _time = time;
    if (_doc->LinkEndChild(new TiXmlDeclaration("1.0", "utf-8", "no")) == NULL)
      throw std::runtime_error("Cannot create XML declaration");
    TiXmlElement *root = new TiXmlElement("Chunk");
    if (_doc->LinkEndChild(root) == NULL)
    {
      delete root;
      throw std::runtime_error("Cannot create root section");
    }
    offset = sizeof(uint64_t) + _maxElemPerChunk * (sizeof(uint64_t) + sizeof(uint64_t));
    _stream->Write(stream->GetBuffer(), (size_t)offset, stream->Length());
  }
  else
  {
    offset = _stream->Position();
    _stream->Write(stream);
  }
  if (_doc->RootElement()->LinkEndChild(elem) == NULL)
    throw std::runtime_error("Cannot link element");
  _offsets[_currentIdx] = offset;
  _sizes[_currentIdx] = stream->Length();
  _currentIdx++;
}

FileExplorer::ChunkElement* FileExplorer::WritingChunk::GetElement(size_t idx)
{
  if(idx >= _currentIdx)
    throw std::runtime_error("Cannot get element");
  TiXmlNode *n = _doc->RootElement()->FirstChild();
  for (size_t i = 0; i  < idx; i++)
    n = n->NextSibling();
  ChunkElement* elem = new ChunkElement(n->ToElement(), (uint8_t*)_stream->GetBuffer() + _offsets[idx], _sizes[idx]);
  _elements.push_back(elem);
  return elem;
}

FileExplorer::WritingChunk::~WritingChunk()
{
  for(auto p = _elements.begin(); p != _elements.end(); p++)
    delete *p;
  _elements.clear();
  delete _doc;
  delete _stream;
  delete[] _offsets;
  delete[] _sizes;
}

void FileExplorer::ReadingChunk::LoadChunk(std::shared_ptr<Chunk> chunk)
{
  _chunk = chunk;
  _maxElemPerChunk = ((uint64_t*)_chunk->BinDataPointer())[0];
  if(_maxElemPerChunk <= 0)
    throw std::runtime_error("FileExplorer::ReadingChunk::LoadChunk - Incorrect max elem per chunk");
  _offsets = (uint64_t*)((uint8_t*)_chunk->BinDataPointer() + sizeof(uint64_t));
  _sizes = (uint64_t*)((uint8_t*)_chunk->BinDataPointer() + sizeof(uint64_t) + _maxElemPerChunk * sizeof(uint64_t));
  for(auto p = _elements.begin(); p != _elements.end(); p++)
    delete *p;
  _elements.clear();
  if(_doc == NULL)
    _doc = new TiXmlDocument();
  else
  {
    _doc->ClearError();
    _doc->Clear();
  }
  std::string metaString((char*)chunk->MetaDataPointer());
  std::stringstream ss(metaString);
  ss >> *_doc;
  if(_doc->Error())
    throw std::runtime_error("FileExplorer::ReadingChunk::LoadChunk - Some error");
  if(_doc->RootElement() == NULL || _doc->RootElement()->FirstChild() == NULL)
    throw std::runtime_error("FileExplorer::ReadingChunk::LoadChunk - Cannot get first child");
  uint64_t i = 0;
  uint64_t offset = sizeof(uint64_t) + _maxElemPerChunk * (sizeof(uint64_t) + sizeof(uint64_t));
  for (TiXmlNode *n = _doc->RootElement()->FirstChild();
      n != NULL;
      n = n->NextSibling())
  {
    if(n->ToElement() == NULL)
      throw std::runtime_error("FileExplorer::ReadingChunk::LoadChunk - Cannot get element");
    if(i >= _maxElemPerChunk)
      throw std::runtime_error("FileExplorer::ReadingChunk::LoadChunk - Too small max elem per chunk");
    if(offset != _offsets[i])
      throw std::runtime_error("FileExplorer::ReadingChunk::LoadChunk - Corrupted offsets");
    _elements.push_back(new ChunkElement(n->ToElement(), (uint8_t*)_chunk->BinDataPointer() + _offsets[i], _sizes[i]));
    offset += _sizes[i];
    i++;
  }
  _currentElemPerChunk = i;
}

FileExplorer::ReadingChunk::~ReadingChunk()
{
  for(auto p = _elements.begin(); p != _elements.end(); p++)
    delete *p;
  _elements.clear();
  if(_doc != NULL)
    delete _doc;
}

FileExplorer::ChunkElement* FileExplorer::ReadingChunk::Read(size_t idx)
{
  if(idx >= _currentElemPerChunk)
    throw std::runtime_error("Too big index");
  return _elements[idx];
}

//--------------------
//--- FileExplorer ---
//--------------------

FileExplorer::FileExplorer(const std::string &file)
{
  _file = file;
  _fc.reset();
  _configuration = NULL;
  _writingChunk = NULL;
  _readingChunk = NULL;
  _simParams.clear();
  _additional.clear();
  _elementsPerChunk.clear();
  _chunkIdx = 0;
  _maxElemPerChunk = 0;
  _currentChunk = -1;
}

void FileExplorer::SaveService(TiXmlElement* elem, MemoryStream* stream)
{
  TiXmlDocument *doc = new TiXmlDocument();
  if (doc->LinkEndChild(new TiXmlDeclaration("1.0", "utf-8", "no")) == NULL)
  {
    delete doc;
    throw std::runtime_error("Cannot create XML declaration");
  }

  TiXmlElement *root = new TiXmlElement("ServiceChunk");
  if (doc->LinkEndChild(root) == NULL)
  {
    delete doc;
    delete root;
    throw std::runtime_error("Cannot create root section");
  }

  if (root->LinkEndChild(elem) == NULL)
  {
    delete doc;
    throw std::runtime_error("Cannot link element");
  }
  std::stringstream ss;
  ss << *doc;
  std::string str = ss.str();
  _fc->AppendServiceChunk((void*)ss.str().c_str(), str.size(), stream->GetBuffer(), stream->Length());
}

size_t FileExplorer::TimeLayerCount()
{
  if(_additional.size() == 0)
    return _chunkIdx;
  return _additional[_additional.size()-1].first + _elementsPerChunk[_elementsPerChunk.size()-1] + _chunkIdx;
}

FileExplorer::ChunkElement* FileExplorer::LoadService(size_t idx)
{
  TiXmlDocument* doc = NULL;
  try
  {
    auto chunk = _fc->LoadChunk(idx);
    doc = new TiXmlDocument();
    std::string metaString((char*)chunk->MetaDataPointer());
    std::stringstream ss(metaString);
    ss >> *doc;
    if(doc->Error())
      throw std::runtime_error("FileExplorer::LoadService - Some error");
    if(doc->RootElement() == NULL || doc->RootElement()->FirstChild() == NULL ||
      doc->RootElement()->FirstChild()->ToElement() == NULL)
      throw std::runtime_error("FileExplorer::LoadService - Cannot get element");
    TiXmlNode* elem = doc->RootElement()->FirstChild();
    ChunkElement* res = new ChunkElement(elem->ToElement(), chunk->BinDataPointer(), chunk->Header().BinDataSize(), true);
    delete doc;
    return res;
  } catch(std::exception &)
  {
    if(doc != NULL)
      delete doc;
    throw;
  }
}

void FileExplorer::ReadTable()
{
  auto table = _fc->Table();
  if(table.size() <= 2)
    throw std::runtime_error("FileExplorer::ReadTable - Too small table");
  _configuration = LoadService(0);
  for(size_t simIdx = 1; simIdx < table.size(); simIdx++)
  {
    ChunkHeader header = table[simIdx];
    if(header.Type() == ChunkType::Service)
      _simParams.push_back(LoadService(simIdx));
  }
  if(_simParams.size() == 0)
    throw std::runtime_error("FileExplorer::ReadTable - Not Found SimParams");
  size_t size = 0;
  size_t currentSimIdx = 0;
  for(size_t chunkIdx = 1; chunkIdx < table.size(); chunkIdx++)
  {
    ChunkHeader header = table[chunkIdx];
    if(header.Type() == ChunkType::Frames)
    {
      size_t count = header.Count();
      _elementsPerChunk.push_back(count);
      _additional.push_back(std::make_pair(size, currentSimIdx));
      size += count;
    }
    else
      currentSimIdx++;
  }
}

const std::pair<FileExplorer::ChunkElement*, FileExplorer::ChunkElement*> FileExplorer::TimeLayer(size_t n)
{
  if(_additional.size() == 0)
    if(_simParams.size() == 0)
      throw std::runtime_error("FileExplorer::TimeLayer - sim params not exist");
    else
      return std::make_pair(_writingChunk->GetElement(n), _simParams[0]);

  size_t chunkNumber = 0;
  while(chunkNumber < _additional.size() && n >= _additional[chunkNumber].first + _elementsPerChunk[chunkNumber])
    chunkNumber++;
  
  if(chunkNumber == _additional.size())
  {
    size_t size = _additional.size() - 1;
    if(_additional[size].second-1 >= _simParams.size())
      throw std::runtime_error("FileExplorer::TimeLayer - sim params not found");
    return std::make_pair(_writingChunk->GetElement(n - _additional[size].first), _simParams[_additional[size].second-1]);
  }

  size_t chunkInTable = chunkNumber + 1 + _additional[chunkNumber].second;

  if(chunkInTable != _currentChunk)
  {
    _readingChunk->LoadChunk(_fc->LoadChunk(chunkInTable));
    _currentChunk = chunkInTable;
  }

  size_t innerIdx = n - _additional[chunkNumber].first;
  if(_additional[chunkNumber].second-1 >= _simParams.size())
    throw std::runtime_error("FileExplorer::TimeLayer - sim params not found");

  return std::make_pair(_readingChunk->Read(innerIdx), _simParams[_additional[chunkNumber].second-1]);
}

void FileExplorer::AppendFrameLayer(double time, TiXmlElement* elem, MemoryStream* stream)
{
  _chunkIdx++;
  _writingChunk->Add(elem, time, stream);
  
  if(_chunkIdx >= _maxElemPerChunk)
    Flush();
}

void FileExplorer::AppendServiceLayer(TiXmlElement* elem, MemoryStream* stream)
{
  Flush();
  _simParams.push_back(new ChunkElement(elem, stream->GetBuffer(), stream->Length(), true));
  SaveService(elem, stream);
}

void FileExplorer::AppendCellConfiguration(TiXmlElement* elem, MemoryStream* stream)
{
  _configuration = new ChunkElement(elem, stream->GetBuffer(), stream->Length(), true);
  SaveService(elem, stream);
}

void FileExplorer::Flush()
{
  if(_chunkIdx == 0)
    return;
  _writingChunk->Flush();
  _elementsPerChunk.push_back(_chunkIdx);
  if(_simParams.size() == 0)
    throw std::runtime_error("Not found sim params");
  size_t simNumber = _simParams.size() - 1;
  size_t counts = _additional.size() == 0 ? 0 : _additional[_additional.size() - 1].first + _chunkIdx;
  _additional.push_back(std::make_pair(counts, simNumber));
  _chunkIdx = 0;
}

FileExplorer::~FileExplorer()
{
  Flush();

  delete _writingChunk;
  delete _readingChunk;

  for(auto p = _simParams.begin(); p != _simParams.end(); p++)
    delete *p;
  _simParams.clear();
  delete _configuration;

  _fc.reset();
}

FileExplorer* FileExplorer::Create(const std::string &file, size_t elemPerChunk, uint64_t version)
{
  auto fc = FileContainer::Create(file, version);

  FileExplorer *res = new FileExplorer(file);
  res->_fc = fc;
  res->_maxElemPerChunk = elemPerChunk;
  res->_writingChunk = new FileExplorer::WritingChunk(fc, elemPerChunk);
  res->_readingChunk = new FileExplorer::ReadingChunk();

  return res;
}

FileExplorer *FileExplorer::Open(const std::string &file, size_t elemPerChunk)
{
  auto fc = FileContainer::Open(file);

  FileExplorer *res = new FileExplorer(file);
  res->_fc = fc;
  res->_writingChunk = new FileExplorer::WritingChunk(fc, elemPerChunk);
  res->_readingChunk = new FileExplorer::ReadingChunk();
  res->_maxElemPerChunk = elemPerChunk;
  res->ReadTable();

  return res;
}

FileExplorer *FileExplorer::Repair(const std::string &file, FrameExtractor fextr, ServiceExtractor sextr,
                                   CellExtractor cextr, size_t elemPerChunk)
{
  auto fc = FileContainer::Repair(file, fextr, sextr, cextr);

  FileExplorer *res = new FileExplorer(file);
  res->_fc = fc;
  res->_writingChunk = new FileExplorer::WritingChunk(fc, elemPerChunk);
  res->_readingChunk = new FileExplorer::ReadingChunk();
  res->_maxElemPerChunk = elemPerChunk;
  res->ReadTable();

  return res;
}

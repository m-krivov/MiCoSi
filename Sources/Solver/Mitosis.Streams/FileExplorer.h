#pragma once
#include "Mitosis.Core/Defs.h"

#include "Mitosis.Formatters/DeSerializer.h"
#include "FileContainer.h"

class FileExplorer
{
  public:
    class ChunkElement
    {
      public:
        ChunkElement(TiXmlElement* elem, void* data, uint64_t sizeInBytes, bool copy = false);
        TiXmlElement* XmlElement()
        { return _elem; }
        void* BinDataPointer()
        { return _data; }
        uint64_t SizeInBytes()
        { return _sizeInBytes; }
        ~ChunkElement();

      private:
        void* _data;
        TiXmlElement* _elem;
        uint64_t _sizeInBytes;
        bool _isCopy;
    };

  private:
    class WritingChunk
    {
      public:
        WritingChunk(std::shared_ptr<FileContainer> fc, size_t MaxElemPerChunk);
        inline size_t CurrentIdx()
        { return _currentIdx; }
        void Flush();
        void Add(TiXmlElement* elem, double time, MemoryStream* stream);
        ChunkElement* GetElement(size_t idx);
        ~WritingChunk();

      private:
        std::shared_ptr<FileContainer> _fc;
        TiXmlDocument* _doc;
        MemoryStream* _stream;
        uint64_t _maxElemPerChunk;
        size_t _currentIdx;
        uint64_t* _offsets;
        uint64_t* _sizes;
        double _time;
        std::vector<ChunkElement*> _elements;
    };

    class ReadingChunk
    {
      public:
        ReadingChunk():
          _doc(0), _offsets(0), _sizes(0), _maxElemPerChunk(0), _currentElemPerChunk(0), _elements(0)
        {}
        void LoadChunk(std::shared_ptr<Chunk> chunk);
        ChunkElement* Read(size_t idx);
        ~ReadingChunk();

      private:
        std::shared_ptr<Chunk> _chunk;
        TiXmlDocument* _doc;
        uint64_t* _offsets;
        uint64_t* _sizes;
        uint64_t _maxElemPerChunk;
        uint64_t _currentElemPerChunk;
        std::vector<ChunkElement*> _elements;
    };

  private:
    std::string _file;
    ChunkElement* _configuration;
    std::vector<ChunkElement*> _simParams;
    std::shared_ptr<FileContainer> _fc;

    WritingChunk* _writingChunk;
    ReadingChunk* _readingChunk;

    size_t _chunkIdx;
    size_t _maxElemPerChunk;
    size_t _currentChunk;

    std::vector<std::pair<size_t, size_t> > _additional;
    std::vector<size_t> _elementsPerChunk;

  private:
    // Creates uninitialized FileExplorer
    FileExplorer(const std::string &file);
    void SaveService(TiXmlElement* elem, MemoryStream* stream);
    ChunkElement* LoadService(size_t idx);
    void ReadTable();

  public:
    // Returns node with version information
    // convert from uint64_t to TiXmlElement?
    uint64_t Version()
    { return _fc->Version(); }

    // Returns node with static cell configuration
    ChunkElement* Configuration()
    { return _configuration; }

    // Returns "TimeLayerNode" + binData + binDataSize + "ActualSimParamsNode"
    // One node with simulation params can be used with multiple time layers
    const std::pair<ChunkElement*, ChunkElement*> TimeLayer(size_t n);

    size_t TimeLayerCount();

    void AppendServiceLayer(TiXmlElement* elem, MemoryStream* stream);

    void AppendFrameLayer(double time, TiXmlElement* elem, MemoryStream* stream);

    void AppendCellConfiguration(TiXmlElement* elem, MemoryStream* stream);

    void Flush();
    // Releases all internal structures
    // Does not save changes
    ~FileExplorer();

    // Creates new empty FileExplorer, that will override existant files
    static FileExplorer *Create(const std::string &file, size_t elemPerChunk, uint64_t version);

    // Opens previously saved results for exploring
    static FileExplorer *Open(const std::string &file, size_t elemPerChunk = 10);

    static FileExplorer *Repair(const std::string &file, FrameExtractor fextr, ServiceExtractor sextr,
                                CellExtractor cextr, size_t elemPerChunk = 10);
};

#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Objects/Cell.h"
#include "Chunk.h"

typedef bool (*FrameExtractor)(const void *metaData, size_t metaDataSize,
                               const void *binData, size_t binDataSize, 
                               Cell &cell, double &time, size_t &layerCount);
typedef bool (*ServiceExtractor)(const void *metaData, size_t metaDataSize,
                                 const void *binData, size_t binDataSize);
typedef bool (*CellExtractor)(const void *metaData, size_t metaDataSize,
                              const void *binData, size_t binDataSize,
                              std::unique_ptr<Cell> &cell);

class FileContainer
{
  public:
    FileContainer() = delete;
    FileContainer(const FileContainer &) = delete;
    FileContainer &operator =(const FileContainer &) = delete;

    // Returns table with all chunks
    const std::vector<ChunkHeader> &Table() const
    { return _table; }

    const uint64_t Version() const
    { return _version; }

    // Loads and returns chunk with required index
    std::shared_ptr<Chunk> LoadChunk(size_t chunkNumber);

    // Immediately appends and writes service chunk. Table will be rewritten by destructor
    void AppendServiceChunk(void *metaData, size_t metaDataSize, void *binData, size_t binDataSize)
    { AppendChunk(ChunkType::Service, 0.0, 1, metaData, metaDataSize, binData, binDataSize); }

    // Immediately appends and writes frame chunk. Table will be rewritten by destructor
    inline void AppendFrameChunk(double time, size_t count, void *metaData,
                                 size_t metaDataSize, void *binData, size_t binDataSize)
    { AppendChunk(ChunkType::Frames, time, count, metaData, metaDataSize, binData, binDataSize); }

    ~FileContainer();

    // Tries to open file
    // Throws exception in case of any error
    static std::shared_ptr<FileContainer> Open(const std::string &filename);

    // Tries to repair broken file (rescans chunks and builds table)
    // Can throw exception
    static std::shared_ptr<FileContainer> Repair(const std::string &filename, 
                                                 FrameExtractor fextr,
                                                 ServiceExtractor sextr,
                                                 CellExtractor cextr);

    // Creates new (empty) file or overwrites existent
    // Can throw exception
    static std::shared_ptr<FileContainer> Create(const std::string &filename, uint64_t version);

  private:
    // Class that formats data
    // All methods return size of the processed block (count of read/written bytes)
    class Formatter
    {
      public:
        static size_t WriteHeader(FILE *f, uint64_t version);
        static size_t WriteChunk(FILE *f, ChunkType::Type type, double time,
                                 void *metaData, size_t metaDataSize,
                                 void *binData, size_t binDataSize);
        static size_t WriteTable(FILE *f, std::vector<ChunkHeader> table);
        static void UpdateHeader(FILE *f, offset_t tablePosition);
        static size_t ReadHeader(FILE *f, uint64_t &version, offset_t &tableOffset);
        static size_t ScanChunk(FILE *f, ChunkType::Type &type, size_t &metaDataSize, size_t &binDataSize);
        static size_t LoadChunk(FILE *f, void *metaData, size_t metaDataSize, void *binData, size_t binDataSize);
        static size_t ReadTable(FILE *f, std::vector<ChunkHeader> &table);
    };

    FileContainer(FILE *file, offset_t newChunkStart,
                  const std::vector<ChunkHeader> &table,
                  uint64_t version, bool rewriteTable)
      : _file(file), _newChunkStart(newChunkStart),
        _table(table), _version(version), _rewriteTable(rewriteTable)
    { /*nothing*/ }

    static FILE *OpenFile(const std::string &filename, const char *opt);

    static bool ValidateChunk(FILE *f, uint64_t &newChunkStart, std::vector<ChunkHeader> &table,
                              std::unique_ptr<Cell> &cell,
                              FrameExtractor fextr, ServiceExtractor sextr, CellExtractor cextr);

    void AppendChunk(ChunkType::Type type, double time, size_t count,
                     void *metaData, size_t metaDataSize,
                     void *binData, size_t binDataSize);

    FILE *_file;
    offset_t _newChunkStart;
    std::vector<ChunkHeader> _table;
    bool _rewriteTable;
    uint64_t _version;
};

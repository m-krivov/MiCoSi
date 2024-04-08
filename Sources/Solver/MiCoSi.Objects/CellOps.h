#pragma once
#include "MiCoSi.Core/Defs.h"

#include "Cell.h"

// The collection of utilities that count some properties of the cell
// The main idea of class is to separate more complex logics from containers
class CellOps
{
  public:
    CellOps() = delete;
    CellOps(const CellOps &) = delete;
    CellOps(const Cell *cell);
    CellOps &operator =(const CellOps &) = delete;

    // Counts the number of MTs that are bound to kinetochores
    // Enumeration corresponds to the vector with chromosomes
    // Note: this operation is not fast, do not abuse it
    std::vector<size_t> CountKMTs() const;

    // Version that counts KMTs for single chromosome
    // Note: this operation is not fast, do not abuse it
    size_t CountKMTs(const Chromosome *chr) const;

    // Extracts MT objects that are bound to kinetochores
    // Enumeration corresponds to the vector with chromosomes
    // Note: this operation is not fast, do not abuse it
    std::vector<std::vector<MT *> > ExtractKMTs() const;

    // Version that extracts KMTs for single chromosome
    // Note: this operation is not fast, do not abuse it
    std::vector<MT *> ExtractKMTs(const Chromosome *chr) const;

  private:
    const Cell *_cell;
};

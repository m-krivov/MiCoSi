
#pragma once

#include "tinyxml.h"
#include "Mitosis.Objects/Interfaces.h"

// Reads parameters from SimParams and initializes cell using initial states, stored in XML.
class XmlCellInitializer : public ICellInitializer
{
	public:
		XmlCellInitializer(const char *xmlFile);

		// ICellInitializer member.
		virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole) override;

		// ICellInitializer member
		virtual void InitializeCell(ICell *cell, Random::State &) override;

		virtual IClonnable *Clone() const override
		{ return new XmlCellInitializer(_filename.c_str()); }

		virtual ~XmlCellInitializer();

  private:
		std::string _filename;
		TiXmlDocument *_doc;
		TiXmlElement *_MTs;
		TiXmlElement *_chrs;
};


#pragma once

#include "tinyxml.h"
#include "Mitosis.Objects/Interfaces.h"

// Reads parameters from SimParams and initializes cell using initial states, stored in XML.
class XmlCellInitializer : public ICellInitializer
{
	protected:
		std::string _filename;
		TiXmlDocument *_doc;
		TiXmlElement *_MTs;
		TiXmlElement *_chrs;

	public:
		XmlCellInitializer(const char *xmlFile);

		// ICellInitializer member.
		virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole);

		// ICellInitializer member.
		virtual void InitializeCell(ICell *cell, uint32_t &seed);

		virtual IClonnable *Clone()
		{ return new XmlCellInitializer(_filename.c_str()); }

		virtual ~XmlCellInitializer();
};

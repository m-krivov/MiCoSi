
#include "ttgUtils.h"
#ifdef WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include <sstream>

using namespace ttgUtils;

#ifdef TTG_UTILS

FILE *Files::_timeStamps = NULL;
FILE *Files::_dumps = NULL;
std::string Files::_dumpDir;
bool Files::_isGoldMode = false;

void Files::Init(const char *timeStamps, const char *dumps, const char *dumpDir)
{
	if (_timeStamps == NULL)
	{
		std::string tsFilename = std::string(timeStamps) + ".txt";
		if ((_timeStamps = fopen(tsFilename.c_str(), "w")) == NULL)
			throw std::runtime_error("Cannot create file for time stamps");
	}

	if (_dumps == NULL)
	{
		std::string dsFilename = std::string(dumps) + ".txt";
		if ((_dumps = fopen(dsFilename.c_str(), "w")) == NULL)
			throw std::runtime_error("Cannot create file for time stamps");
	}

#ifdef WIN32
	CreateDirectoryA(dumpDir, NULL);

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA((std::string(dumpDir) + std::string("\\*.dump")).c_str(), &FindFileData);
	_isGoldMode = hFind == INVALID_HANDLE_VALUE;
#else
	FILE *dumpF = popen((std::string("mkdir -p ") + std::string(dumpDir)).c_str(), "rt");
	if (dumpF)
		pclose(dumpF);
	//ACHTUNG_ACHTUNG_ACHTUNG_ACHTUNG();
#endif
	_dumpDir = dumpDir;
}

void Files::Init(int procNum, const char *timeStamps, const char *dumps, const char *dumpDir)
{
	std::stringstream newStamps;
	newStamps << timeStamps << "_proc" << procNum;

	std::stringstream newDumps;
	newDumps << dumps << "_proc" << procNum;

	std::stringstream newDumpDir;
	newDumpDir << dumpDir << "_proc" << procNum;

	Init(newStamps.str().c_str(), newDumps.str().c_str(), newDumpDir.str().c_str());
}

void Files::Close()
{
#ifdef TTG_UTILS_TIME_STAMPS
	TimeStamps::PrintStatistics();
#endif

	if (_timeStamps != NULL)
		fclose(_timeStamps);
	if (_dumps != NULL)
		fclose(_dumps);
}


#ifdef TTG_UTILS_DUMPS

//----------------------
//--- SubIdentifiers ---
//----------------------

std::map<std::string, size_t> SubIdentifiers::_refs;

size_t SubIdentifiers::GenerateSubID(std::string stringID)
{
	std::map<std::string, size_t>::iterator it = _refs.find(stringID);

	if (it == _refs.end())
	{
		_refs[stringID] = 0;
		return 0;
	}
	else
	{
		it->second += 1;
		return it->second;
	}
}

size_t SubIdentifiers::GenerateSubID(const char *file, int line)
{
	std::stringstream ss;
	ss << file << " : " << line;
	return GenerateSubID(ss.str());
}

//-------------
//--- Dumps ---
//-------------

std::string Dumps::GenerateFilename(const char *name, size_t id)
{
	char buf[1024];
	sprintf_s(buf, "%s\\%s_%d.dump", Files::GetDumpDir(), name, id);
	return buf;
}

std::string Dumps::GenerateTitle(const char *name, size_t id)
{
	char buf[1024];
	sprintf_s(buf, "Dump \"%s\" (check #%d)", name, id);
	return buf;
}

void Dumps::StoreDump(const void * const arr, size_t sizeInBytes, const char *fileName)
{
	FILE *f = fopen(fileName, "wb");
	if (f == NULL)
		throw std::runtime_error("Cannot create DUMP-file");
#ifdef WIN32
	_setmode(_fileno(f), _O_BINARY);
#endif

	const char * const bArr = (const char * const)arr;
	fprintf(f, "%ld #", sizeInBytes);
	for (size_t i = 0; i < sizeInBytes; i++)
		fputc(bArr[i], f);

	fclose(f);
}

void Dumps::LoadDump(void *&arr, size_t &sizeInBytes, const char *fileName)
{
	FILE *f = fopen(fileName, "rb");
	if (f == NULL)
		throw std::runtime_error("Cannot open DUMP-file");
#ifdef WIN32
	_setmode(_fileno(f), _O_BINARY);
#endif

	fscanf(f, "%ld #", &sizeInBytes);
	if (sizeInBytes == 0 || sizeInBytes > 1024 * 1024 * 1024)
	{
		fclose(f);
		throw std::runtime_error("Broken DUMP-file");
	}

	size_t i = 0;
	char *bArr = new char[sizeInBytes];
	int c;
	while (i < sizeInBytes && (c = fgetc(f)) != EOF)
	{
		bArr[i++] = (char)c;
	}
	if (i != sizeInBytes)
	{
		fclose(f);
		delete[] bArr;
		throw std::runtime_error("Broken DUMP-file");
	}

	arr = bArr;
	fclose(f);
}

void Dumps::MakeRecord(const char *title, const char *message)
{
	fprintf(Files::GetDumpFile(), "%s: %s\n", title, message);
	fflush(Files::GetDumpFile());
}

#endif

#ifdef TTG_UTILS_TIME_STAMPS

//------------------
//--- Timestamps ---
//------------------

std::vector<std::pair<std::string, double> > TimeStamps::trace;
std::map<std::string, double> TimeStamps::sums;
std::map<std::string, int> TimeStamps::counts;

void TimeStamps::printOffset(size_t size)
{
	for (size_t i = 0; i < size; i++)
		fprintf_s(Files::GetTimeStampFile(), "   ");
}

void TimeStamps::PrintStatistics()
{
	fprintf(Files::GetTimeStampFile(), "\nSummed by alphabet:\n");
	std::map<double, std::string> tmap;

	for (std::map<std::string, double>::iterator it = sums.begin(); it != sums.end(); it++)
	{
		fprintf(Files::GetTimeStampFile(), "Summed %s: %g seconds (%d fragments)\n", it->first.c_str(), it->second, counts[it->first]);
		tmap.insert(make_pair(it->second, it->first));
	}
	fprintf(Files::GetTimeStampFile(), "\nSummed by value:\n");
	for (std::map<double, std::string>::iterator it = tmap.begin(); it != tmap.end(); it++)
		fprintf(Files::GetTimeStampFile(), "Summed %s: %g seconds (%d fragments)\n", it->second.c_str(), it->first, counts[it->second]);
}

void TimeStamps::AddStamp(const char *name)
{
	std::string strName(name);
	if (trace.size() != 0 && trace[trace.size() - 1].first == strName)
	{
		double result = GetSeconds() - trace[trace.size() - 1].second;
		sums[strName] += result;
		counts[strName]++;
		//printOffset(trace.size() - 1);
		//fprintf(f, "%s: %g seconds\n", name, result);
		//fflush(Files::GetTimeStampFile());
		trace.pop_back();
	}
	else
	{
		trace.push_back(std::make_pair(std::string(name), GetSeconds()));
	}
}

#endif

#endif

/*------------------------------------------------------------------------*/ 
/*--- This file was generated automatically - all changes will be lost ---*/ 
/*------------------------------------------------------------------------*/ 
#define TTG_UTILS_BUILDED 

#ifndef _TTG_UTILS_DEFINITIONS_
#define _TTG_UTILS_DEFINITIONS_

//Turning on ttgUtils.
//#define TTG_UTILS

//Allows to measure times and generate time-report.
//#define TTG_UTILS_TIME_STAMPS
#define TTG_UTILS_TIME_STAMPS_FILE "PerformanceReport"

//Allows to create dumps in order to compare correctness of the results.
//#define TTG_UTILS_DUMPS
#define TTG_UTILS_DUMPS_DIR "Dumps"
#define TTG_UTILS_DUMPS_FILE "CorrectnessReport"

//Request for CUDA support.
//#define TTG_UTILS_CUDA_SUPPORT









#ifdef WIN32
#include <windows.h>
#undef min
#undef max
#else
#include <sys/time.h>
#endif
#include <cstdlib>

namespace ttgUtils
{
	//Returns current time in abstract seconds. It uses High Precision timer.
	inline double GetSeconds()
	{
	#ifdef WIN32
		static double coeff = -1.0;
		static LARGE_INTEGER count;
		if (coeff <= 0.0)
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			coeff = 1.0 / freq.QuadPart;
		}
		QueryPerformanceCounter(&count);
		return coeff * count.QuadPart;
	#else
		timeval t;
		gettimeofday(&t, NULL);
		return t.tv_sec + (double)t.tv_usec / 1e6;
	#endif
	}
}

#ifdef TTG_UTILS

#include <stdio.h>
#include <string>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef abs
#undef abs
#endif

namespace ttgUtils
{
	class Files
	{
		private:
			static FILE *_timeStamps;
			static FILE *_dumps;
			static std::string _dumpDir;
			static bool _isGoldMode;

		public:
			static void Init(const char *timeStamps, const char *dumps, const char *dumpDir);

			static void Init(int procNum, const char *timeStamps, const char *dumps, const char *dumpDir);

			static void Close();

			static inline bool IsGoldMode()
			{ return _isGoldMode; }

			static inline FILE *GetTimeStampFile()
			{ return _timeStamps; }

			static inline FILE *GetDumpFile()
			{ return _dumps; }

			static inline const char *GetDumpDir()
			{ return _dumpDir.c_str(); }
	};
}

#define ttg_init() { ttgUtils::Files::Init(TTG_UTILS_TIME_STAMPS_FILE, TTG_UTILS_DUMPS_FILE, TTG_UTILS_DUMPS_DIR); }
#define ttg_init_mpi(procNum) { ttgUtils::Files::Init(procNum, TTG_UTILS_TIME_STAMPS_FILE, TTG_UTILS_DUMPS_FILE, TTG_UTILS_DUMPS_DIR); }
#define ttg_close() { ttgUtils::Files::Close(); }

#else

#define ttg_init() void();
#define ttg_init_mpi(procNum) void();
#define ttg_close() void();

#endif

#endif

#ifndef _TTG_UTILS_TIME_STAMPS_
#define _TTG_UTILS_TIME_STAMPS_

#ifndef TTG_UTILS_BUILDED
#include "Definitions.h"
#endif

#if defined(TTG_UTILS_TIME_STAMPS) && defined(TTG_UTILS)

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#ifndef WIN32
#define fprintf_s fprintf
#endif

namespace ttgUtils
{
	//Keeps all time stamps.
	class TimeStamps
	{
		private:
			static std::vector<std::pair<std::string, double> > trace;
			static std::map<std::string, double> sums;
			static std::map<std::string, int> counts;
		private:
			static void printOffset(size_t size);

		public:
			static void AddStamp(const char *name);
			static void PrintStatistics();
	};
}


#define ttg_time_stamp(name) { ttgUtils::TimeStamps::AddStamp(name); }

#ifdef TTG_UTILS_CUDA_SUPPORT
#define ttg_cuda_time_stamp(name) { cudaThreadSynchronize(); ttgUtils::TimeStamps::AddStamp(name); }
#else
#define ttg_cuda_time_stamp(name) void();
#endif

#else

#define ttg_time_stamp(name) void();
#define ttg_cuda_time_stamp(name) void();
#define ttg_close_time_stamps() void();

#endif

#endif

#ifndef _TTG_UTILS_DUMPS_
#define _TTG_UTILS_DUMPS_

#ifndef TTG_UTILS_BUILDED
#include "Definitions.h"
#endif

#if defined(TTG_UTILS_DUMPS) && defined(TTG_UTILS)

#include <map>
#include <string>
#include <cmath>
#include <stdexcept>

#ifndef WIN32
#define sprintf_s sprintf
#endif

namespace ttgUtils
{
	class SubIdentifiers
	{
		private:
			static std::map<std::string, size_t> _refs;

		public:
			static size_t GenerateSubID(std::string stringID);

			static size_t GenerateSubID(const char *file, int line);
	};

	class Dumps
	{
		private:
			static std::string GenerateFilename(const char *name, size_t id);

			static std::string GenerateTitle(const char *name, size_t id);

			static void StoreDump(const void * const arr, size_t sizeInBytes, const char *fileName);

			static void LoadDump(void *&arr, size_t &sizeInBytes, const char *fileName);

			static void MakeRecord(const char *title, const char *message);

			//"arr2" is a gold array.
			template <class T>
			static void CheckDumpAndMakeRecord(const T * const arr1, size_t size1,
											   const T * const arr2, size_t size2,
											   const char *title)
			{
				if (size1 != size2)
				{
					MakeRecord(title, "Size does not correspond to size of the gold array");
				}
				else
				{

					T cNorm_delta = 0.0, cNorm_arr1 = 0.0, cNorm_arr2 = 0.0;
					T l2Norm_delta = 0.0, l2Norm_arr1 = 0.0, l2Norm_arr2 = 0.0;
					for (size_t i = 0; i < size1; i++)
					{
						T delta = arr1[i] - arr2[i];

						cNorm_delta = std::max(cNorm_delta, (T)std::abs(delta));
						cNorm_arr1 = std::max(cNorm_arr1, (T)std::abs(arr1[i]));
						cNorm_arr2 = std::max(cNorm_arr2, (T)std::abs(arr2[i]));

						l2Norm_delta += delta * delta;
						l2Norm_arr1 += arr1[i] * arr1[i];
						l2Norm_arr2 += arr2[i] * arr2[i];
					}
					l2Norm_delta = std::sqrt(l2Norm_delta / size1);
					l2Norm_arr1 = std::sqrt(l2Norm_arr1 / size1);
					l2Norm_arr2 = std::sqrt(l2Norm_arr2 / size1);

					double cNorm_av = cNorm_arr2 == 0.0 ? cNorm_delta * 1e-3 : cNorm_arr2;
					double l2Norm_av = l2Norm_arr2 == 0.0 ? l2Norm_delta * 1e-3 : l2Norm_arr2;

					char buf[1024];
					sprintf_s(buf, "C == %le (%2.2lf%%), L2 == %le (%2.2lf%%)",
								   cNorm_delta, (cNorm_av == 0.0 ? 0.0 : 100.0 * cNorm_delta / cNorm_av),
								   l2Norm_delta, (l2Norm_av == 0.0 ? 0.0 : 100.0 * l2Norm_delta / l2Norm_av));
					MakeRecord(title, buf);
				}
			}

		public:
			template <class T>
			static void MakeArrayDump(const T * const arr, size_t size, const char *name, size_t id)
			{
				if (Files::IsGoldMode())
				{
					StoreDump(arr, size * sizeof(T), GenerateFilename(name, id).c_str());
					MakeRecord(GenerateTitle(name, id).c_str(), "Gold dump created");
				}
				else
				{
					try
					{
						T *gold_arr = NULL;
						size_t gold_arr_size = 0;
						LoadDump((void *&)gold_arr, gold_arr_size, GenerateFilename(name, id).c_str());
						CheckDumpAndMakeRecord<T>(arr, size, gold_arr, gold_arr_size / sizeof(T), GenerateTitle(name, id).c_str());
						if (gold_arr != NULL)
							delete[] gold_arr;
					}
					catch (std::runtime_error &err)
					{ MakeRecord(GenerateTitle(name, id).c_str(), "Cannot find gold dump"); }
				}
			}
	};
}

#define ttg_array_dump(name, arr, size) { ttgUtils::Dumps::MakeArrayDump(arr, size, name, ttgUtils::SubIdentifiers::GenerateSubID(__FILE__, __LINE__)); }
#define ttg_value_dump(name, value) { ttgUtils::Dumps::MakeArrayDump(&value, 1, name, ttgUtils::SubIdentifiers::GenerateSubID(__FILE__, __LINE__)); }

#else

#define ttg_array_dump(name, arr, size) void();
#define ttg_value_dump(name, value) void();

#endif

#endif

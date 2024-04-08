#pragma once

#pragma warning( disable: 26812 )
#pragma warning( disable: 26495 )

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <functional>
#include <fstream>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include <stdint.h>

// Uncomment to disable CUDA support
#define NO_CUDA


#ifdef MICOSI_PRECISION_FP64
typedef double real;
#else
typedef float real;
#endif

#if defined(WIN32_) && !defined(WIN32)
#define WIN32
#endif

#define PI (3.14159265358979323846264338327950288419716939937510)

typedef int64_t offset_t;

#if !defined(NO_CUDA)

#include <cuda_runtime_api.h>

static inline void CheckErrorImpl(cudaError_t err, const char *file, int line)
{
  if (err != cudaSuccess)
  {
    std::stringstream ss;
    ss << file << ": " << line << ", ";
    ss << "CUDA error #" << (int)err << " detected - " << cudaGetErrorString(err);
    throw std::runtime_error(ss.str());
  }
}

#define CheckError(errCode) CheckErrorImpl(errCode, __FILE__, __LINE__)

#endif




#pragma once

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include <stdint.h>

// Uncomment this line if you want to work with double precision
//#define DOUBLE_PRECISION

// Uncomment to disable CUDA support.
#define NO_CUDA


#ifdef DOUBLE_PRECISION
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



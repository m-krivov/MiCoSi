
#pragma once

#include "Defs.h"

// Static class that generates pseudo-random numbers.
// CUDA-compatible.
class Random
{
  public:
    class Func
    {
      public:
#ifdef __CUDACC__
        __host__ __device__
#endif
        static inline uint32_t TypeA(uint32_t &seed)
        {
          seed = 214013 * seed + 2531011;
          return seed >> 16 & 0x7FFF;
        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        static inline uint32_t TypeB(uint32_t &seed)
        {
          seed = 214013 * seed + 2531011;
          return (~(seed >> 16)) & 0x7FFF;
        }

#ifdef __CUDACC__
        __host__ __device__
#endif
        static inline uint32_t MaxRandom()
        { return 0x7FFF; }
    };

  private:
    Random() = delete;
    Random(const Random &) = delete;
    void operator =(const Random &) = delete;

  public:
    // Returns pseudo-random not negative integer.
    static inline int Next(uint32_t &seed)
    { return (int)Func::TypeA(seed); }

    // Returns pseudo-random integer.
    static inline int Next(uint32_t &seed, int lo, int hi)
    { return Next(seed) * (hi - lo) + lo; }

    // Returns pseudo-random float number from the range [0.0, 1.0].
    static inline real NextReal(uint32_t &seed)
    { return (real)Func::TypeA(seed) / Func::MaxRandom(); }

    // Returns pseudo-random float number from the range [lo, hi].
    static inline real NextReal(uint32_t &seed, real lo, real hi)
    { return NextReal(seed) * (hi - lo) + lo; }

    // Creates a new seed using current time.
    static uint32_t CreateSeed();
};

#pragma once
#include "Defs.h"

// Our built-in linear congruential generator that may be used in CUDA-kernels
// in order to provide the identical results
class CongruentialGenerator
{
  public:
    typedef uint32_t State;

    CongruentialGenerator() = default;
    CongruentialGenerator(const CongruentialGenerator &) = delete;
    CongruentialGenerator &operator =(const CongruentialGenerator &) = delete;

    void Initialize(State &state) const;

    void Initialize(State &state, uint32_t seed) const;

    void Multiply(State &oldState, std::vector<State> &newStates) const;

    std::string Serialize(const State &state) const;

    void Deserialize(std::string serialized, State &state) const;

    uint32_t Next(State &state) const;

    uint32_t Max() const;
};

// Wrapper for std::mt19937 random engine
class MersenneTwisterGenerator
{
  public:
    typedef std::mt19937 State;

    MersenneTwisterGenerator() = default;
    MersenneTwisterGenerator(const MersenneTwisterGenerator &) = delete;
    MersenneTwisterGenerator &operator =(const MersenneTwisterGenerator &) = delete;

    void Initialize(State &state) const;

    void Initialize(State &state, uint32_t seed) const;

    void Multiply(State &oldState, std::vector<State> &newStates) const;

    std::string Serialize(const State &state) const;

    void Deserialize(std::string serialized, State &state) const;

    uint32_t Next(State &seed) const;

    uint32_t Max() const;
};

// Extends our simple RNG and provides some useful methods
class Random
{
  public:
#if defined(MICOSI_RNG_LCG)
    typedef CongruentialGenerator Generator;
#else
    typedef MersenneTwisterGenerator Generator;
#endif
    typedef Generator::State State;

    Random() = delete;
    Random(const Random &) = delete;
    Random &operator =(const Random &) = delete;

    // Returns uniformly distributed pseudo-random integer
    static uint32_t Next(State &state) { return gen_.Next(state); }

    // Returns uniformly distributed pseudo-random float number from the range [0.0, 1.0]
    static inline real NextReal(State &state)
    { return (real)gen_.Next(state) / gen_.Max(); }

    // Returns uniformly distributed pseudo-random float number from the range [lo, hi]
    static inline real NextReal(State &state, real lo, real hi)
    { return NextReal(state) * (hi - lo) + lo; }

    // Creates a new state that is based on the current time
    static void Initialize(State &state)
    { gen_.Initialize(state); }

    // Deterministically creates a new state using some seed
    static void Initialize(State &state, uint32_t seed)
    { gen_.Initialize(state, seed); }

    // Deterministically creates new states that are based on the given one
    // May be useful to split some global RNG between parallel threads
    static void Multiply(State &oldState, std::vector<State> &newStates)
    { gen_.Multiply(oldState, newStates); }

    // Stores some state using string-based format
    static std::string Serialize(const State &state)
    { return gen_.Serialize(state); }

    // Restores some state from the string
    // If format is wrong, throws exception
    static void Deserialize(std::string serialized, State &state)
    { gen_.Deserialize(serialized, state); }

  private:
    static Generator gen_;
};


#include "Random.h"
#include "Timer.h"

namespace
{

uint32_t CreateTimeBasedSeed()
{
  double t = Timer::GetRef()->GetMilliSeconds();

  t += (t - std::round(t)) * 1e6;
  if (t < 1.0)
  { t = 1.0 / (t + 1e-6); }

  return (uint32_t)t;
}

template <class STATE>
void MultiplyState(const std::function<uint32_t()> &next,
                   const std::function<void(STATE &, uint32_t)> &applySeed,
                   std::vector<STATE> &newStates)
{
  std::set<uint32_t> generated;
  int attempts = 1024;
  for (size_t i = 0; i < newStates.size(); i++)
  {
    auto nxt = next();
    while (generated.find(nxt) != generated.end() && --attempts > 0)
    { nxt = next(); }
    
    if (attempts <= 0)
    { throw std::runtime_error("our RNG is not designed for such large-scale use"); }
    
    generated.insert(nxt);
    applySeed(newStates[i], nxt);
  }
}

} // unnamed namespace

//-----------------------------
//--- CongruentialGenerator ---
//-----------------------------

void CongruentialGenerator::Initialize(CongruentialGenerator::State &state) const
{
  std::random_device rd;
  state = rd() ^ CreateTimeBasedSeed();
}

void CongruentialGenerator::Initialize(State &state, uint32_t seed) const
{
  state = seed;
}

void CongruentialGenerator::Multiply(CongruentialGenerator::State &oldState,
                                     std::vector<CongruentialGenerator::State> &newStates) const
{
  std::function<uint32_t()> next
    = [&oldState]() -> uint32_t
      { return oldState = 1664525 * oldState + 1013904223; };
  std::function<void(CongruentialGenerator::State &, uint32_t)> applySeed
    = [](CongruentialGenerator::State &state, uint32_t val) -> void
      { state = (CongruentialGenerator::State)val; };

  MultiplyState<CongruentialGenerator::State>(next, applySeed, newStates);
}

std::string CongruentialGenerator::Serialize(const State &state) const
{
  std::ostringstream ss;
  ss << state;
  return ss.str();
}

void CongruentialGenerator::Deserialize(std::string serialized, State &state) const
{
  std::istringstream ss(serialized);
  if (!(ss >> state) || ss.bad() || !ss.eof())
  { throw std::runtime_error("failed to deserialize RNG state"); }
}

uint32_t CongruentialGenerator::Next(CongruentialGenerator::State &state) const
{
  state = 214013 * state + 2531011;
  return (state >> 16) & 0x7FFF;
}

uint32_t CongruentialGenerator::Max() const
{
  return 0x7FFF;
}

//--------------------------------
//--- MersenneTwisterGenerator ---
//--------------------------------

void MersenneTwisterGenerator::Initialize(State &state) const
{
  std::random_device rd;
  state.seed(rd() ^ CreateTimeBasedSeed());
}

void MersenneTwisterGenerator::Initialize(State &state, uint32_t seed) const
{
  state.seed(seed);
}

void MersenneTwisterGenerator::Multiply(MersenneTwisterGenerator::State &oldState,
                                        std::vector<MersenneTwisterGenerator::State> &newStates) const
{
  std::function<uint32_t()> next
    = [&oldState]() -> uint32_t
      { return oldState(); };
  std::function<void(MersenneTwisterGenerator::State &, uint32_t)> applySeed
    = [](MersenneTwisterGenerator::State &seed, uint32_t val) -> void
      { seed.seed(~val); };

  MultiplyState<MersenneTwisterGenerator::State>(next, applySeed, newStates);
}

std::string MersenneTwisterGenerator::Serialize(const MersenneTwisterGenerator::State &state) const
{
  std::ostringstream ss;
  ss << state;
  return ss.str();
}

void MersenneTwisterGenerator::Deserialize(std::string serialized, MersenneTwisterGenerator::State &state) const
{
  std::istringstream ss(serialized);
  if (!(ss >> state) || ss.bad())
  { throw std::runtime_error("failed to deserialize RNG state"); }
}

uint32_t MersenneTwisterGenerator::Next(MersenneTwisterGenerator::State &seed) const
{
  return seed();
}

uint32_t MersenneTwisterGenerator::Max() const
{
  return State::max();
}

//--------------
//--- Random ---
//--------------

Random::Generator Random::gen_;

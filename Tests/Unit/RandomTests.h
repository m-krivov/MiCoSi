#pragma once
#include "Defs.h"

#include "Mitosis.Core/Random.h"

TEST(Random, NewStates)
{
  {
    CongruentialGenerator gen;
    CongruentialGenerator::State state1, state2;
    gen.Initialize(state1);
    gen.Initialize(state2);
    ASSERT_NE(state1, state2);
  }
  {
    MersenneTwisterGenerator gen;
    MersenneTwisterGenerator::State state1, state2;
    gen.Initialize(state1);
    gen.Initialize(state2);
    ASSERT_NE(state1, state2);
  }
}

TEST(Random, MaxIsBigEnough)
{
  {
    CongruentialGenerator gen;
    ASSERT_GE(gen.Max(), 0x7FFFu);
  }
  {
    MersenneTwisterGenerator gen;
    ASSERT_GE(gen.Max(), 0x7FFFu);
  }
}

#define MultiplyStateTestBody()                             \
{                                                           \
  const size_t N = 32;                                      \
  Generator gen;                                            \
  Generator::State state1, state2, state3;                  \
  gen.Initialize(state1);                                   \
  state2 = state1;                                          \
  gen.Initialize(state3);                                   \
                                                            \
  std::vector<Generator::State> v1(N), v2(N), v3(N);        \
  gen.Multiply(state1, v1);                                 \
  ASSERT_EQ(v1.size(), N);                                  \
  gen.Multiply(state2, v2);                                 \
  gen.Multiply(state3, v3);                                 \
  ASSERT_EQ(v1, v2);                                        \
  ASSERT_NE(v1, v3);                                        \
                                                            \
  std::set<std::string> rnd;                                \
  for (auto &s : v1) { rnd.insert(gen.Serialize(s)); }      \
  ASSERT_EQ(rnd.size(), v1.size());                         \
}

TEST(Random, MultiplyState_Lcg)
{
  using Generator = CongruentialGenerator;
  MultiplyStateTestBody();
}

TEST(Random, MultiplyState_Mtg)
{
  using Generator = MersenneTwisterGenerator;
  MultiplyStateTestBody();
}

#define ReproducibilityTestBody()           \
{                                           \
  Generator gen;                            \
  Generator::State state;                   \
  gen.Initialize(state);                    \
  auto tmp = state;                         \
  std::array<uint32_t, 128> arr1, arr2;     \
                                            \
  for (auto &val : arr1)                    \
  {                                         \
    val = gen.Next(tmp);                    \
    ASSERT_NE(tmp, state);                  \
  }                                         \
  tmp = state;                              \
  for (auto &val : arr2)                    \
  { val = gen.Next(tmp); }                  \
                                            \
  ASSERT_EQ(arr1, arr2);                    \
}

TEST(Random, Reproducibility_Lcg)
{
  using Generator = CongruentialGenerator;
  ReproducibilityTestBody();
}

TEST(Random, Reproducibility_Mtg)
{
  using Generator = MersenneTwisterGenerator;
  ReproducibilityTestBody();
}

#define HistogramsTestBody()                                  \
{                                                             \
  const size_t N = 0xFFFF;                                    \
  Generator gen;                                              \
  Generator::State state;                                     \
  gen.Initialize(state);                                      \
                                                              \
  std::array<size_t, 8> hist{ 0, 0, 0, 0, 0, 0, 0, 0 };       \
  for (size_t i = 0; i < N; i++)                              \
  { hist[gen.Next(state) % hist.size()] += 1; }               \
                                                              \
  auto min = *std::min_element(hist.begin(), hist.end());     \
  auto max = *std::max_element(hist.begin(), hist.end());     \
  auto av = N / hist.size();                                  \
  ASSERT_LE((double)(max - min), av * 0.2);                   \
  ASSERT_GE(max - av, 5);                                     \
  ASSERT_GE(av - min, 5);                                     \
}

TEST(Random, Histograms_Lcg)
{
  using Generator = CongruentialGenerator;
  HistogramsTestBody();
}

TEST(Random, Histograms_Mtg)
{
  using Generator = MersenneTwisterGenerator;
  HistogramsTestBody();
}

#define MidPointTestBody()                                  \
{                                                           \
  const int N = 4 * 1024;                                   \
  Generator gen;                                            \
  Generator::State state;                                   \
  gen.Initialize(state, 100500);                            \
  int mid = (int)(gen.Max() / 2);                           \
                                                            \
  int lower = 0, upper = 0;                                 \
  for (int i = 0; i < N; i++)                               \
  {                                                         \
    if (gen.Next(state) >= (uint32_t)mid) { lower += 1; }   \
    else { upper += 1; }                                    \
  }                                                         \
                                                            \
  ASSERT_LE(std::abs(lower - upper), N / 20);               \
  ASSERT_GE(std::abs(lower - N / 2), 3);                    \
  ASSERT_GE(std::abs(upper - N / 2), 3);                    \
}

TEST(Random, MidPoint_Lcg)
{
  using Generator = CongruentialGenerator;
  MidPointTestBody();
}

TEST(Random, MidPoint_Mtg)
{
  using Generator = MersenneTwisterGenerator;
  MidPointTestBody();
}

#define GoodSerializationTestBody()                         \
{                                                           \
  Generator gen;                                            \
  Generator::State state;                                   \
  gen.Initialize(state);                                    \
  for (size_t i = 0; i < 5; i++) { gen.Next(state); }       \
                                                            \
  auto serialized = gen.Serialize(state);                   \
  for (size_t i = 0; i < 5; i++) { gen.Next(state); }       \
  auto val1 = gen.Next(state);                              \
                                                            \
  gen.Deserialize(serialized, state);                       \
  for (size_t i = 0; i < 5; i++) { gen.Next(state); }       \
  auto val2 = gen.Next(state);                              \
                                                            \
  ASSERT_EQ(val1, val2);                                    \
  auto serializedAgain = gen.Serialize(state);              \
  ASSERT_NE(serialized, serializedAgain);                   \
}

TEST(Random, GoodSerialization_Lcg)
{
  using Generator = CongruentialGenerator;
  GoodSerializationTestBody();
}

TEST(Random, GoodSerialization_Mtg)
{
  using Generator = MersenneTwisterGenerator;
  GoodSerializationTestBody();
}

#define BadSerializationTestBody()                                                  \
{                                                                                   \
  Generator gen;                                                                    \
  Generator::State state;                                                           \
  gen.Initialize(state);                                                            \
  auto serialized = gen.Serialize(state);                                           \
  ASSERT_GE(serialized.size(), 3u);                                                 \
  {                                                                                 \
    bool error = false;                                                             \
    Generator::State newState;                                                      \
    try                                                                             \
    { gen.Deserialize(serialized.substr(2u, serialized.size() - 2), newState); }    \
    catch (std::exception &)                                                        \
    { error = true; }                                                               \
                                                                                    \
    if (!error) { error = state != newState; }                                      \
    ASSERT_TRUE(error);                                                             \
  }                                                                                 \
  {                                                                                 \
    bool error = false;                                                             \
    try { gen.Deserialize("a-a-a-a-a-" + serialized, state); }                      \
    catch (std::exception &) { error = true; }                                      \
    ASSERT_TRUE(error);                                                             \
  }                                                                                 \
  {                                                                                 \
    bool error = false;                                                             \
    try { gen.Deserialize("Noone expects the Spanish Inquisition!", state); }       \
    catch (std::exception &) { error = true; }                                      \
    ASSERT_TRUE(error);                                                             \
  }                                                                                 \
}

TEST(Random, BadSerialization_Lcg)
{
  using Generator = CongruentialGenerator;
  BadSerializationTestBody();
}

TEST(Random, BadSerialization_Mtg)
{
  using Generator = MersenneTwisterGenerator;
  BadSerializationTestBody();
}

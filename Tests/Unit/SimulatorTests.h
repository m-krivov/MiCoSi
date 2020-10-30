#pragma once
#include "Defs.h"

#include "Mitosis.Solvers/SimulatorConfig.h"

static inline bool SerializeDeserializeTest(const SimulatorConfig &config)
{
  try
  {
    auto str = SimulatorConfig::Serialize(config);
    SimulatorConfig tmp = SimulatorConfig::Parse(str);
    int num1 = 0, num2 = 0;
    if (config.Type() != tmp.Type()) { return false; }
    if (config.HasDeviceNumber(num1) ^ tmp.HasDeviceNumber(num2)) { return false; }
    if (num1 != num2) { return false; }

    return true;
  }
  catch (std::exception &)
  { return false; }
}

TEST(Simulator, DefaultConfig)
{
  auto config = SimulatorConfig::Default();
  ASSERT_TRUE(SerializeDeserializeTest(config));
  ASSERT_TRUE(config.Type() == SimulatorConfig::CPU || config.Type() == SimulatorConfig::CUDA);
}

TEST(Simulator, CpuConfig)
{
  int num;
  auto config = SimulatorConfig(SimulatorConfig::CPU);
  ASSERT_TRUE(SerializeDeserializeTest(config));
  ASSERT_EQ(config.Type(), SimulatorConfig::CPU);
  ASSERT_FALSE(config.HasDeviceNumber(num));

  config = SimulatorConfig(SimulatorConfig::CPU, 2);
  ASSERT_TRUE(SerializeDeserializeTest(config));
  ASSERT_EQ(config.Type(), SimulatorConfig::CPU);
  ASSERT_TRUE(config.HasDeviceNumber(num));
  ASSERT_EQ(num, 2);
}

TEST(Simulator, CudaConfig)
{
  int num;
  auto config = SimulatorConfig(SimulatorConfig::CUDA);
  ASSERT_TRUE(SerializeDeserializeTest(config));
  ASSERT_EQ(config.Type(), SimulatorConfig::CUDA);
  ASSERT_FALSE(config.HasDeviceNumber(num));

  config = SimulatorConfig(SimulatorConfig::CUDA, 1);
  ASSERT_TRUE(SerializeDeserializeTest(config));
  ASSERT_EQ(config.Type(), SimulatorConfig::CUDA);
  ASSERT_TRUE(config.HasDeviceNumber(num));
  ASSERT_EQ(num, 1);
}

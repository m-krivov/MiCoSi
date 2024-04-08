#include "Defs.h"

#include "DistanceTests.h"
#include "RandomTests.h"
#include "SimulatorTests.h"

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

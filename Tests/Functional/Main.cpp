
#include "Defs.h"
#include "Helpers.h"

// Tier1: checks a simple actions that are not connected with mitosis
// For example, launch solver with different options, generate configs, etc
#include "Tier1/CommandLineTests.h"
#include "Tier1/PropsTests.h"
#include "Tier1/InitialSetupTests.h"
#include "Tier1/RepairTests.h"

// Tier3: verifies the implemented models, compares our results with the published ones
#include "Tier3/MtDistributionTests.h"

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

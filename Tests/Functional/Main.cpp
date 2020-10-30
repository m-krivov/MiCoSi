
#include "Defs.h"
#include "Helpers.h"

// Tier1: checks correcteness of simple actions that are not connected with mitosis
// For example, solver can be launched with different options, configs are generated, etc
#include "Tier1/CommandLineTests.h"
#include "Tier1/PropsTests.h"
#include "Tier1/InitialSetupTests.h"
#include "Tier1/RepairTests.h"

// Tier2: checks that some trivial logics and iteractions between cell objects are correct
// For example, forces move chromosomes, MTs bind kinetochore, etc
#include "Tier2/BindTests.h"
#include "Tier2/CollisionTests.h"
//#include "Tier2/CudaTests.h"
#include "Tier2/ForceTests.h"
#include "Tier2/KinetochoreTests.h"
#include "Tier2/MovementTests.h"
#include "Tier2/RngTests.h"
#include "Tier2/SpringTests.h"

// Tier3: verifies implemented models, compares our results with the published ones
#include "Tier3/MtDistributionTests.h"

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

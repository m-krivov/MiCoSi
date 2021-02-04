
#include "StaticPoleUpdater.h"

#include "Mitosis.Core/SimParams.h"

//-------------------------
//--- StaticPoleUpdater ---
//-------------------------

void StaticPoleUpdater::SetInitial(Pole *left, Pole *right, Random::State &state)
{
  real l_poles = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::L_Poles, true);
  vec3r pos = vec3r::DEFAULT_LEFT * (l_poles / 2);
  left->Position() = pos;
  right->Position() = -pos;
}

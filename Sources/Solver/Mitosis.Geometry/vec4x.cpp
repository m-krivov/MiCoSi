
#include "vec4x.h"

//-------------
//--- vec4f ---
//-------------

const vec4f vec4f::ZERO = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
const vec4f vec4f::DEFAULT_DIRECT = vec4f(0.0f, 0.0f, -1.0f, 1.0f);
const vec4f vec4f::DEFAULT_UP = vec4f(0.0f, -1.0f, 0.0f, 1.0f);
const vec4f vec4f::DEFAULT_LEFT = vec4f(-1.0f, 0.0f, 0.0f, 1.0f);

//-------------
//--- vec4d ---
//-------------

const vec4d vec4d::ZERO = vec4d(0.0, 0.0, 0.0, 1.0);
const vec4d vec4d::DEFAULT_DIRECT = vec4d(0.0, 0.0, -1.0, 1.0);
const vec4d vec4d::DEFAULT_UP = vec4d(0.0, -1.0, 0.0, 1.0);
const vec4d vec4d::DEFAULT_LEFT = vec4d(-1.0, 0.0, 0.0, 1.0);

#pragma once

#include "math_base.h"

#if defined(ARCH_X64)
#include "math_x64.h"
#elif defined(ARCH_ARM64)
#include "math_arm64.h"
#else
#error Architecture not supported
#endif

#include "GameTypes/StdTypes.h"

#if defined(DEBUGGING)
#define ASSERTS 1
#else
#define ASSERTS 0
#endif
#include "Asserts/Asserts.h"

#if defined(SHIPPING)
#define DEBUG_LOGGING 0
#else
#define DEBUG_LOGGING 1
#endif
#include "Logger/Logger.h"

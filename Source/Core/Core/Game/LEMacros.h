#include "LECoreUtils/StdTypes.h"

#if defined(DEBUGGING)
#define ASSERTS 1
#else
#define ASSERTS 0
#endif
#include "LECoreUtils/LEAsserts/Asserts.h"

#if defined(SHIPPING)
#define DEBUG_LOGGING 0
#else
#define DEBUG_LOGGING 1
#endif
#include "LECoreUtils/LELogger/Logger.h"

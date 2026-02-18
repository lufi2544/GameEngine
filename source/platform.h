/* date = January 22nd 2026 7:52 pm */

//////// Platform related functions. This will get expaned in the platform.cpp


#ifdef _WINDOWS
#include "Windows.h"
#endif // WINDOWS


global_f void
PlatformUpdate();


/** Time since the system starterd. */
global_f f64
PlatformNow();


global_f void
PlatformSleep(f64 sleep_for_ms);
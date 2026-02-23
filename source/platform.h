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

///////
/// Platform Event signal

struct platform_event_t;

global_f platform_event_t*
PlatformEventCreate(arena_t *arena);

global_f void 
PlatformEventWait(platform_event_t *event);


global_f void
PlatformEventSignal(platform_event_t *event);


global_f void
PlatformMemoryBarrier();

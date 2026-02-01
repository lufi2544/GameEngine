// This is the platform for MacOs, this is the bridge from objective-c to c++

// platform_macos.mm
#import <Cocoa/Cocoa.h>


// ------------------------------------------------------------
// Globals
// ------------------------------------------------------------

global NSApplication* g_app = nil;
global bool g_app_initialized = false;

// ------------------------------------------------------------
// Window delegate (handles close event)
// ------------------------------------------------------------

@interface MacOSWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation MacOSWindowDelegate

- (void)windowWillClose:(NSNotification*)notification
{
    g_engine->is_running = false;
}

@end

// ------------------------------------------------------------
// Platform update (event pump)
// ------------------------------------------------------------

global_f void
PlatformUpdate()
{
    @autoreleasepool
    {
        NSEvent* event = nil;
        
        do
        {
            event = [g_app nextEventMatchingMask:NSEventMaskAny
                     untilDate:[NSDate distantPast]
                     inMode:NSDefaultRunLoopMode
                     dequeue:YES];
            
            if(event)
            {
                [g_app sendEvent:event];
            }
            
        } while(event);
    }
}

// ------------------------------------------------------------
// Window creation (macOS)
// ------------------------------------------------------------

global_f window_t
CreateAWindow(u32 x, u32 y, u32 h, u32 w)
{
    window_t result = {};
    
    @autoreleasepool
    {
        // Init NSApplication once
        if(!g_app_initialized)
        {
            g_app = [NSApplication sharedApplication];
            [g_app setActivationPolicy:NSApplicationActivationPolicyRegular];
            [g_app finishLaunching];
            [g_app activateIgnoringOtherApps:YES];
            g_app_initialized = true;
        }
        
        NSRect rect = NSMakeRect((CGFloat)x, (CGFloat)y,
                                 (CGFloat)w, (CGFloat)h);
        
        NSWindowStyleMask style =
            NSWindowStyleMaskTitled |
            NSWindowStyleMaskClosable |
            NSWindowStyleMaskResizable;
        
        NSWindow* window =
            [[NSWindow alloc] initWithContentRect:rect
             styleMask:style
             backing:NSBackingStoreBuffered
             defer:NO];
        
        [window setTitle:@"engine"];
        [window makeKeyAndOrderFront:nil];
        [window center];
        
        MacOSWindowDelegate* delegate = [[MacOSWindowDelegate alloc] init];
        [window setDelegate:delegate];
        
        result.handle = (void*)window;
        result.x = x;
        result.y = y;
        result.width  = w;
        result.height = h;
    }
    
    return result;
}

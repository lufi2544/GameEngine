#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

global Display* g_x11_display = NULL;
global ::Window  g_x11_window  = 0;

internal_f u8
TranslateLinuxInputKey(KeySym key)
{
    if (key >= XK_a && key <= XK_z)
        return (u8)(InputKey_A + (key - XK_a));
    if (key >= XK_A && key <= XK_Z)
        return (u8)(InputKey_A + (key - XK_A));
    if (key >= XK_0 && key <= XK_9)
        return (u8)(InputKey_Num0 + (key - XK_0));

    switch (key)
    {
        case XK_Up:        return InputKey_Up;
        case XK_Down:      return InputKey_Down;
        case XK_Left:      return InputKey_Left;
        case XK_Right:     return InputKey_Right;
        case XK_Escape:    return InputKey_Escape;
        case XK_Return:    return InputKey_Enter;
        case XK_space:     return InputKey_Space;
        case XK_Tab:       return InputKey_Tab;
        case XK_BackSpace: return InputKey_Backspace;
        case XK_Shift_L:
        case XK_Shift_R:   return InputKey_Shift;
        case XK_Control_L:
        case XK_Control_R: return InputKey_Ctrl;
        case XK_Alt_L:
        case XK_Alt_R:     return InputKey_Alt;
        default:           return InputKey_None;
    }
}

global_f void
PlatformUpdate()
{
    if (!g_x11_display)
        return;

    XEvent event;
    while (XPending(g_x11_display))
    {
        XNextEvent(g_x11_display, &event);

        switch (event.type)
        {
            case ClientMessage:
            {
                g_engine->is_running = false;
            } break;

            case KeyPress:
            {
                KeySym sym = XLookupKeysym(&event.xkey, 0);
                u8 key = TranslateLinuxInputKey(sym);
                bool current = IsKeyPressed(key);
                if (!current)
                    SetKeyState(key, true);
            } break;

            case KeyRelease:
            {
                KeySym sym = XLookupKeysym(&event.xkey, 0);
                u8 key = TranslateLinuxInputKey(sym);
                bool current = IsKeyPressed(key);
                if (current)
                    SetKeyState(key, false);
            } break;
        }
    }
}

global_f f64
PlatformNow()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (f64)(ts.tv_sec * 1000.0) + (f64)(ts.tv_nsec / 1000000.0);
}

global_f void
PlatformSleep(f64 sleep_for_ms)
{
    usleep((useconds_t)(sleep_for_ms * 1000.0));
}

struct platform_event_t
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool            signaled;
};

global_f platform_event_t*
PlatformEventCreate(arena_t *arena)
{
    platform_event_t *result = (platform_event_t*)push_size(arena, sizeof(platform_event_t));
    pthread_mutex_init(&result->mutex, NULL);
    pthread_cond_init(&result->cond, NULL);
    result->signaled = false;
    return result;
}

global_f void
PlatformEventWait(platform_event_t *event)
{
    pthread_mutex_lock(&event->mutex);
    while (!event->signaled)
        pthread_cond_wait(&event->cond, &event->mutex);
    event->signaled = false;
    pthread_mutex_unlock(&event->mutex);
}

global_f void
PlatformEventSignal(platform_event_t *event)
{
    pthread_mutex_lock(&event->mutex);
    event->signaled = true;
    pthread_cond_signal(&event->cond);
    pthread_mutex_unlock(&event->mutex);
}

global_f void
PlatformMemoryBarrier()
{
    __sync_synchronize();
}

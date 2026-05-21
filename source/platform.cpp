

#ifdef _WINDOWS
#include "platform_windows.cpp"
#endif // _WINDOWS


#ifdef _APPLE
#include "platform_macos.mm"
#endif // _MACOS

#ifdef _LINUX
#include "platform_linux.cpp"
#endif // _LINUX
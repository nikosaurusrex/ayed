#include "base_arena.cpp"
#include "base_os.cpp"
#include "base_string.cpp"

#if defined(OS_WINDOWS)
#include "base_os_win.cpp"
#elif defined(OS_LINUX) || defined(OS_MAC)
#include "base_os_unix.cpp"
#endif

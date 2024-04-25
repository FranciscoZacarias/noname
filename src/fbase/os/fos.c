#if defined(OS_WINDOWS)
# include <Windows.h>
# include <userenv.h>
# include <stdio.h>
# include "os/win32/win32.c"
#elif defined(OS_LINUX)
# error "OS_LINUX Not supported"
#elif defined(OS_MAC)
# error "OS_MAC Not supported"
#endif
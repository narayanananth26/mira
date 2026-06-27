
#ifdef WIN32
#include "windows.h"
#else
#include <sys/time.h>
#endif

int GetTimeMs() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

// macOS
#include <sys/time.h>

int GetTimeMs() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

#include "utils.h"


long long diff_ms(struct timeval start, struct timeval end)
{
    long mtime, seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    return mtime;
}

long long diff_us(struct timeval start, struct timeval end)
{
    long mtime, seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = (seconds) * 1000000 + useconds;
    return mtime;
}


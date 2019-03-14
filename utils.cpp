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

// r1 + r2 -> r3
void fast_xor(char* r1, char* r2, char* r3, int size)
{
#ifdef VEC128
    __m128i *b1, *b2, *b3;
    int vec_width = 16;
    int loops = size / vec_width;

    for(int j = 0;j<loops;j++)
    {
        b1 = (__m128i *)(r1+j*vec_width);
        b2 = (__m128i *)(r2+j*vec_width);
        b3 = (__m128i *)(r3+j*vec_width);
        *b3 = _mm_xor_si128(*b1,*b2);
    }
#endif
#ifdef VEC256
    int vec_width = 32;
    int loops = size / vec_width;

    for(int j = 0;j<loops;j++)
    {
        __m256i *e1, *e2, *e3;
        e1 = (__m256i *)(r1+j*vec_width);
        e2 = (__m256i *)(r2+j*vec_width);
        e3 = (__m256i *)(r3+j*vec_width);
        *e3 = _mm256_xor_si256(*e1,*e2);
    }
#endif
}

void freeVectorPointer(vector<int*> &p)
{
    for(int i = 0;i<p.size();i++)
        free(p[i]);
}

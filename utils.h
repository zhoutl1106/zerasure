#ifndef UTILS_H
#define UTILS_H

/* define vectorization size, enable only one */
//#define VEC128
#define VEC256
/* ****************************************** */

#include <x86intrin.h>

long long diff_us(struct timeval start, struct timeval end);
long long diff_ms(struct timeval start, struct timeval end);

// r1 + r2 -> r3
// inline funcion in header file to avoid link issue
inline void fast_xor(char* r1, char* r2, char* r3, int size)
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


#endif // UTILS_H

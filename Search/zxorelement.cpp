#include "zxorelement.h"
#include <sys/time.h>
#include <immintrin.h>
#include <cstring>
#include "../utils.h"
#include <stdio.h>

int ZXORElement::cpy_weight = 3065;
int ZXORElement::xor_weight = 5998;

ZXORElement::ZXORElement(int tK)
{
    K = tK;
}

int ZXORElement::value()
{
    return 0;
}

// test the weight of memcpy and xor using given size of data, 1GB by default
// test 10 times, in each test run memcpy and xor *loops* times
void ZXORElement::test_cost_weight(int size, int loops)
{
    cpy_weight = 0;
    xor_weight = 0;
    for(int i = 0;i<10;i++)
    {
        char *dat, *dat1, *dat2;

        posix_memalign((void**)&dat, 32, size);
        posix_memalign((void**)&dat1, 32, size);
        posix_memalign((void**)&dat2, 32, size);
        struct timeval t0,t1;

        gettimeofday(&t0,NULL);
        for(int j = 0;j<loops;j++)
            memcpy(dat1,dat,size);
        gettimeofday(&t1,NULL);
        cpy_weight += diff_us(t0,t1);

        gettimeofday(&t0,NULL);
        for(int j = 0;j<loops;j++)
            fast_xor(dat,dat1,dat1, size);
        gettimeofday(&t1,NULL);
        xor_weight += diff_us(t0,t1);

        free(dat);
        free(dat1);
        free(dat2);
    }

    // scale to 0~9999 to avoid overflow
    while(cpy_weight > 10000)
    {
        cpy_weight /= 10;
        xor_weight /= 10;
    }
    printf("cpy_weight = %d, xor_weight = %d\n", cpy_weight, xor_weight);
}

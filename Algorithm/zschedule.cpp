#include "zschedule.h"
extern "C"{
#include "Jerasure-1.2A/jerasure.h"
#include "Jerasure-1.2A/cauchy.h"
}
#include <x86intrin.h>

//#define XOR slow_xor
#define XOR fast_xor

static void slow_xor(char* r0, char* r1, char *r2, int size)
{
    char t = r2[0];
    for(int i = 0;i<size;i++)
        r2[i] = r1[i]^r0[i];
    //    printf("after xor %d+%d->%d\n", t,r0[0],r2[0]);
}

static void fast_xor(char* r0, char* r1, char *r2, int size)
{
    __m128i *b1, *b2, *b3;
    int vec_width = 16;
    //    assert(size % vec_width == 0);
    int loops = size / vec_width;
    //#pragma omp parallel for
    for(int j = 0;j<loops;j++)
    {
        b1 = (__m128i *)(r0+j*vec_width);
        b2 = (__m128i *)(r1+j*vec_width);
        b3 = (__m128i *)(r2+j*vec_width);
        *b3 = _mm_xor_si128(*b1,*b2);
    }
}

ZSchedule::ZSchedule(int size)
{
    sch.clear();
    idx = 0;
    packetsize = size;

    cpy_weight = 3065;
    xor_weight = 5998;
}

int ** ZSchedule::getSch()
{
    return sch.data();
}

void ZSchedule::addSch(int s0,int s1, int d0,int d1, int op)
{
    //    printf("Add sch %d,%d -> %d, %d\n", s0,s1,d0,d1);
    int * row;
    posix_memalign((void**)&row, 32, 5*sizeof(int));
    row[0] = s0;
    row[1] = s1;
    row[2] = d0;
    row[3] = d1;
    row[4] = op;
    sch.push_back(row);
}

void ZSchedule::printSch()
{
    int idx = 0;
    for(int i = 0;i<sch.size();i++)
    {
        printf("%d %d %d %d %d\n", sch[idx][0],sch[idx][1],sch[idx][2],sch[idx][3],sch[idx][4]);
        idx++;
    }
}

void ZSchedule::printSchLen(char* p)
{
    int n_xor = 0;
    int n_cpy = 0;

    for(int i = 0;i<sch.size();i++)
        if(sch[idx][4] == 0)
            n_cpy ++;
        else
            n_xor ++;
    printf("%s : len %d, xor %d, cpy %d, weighted %d\n", p, sch.size(), n_xor, n_cpy, xor_weight*n_xor + cpy_weight * n_cpy);
}


void ZSchedule::run(char**& ptrs)
{
    for(int i = 0;i<sch.size();i++)
    {
        //                printf("%d,%d,%d,%d,%d\n", sch[i][0],sch[i][1],sch[i][2],sch[i][3],sch[i][4]);
        char *src = ptrs[sch[i][0]]+packetsize*sch[i][1];
        char *dst = ptrs[sch[i][2]]+packetsize*sch[i][3];
        switch(sch[i][4])
        {
        case 0:
            memcpy(dst,src,packetsize);
            break;
        case 1:
            XOR(src,dst,dst,packetsize);
            break;
        }
        //        printf("%d,%d->%d,%d = %d\n", sch[i][0],sch[i][1],sch[i][2],sch[i][3],dst[0]);

    }
}

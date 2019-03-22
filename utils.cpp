/*
utils.cpp
Tianli Zhou

Fast Erasure Coding for Data Storage: A Comprehensive Study of the Acceleration Techniques

Revision 1.0
Mar 20, 2019

Tianli Zhou
Department of Electrical & Computer Engineering
Texas A&M University
College Station, TX, 77843
zhoutianli01@tamu.edu

Copyright (c) 2019, Tianli Zhou
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

- Neither the name of the University of Tennessee nor the names of its
  contributors may be used to endorse or promote products derived
  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <cassert>

long long diff_ms(struct timeval start, struct timeval end)
{
    long long mtime, seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    return mtime;
}

long long diff_us(struct timeval start, struct timeval end)
{
    long long mtime, seconds, useconds;

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

void free2dSchedule5(int **p)
{
    int idx = 0;
    while(p[idx][0] != -1)
    {
        free(p[idx]);
        idx++;
    }
    p[idx]++;
    free(p);
}


char** malloc2d(int row, int col)
{
    printf("Malloc... 2d %d x %d\n",row, col);
    char ** ret = (char**) aligned_alloc(32,row * sizeof(char*));

    assert(ret != NULL);
    for(int i = 0;i<row;i++)
    {
        ret[i] = (char*)aligned_alloc(32,col);
        assert(ret[i] != NULL);
        memset(ret[i], 0, col);
    }
    return ret;
}

void free2d(char** &p, int len)
{
    printf("Free2d %d\n",len);
    for(int i = 0;i<len;i++)
    {
        if(p[i] != NULL)
            free(p[i]);
    }
    free(p);
    p = NULL;
}

/*
zcode.cpp
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
#include "zcode.h"
#include <sstream>
#include <iostream>
#include <sys/time.h>
#include <algorithm>
#include <string.h>
#include "../Search/zrandomarray.h"

ZCode::ZCode(int tk, int tm, int tw, int mp)
{
    K = tk;
    M = tm;
    W = tw;
    packetsize = mp;
}

void printArr(char** p, int x, int y, char* pp)
{
    printf("-- %s --\n",pp);
    for(int i = 0;i<x;i++)
    {
        for(int j = 0;j<y;j++)
            printf("%d ", p[i][j]);
        printf("\n");
    }
}

void print1DArr(char* p, int x, char* pp)
{
    printf("-- %s --\n",pp);
    for(int i = 0;i<x;i++)
    {
        printf("%d ", p[i]);
    }
    printf("\n");
}

void ZCode::test_speed()
{
    // gen original data
    int size = 1024*1024*1024;
    // padded to multiple blocksize
    while(size%(blocksize) != 0)
        size ++;
    int loops = size/(blocksize);
    printf("size is %d Byte, %d loops\n",size,loops);
    char *dat = (char*)malloc(size);
    for(int i = 0;i<size;i++)
    {
        dat[i] = rand();
    }

    char** par = malloc2d(M, size/K);
    char** parities = new char*[M];
    for(int i = 0;i<M;i++)
        parities[i] = par[i];
    // Encode
    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    for(int i = 0;i<loops;i++)
    {
        encode_single_chunk(dat+i*blocksize,blocksize,parities);
        for(int j = 0;j<M;j++)
        {
            parities[j] += blocksize/K;
        }
    }
    gettimeofday(&t1,NULL);
    long long diff = diff_us(t0,t1);
    printf(" !!! Encode cost %d us, init %d us, total %d us, size = %d, speed = %f\n", diff, init_time, diff+init_time, size, size * 1000.0 * 1000/ 1024.0/1024 / (diff));

    // gen erased data
    char** adat = malloc2d(K, size/K);

    for(int i = 0;i<loops;i++)
    {
        for(int j = 0;j<K;j++)
        {
//            printf("copy dat off %d, size %d to adat[%d]\n", i*blocksize+j*blocksize/K, blocksize/K, j);
            memcpy(adat[j]+i*blocksize/K,dat + i*blocksize+j*blocksize/K,blocksize/K);
        }
    }

    for(int i = 0;i<M;i++)
        parities[i] = par[i];

    // gen lost pattern
    ZRandomArray zra(K+M);
    int* p = zra.next_random(K+M,K+M);
    vector<int> lost;
    printf("Lost : ");
    for(int i = 0;i<M;i++)
    {
        if(p[i] < K)
            printf(" data %d,", p[i]);
        else
            printf(" parity %d,", p[i]-K);
        lost.push_back(p[i]);
    }
    printf("\n");

    // copy erased data
    for(int i = 0;i<M;i++)
    {
        if(lost[i] < K)
        {
//            printf("clear adat[%d]\n",i);
            memset(adat[lost[i]],0,size/K);
        }
        else
        {
//            printf("clear par[%d]\n",lost[i]-K);
            memset(par[lost[i]-K],0,size/K);
        }
    }

    // Decode
    set_erasure(lost);

    char** tdata = malloc2d(K, sizeof(char*));
    for(int i = 0;i<K;i++)
        tdata[i] = adat[i];
    gettimeofday(&t0,NULL);
    for(int i = 0;i<loops;i++)
    {
        decode_single_chunk(tdata,parities);
        for(int j = 0;j<K;j++)
            tdata[j] += blocksize/K;
        for(int j = 0;j<M;j++)
            parities[j] += blocksize/K;
    }
    gettimeofday(&t1,NULL);
    diff = diff_us(t0,t1);
    printf(" !!! Decode cost %d us, init %d us, total %d us, size = %d, speed = %f\n", diff, init_time, diff+init_time, size, size * 1000.0 * 1000/ 1024.0/1024 / (diff));

    for(int i = 0;i<K;i++)
        tdata[i] = adat[i];

    // Check correctness
    bool isSame = true;
    for(int i = 0;i<loops;i++)
    {
        for(int j = 0;j<K;j++)
        {
            if(memcmp(tdata[j], dat + i*blocksize + j*blocksize/K, blocksize/K) != 0)
            {
                isSame = false;
                printf("loop %d, tdata[%d] diff\n", i,j);
                break;
            }
        }
        if(!isSame) break;

        for(int j = 0;j<K;j++)
            tdata[j] += blocksize/K;
    }
    if(isSame)
        printf(" !!! Same\n");
    else
        printf(" ??? Diff\n");

    free(adat);
}

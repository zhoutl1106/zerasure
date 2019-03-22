/*
zgrouping.cpp
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

- Neither the name of the Texas A&M University nor the names of its
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
#include "zgrouping.h"
extern "C"{
#include "../Jerasure-1.2A/jerasure.h"
#include "../Jerasure-1.2A/cauchy.h"
}
#include <cassert>
#include <cstring>
#include <sys/time.h>
#include <x86intrin.h>

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

ZGrouping::ZGrouping(int tK, int tM, int tW, vector<int>& arr, bool isNormal, bool isWeightedGrouping, int m_packagesize): ZCode(tK,tM,tW,m_packagesize)
{
    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    xc = new ZOXC(K*W, M*W);
    printf("new ZGrouping K = %d, M= %d, W= %d, arr = %p, isNormal = %d, isWeightedGrouping = %d\n",K,M,W,arr.data(),isNormal,isWeightedGrouping);

    for(int i = 0;i<arr.size();i++)
        printf("%d ", arr[i]);
    printf("\n");
    matrix = cauchy_xy_coding_matrix(K,M,W,arr.data()+K,arr.data());
    if(isNormal)
        cauchy_improve_coding_matrix(K,M,W,matrix);
    bitmatrix = jerasure_matrix_to_bitmatrix(K,M,W,matrix);

//        printf("Generator Matrix:\n");
//        jerasure_print_matrix(matrix,M,K,W);
//        printf("Bitmatrix :\n");
//        jerasure_print_bitmatrix(bitmatrix,M*W,K*W,W);

    xc->grouping_1s(bitmatrix,isWeightedGrouping);
    gettimeofday(&t1,NULL);
    init_time = diff_us(t0,t1);
    for(int i = 0;i<xc->intermedia_schedule.size();i++)
    {
        char *tmp;
        posix_memalign((void**)&tmp,32,packetsize);
        intermedia.push_back(tmp);
    }
    erasures = (int*) malloc(sizeof(int) * (K+M));
    int n_cpy = 0,  n_xor = 0;
    for(vector<int*>::iterator it = xc->schedule.begin(); it != xc->schedule.end();it++)
    {
        if((*it)[2] % 2 == 0)
            n_cpy++;
        else
            n_xor++;
        //            printf("%d,%d,%d\n", (*it)[0],(*it)[1],(*it)[2]);
    }
    printf(" !!! Grouping Schedule len = %d, mem = %d, xor = %d, weighted = %d, nit %d us\n", xc->schedule.size(), n_cpy, n_xor, n_cpy*3065+5998*n_xor,init_time);
    blocksize =  packetsize * K * W;
}

ZGrouping::~ZGrouping()
{
    delete xc;
}

void print_array(char* title, char*p, int len)
{
    fprintf(stderr,"%s\n",title);
    for(int i = 0;i<len;i++)
        fprintf(stderr,"%02X ", (unsigned char)p[i]);
    fprintf(stderr,"\n");
}

// ops
// 0: copy data -> parity
// 1: xor data -> parity
// 2: copy intermedia -> parity
// 3: xor intermedia -> parity
// 4: copy data -> intermedia
// 5: xor data -> intermedia

void ZGrouping::do_scheduled_operations(vector<int*> &schedule, char **&data, char **&parities)
{
    char *sptr=NULL;
    char *dptr=NULL;
    int sch_len = schedule.size();
    int s,d,op,d_idx,d_off, s_idx, s_off;
    for(int i = 0;i<sch_len;i++)
    {
        s = schedule[i][0];
        d = schedule[i][1];
        op = schedule[i][2];
        s_idx = s / W;
        s_off = (s % W)*packetsize;
        d_idx = d / W;
        d_off = d % W * packetsize;
        switch(op)
        {
        case 0:
            sptr = data[s_idx] + s_off;
            dptr = parities[d_idx] + d_off;
            memcpy(dptr, sptr, packetsize);
            break;
        case 1:
            sptr = data[s_idx] + s_off;
            dptr = parities[d_idx] + d_off;
            fast_xor(sptr, dptr, dptr, packetsize);
            break;
        case 2:
            sptr = intermedia[s];
            dptr = parities[d_idx] + d_off;
            memcpy(dptr, sptr, packetsize);
            break;
        case 3:
            sptr = intermedia[s];
            dptr = parities[d_idx] + d_off;
            fast_xor(sptr, dptr, dptr, packetsize);
            break;
        case 4:
            sptr = data[s_idx] + s_off;
            dptr = intermedia[d];
            memcpy(dptr, sptr, packetsize);
            break;
        case 5:
            sptr = data[s_idx] + s_off;
            dptr = intermedia[d];
            fast_xor(sptr, dptr, dptr, packetsize);
            break;
        }
    }
}

void ZGrouping::encode_single_chunk(char *data, int len, char **&parities)
{
    assert(data != NULL);
    assert(parities != NULL);
    assert(len == packetsize * K * W);
    char **tdata = (char**)malloc(K*sizeof(char*));
    for(int i = 0;i<K;i++)
        tdata[i] = data + packetsize * i * W;

    do_scheduled_operations(xc->schedule,tdata,parities);
    free(tdata);
}

void ZGrouping::set_erasure(vector<int> arr)
{
    int i;
    for(i = 0;i<arr.size();i++)
    {
        erasures[i] = arr[i];
    }
    erasures[i] = -1;
    /****** hard copy from jerasure ******/
    int j, x, drive, y, index, z;
    int *decoding_matrix, *inverse, *real_decoding_matrix;
    int *ptr;
    int *row_ids;
    int cdf, ddf;
    int *ind_to_row;
    int *b1, *b2;
    int k = K;
    int m = M;
    int w = W;

    /* First, figure out the number of data drives that have failed, and the
        number of coding drives that have failed: ddf and cdf */

    ddf = 0;
    cdf = 0;
    for (i = 0; erasures[i] != -1; i++) {
        if (erasures[i] < k) ddf++; else cdf++;
    }

    row_ids = talloc(int, k+m);
    ind_to_row = talloc(int, k+m);

    if (set_up_ids_for_scheduled_decoding(k, m, erasures, row_ids, ind_to_row) < 0) return;

    /* Now, we're going to create one decoding matrix which is going to
         decode everything with one call.  The hope is that the scheduler
         will do a good job.    This matrix has w*e rows, where e is the
         number of erasures (ddf+cdf) */

    real_decoding_matrix = talloc(int, k*w*(cdf+ddf)*w);

    /* First, if any data drives have failed, then initialize the first
         ddf*w rows of the decoding matrix from the standard decoding
         matrix inversion */

    if (ddf > 0) {

        decoding_matrix = talloc(int, k*k*w*w);
        ptr = decoding_matrix;
        for (i = 0; i < k; i++) {
            if (row_ids[i] == i) {
                bzero(ptr, k*w*w*sizeof(int));
                for (x = 0; x < w; x++) {
                    ptr[x+i*w+x*k*w] = 1;
                }
            } else {
                memcpy(ptr, bitmatrix+k*w*w*(row_ids[i]-k), k*w*w*sizeof(int));
            }
            ptr += (k*w*w);
        }
        inverse = talloc(int, k*k*w*w);
        jerasure_invert_bitmatrix(decoding_matrix, inverse, k*w);

        /*    printf("\nMatrix to invert\n");
        jerasure_print_bitmatrix(decoding_matrix, k*w, k*w, w);
        printf("\n");
        printf("\nInverse\n");
        jerasure_print_bitmatrix(inverse, k*w, k*w, w);
        printf("\n"); */

        free(decoding_matrix);
        ptr = real_decoding_matrix;
        for (i = 0; i < ddf; i++) {
            memcpy(ptr, inverse+k*w*w*row_ids[k+i], sizeof(int)*k*w*w);
            ptr += (k*w*w);
        }
        free(inverse);
    }

    /* Next, here comes the hard part.  For each coding node that needs
         to be decoded, you start by putting its rows of the distribution
         matrix into the decoding matrix.  If there were no failed data
         nodes, then you're done.  However, if there have been failed
         data nodes, then you need to modify the columns that correspond
         to the data nodes.  You do that by first zeroing them.  Then
         whereever there is a one in the distribution matrix, you XOR
         in the corresponding row from the failed data node's entry in
         the decoding matrix.  The whole process kind of makes my head
         spin, but it works.
       */

    for (x = 0; x < cdf; x++) {
        drive = row_ids[x+ddf+k]-k;
        ptr = real_decoding_matrix + k*w*w*(ddf+x);
        memcpy(ptr, bitmatrix+drive*k*w*w, sizeof(int)*k*w*w);

        for (i = 0; i < k; i++) {
            if (row_ids[i] != i) {
                for (j = 0; j < w; j++) {
                    bzero(ptr+j*k*w+i*w, sizeof(int)*w);
                }
            }
        }

        /* There's the yucky part */

        index = drive*k*w*w;
        for (i = 0; i < k; i++) {
            if (row_ids[i] != i) {
                b1 = real_decoding_matrix+(ind_to_row[i]-k)*k*w*w;
                for (j = 0; j < w; j++) {
                    b2 = ptr + j*k*w;
                    for (y = 0; y < w; y++) {
                        if (bitmatrix[index+j*k*w+i*w+y]) {
                            for (z = 0; z < k*w; z++) {
                                b2[z] = b2[z] ^ b1[z+y*k*w];
                            }
                        }
                    }
                }
            }
        }
    }
    /****** end hard copy ******/

    unsigned long long pattern = 0LL;
    for(i = 0;i<arr.size();i++)
        pattern |= 1LL << arr[i];

    if(de_schedules_map.find(pattern) != de_schedules_map.end())
    {
        printf("found decode schedule for pattern %llu\n",pattern);
        de_schedule = de_schedules_map[pattern];
        ids = ids_map[pattern];
    }
    else
    {
        printf("new decode schedule for pattern %llu\n",pattern);
        ZOXC txc(K*W,(cdf+ddf)*w);
        txc.grouping_1s(real_decoding_matrix,true);
        vector<int> tid;
        for(int i = 0;i<k+m;i++)
        {
            tid.push_back(row_ids[i]);
        }
        ids = tid;
        printf("intermedia len = %d / %d\n", txc.intermedia_schedule.size(), intermedia.size());
        if(txc.intermedia_schedule.size() > intermedia.size())
        {
            int old_len = intermedia.size();
            int new_len = txc.intermedia_schedule.size();

            for(int i = old_len;i<new_len;i++)
            {
                char *tmp;
                posix_memalign((void**)&tmp,32,packetsize);
                intermedia.push_back(tmp);
            }
        }
        vector<int*> tsh;
        tsh.resize(txc.schedule.size());
        for(int i = 0;i<txc.schedule.size();i++)
        {
            tsh[i] = (int*) malloc(3*sizeof(int));
            memcpy(tsh[i], txc.schedule[i], 3*sizeof(int));
        }
        ids_map[pattern] = tid;
        de_schedule = tsh;
        de_schedules_map[pattern] = de_schedule;
    }
    free(row_ids);
    free(ind_to_row);
    free(real_decoding_matrix);
}

void ZGrouping::decode_single_chunk(char **&data, char **&parities)
{
    char **tdata = (char**)malloc(K*sizeof(char*));
    char **tpar = (char**)malloc(M*sizeof(char*));
    for(int i = 0;i<K;i++)
    {
        if(ids[i] < K)
            tdata[i] = data[ids[i]];
        else
            tdata[i] = parities[ids[i] - K];
    }
    for(int i = K;i<K+M;i++)
    {
        if(ids[i] < K)
        {
            tpar[i-K] = data[ids[i]];
        }
        else
        {
            tpar[i-K] = parities[ids[i] - K];
        }
    }

    do_scheduled_operations(de_schedule,tdata,tpar);

    free(tdata);
    free(tpar);
}

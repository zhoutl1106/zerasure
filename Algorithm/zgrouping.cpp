#include "zgrouping.h"
extern "C"{
#include "Jerasure-1.2A/jerasure.h"
#include "Jerasure-1.2A/cauchy.h"
}
#include <cassert>
#include <cstring>
#include <sys/time.h>
#include <x86intrin.h>

int n_of_1s(int *bm, int len);
char** malloc2d(int row, int col);
extern "C" long long diff_us(struct timeval start, struct timeval end);
long long schedule_len_3(vector<int*> &sch);
//extern int n_tc_cpy, n_tc_xor, n_tg_cpy,n_tg_xor;

ZGrouping::ZGrouping(int tK, int tM, int tW, vector<int>& arr, bool isNormal, bool isWeightedGrouping, int m_packagesize): ZCode(tK,tM,tW,m_packagesize)
{
    //    running_time = 0;

    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    xc = new ZOXC(K*W, M*W);

        printf("new ZDPG K = %d, M= %d, W= %d, arr = %p, isNormal = %d, isWeightedGrouping = %d\n",K,M,W,arr.data(),isNormal,isWeightedGrouping);

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
    //    printf("#XORs = %d\n", n_of_1s(bitmatrix,K*M*W*W)-K*W);
    int ret = xc->grouping_1s(bitmatrix,isWeightedGrouping);
    gettimeofday(&t1,NULL);
    //    running_time += diff_us(t0,t1);
    init_time = diff_us(t0,t1);
    for(int i = 0;i<xc->intermedia_schedule.size();i++)
    {
        //        char* tmp = (char*)aligned_alloc(32,packetsize);
        char *tmp;
        posix_memalign((void**)&tmp,32,packetsize);
        //        printf("new_inter_buf %d of size %d, %p\n",i, packetsize,tmp);
        intermedia.push_back(tmp);
    }
    erasures = (int*) malloc(sizeof(int) * (K+M));
    //    printf("Grouping #XORs = %d\n",ret);
    //    printf("Grouping schedule len = %d\n", xc->schedule.size());

//    printf(" !!! intermedia_schedule: %d\n", xc->intermedia_schedule.size());
    //    for(vector<int* >::iterator it = xc->intermedia_schedule.begin(); it != xc->intermedia_schedule.end(); it++ )
    //    {
    //        printf("%d,%d,%d\n", (*it)[0],(*it)[1],(*it)[2]);
    //    }

    //    printf("Schedule: %d\n", xc->schedule.size());
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
extern int tg_cpy, tg_xor;
void myMemcpy(char *dst, char* src, int len)
{
    memcpy(dst,src,len);
}

void myXor(char *r1, char *r2, char* r3, int len)
{
//    __m128i *b1, *b2, *b3;
//    int vec_width = 16;
//    //    assert(size % vec_width == 0);
//    int loops = len / vec_width;
//    //#pragma omp parallel for
//    for(int j = 0;j<loops;j++)
//    {
//        b1 = (__m128i *)(r1+j*vec_width);
//        b2 = (__m128i *)(r2+j*vec_width);
//        b3 = (__m128i *)(r3+j*vec_width);
//        *b3 = _mm_xor_si128(*b1,*b2);
//    }

    long *l1;
    long *l2;
    long *l3;
    long *ltop;
    char *ctop;

    ctop = r1 + len;
    ltop = (long *) ctop;
    l1 = (long *) r1;
    l2 = (long *) r2;
    l3 = (long *) r3;

    while (l1 < ltop) {
        *l3 = ((*l1)  ^ (*l2));
        l1++;
        l2++;
        l3++;
    }
}

void ZGrouping::do_scheduled_operations(vector<int*> &schedule, char **&data, char **&parities)
{
    char *sptr=NULL;
    char *dptr=NULL;
    int sch_len = schedule.size();
    int s,d,op,d_idx,d_off, s_idx, s_off;
    //    printf("ZGrouping, Schedule len = %d\n", sch_len);
    for(int i = 0;i<sch_len;i++)
    {
        s = schedule[i][0];//(*it)[0];
        d = schedule[i][1];//(*it)[1];
        op = schedule[i][2];//(*it)[2];
        s_idx = s / W;
        s_off = (s % W)*packetsize;
        d_idx = d / W;
        d_off = d % W * packetsize;
        //                fprintf(stderr,"  -- op %d: KMW=%d,%d,%d: s=%d(%d,%d),d=%d(%d,%d)\n",op, K,M,W,s,s_idx,s_off,d,op,d_idx, d_off);
        switch(op)
        {
        case 0:
            //            sptr = data + packetsize * s;
            sptr = data[s_idx] + s_off;
            dptr = parities[d_idx] + d_off;
            //            fprintf(stderr,"0: copy data %d, (%d,%d) = %d\n", s, parity_idx, parity_off, dptr[0]);
            //            print_array(parities[parity_idx], packetsize * W);
            myMemcpy(dptr, sptr, packetsize);
            break;
        case 1:
            //            sptr = data + packetsize * s;
            sptr = data[s_idx] + s_off;
            dptr = parities[d_idx] + d_off;
            //            fprintf(stderr,"1: xor data %d, (%d,%d) = %d\n", s, parity_idx, parity_off, dptr[0]);
            //            print_array(parities[parity_idx], packetsize * W);
            myXor(sptr, dptr, dptr, packetsize);
            //            galois_region_xor(sptr,dptr,dptr,packetsize);
            break;
        case 2:
            sptr = intermedia[s];
            dptr = parities[d_idx] + d_off;
            //            fprintf(stderr,"2: copy inter %d, (%d,%d) = %d\n", s, parity_idx, parity_off, dptr[0]);
            //            print_array(parities[parity_idx], packetsize * W);
            myMemcpy(dptr, sptr, packetsize);
            break;
        case 3:
            sptr = intermedia[s];
            dptr = parities[d_idx] + d_off;
            //            fprintf(stderr,"3: xor inter %d, (%d,%d) = %d\n", s, parity_idx, parity_off, dptr[0]);
            //            print_array(parities[parity_idx], packetsize * W);
            myXor(sptr, dptr, dptr, packetsize);
            //            galois_region_xor(sptr,dptr,dptr,packetsize);
            break;
        case 4:
            //            sptr = data + packetsize * s;
            sptr = data[s_idx] + s_off;
            dptr = intermedia[d];
            //            fprintf(stderr,"4: copy to inter %d, %p->%p\n",d,sptr,dptr);
            //            print_array(intermedia[d], packetsize);
            myMemcpy(dptr, sptr, packetsize);
            break;
        case 5:
            //            sptr = data + packetsize * s;
            sptr = data[s_idx] + s_off;
            dptr = intermedia[d];
            //            fprintf(stderr,"5: xor inter %d, (%d,%d) = %d\n", s, parity_idx, parity_off, dptr[0]);
            //            print_array(intermedia[d], packetsize);
            myXor(sptr, dptr, dptr, packetsize);
            //            galois_region_xor(sptr,dptr,dptr,packetsize);
            break;
        }
    }
}

extern int method_type;
void ZGrouping::encode_single_chunk(char *data, int len, char **&parities)
{
    //    struct timeval t0,t1,t2,t3;
    //    int i,j;
    //    assert(data != NULL);
    //    assert(parities != NULL);
    //    assert(len == packetsize * K * W);
    char **tdata = (char**)malloc(K*sizeof(char*));
    for(int i = 0;i<K;i++)
        tdata[i] = data + packetsize * i * W;

    //    printf("do schedule\n");
    //    printf("Schedule len = %d\n", schedule_len_3(xc->schedule));
    //    gettimeofday(&t0, NULL);
    do_scheduled_operations(xc->schedule,tdata,parities);
    //    gettimeofday(&t1, NULL);
    free(tdata);
    //    running_time += diff_us(t0,t1);
}
#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

void ZGrouping::set_erasure(vector<int> arr)
{
    int i;
    for(i = 0;i<arr.size();i++)
    {
        erasures[i] = arr[i];
    }
    erasures[i] = -1;
    /* hard copy from jerasure */
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
    /* end hard copy */

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
        //        printf("DPG schedule len %d\n", schedule_len_3(txc.schedule));

        //        printf("cdf = %d, ddf = %d\n", cdf,ddf);
        vector<int> tid;
        for(int i = 0;i<k+m;i++)
        {
            //            printf("%d ", row_ids[i]);
            tid.push_back(row_ids[i]);
        }
        //        printf("\n");
        //        for(int i = 0;i<k+m;i++)
        //            printf("%d ", ind_to_row[i]);
        //        printf("\n");
        ids = tid;
        //        jerasure_print_bitmatrix(real_decoding_matrix,(cdf+ddf)*W, K*W,W);
        printf("intermedia len = %d / %d\n", txc.intermedia_schedule.size(), intermedia.size());
        if(txc.intermedia_schedule.size() > intermedia.size())
        {
            int old_len = intermedia.size();
            int new_len = txc.intermedia_schedule.size();
            printf("append new intermedia buffer of len %d, %d->%d\n", new_len - old_len, old_len , new_len);
            for(int i = old_len;i<new_len;i++)
            {
                //        char* tmp = (char*)aligned_alloc(32,packetsize);
                char *tmp;
                posix_memalign((void**)&tmp,32,packetsize);
                //        printf("new_inter_buf %d of size %d, %p\n",i, packetsize,tmp);
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
        //       for(int i = 0;i<txc.schedule.size();i++)
        //            printf("  %d %d %d : %d %d %d\n", txc.schedule[i][0], txc.schedule[i][1], txc.schedule[i][2], tsh[i][0], tsh[i][1], tsh[i][2]);
        //        de_schedule = (int*)malloc()
        ////        de_schedule = jerasure_generate_decoding_schedule(K,M,W,real_decoding_matrix,erasures,1);
        ////        assert(de_schedule != NULL);
        ids_map[pattern] = tid;
        de_schedule = tsh;
        de_schedules_map[pattern] = de_schedule;
    }
    free(row_ids);
    free(ind_to_row);
    free(real_decoding_matrix);
}

void printArr(char*s, char *p, int len);
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
            //            printf("tpar[%d] = data[%d]\n", i-K, ids[i]);
            tpar[i-K] = data[ids[i]];
        }
        else
        {
            //            printf("tpar[%d] = parities[%d]\n", i-K, ids[i]-K);
            tpar[i-K] = parities[ids[i] - K];
        }
    }

    //    for(int i = 0;i<K;i++)
    //        printf("Data %d=%p, tdata %d=%p\n", i,data[i], i,tdata[i]);
    //    for(int i = 0;i<M;i++)
    //        printf("Pari %d=%p, tpar %d=%p\n", i,parities[i], i,tpar[i]);

    //    for(int i = 0;i<K;i++)
    //        printArr("Datab i", data[i], packetsize*W);
    //    for(int i = 0;i<M;i++)
    //        printArr("Parib i", parities[i], packetsize*W);

    //    for(int i = 0;i<K;i++)
    //        printArr("Databb i", tdata[i], packetsize*W);
    //    for(int i = 0;i<M;i++)
    //        printArr("Paribb i", tpar[i], packetsize*W);
    do_scheduled_operations(de_schedule,tdata,tpar);

    //    for(int i = 0;i<K;i++)
    //        printArr("Dataa i", data[i], packetsize*W);
    //    for(int i = 0;i<M;i++)
    //        printArr("Paria i", parities[i], packetsize*W);


    //    for(int i = 0;i<K;i++)
    //        printArr("Dataaa i", tdata[i], packetsize*W);
    //    for(int i = 0;i<M;i++)
    //        printArr("Pariaa i", tpar[i], packetsize*W);

    //    for(int i = 0;i<K;i++)
    //        printf("Data %d=%p, tdata %d=%p\n", i,data[i], i,tdata[i]);
    //    for(int i = 0;i<M;i++)
    //        printf("Pari %d=%p, tpar %d=%p\n", i,parities[i], i,tpar[i]);


    free(tdata);
    free(tpar);
}

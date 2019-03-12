#include "zcauchy.h"
#include <sys/time.h>

char** malloc2d(int row, int col);
void free2d(char** &p, int len);
extern "C" long long diff_us(struct timeval start, struct timeval end);
extern int n_tc_cpy, n_tc_xor;
ZCauchy::ZCauchy(int tK, int tM, int tW, vector<int> &arr, bool isSmart, bool isNormal, int m_packetsize): ZCode(tK,1,1,m_packetsize)
{
//    running_time = 0;
    K = tK;
    M = tM;
    W = tW;
    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    int *matrix = cauchy_xy_coding_matrix(K,M,W,arr.data()+K,arr.data());
    if(isNormal)
        cauchy_improve_coding_matrix(K,M,W,matrix);
    bitmatrix = jerasure_matrix_to_bitmatrix(K,M,W,matrix);
    if(!isSmart)
        en_schedule = jerasure_dumb_bitmatrix_to_schedule(K,M,W,bitmatrix);
    else
        en_schedule = jerasure_smart_bitmatrix_to_schedule(K,M,W,bitmatrix);
    gettimeofday(&t1,NULL);
//    running_time += diff_us(t0,t1);
    init_time = diff_us(t0,t1);
    int i = 0;
    int n_cpy = 0,  n_xor = 0;
    while(1)
    {
        if(en_schedule[i][0] == -1)break;
//        printf("%d,%d,%d,%d,%d\n", en_schedule[i][0],en_schedule[i][1],en_schedule[i][2],en_schedule[i][3],en_schedule[i][4]);
        if(en_schedule[i][4] == 0)
            n_cpy ++;
        else
            n_xor ++;
        i++;
    }
    printf(" !!! Cauchy Schedule Len %d, memcpy %d, xor %d, cost %d, init time %d us\n", i, n_cpy, n_xor, n_cpy*3065+5998*n_xor,init_time);
//    printf("Generator Matrix:\n");
//    jerasure_print_matrix(matrix,M,K,W);
    //    printf("Bitmatrix :\n");
//    jerasure_print_bitmatrix(bitmatrix,M*W,K*W,W);
    encode_buf = malloc2d(K, W*packetsize);
    erasures = (int*) malloc(sizeof(int) * (K+M));
    free(matrix);
}

ZCauchy::~ZCauchy()
{
    free(bitmatrix);
    free2d(encode_buf, K);
    if(erasures != NULL)
        free(erasures);
}

void ZCauchy::encode_single_chunk(char* data, int len, char**& parities)
{
    int i,j;
//    struct timeval t0,t1;
    assert(data != NULL);
    assert(parities != NULL);
    assert(len ==  packetsize * K * W);
    for(i = 0;i<K;i++)
    {
        //        printf("%d: copy data len %d, from %p to %p\n", i,packetsize*W, data + i*packetsize * W, encode_buf[i]);
        memcpy(encode_buf[i], data + i*packetsize * W, packetsize * W);
    }
//    gettimeofday(&t0,NULL);
    jerasure_schedule_encode(K,M,W,en_schedule,encode_buf,parities, W*packetsize, packetsize);
//    gettimeofday(&t1,NULL);
//    running_time += diff_us(t0,t1);
//    printf("ZCauchy encode %d using %d us\n", len, diff_us(t0,t1));
}

void ZCauchy::set_erasure(vector<int> arr)
{
    unsigned long long pattern = 0LL;
    int i;
    for(i = 0;i<arr.size();i++)
    {
        erasures[i] = arr[i];
        pattern |= 1LL << arr[i];
    }
    erasures[i] = -1;
//    for(i = 0;i<arr.size()+1;i++)
//        printf("%d ", erasures[i]);
//    printf("\n");

    if(de_schedules_map.find(pattern) != de_schedules_map.end())
    {
        printf("found decode schedule for pattern %llu\n",pattern);
        de_schedule = de_schedules_map[pattern];
    }
    else
    {
        printf("new decode schedule for pattern %llu\n",pattern);
        de_schedule = jerasure_generate_decoding_schedule(K,M,W,bitmatrix,erasures,1);
        assert(de_schedule != NULL);
        de_schedules_map[pattern] = de_schedule;
    }
}

void ZCauchy::decode_single_chunk(char **&data, char **&parities)
{
    char **ptrs;
    ptrs = set_up_ptrs_for_scheduled_decoding(K, M, erasures, data, parities);   
    jerasure_do_scheduled_operations(ptrs, de_schedule, packetsize);
    //      for (i = 0; i < k+m; i++) ptrs[i] += (packetsize*w);
//    for(int i = 0;i<K;i++)
//        memcpy(out + i*packetsize*W, data[i], packetsize*W);
    free(ptrs);
}

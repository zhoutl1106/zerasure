#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
extern "C"{
#include "Jerasure-1.2A/cauchy.h"
#include "Jerasure-1.2A/jerasure.h"
#include <time.h>
}
#include "rs64.h"
#include "Algorithm/zgrouping.h"
#include "Algorithm/zcauchy.h"
#include "Algorithm/zcode.h"
#include "Algorithm/zevenodd.h"
#include "Algorithm/zstar.h"
#include "Algorithm/zrdp.h"
#include "Algorithm/zqfs.h"
//#include "zoxc.h"
#include <vector>
#include <x86intrin.h>
#include <string.h>
#include <sys/time.h>
#include <cassert>
#include "Search/zsearch.h"
#include "Search/zrandomarray.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include "functions.cpp"

using namespace std;


int main(int argc, char *argv[])
{
//    single_1result_vs_dp(argc,argv);
//    search(argc,argv);
    test_single_array();

//    if(argc != 10)
//        usage();

//    printf(" !!! \n");
//    for(int i = 1;i<9;i++)
//        printf("%s ", argv[i]);
//    printf("\n");



    /*
//    printf("Processing %s of method %d\n", argv[1], method);

//    const int packetsize = 1;
    ZCode *zc;
    bool isSmart;
    bool isNormal;
    bool isWeightedGrouping;

    switch(method)
    {
    case 0:
        // natual dumb
        isSmart = false;
        isNormal = false;
        isWeightedGrouping = true;
        break;
    case 1:
        // natual smart
        isSmart = true;
        isNormal = false;
        isWeightedGrouping = true;
        break;
    case 2:
        // natual grouping unweighed
        isSmart = true;
        isNormal = false;
        isWeightedGrouping = false;
        break;
    case 3:
        // natual grouping weightd
        isSmart = true;
        isNormal = false;
        isWeightedGrouping = true;
        break;
    case 4:
        // normal dumb
        isSmart = false;
        isNormal = true;
        isWeightedGrouping = true;
        break;
    case 5:
        // normal smart
        isSmart = true;
        isNormal = true;
        isWeightedGrouping = true;
        break;
    case 6:
        // normal unweighted
        isSmart = true;
        isNormal = true;
        isWeightedGrouping = false;
        break;
    case 7:
        // normal weighted
        isSmart = true;
        isNormal = true;
        isWeightedGrouping = true;
        break;
    case 8:
        break;
    default:
        printf("Unknown method, exiting...\n");
        exit(0);
    }
    int packetsize,M,W;
    if(method < 8)
    {
        M = atoi(argv[5]);
        W = atoi(argv[6]);
//        printf("kwm = %d,%d,%d\n",K,M,W);
//        packetsize = find_packetsize(K,M,W);
//        printf(" !!! Find packetsize = %d\n", packetsize);
        packetsize = 16384;
//        printf(" !!! packetsize = %d\n", packetsize);
        vector<int> arr = find_config(argv[2],K,M,W,method);
//        printf("arr: ");
//        for(int i = 0;i<K+M;i++)
//            printf("%d ", arr[i]);
//        printf("\n");

        if(method == 0 || method == 1 || method == 4 || method == 5)
        {
//            printf("Creating ZCauchy object, smart = %d, normal = %d\n", isSmart, isNormal);
            zc = new ZCauchy(K,M,W,arr,isSmart, isNormal, packetsize);
        }
        else if(method == 2 || method == 3 || method == 6 || method == 7)
        {
//            printf("Creating ZGrouping object, normal = %d, weighted = %d\n", isNormal, isWeightedGrouping);
            zc = new ZGrouping(K,M,W,arr,isNormal, isWeightedGrouping, packetsize);
        }
    }
    else if(method == 8)
    {
        packetsize = atoi(argv[5]);
        printf("Creating ZEvenOdd object\n");
        zc = new ZEvenOdd(K,packetsize);
    }

    //    if(strcmp(argv[1], "encode") == 0)
    //    {
    //        zc->encodeFile(argv);
    //    }
    //    else if(strcmp(argv[1], "decode") == 0)
    //    {
    //        zc->decodeFile(argv);
    //    }
    //    printf("Exiting ...\n");
    //    return 0;*/
//    int packetsize = 16384;
//    int packetsize = 1;
//    int K = atoi(argv[1]);;
//    ZCode *zc = new ZQFS(K,packetsize);
//    zc->test_speed();

//////    int packetsize = atoi(argv[3]);
//    int K,M,W;
//    vector<int> arr,arr_nat;
//    cin >> K >> M >> W;
//    printf("%d %d %d\n", K,M,W);
//    arr.resize(K+M);
//    arr_nat.resize(K+M);
//    for(int i = 0;i<K+M;i++)
//    {
//        cin>>arr[i];
//        arr_nat[i] = i;
//    }
//////    int K = atoi(argv[1]);
//////    ZCode *zc = new ZEvenOdd(K,packetsize);
//    ZCode *zc1 = new ZGrouping(K,M,W,arr_nat,true,true,packetsize);
//    ZCode *zc = new ZGrouping(K,M,W,arr,true,true,packetsize);

/*
//    int R = ((ZRDP*)zc)->R;
//    printf("K = %d, R = %d\n", K, R);
    char* dat;
    int M = 3;
    posix_memalign((void**)&dat,32,K*packetsize);
    int* p = (int*) dat;
////    for(int i = 0;i<R*K*packetsize/4;i++)
////        p[i] = i;
    for(int i = 0;i<K*packetsize;i++)
        dat[i] = i;
    char** par = malloc2d(M, packetsize);
    zc->encode_single_chunk(dat,K*packetsize,par);

    printf("\n----------\nBack to main\n");
//    for(int i = 0;i<1;i++)
//    {
//        for(int j = 0;j<K;j++)
//            printf("%d\t", dat[j*packetsize+i]);
//        printf("|\t%d\t", par[0][i]);
//        printf("%d\t", par[1][i]);
//        printf("%d\t", par[2][i]);
//        printf("\n");
//    }
//    printf("\n");

    printf("End Encode\n\n");

    int R = 1;
    char** adat = malloc2d(K, R*packetsize);
    for(int i = 0;i<K;i++)
        memcpy(adat[i],dat + i*R*packetsize,R*packetsize);


    vector<int> lost;
    lost.push_back(0);
    lost.push_back(1);
    lost.push_back(2);
//    lost.push_back(atoi(argv[2]));
//    lost.push_back(atoi(argv[3]));
//    lost.push_back(atoi(argv[4]));

    for(int i = 0;i<M;i++)
    {
        if(lost[i] < K)
        {
            printf("clear adat[%d]\n",i);
            memset(adat[lost[i]],0,R*packetsize);
        }
        else
        {
            printf("clear par[%d]\n",lost[i]-K);
            memset(par[lost[i]-K],0,R*packetsize);
        }
    }

    zc->set_erasure(lost);
    zc->decode_single_chunk(adat,par);

    printf("\nFinal\n");
//    for(int i = 0;i<R;i++)
//    {
//        for(int j = 0;j<K;j++)
//            printf("%d\t", adat[j][i]);
//        printf("\n");
//    }
//    printf("\n");

    for(int i = 0;i<K;i++)
        printf("column %d: %s\n", i,memcmp(adat[i], dat + i*R*packetsize,R*packetsize)==0?"Same":"Diff");
        */
}

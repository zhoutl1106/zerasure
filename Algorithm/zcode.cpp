#include "zcode.h"
#include <sstream>
#include <iostream>
#include <sys/time.h>
#include <algorithm>
#include <string.h>

char** malloc2d(int row, int col);
void free2d(char** &p, int len);
long diff_ms(struct timeval start, struct timeval end);
extern "C" long long diff_us(struct timeval start, struct timeval end);

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
    int size = 1024*1024*1024;
    while(size%(blocksize) != 0)
        size ++;
    int loops = size/(blocksize);
    printf("size is %d Byte, %d loops\n",size,loops);
    char *dat = (char*)malloc(size);
    for(int i = 0;i<size;i++)
    {
//        dat[i] = rand();
        dat[i] = i;
    }

    char** par = malloc2d(M, size/K);
    char** parities = new char*[M];
    for(int i = 0;i<M;i++)
        parities[i] = par[i];
    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    for(int i = 0;i<loops;i++)
    {
//        printf("encode offset %d\n", i*blocksize);
        encode_single_chunk(dat+i*blocksize,blocksize,parities);
        for(int j = 0;j<M;j++)
        {
            parities[j] += blocksize/K;
        }
    }
    gettimeofday(&t1,NULL);
    long long diff = diff_us(t0,t1);
    printf(" !!! Encode cost %d us, init %d us, total %d us, size = %d, speed = %f\n", diff, init_time, diff+init_time, size, size * 1000.0 * 1000/ 1024.0/1024 / (diff));

//    printArr(&dat,1,size,"ori dat");
//    printArr(par, M, size/K,"par");

    char** adat = malloc2d(K, size/K);

    for(int i = 0;i<loops;i++)
    {
        for(int j = 0;j<K;j++)
        {
//            printf("copy dat off %d, size %d to adat[%d]\n", i*blocksize+j*blocksize/K, blocksize/K, j);
            memcpy(adat[j]+i*blocksize/K,dat + i*blocksize+j*blocksize/K,blocksize/K);
//            printArr(adat,K, size/K, "loop");
        }
    }
//    printArr(adat,K, size/K, "before clean");

    for(int i = 0;i<M;i++)
        parities[i] = par[i];


    vector<int> lost;
    for(int i = 0;i<M;i++)
        lost.push_back(i);

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

//    printArr(adat,K, size/K, "after clean");

    set_erasure(lost);

    char** tdata = malloc2d(K, sizeof(char*));
    for(int i = 0;i<K;i++)
        tdata[i] = adat[i];
    gettimeofday(&t0,NULL);
//    char* pdat = dat;
    for(int i = 0;i<loops;i++)
    {
        decode_single_chunk(tdata,parities);
//        printArr(tdata,K, blocksize/K,"decode loop");
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
//    printArr(tdata,K, size/K,"after decode");


    bool isSame = true;
    for(int i = 0;i<loops;i++)
    {
        for(int j = 0;j<K;j++)
        {
            if(memcmp(tdata[j], dat + i*blocksize + j*blocksize/K, blocksize/K) != 0)
            {
                isSame = false;
                printf("loop %d, tdata[%d] diff\n", i,j);
//                print1DArr(tdata[j], blocksize/K, "tdata");
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

void ZCode::encode(char* &dat, int len, char** &par)
{
    char** parities = new char*[M];
    assert((len%(K*W*packetsize))== 0);
    for(int i = 0;i<M;i++)
        parities[i] = par[i];
    int loops = len / (K*W*packetsize);
    printf("Zcode, encode, loops = %d\n", loops);
    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    for(int i = 0;i<loops;i++)
    {
        encode_single_chunk(dat + i*K*W*packetsize,K*W*packetsize,parities);
        for(int j = 0;j<M;j++)
        {
            //            stringstream ss;
            //            ss << j;
            //            string name = "/home/zhou/tmp/p" + ss.str();
            //            FILE* fd = fopen(name.c_str(),"a");
            //            assert(fd != NULL);
            //            fwrite(parities[j], W*packetsize,1,fd);
            //            fclose(fd);
            parities[j] += W*packetsize;
        }
    }
    gettimeofday(&t1,NULL);
    long long diff = diff_us(t0,t1);
    printf(" !!! Encode cost %d us, init %d us, total %d us, size = %d, speed = %f\n", diff, init_time, diff+init_time, len, len * 1000.0 * 1000/ 1024.0/1024 / (diff));

    delete [] parities;
}

void ZCode::encodeFile(char* argv[])
{
    string infile = argv[7];
    string outDir = argv[8];
    string filename;
    int i = infile.rfind('/');
    if(i == string::npos)
        filename = infile;
    else
        filename = infile.substr(i+1,infile.length());
    long long size;
    FILE *p = fopen(infile.c_str(),"rb");
    fseek(p,0,SEEK_END);
    size = ftell(p);
    fseek(p,0,SEEK_SET);
    while(size%(K*W*packetsize) != 0)
        size++;

    printf("Encoding %d,%d,%d,infile = %s,\n filename = %s,\n outDir = %s,\n size = %lld\n", K,M,W,infile.c_str(), filename.c_str(), outDir.c_str(),size);

    char* infileData;// = new char[size];
    posix_memalign((void**)&infileData,32,size);
    fread(infileData,size,1,p);
    fclose(p);
    char** parities = malloc2d(M, size/K);
    encode(infileData,size, parities);
    //    printf("End encoding, size = %lld, time = %d us, speed = %f MB/s\n", size, running_time, size * 1000 * 1000/ 1024.0/1024/running_time);

    //    char* pdata = infileData;
    //    for(int i = 0;i<size/W/packetsize/K;i++)
    //        for(int j = 0;j<K;j++)
    //        {
    //            stringstream ss;
    //            ss << j;
    //            string name = outDir + filename + "." + ss.str();
    //            FILE *fd;
    //            if(i == 0) fd = fopen(name.c_str(), "w");
    //            else fd = fopen(name.c_str(), "a+");
    //            if(fd == NULL)
    //            {
    //                printf("Cannot open %s\n", name.c_str());
    //                exit(-1);
    //            }
    //            fwrite(pdata, W*packetsize, 1, fd);
    //            fclose(fd);
    //            pdata += W*packetsize;
    //        }
    //    for(int i = 0;i<M;i++)
    //    {
    //        stringstream ss;
    //        ss << i + K;
    //        string name = outDir + filename + "." + ss.str();
    //        FILE *fd = fopen(name.c_str(), "w");
    //        if(fd == NULL)
    //        {
    //            printf("Cannot open %s\n", name.c_str());
    //            exit(-1);
    //        }
    //        fwrite(parities[i], size/K, 1, fd);
    //        fclose(fd);
    //    }
    free2d(parities, M);
    free(infileData);

    //    string name = outDir + filename + ".meta";
    //    FILE *fd = fopen(name.c_str(), "wt");
    //    fprintf(fd,"%d %d %d\n", K,M,W);
    //    fprintf(fd,"%d\n", size);
    //    fclose(fd);
}

void ZCode::decode(char** &dat, char** &par, int len)
{
    assert((len%(K*W*packetsize))== 0);
    int loops = len / (K*W*packetsize);
    printf("Zcode, decode, loops = %d\n", loops);
    char** tdata = (char**)malloc(K*sizeof(char*));
    char** tpar = (char**)malloc(M*sizeof(char*));
    for(int i = 0;i<K;i++)
        tdata[i] = dat[i];
    for(int i = 0;i<M;i++)
        tpar[i] = par[i];
    struct timeval t0,t1;
    gettimeofday(&t0,NULL);
    for(int i = 0;i<loops;i++)
    {
        decode_single_chunk(tdata,tpar);
        for(int j = 0;j<K;j++)
            tdata[j] += W*packetsize;
        for(int j = 0;j<M;j++)
            tpar[j] += W*packetsize;
    }
    gettimeofday(&t1,NULL);
    long long diff = diff_us(t0,t1);
    printf(" !!! Decode cost %d us, init %d us, total %d us, size = %d, speed = %f\n", diff, init_time, diff+init_time, len, len * 1000.0 * 1000/ 1024.0/1024 / (diff));

    free(tdata);
    free(tpar);
}

void ZCode::decodeFile(char* argv[])
{
    string filename = argv[7];
    string outDir = argv[8];
    int size;
    int blocksize;

    FILE *fd = fopen((outDir + filename + ".meta").c_str(), "rt");
    if(fd == NULL)
    {
        printf("%s.meta not exist in %s\n", filename.c_str(), outDir.c_str());
        exit(-1);
    }
    fscanf(fd, "%d %d %d\n", &K,&M,&W);
    fscanf(fd, "%d\n", &size);
    blocksize = size/K;

    printf("Decoding %d,%d,%d,filename = %s,\n outDir = %s, size = %d\n", K,M,W, filename.c_str(), outDir.c_str(), size);

    vector<int> lost(M,-1);
    for(int i = 0;i<M;i++)
        cin >> lost[i];
    sort(lost.begin(),lost.end());
    printf("Lost : ");
    for(int i = 0;i<M;i++)
        printf("%d ", lost[i]);
    printf("\n");

    char** data = malloc2d(K,size/K);
    char** parities = malloc2d(M, size/K);
    int lost_idx = 0;
    for(int i = 0;i<K+M;i++)
    {
        if(i == lost[lost_idx])
        {
            printf("Skip %d due to lost\n", lost[lost_idx]);
            lost_idx ++;
            continue;
        }
        stringstream ss;
        ss << i;
        string name = outDir + filename + "." + ss.str();
        if(i<K)
        {
            printf("Reading data %s\n", name.c_str());
            FILE *fd = fopen(name.c_str(), "r");
            fread(data[i], blocksize, 1, fd);
            fclose(fd);
        }
        else
        {
            printf("Reading parity %s\n", name.c_str());
            FILE *fd = fopen(name.c_str(), "r");
            fread(parities[i-K], blocksize, 1, fd);
            fclose(fd);
        }
    }
    set_erasure(lost);
    decode(data,parities,size);

    string name = outDir + filename + ".decoded";
    printf("Writing decoded data to %s\n", name.c_str());
    fd = fopen(name.c_str(), "w");
    for(int i = 0;i<size/K/W/packetsize;i++)
        for(int j = 0;j<K;j++)
        {
            fwrite(data[j]+i*W*packetsize, W*packetsize, 1, fd);
        }
    fclose(fd);

    free2d(parities, M);
    free2d(data, K);
}

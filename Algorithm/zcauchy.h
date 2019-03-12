#ifndef ZCAUCHY_H
#define ZCAUCHY_H
#include <stdlib.h>
#include <string.h>
#include <map>
extern "C"{
#include "Jerasure-1.2A/jerasure.h"
#include "Jerasure-1.2A/cauchy.h"
}
#include <cassert>
#include <vector>
#include "zcode.h"
using namespace std;

class ZCauchy : public ZCode
{
public:
    ZCauchy(int tK, int tM, int tW, vector<int> &arr, bool isSmart, bool isNormal, int packetsize);
    ~ZCauchy();
    void encode_single_chunk(char* data, int len, char**& parities);
    void set_erasure(vector<int> arr);
    void decode_single_chunk(char** &data, char** &parities);
    int* bitmatrix;
private:
//    int K;
//    int M;
//    int W;
    int **en_schedule;
    int **de_schedule;
    char** encode_buf;
    int* erasures;
    map<unsigned long long, int**> de_schedules_map;
};

#endif // ZCAUCHY_H

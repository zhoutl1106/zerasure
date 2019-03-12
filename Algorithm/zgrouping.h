#ifndef ZDPG_H
#define ZDPG_H

#include "zoxc.h"
#include <map>
#include <vector>
#include <cassert>
#include "zcode.h"
using namespace std;

class ZGrouping : public ZCode
{
public:
    ZGrouping(int tK, int tM, int tW, vector<int> &arr, bool isNormal, bool isWeightedGrouping, int packagesize);
    ~ZGrouping();
    void encode_single_chunk(char* data, int len, char**& parities);
    void set_erasure(vector<int> arr);
    void decode_single_chunk(char** &data, char** &parities);

private:
    int *matrix;
    int *bitmatrix;
    vector<int*> de_schedule;
    ZOXC* xc;
    void do_scheduled_operations(vector<int*>& schedule, char **&data, char **&parities);
    vector<char* > intermedia;
    int* erasures;
    vector<int> ids;
    map<unsigned long long, vector<int> > ids_map;
    map<unsigned long long, vector<int*> > de_schedules_map;
};

#endif // ZDPG_H

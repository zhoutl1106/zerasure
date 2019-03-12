#ifndef ZCODE_H
#define ZCODE_H

#include <map>
#include <vector>
#include <string>
#include <cassert>
using namespace std;

class ZCode
{
public:
    enum code_t{CAUCHY, GROUPING};
    ZCode(int tk, int tm, int tw, int mp);
    void encode(char* &dat, int len, char** &par);
    void decode(char** &dat, char** &par, int len);
    void encodeFile(char* argv[]);
    void decodeFile(char* argv[]);
    void test_speed();

    virtual void encode_single_chunk(char* data, int len, char**& parities)=0;
    virtual void set_erasure(vector<int> arr)=0;
    virtual void decode_single_chunk(char** &data, char** &parities)=0;

    int K;
    int M;
    int W;
    int packetsize;
    int blocksize;
//    long long running_time;
    long long init_time;
};

#endif // ZCODE_H

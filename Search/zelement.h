#ifndef ZELEMENT_H
#define ZELEMENT_H

#include <vector>

using namespace std;

class ZElement
{
public:
    enum cost_type{COST_XOR, COST_SUM, COST_WEIGHTED};
    ZElement(int *p=NULL);
    ZElement(vector<int> p);
    long long value();
    long long v;            // save the ret of value(), avoid re-compute

    static void test_cost_weight(int size = 100*1024*1024, int loops = 10);
    static int cpy_weight;
    static int xor_weight;

    // must be called before create any object
    static void init(int tK, int tM, int tW, int tcost, int tstrategy);

    static int K;
    static int M;
    static int W;
    static int cost_func; // 0: # of XOR, 1: # of ops, 2: weight sum
    static int strategy; // see paper, 0-7, strategy-(@/4,@%4)
    static bool isInited; // ensure init() has been called

    vector<int> array;
};

#endif // ZELEMENT_H

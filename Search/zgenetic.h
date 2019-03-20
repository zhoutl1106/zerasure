#ifndef ZXORGENETIC_H
#define ZXORGENETIC_H
#include "zelement.h"
#include "zrandomarray.h"
#include <set>

using namespace std;

class ZGenetic
{
public:
    ZGenetic(int tK, int tM, int tW, int tS, int init, double tselect_rate, double tcrossover_rate,double tmutation_rate, int tmax_population);
    long long mutation(ZElement* e);
    ZElement* cross_over(ZElement* e1,ZElement* e2);
    void run();


private:
    ZRandomArray *zra;
    int K;
    int M;
    int W;
    int S;
    double select_rate;
    double crossover_rate;
    double mutation_rate;
    int max_population;
    set<ZElement*> population;

    long long min_best;         // best value in population
    long long min_worst;        // worst value in population
    int *min_array;
    set<int> rm_ori;
};

#endif // ZXORGENETIC_H

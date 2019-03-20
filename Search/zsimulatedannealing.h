#ifndef ZSIMULATEDANNEALING_H
#define ZSIMULATEDANNEALING_H

#include "zrandomarray.h"

class ZSimulatedAnnealing
{
public:
    ZSimulatedAnnealing(int tK, int tM, int tW, int tS, double tacc_rate);
    double annealing(int idx);
    void run();

private:
    int K;
    int M;
    int W;
    int S;
    double acc_rate;
    ZRandomArray *zra;
};

#endif // ZSIMULATEDANNEALING_H

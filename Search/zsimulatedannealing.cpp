#include "zsimulatedannealing.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include "zelement.h"

ZSimulatedAnnealing::ZSimulatedAnnealing(int tK, int tM, int tW, int tS, double tacc_rate)
{
    K = tK;
    M = tM;
    W = tW;
    S = tS;
    acc_rate = tacc_rate;
    zra = new ZRandomArray(W*W);
    printf("Begin Simulated Annealing, K = %d, M = %d, W = %d, S = %d, init accept rate = %f\n", K,M,W,S,acc_rate);
}

double ZSimulatedAnnealing::annealing(int idx)
{
    // updated with your own annealing function
    return acc_rate * exp(-0.05*idx);
}

void ZSimulatedAnnealing::run()
{
    long long min_sa = 1e10;        // minimum of simulated annealing, COULD increase during iterating
    long long min_global = 1e10;    // global minimum, NEVER increase
    int cnt = 0;                    // # of iterations that global minimum haven't changed
    long long T = 0;                // time index
    int min_array[K+M];             // optimized X,Y array
    fprintf(stderr, "\nK\tM\tW\tcnt\t%10s\t%10s\n","min_global","min_sa");
    fprintf(stderr, "------------------------------------------------------------\n");

    while(1)
    {
        long long ret;
        int change = (1-cnt*1.0/S)*(K+M);       // less elements will be changed as cnt reaching S
        int *p = zra->next_random(K+M, change);
        ZElement e(p);
        ret = e.value();
        if(ret < min_global)
        {
            min_sa = ret;
            memcpy(min_array, p, (K+M)*sizeof(int));
            min_global = ret;
            cnt = 0;
        }
        else if(ret < min_sa)
            min_sa = ret;
        else if(static_cast <double> (rand()) / static_cast <double> (RAND_MAX) < annealing(T))
        {
            min_sa = ret;
        }
        else
        {
            cnt ++;
            if(cnt == S)
                break;      // exit if No update of min_sa nor min_global in S continuous iterations
        }
        T++;
        fprintf(stderr,"%d \t%d \t%d \t%d \t%10lld \t%10lld\n", K,M,W,cnt, min_global, min_sa);  // used to plot figure and monitor prograss.
    }
    fprintf(stderr, "------------------------------------------------------------\n");
    // output
    printf("%d %d %d ", K,M,W);
    for(int i = 0;i<K+M;i++)
        printf("%d ", min_array[i]);
    printf("\n");
}

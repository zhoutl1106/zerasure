#include "zgenetic.h"
#include <stdio.h>
#include <string.h>

ZGenetic::ZGenetic(int tK, int tM, int tW, int tS, int init, double tselect_rate, double tcrossover_rate,double tmutation_rate, int tmax_population)
{
    K = tK;
    M = tM;
    W = tW;
    S = tS;
    select_rate = tselect_rate;
    crossover_rate = tcrossover_rate;
    mutation_rate = tmutation_rate;
    max_population = tmax_population;
    zra = new ZRandomArray(W*W);
    min_array = new int[K+M];
    sum = 0;
    min_best = 1e9;
    // generate first init elements as seed
    for(int i = 0;i<init;i++)
    {
//        ZElement* peo = new ZElement(K,M,W);
////        int ret;
//        int *p = zra->next_random(K+M, K+M);
//        peo->array.resize(K+M);
//        memcpy(peo->array.data(),p,(K+M)*sizeof(int));
//        ret = got_1_result(K,M,W,p,p+K);
//        sum += ret;
//        peo->value = ret;

//        if(ret > min_worst)
//            min_worst = ret;
//        if(ret < min_best)
//        {
//            min_best = ret;
//            memcpy(min_array, p, (K+M)*sizeof(int));
//        }
//        population.insert(peo);
    }
}

void ZGenetic::mutation(ZElement *e)
{
//    for(int i = 0;i<W*W;i++)
//        rm.insert(i);

}

ZElement* ZGenetic::cross_over(ZElement *e1, ZElement *e2)
{
}

void ZGenetic::run()
{

}

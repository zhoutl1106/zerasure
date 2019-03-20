#include "zexample.h"
#include "../Search/zelement.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "../Search/zsimulatedannealing.h"
#include "../Search/zgenetic.h"
using namespace std;

ZExample::ZExample()
{

}

void ZExample::test_cost_weight(int argc, char *argv[])
{
    ZElement* e = new ZElement();
    if(argc == 2)
        e->test_cost_weight();
    else if(argc == 3)
        e->test_cost_weight(atoi(argv[2]));
    else if(argc == 4)
        e->test_cost_weight(atoi(argv[2]),atoi(argv[3]));
    else
        printf("usage: size loops\n");
    delete e;
}

void ZExample::single(int argc, char *argv[])
{
    if(argc == 4)
    {
        int K,M,W,costf,stra;
        cin >> K >> M >> W;
        costf = atoi(argv[2]);
        stra = atoi(argv[3]);
        vector<int> arr(K+M,0);
        for(int i = 0;i<K+M;i++)
            cin >> arr[i];

        printf("cost_func = %d, strategy = %d, K = %d, M = %d, W = %d, arr = ",costf,stra,K,M,W);
        for(int i = 0;i<K+M;i++)
            printf("%d ",arr[i]);
        printf("\n");
        ZElement::init(K,M,W,costf,stra);
        ZElement* e = new ZElement(arr.data());
        long long ret = e->value();
        printf("Cost for given input array : %d\n", ret);
        delete e;
    }
    else
        printf("usage: cost_func[0..2] strategy[0..7]\n");
}

void ZExample::sa(int argc, char *argv[])
{
    if(argc == 9)
    {
        int K,M,W,S,costf,stra;
        double acc_rate;
        K = atoi(argv[2]);
        M = atoi(argv[3]);
        W = atoi(argv[4]);
        S = atoi(argv[5]);
        acc_rate = atof(argv[6]);
        costf = atoi(argv[7]);
        stra = atoi(argv[8]);

        printf("cost_func = %d, strategy = %d\n", costf, stra);
        ZElement::init(K,M,W,costf,stra);
        ZSimulatedAnnealing sa(K,M,W,S,acc_rate);
        sa.run();
    }
    else
        printf("usage: K M W S acc_rate cost_func[0..2] strategy[0..7]\n");
}

void ZExample::ge(int argc, char *argv[])
{
    if(argc == 13)
    {
        int K,M,W,S,init_pop, max_pop,costf,stra;
        double select_rate, crossover_rate, mutation_rate;
        K = atoi(argv[2]);
        M = atoi(argv[3]);
        W = atoi(argv[4]);
        S = atoi(argv[5]);
        init_pop = atoi(argv[6]);
        select_rate = atof(argv[7]);
        crossover_rate = atof(argv[8]);
        mutation_rate = atof(argv[9]);
        max_pop = atoi(argv[10]);
        costf = atoi(argv[11]);
        stra = atoi(argv[12]);

        printf("cost_func = %d, strategy = %d\n", costf, stra);
        ZElement::init(K,M,W,costf,stra);
        ZGenetic ge(K,M,W,S,init_pop,select_rate,crossover_rate,mutation_rate,max_pop);
        ge.run();
    }
    else
        printf("usage: K M W S init_population select_rate crossover_rate tmutation_rate max_population cost_func[0..2] strategy[0..7]\n");
}

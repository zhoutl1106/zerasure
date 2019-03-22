/*
zexample.cpp
Tianli Zhou

Fast Erasure Coding for Data Storage: A Comprehensive Study of the Acceleration Techniques

Revision 1.0
Mar 20, 2019

Tianli Zhou
Department of Electrical & Computer Engineering
Texas A&M University
College Station, TX, 77843
zhoutianli01@tamu.edu

Copyright (c) 2019, Tianli Zhou
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

- Neither the name of the Texas A&M University nor the names of its
  contributors may be used to endorse or promote products derived
  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include "zexample.h"
#include "../Search/zelement.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "../Search/zsimulatedannealing.h"
#include "../Search/zgenetic.h"
#include "../Algorithm/zcauchy.h"
#include "../Algorithm/zgrouping.h"
using namespace std;

ZExample::ZExample()
{

}

void ZExample::test_cost_weight(int argc, char *argv[])
{
    ZElement::init(0,0,0,0,0);
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

void ZExample::code(int argc, char *argv[])
{
    if(argc == 4)
    {
        int K,M,W,packetsize,stra;
        cin >> K >> M >> W;
        packetsize = atoi(argv[2]);
        stra = atoi(argv[3]);
        vector<int> arr(K+M,0);
        for(int i = 0;i<K+M;i++)
            cin >> arr[i];

        printf("packetsize = %d, strategy = %d, K = %d, M = %d, W = %d, arr = ",packetsize,stra,K,M,W);
        for(int i = 0;i<K+M;i++)
            printf("%d ",arr[i]);
        printf("\n");

        //    const int packetsize = 1;
        ZCode *zc;
        bool isSmart;
        bool isNormal;
        bool isWeightedGrouping;

        switch(stra)
        {
        case 0:
            // natual dumb
            isSmart = false;
            isNormal = false;
            zc = (ZCode*) new ZCauchy(K,M,W,arr,isSmart,isNormal,packetsize);
            break;
        case 1:
            // natual smart
            isSmart = true;
            isNormal = false;
            zc = (ZCode*) new ZCauchy(K,M,W,arr,isSmart,isNormal,packetsize);
            break;
        case 2:
            // natual grouping unweighed
            isNormal = false;
            isWeightedGrouping = false;
            zc = (ZCode*) new ZGrouping(K,M,W,arr,isNormal,isWeightedGrouping,packetsize);
            break;
        case 3:
            // natual grouping weightd
            isNormal = false;
            isWeightedGrouping = true;
            zc = (ZCode*) new ZGrouping(K,M,W,arr,isNormal,isWeightedGrouping,packetsize);
            break;
        case 4:
            // normal dumb
            isSmart = false;
            isNormal = true;
            zc = (ZCode*) new ZCauchy(K,M,W,arr,isSmart,isNormal,packetsize);
            break;
        case 5:
            // normal smart
            isSmart = true;
            isNormal = true;
            zc = (ZCode*) new ZCauchy(K,M,W,arr,isSmart,isNormal,packetsize);
            break;
        case 6:
            // normal unweighted
            isNormal = true;
            isWeightedGrouping = false;
            zc = (ZCode*) new ZGrouping(K,M,W,arr,isNormal,isWeightedGrouping,packetsize);
            break;
        case 7:
            // normal weighted
            isNormal = true;
            isWeightedGrouping = true;
            zc = (ZCode*) new ZGrouping(K,M,W,arr,isNormal,isWeightedGrouping,packetsize);
            break;
        default:
            printf("Unknown strategy, exiting...\n");
            exit(0);
        }
        zc->test_speed();
    }
    else
        printf("usage: packetsize strategy[0..7]\n");
}

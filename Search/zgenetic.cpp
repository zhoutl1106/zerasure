/*
zgenetic.cpp
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
#include "zgenetic.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <map>
#include <algorithm>
using namespace std;

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
    zra = new ZRandomArray(1<<W);
    min_array = new int[K+M];
    min_best = 1e9;
    min_worst = -1;

    for(int i = 0;i<(1<<W);i++)
        rm_ori.insert(i);

    long long ret;
    // generate first init elements as seed
    fprintf(stderr, "Genetic, K = %d, M = %d, W = %d, S = %d, init_pop = %d\n",K,M,W,S,init);
    fprintf(stderr,"select rate = %f, crossover rate = %f, mutation rate = %f, max pop = %d\n",select_rate, crossover_rate, mutation_rate, max_population);
    fprintf(stderr, "initing first %d elements\n", init);
    for(int i = 0;i<init;i++)
    {
        int *p = zra->next_random(K+M, K+M);
        ZElement* peo = new ZElement(p);
        ret = peo->value();
        if(ret > min_worst)
        {
            min_worst = ret;
        }
        if(ret < min_best)
        {
            min_best = ret;
            memcpy(min_array, p, (K+M)*sizeof(int));
        }
        population.insert(peo);
    }
}

long long ZGenetic::mutation(ZElement *e)
{
    int p1 = rand() % (K+M);
    set<int> rm = rm_ori;
    for(int i = 0;i<K+M;i++)
        rm.erase(e->array[i]);
    int p2;
    if(rm.size() == 0)
        return e->v;
    else
        p2 = rand() % rm.size();
    set<int>::iterator it = rm.begin();
    advance(it, p2);
    int x = *it;
    e->array[p1] = x;
    return e->value();
}

template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                   flip_pair<A,B>);
    return dst;
}

ZElement* ZGenetic::cross_over(ZElement *e1, ZElement *e2)
{
    // 1. Get all XYs in parents
    map<int,int> condidates;
    for(vector<int>::iterator it1 = e1->array.begin(); it1 != e1->array.end(); it1++)
        condidates[(*it1)] ++;
    for(vector<int>::iterator it1 = e2->array.begin(); it1 != e2->array.end(); it1++)
        condidates[(*it1)] ++;

    // 2. insert most common elements first until length is K+M
    vector<int> arr;
    std::multimap<int, int> dst = flip_map(condidates);
    for(multimap<int,int>::reverse_iterator it5 = dst.rbegin(); it5 != dst.rend() && arr.size() < K+M; it5 ++)
    {
        arr.push_back(it5->second);
    }
    return new ZElement(arr);
}

void ZGenetic::run()
{
    int cnt = 0;
    long long ret;
    int parent_cnt;
    int threshold;
    fprintf(stderr, "Running\n");
    fprintf(stderr, "\ncnt \t%10s\t%10s\n","population","min");
    fprintf(stderr, "------------------------------------------------------------\n");

    while(1)
    {
        // Cross over, pick first {select_rate} between {min_worst and min_best}
        // crossover then insert child, modify {free_slots},
        parent_cnt = 0;
        set<ZElement*>::iterator parent0;
        set<ZElement*>::iterator parent1;
        threshold = min_best + (min_worst-min_best) * select_rate;
        for (set<ZElement*>::iterator it = population.begin(); it != population.end(); it++)
        {
            if((*it)->v <= threshold)
            {
                // found a parent condidate
                if(parent_cnt == 0)
                {
                    parent0 = it;
                    parent_cnt ++;
                }
                else
                {
                    parent1 = it;
                    parent_cnt = 0;

                    // found 2 parents, crossover under {crossover_rate}
                    if(static_cast <double> (rand()) / static_cast <double> (RAND_MAX) < crossover_rate)
                    {
                        ZElement* child = cross_over(*parent0, *parent1);

                        ret = child->value();

                        if(ret > min_worst)
                            min_worst = ret;
                        if(ret < min_best)
                        {
                            cnt = 0;
                            min_best = ret;
                            memcpy(min_array, child->array.data(), (K+M)*sizeof(int));
                        }
                        population.insert(child);
                    }
                }
            }
        }

        // Mutation
        for (set<ZElement*>::iterator it = population.begin(); it != population.end(); it++)
        {
            if(static_cast <double> (rand()) / static_cast <double> (RAND_MAX) < mutation_rate)
            {
                ret = mutation(*it);
                if(ret > min_worst)
                    min_worst = ret;
                if(ret < min_best)
                {
                    cnt = 0;
                    min_best = ret;
                    memcpy(min_array, (*it)->array.data(), (K+M)*sizeof(int));
                }
            }
        }

        // Weed out if reach max_population
        int weed_threshold = min_worst - (min_worst-min_best) * select_rate;
        while(population.size() > max_population)
        {
            int reduceCnt = 0;
            for(set<ZElement*>::iterator it= population.begin(); it!=population.end();)
            {
                if((*it)->v > weed_threshold)
                {
                    population.erase(it++);
                    reduceCnt++;
                    break;
                }
                else
                    it++;
            }

            // if no element weed out, increase threshold
            if(reduceCnt == 0)
                weed_threshold --;
        }

        // iteration result
        fprintf(stderr, "%d \t%10d \t%10d\n",cnt,population.size(),min_best);

        cnt ++;
        if(cnt > S)
        {
            break;
        }
    }
    fprintf(stderr, "------------------------------------------------------------\n");
    printf("%d %d %d ", K,M,W);
    for(int i = 0;i<K+M;i++)
        printf("%d ", min_array[i]);
    printf("\n");
    exit(0);
}

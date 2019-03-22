/*
zgenetic.h
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
    set<int> rm_ori;            // used for delete from in crossover phase
};

#endif // ZXORGENETIC_H

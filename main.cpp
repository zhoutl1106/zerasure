/*
main.cpp
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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Example/zexample.h"

void usage()
{
    printf("Usage:\n");
    printf("\t./zerasure test_cost_weight [size] [loops]\n");
    printf("\t\t- Test the weight of memcpy and XOR in the schedule, \n\t\tusing data size [size] and run [loops] times\n\n");

    printf("\t... | ./zerasure single cost_func[0..2] strategy[0..7]\n");
    printf("\t\t- Read K,M,W,X,Y from stdin, generate schedule for given X,Y array\n");
    printf("\t\tusing cost function [cost_func] and strategy [strategy]\n\n");

    printf("\t./zerasure sa K M W S acc_rate cost_func[0..2] strategy[0..7]\n");
    printf("\t\t- Perform simulated annealing optimization, stop if minimum\n");
    printf("\t\thaven't change for S consecutive iterations. Initial rate\n");
    printf("\t\tof accept worse successor is given as acc_rate\n");
    printf("\t\tusing cost function [cost_func] and strategy [strategy]\n\n");

    printf("\t./zerasure ge K M W S init_population select_rate crossover_rate\n");
    printf("\t           tmutation_rate max_population cost_func[0..2] strategy[0..7]\n");
    printf("\t\t- Perform genetic optimization using given parameters, stop if minimum\n");
    printf("\t\thaven't change for S consecutive iterations. Initial rate\n");
    printf("\t\tusing cost function [cost_func] and strategy [strategy]\n\n");

    printf("\t... | ./zerasure code packetsize strategy[0..7]\n");
    printf("\t\t- Read K,M,W,X,Y from stdin, generate schedule for given\n");
    printf("\t\tusing cost function [cost_func] and strategy [strategy]\n");
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        usage();
        exit(-1);
    }

#ifdef VEC128
    printf("Using 128-bit vectorization\n");
#elif VEC256
    printf("Using 256-bit vectorization\n");
#else
    printf("Unknown Vectorization\n");
#endif
    printf("\n\n ********* Start zerasure *********\n\n");

    if(strcmp(argv[1], "test_cost_weight") == 0)
        ZExample::test_cost_weight(argc,argv);

    if(strcmp(argv[1], "single") == 0)
        ZExample::single(argc,argv);

    if(strcmp(argv[1], "sa") == 0)
        ZExample::sa(argc,argv);

    if(strcmp(argv[1], "ge") == 0)
        ZExample::ge(argc,argv);

    if(strcmp(argv[1], "code") == 0)
        ZExample::code(argc,argv);

    printf("\n\n *********  End zerasure **********\n\n");
    return 0;
}

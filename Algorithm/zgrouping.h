/*
zgrouping.h
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
#ifndef ZDPG_H
#define ZDPG_H

#include "zoxc.h"
#include <map>
#include <vector>
#include <cassert>
#include "zcode.h"
using namespace std;

class ZGrouping : public ZCode
{
public:
    ZGrouping(int tK, int tM, int tW, vector<int> &arr, bool isNormal, bool isWeightedGrouping, int packagesize);
    ~ZGrouping();
    void encode_single_chunk(char* data, int len, char**& parities);
    void set_erasure(vector<int> arr);
    void decode_single_chunk(char** &data, char** &parities);

private:
    int *matrix;
    int *bitmatrix;
    vector<int*> de_schedule;
    ZOXC* xc;
    void do_scheduled_operations(vector<int*>& schedule, char **&data, char **&parities);
    vector<char* > intermedia;
    int* erasures;
    vector<int> ids;
    map<unsigned long long, vector<int> > ids_map;
    map<unsigned long long, vector<int*> > de_schedules_map;
};

#endif // ZDPG_H

/*
zoxc.h
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
#ifndef ZOXC_H
#define ZOXC_H

#include <vector>
#include <stdio.h>

using namespace std;

// Simple Graph definition
class znode;
class zedge
{
public:
    int weight;
    znode *n0;
    znode *n1;
};

class znode
{
public:
    int id;
    int degree;
    vector<zedge *>e;
};

class zgraph
{
public:
    vector<znode*> all_nodes;
    vector<zedge *> all_edges;
    znode* find_node(int id)
    {
        for(vector<znode*>::iterator it = all_nodes.begin();it != all_nodes.end();it++)
            if((*it)->id == id)
                return *it;
        return 0;
    }

    zedge* find_edge(int id0, int id1)
    {
        for(vector<zedge*>::iterator it = all_edges.begin();it != all_edges.end();it++)
            if(((*it)->n0->id == id0 && (*it)->n1->id == id1) ||
                    ((*it)->n0->id == id1 && (*it)->n1->id == id0))
                return *it;
        return 0;
    }

    void set_node_degree()
    {
        for(vector<znode*>::iterator it = all_nodes.begin();it != all_nodes.end();it++)
        {
            int degree = 0;
            for(vector<zedge*>::iterator it1 = (*it)->e.begin();it1 != (*it)->e.end(); it1++)
            {
                if((*it1)->weight > 0)
                    degree++;
            }
            (*it)->degree = degree;
        }
    }

    void print()
    {
        printf("Printing Graph\n");
        for(vector<znode*>::iterator it = all_nodes.begin();it != all_nodes.end();it++)
        {
            printf("%d[%d]: \n", (*it)->id, (*it)->degree);
            for(vector<zedge*>::iterator it1 = (*it)->e.begin();it1 != (*it)->e.end(); it1++)
            {
                if((*it1)->weight > 0)
                    printf("  [%d] %d -> %d\n", (*it1)->weight, (*it1)->n0->id, (*it1)->n1->id);
            }
        }
    }
};

class ZOXC
{
public:
    ZOXC(int width, int height);
    ~ZOXC();
    int grouping_1s(int *bm, bool isWeighted);
    int bitmatrix_to_graph(int *bm);
    vector<int* >intermedia_schedule;  //(s1bit, s2bit, intermedia_index)
    vector<int* >schedule;
    // (sbit, dbit, op),
    // op =
    //      0 for copy data,
    //      1 for xor data,
    //      2 for copy from intermedia,
    //      3 for xor from intermedia,
    //      4 for copy to intermedia,
    //      5 for xor to intermedia

private:
    vector<int> bm_copy;
    int m_width;
    int m_height;
    zgraph *g;
    int set_sg(int weight, bool isWeighted, bool isGenSchedule);
    void clear_g_weight();
    int n_vertices;
    int n_XOR;
    void print_bm(int *bm);
    int intermedia_index;
    int* new_schedule_element(int s, int d, int op);
    void add_new_intermedia(int s1, int s2, int idx);
};

#endif // ZOXC_H

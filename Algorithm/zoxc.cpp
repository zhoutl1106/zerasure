/*
zoxc.cpp
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

- Neither the name of the University of Tennessee nor the names of its
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
#include "zoxc.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/matching.h>
#include "../utils.h"
using namespace lemon;
using namespace std;

#define E 1000

#include <cassert>

ZOXC::ZOXC(int width, int height)
{
    n_XOR = 0;
    m_width = width;
    m_height = height;
    g = new zgraph;
    for(int i = 0;i<width;i++)
    {
        znode *n = new znode;
        n->id = i;
        n->degree = 0;
        g->all_nodes.push_back(n);
    }

    for(int i = 0;i<width-1;i++)
    {
        for(int j = i+1;j<width;j++)
        {
            zedge *te;
            te = new zedge;
            te->weight = 0;
            te->n0 = g->find_node(i);
            te->n1 = g->find_node(j);
            te->n0->e.push_back(te);
            te->n1->e.push_back(te);
            g->all_edges.push_back(te);
        }
    }
}

ZOXC::~ZOXC()
{
    for(int i = 0;i<g->all_edges.size();i++)
    {
        delete g->all_edges[i];
    }
    for(int i = 0;i<g->all_nodes.size();i++)
    {
        g->all_nodes[i]->e.clear();
        delete g->all_nodes[i];
    }

    g->all_edges.clear();
    g->all_nodes.clear();

    delete g;
    freeVectorPointer(intermedia_schedule);
    freeVectorPointer(schedule);
}

void ZOXC::clear_g_weight()
{
    for(vector<zedge*>::iterator it = g->all_edges.begin(); it != g->all_edges.end(); it++)
        (*it)->weight = 0;
}

int* ZOXC::new_schedule_element(int s, int d, int op)
{
    int *ret = (int*)malloc(3*sizeof(int));
    ret[0] = s;
    ret[1] = d;
    ret[2] = op;
    return ret;
}

void ZOXC::add_new_intermedia(int s1, int s2, int idx)
{
    int* tmp = (int*)malloc(3*sizeof(int));
    tmp[0] = s1;
    tmp[1] = s2;
    tmp[2] = idx;
    intermedia_schedule.push_back(tmp);
}

// perform 1-layer greedy matching as described in huang's paper
int ZOXC::set_sg(int weight, bool isWeighted, bool isGenSchedule = false)
{
    bool ret = false;
    ListGraph sg;
    for(int i = 0;i<m_width;i++)
    {
        sg.addNode();
    }

    for(vector<zedge*>::iterator it = g->all_edges.begin(); it != g->all_edges.end(); it++)
    {
        if((*it)->weight == weight)
        {
            ret = true;
            sg.addEdge(sg.nodeFromId((*it)->n0->id), sg.nodeFromId((*it)->n1->id));
        }
    }

    if(ret == false) return 0;

    ListGraph::EdgeMap<int> cost(sg);

    for(ListGraph::EdgeIt e(sg); e != INVALID; ++e)
    {
        if(isWeighted)
        {
            int id1 = sg.id(sg.u(e));
            int id2 = sg.id(sg.v(e));
            int d1 = g->find_node(id1)->degree;
            int d2 = g->find_node(id2)->degree;
            cost[e] = E - d1 - d2;
        }
        else
        {
            cost[e] = 1;
        }
    }
    MaxWeightedMatching<ListGraph, ListGraph::EdgeMap<int> > mm(sg,cost);
    mm.run();
    //    printf("maximum weight = %d @ %d\n", mm.matchingWeight(),mm.matchingSize());
    for(ListGraph::EdgeIt e(sg); e != INVALID; ++e)
    {
        if(mm.matching(e))
        {
            int id0 = sg.id(sg.u(e));
            int id1 = sg.id(sg.v(e));
            //            cout<<"matching of weight " << weight << " " << id0<<"->"<<id1<<endl;
            // insert intermedian
            //            printf("add intermedia %d->%d, idx = %d(%d)\n", id0, id1, intermedia_index,intermedia_index+m_width);
            add_new_intermedia(id0,id1,intermedia_index);
            // insert schedule
            for(int i = 0;i<m_height;i++)
            {
                if(bm_copy[i * m_width + id0] == 1 && bm_copy[i*m_width + id1] == 1)
                {
                    bm_copy[i * m_width + id0] = bm_copy[i*m_width + id1] = 0;
                    //                    printf("parity #%d need this intermedia\n", i);
                    schedule.push_back(new_schedule_element(intermedia_index+m_width,i,3));
                }
            }
            intermedia_index++;
        }
    }
    return mm.matchingSize();
}

void ZOXC::print_bm(int *bm)
{
    for(int i = 0;i<m_height;i++)
    {
        for(int j = 0;j<m_width;j++)
            printf("%d  ", bm[i*m_width+j]);
        printf("\n");
    }
}

int ZOXC::bitmatrix_to_graph(int *bm)
{
    int max_edge_weight = 0;
    for(int i = 0;i<m_height;i++)
    {
        vector<int> v;
        for(int j = 0;j<m_width;j++)
        {
            if(bm[i*m_width+j] == 1)
            {
                v.push_back(j);
            }
        }
        if(v.size() == 0) continue;
        for(int i1 = 0;i1 < v.size() - 1;i1++)
        {
            for(int i2 = i1+1; i2<v.size(); i2++)
            {
                zedge *te = g->find_edge(v[i1], v[i2]);
                te->weight++;
                if(te->weight > max_edge_weight) max_edge_weight = te->weight;
            }
        }
    }
    return max_edge_weight;
}

// High-level entry
int ZOXC::grouping_1s(int *bm, bool isWeighted)
{
    intermedia_index = 0;
    intermedia_schedule.clear();
    n_XOR = 0;
    bm_copy.resize(m_width*m_height);
    memcpy(bm_copy.data(), bm, m_width*m_height*sizeof(int));

    // start from the edges with the highest degree, see huang's paper.
    int n, cnt;
    n = bitmatrix_to_graph(bm_copy.data());
    g->set_node_degree();

    while(n > 1)
    {
        cnt = set_sg(n,isWeighted);
        //        printf("found n = %d, cnt = %d\n", n, cnt);
        if(cnt > 0)
        {
            n_XOR += (n + 1)*cnt;
            //            printf("*** n_XOR=%d\n", n_XOR);
            //            print_bm(bm_copy.data());
            clear_g_weight();
        }
        n = bitmatrix_to_graph(bm_copy.data());
    }
    //    printf("%d, %d\n", n_XOR, n_of_1s(bm_copy.data(), m_width * m_height));

    // add remain 1s as XOR from original data
    for(int i = 0;i<m_height*m_width;i++)
    {
        if(bm_copy[i])
        {
            int par = i/m_width;
            int off = i%m_width;
            //            printf("parity #%d need ori data %d\n", par, off);
            schedule.push_back(new_schedule_element(off,par,1));
        }
    }

    vector<int> occ_data, occ_inter;
    occ_data.resize(m_width,0);
    occ_inter.resize(intermedia_schedule.size(),0);

    // convert intermedia into schedule
    for(vector<int* >::iterator it = intermedia_schedule.begin(); it < intermedia_schedule.end(); it++ )
    {
        int s1 = (*it)[0];
        int s2 = (*it)[1];
        int inter_idx = (*it)[2];
        schedule.push_back(new_schedule_element(s1,inter_idx,5));
        schedule.push_back(new_schedule_element(s2,inter_idx,5));
    }

    // group by orginal data index
    for(int i = 0; i<schedule.size()-1;i++)
    {
        for(int j = i+1;j<schedule.size();j++)
        {
            if(schedule[j][0] < schedule[i][0])
            {
                int *t = schedule[j];
                schedule[j] = schedule[i];
                schedule[i] = t;
            }
        }
    }

    // update all intermedia_idx + m_width to intermedia_idx
    for(int i = 0; i<schedule.size();i++)
        if(schedule[i][0] >= m_width)
            schedule[i][0] -= m_width;

    // change first occurance of each parity/intermeida as dest to copy
    for(int i = 0; i < schedule.size(); i++)
    {
        //        printf("checking %d,%d,%d\n", schedule[i][0], schedule[i][1], schedule[i][2]);
        int dest_idx = schedule[i][1];
        if(schedule[i][2] == 5)
        {
            // occurance of intermedia as dest
            if(occ_inter[dest_idx] == 0)
            {
                schedule[i][2] --;
                occ_inter[dest_idx] = 1;
            }
        }
        else
        {
            if(occ_data[dest_idx] == 0)
            {
                schedule[i][2]--;
                occ_data[dest_idx] = 1;
            }
        }
    }

    return n_XOR;
}


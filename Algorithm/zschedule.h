#ifndef ZSCHEDULE_H
#define ZSCHEDULE_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
using namespace std;

class ZSchedule
{
public:
    ZSchedule(int size);
    int **getSch();
    void addSch(int s0,int s1, int d0,int d1, int op=1);
    void printSch();
    void printSchLen(char* p);
    void run(char**& ptrs);
private:
    vector<int* > sch;
    int idx;
    int packetsize;
    int xor_weight;
    int cpy_weight;
};

#endif // ZSCHEDULE_H

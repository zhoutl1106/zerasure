#include "zrandomarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctime>

ZRandomArray::ZRandomArray(int l)
{
    srand(time(NULL));
    m_l = l;
    dat = new int[l];
    for(int i = 0;i<l;i++)
        dat[i] = i;
}

ZRandomArray::~ZRandomArray()
{
    delete [] dat;
}

int* ZRandomArray::next_random(int KM, int n_first)
{
    for(int i = 0;i<n_first;i++)
    {
        int p1 = rand() % (m_l-KM+1) + KM - 1;
        if(KM == m_l)
            p1 = rand() % KM;
        int p2 = rand() % KM;
        int t = dat[p1];
        dat[p1] = dat[p2];
        dat[p2] = t;
    }
    return dat;
}

#ifndef ZXORELEMENT_H
#define ZXORELEMENT_H

#include "zabstractelement.h"

class ZXORElement : public ZAbstractElement
{
public:
    ZXORElement(int tK);
    int value();
    static void test_cost_weight(int size = 1024*1024*1024, int loops = 10);
    static int cpy_weight;
    static int xor_weight;


    int K;
};

#endif // ZXORELEMENT_H

#ifndef ZXORGENETIC_H
#define ZXORGENETIC_H
#include "zabstractgenetic.h"
#include "zxorelement.h"

class ZXORGenetic : public ZAbstractGenetic
{
public:
    ZXORGenetic();
    void mutation(ZAbstractElement* e);
};

#endif // ZXORGENETIC_H

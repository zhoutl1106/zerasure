#ifndef ZABSTRACTGENETIC_H
#define ZABSTRACTGENETIC_H

#include "zabstractelement.h"

class ZAbstractGenetic
{
public:
    ZAbstractGenetic();
    virtual void mutation(ZAbstractElement* e) = 0;
};

#endif // ZABSTRACTGENETIC_H

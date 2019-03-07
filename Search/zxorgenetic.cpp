#include "zxorgenetic.h"
#include <stdio.h>

ZXORGenetic::ZXORGenetic()
{

}

void ZXORGenetic::mutation(ZAbstractElement *e)
{
    ZXORElement *p = (ZXORElement*) e;
    printf("%d\n",p->K);
}

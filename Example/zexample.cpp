#include "zexample.h"
#include "../Search/zelement.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

ZExample::ZExample()
{

}

void ZExample::test_cost_weight(int argc, char *argv[])
{
    ZElement* e = new ZElement();
    if(argc == 2)
        e->test_cost_weight();
    else if(argc == 3)
        e->test_cost_weight(atoi(argv[2]));
    else if(argc == 4)
        e->test_cost_weight(atoi(argv[2]),atoi(argv[3]));
    else
        printf("usage: size loops\n");
    delete e;
}

void ZExample::single(int argc, char *argv[])
{
    if(true)//argc == 4)
    {
        int K,M,W,costf,stra;
        cin >> K >> M >> W;
        costf = atoi(argv[2]);
        stra = atoi(argv[3]);
        vector<int> arr(K+M,0);
        for(int i = 0;i<K+M;i++)
            cin >> arr[i];

        printf("cost_func=%d,strategy=%d,K=%d,M=%d,W=%d,arr=",costf,stra,K,M,W);
        for(int i = 0;i<K+M;i++)
            printf("%d ",arr[i]);
        printf("\n");
        ZElement::init(K,M,W,costf,stra);
        ZElement* e = new ZElement(arr.data());
        long long ret = e->value();
        printf("Cost for given input array : %d\n", ret);
        delete e;
    }
    else
        printf("usage: cost_func[0..2] strategy[0..7]\n");
}

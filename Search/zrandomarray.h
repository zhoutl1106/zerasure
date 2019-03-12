#ifndef ZRANDOMARRAY_H
#define ZRANDOMARRAY_H


class ZRandomArray
{
public:
    ZRandomArray(int l);
    ~ZRandomArray();
    int* next_random(int KM, int n_first);
    int *dat;

private:
    int m_l;
};

#endif // ZRANDOMARRAY_H

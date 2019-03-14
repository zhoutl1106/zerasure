#ifndef UTILS_H
#define UTILS_H

/* define vectorization size, enable only one */
#define VEC128
//#define VEC256
/* ****************************************** */

#include <x86intrin.h>
#include <vector>

using namespace std;

long long diff_us(struct timeval start, struct timeval end);
long long diff_ms(struct timeval start, struct timeval end);

void fast_xor(char* r1, char* r2, char* r3, int size);


void freeVectorPointer(vector<int*> &p);


#endif // UTILS_H

# Zerasure
Zerasure is the source code repository for the FAST'19 paper

**Fast Erasure Coding for Data Storage: A Comprehensive Study of the Acceleration Techniques**  
Tianli Zhou and Chao Tian, Texas A&M University

Link: [https://www.usenix.org/conference/fast19/presentation/zhou]

Zerasure is released under the new BSD license.

# Usage
## Software reference
- Jerasure 1.2A: 
http://web.eecs.utk.edu/~plank/plank/www/software.html  
Source code of this library is partially ported into the repo for convinient compiling purpose. However, neither the authors nor Texas A&M University own this source code. All rights are reserved to Dr. Plank, University of Tennessee, Knoxville who has published Jerasure under the [New BSD License](https://opensource.org/licenses/BSD-3-Clause).
- LEMON:
https://lemon.cs.elte.hu/trac/lemon  
Source code of this library is not included in this repo. Please compile and install via link above. The version tested is *lemon-1.3.1*. Neither the authors nor Texas A&M University own this source code. All rights are reserved to the Egerváry Combinatorial Optimization Research Group under the [Boost License](https://opensource.org/licenses/BSL-1.0).

## Compile
Zerasue use **qmake** to organize the project. No *QT* module, however, is used in the source code.
A **qmake** *.pro* file is included in the repo. Compilation could be simply:
~~~~
qmake
make
~~~~
in the root directory of the source code. An executable file *zerasure* will be generated.
A *makefile* with default compilation flags is also provided named **mfile**, you can compile using
~~~
make -f mfile
~~~

## Pre-optimized Cauchy Matrix
Several pre-optimzed X,Y arrays to define *Cauchy matrix* are provided in **PreOpt/ge_100_03_06_01_1000_weighted_s13.txt**.  
Each row corresponds to one specific (k,m,w) parameters obtained by *genetic algorithm, weighted cost function, strategy-(1,3)* with

- initial population = 100
- select rate = 0.3
- crossover rate = 0.6
- mutation rate = 0.1
- maximum population = 1000

Within one row, the first 3 elements are (k,m,w), then the k+m other numbers for the X,Y array (see the definition of Cauchy matrix). An example of using these numbers will be shown in next section.
This will also be the output format of Simulated Annealing and Genetic Algorithm.

## Examples
All examples are included in the *ZExample* class in Example directory, the *main* function will parse the parameters and call corresponding routine.

### data coding 
zerasure will not handle any data padding or chunking. Only single frame data (*blocksize* in source code) will be processed. Assertions are added to assure only a fixed amount of data is passed. Refer to the *code* example for details. 

The **code** example shows how to use the coding routines *ZCode::encode_single_chunk, ZCode::set_erasure, ZCode::decode_single_chunk*, it also shows how to manage data to be fed into these routines.
Example:
~~~
head -n 1 PreOpt/ge_100_03_06_01_1000_weighted_s13.txt | ./zerasure code 16384 7
~~~
will read K,M,W,X,Y from stdin (one line in txt file), then perform strategy-(1,3) using 16384(16KB) as packetsize.

### optimization
Code for optimizations using simulated annealing and genetic algorithms is provided.

The **test_cost_weight** example shows how to obtain the weight of memcpy and XOR operations.

The **single** example shows the way to calculate the cost for one given input array.
Example:
~~~
head -n 1 PreOpt/ge_100_03_06_01_1000_weighted_s13.txt | ./zerasure single 0 7
~~~
will read K,M,W,X,Y from stdin (one line in txt file), then perform strategy-(1,3) using 0 (# of XOR) as the cost function.
The cost function can be:

- 0: # of XOR
- 1: # of total operations
- 2: weighted sum of operations, obtained by previous step, default value memcpy is ~1.5x faster than XOR.

The **sa** example is an implementation of simulated annealing, using
$$rate_{accept} * exp(-0.05*idx)$$
as the annealing function. You can update it correspondingly.

The **ge** example is an implementation of genetic algorithm. It has detailed comments. 

## Code structure
- Algorithm/
    - ZCauchy: Wrapper of Cauchy RS code of Jerasure-1.2A, subclass of ZCode
    - ZCode: Abstract class of all erasure codes
    - ZGrouping: The proposed erasure code in our FAST'19 paper, subclass of ZCode
    - ZOXC: Implementation of the matching algorithm in huang's paper. [(Link)](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/11/On-Optimizing-XOR-Based-Codes-for-Fault-Tolerant-Storage-Applications.pdf) Have a dependency of the implementation of *maximum cardinality matching* in *LEMON* library.
- Example/
    - ZExample: All examples
- Jerasure-1.2A/
    - Partial source code ported from Jerasure-1.2A, minor modification for compile purpose.
- Search/
    - ZElement: Input([X Y] array) -> output (cost)
    - ZGenetic: Implementation of the genetic algorithm 
    - ZRandomArray: Random array generator
    - ZSimulatedAnnealing: Implementation of the simulated annealing
- utils: Some utility functions such as memory management and timing.

### Notes
1. Width of vectorization (128/256 bits) is defined in *mfile* and *zerasure.pro*, using a makefile variable *VEC* and it could be VEC128/VEC256. You can update the variable in ONE file depends on how you organize the project (make or qmake). We do not have a machine to test the AVX-512 ISA but it will be only few lines codes change as described in the paper.
2. If you wish to use 256-bit AVX2 ISA, all the data pointers have to be **32 Bytes aligned**. Neither *malloc* in C nor *new* in C++ will provide this alignment. Please use *aligned_alloc* or *posix_memalign*, depending on your OS.
3. The pre-optimized results are obtained using the same polynomial in Galois Field as discribed in [This paper](https://web.eecs.utk.edu/~plank/plank/papers/CS-08-627.pdf).
4. Strategy-(i,j) in our FAST'19 paper corresponds to the (ix4+j)-th strategy in the source code.

### Update
A new option *VEC512* has been added to feature/avx512 branch, it's still under development but you can see how to utilize the AVX-512 ISA set of latest Intel CPUs.

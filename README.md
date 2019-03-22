# Zerasure
Zerasure is the source code for the FAST'19 paper

**Fast Erasure Coding for Data Storage: A Comprehensive Study of the Acceleration Techniques**  
Tianli Zhou and Chao Tian, Texas A&M University

Link: [https://www.usenix.org/conference/fast19/presentation/zhou]

Zerasure is released with new BSD license.

# Usage
## Software reference
- Jerasure 1.2A: 
http://web.eecs.utk.edu/~plank/plank/www/software.html  
Source codes are partially ported into the repo for convinient compiling purpose. However, neither the authors nor the Texas A&M University own these codes. All rights are reserved to Dr. Plank, University of Tennessee, Knoxville.
- LEMON:
https://lemon.cs.elte.hu/trac/lemon  
Source codes are not included in repo. Please compile and install via above link. The version tested is *lemon-1.3.1*. Neither the authors nor the Texas A&M University own these codes. All rights are reserved to original developer.

## Compile
Zerasue use **qmake** to organize the project. No *QT* module, however, is used in the source code. You are free to include the source code in any other form of projects, like *makefile* or *cmake*.
An **qmake** *.pro* file is included in the repo. Compilation could be simplely:
~~~~
qmake
make
~~~~
in the root directory in the source code. An executable file *zerasure* will be generated.
A *makefile* is also provided named **mfile**, you can compile using
~~~
make -f mfile
~~~

## Pre-optimized Cauchy Matrix
Several pre-optimzed $X,Y$ array to define *Cauchy matrix* are provided in **PreOpt/ge_100_03_06_01_1000_weighted_s13.txt**.  
Each row corresponding to one specified $(k,m,w)$ parameters obtained by *genetic algorithm, weighted cost function, strategy-(1,3)* with

- initial population = 100
- select rate = 0.3
- crossover rate = 0.6
- mutation rate = 0.1
- maximum population = 1000

Within one row, first 3 elements are $(k,m,w)$, then $k+m$ rest numbers for $X,Y$ array (see definition of Cauchy matrix). An example of use these numbers will be shown in next section.
This will also be the output format of Simulated Annealing and Genetic Algorithm.

## Examples
All examples are included in the *ZExample* class in Example directory, *main* function will parse the parameters and call corresponding routine.

### data coding 
zerasure will not handle any data padding or chunking. Only single frame data (blocksize in source code) will be processed. Assertions are added to assure only fix amount of data is passed. Refer to the *code* example for details. 
**code** example shows how to use the coding routines *ZCode::encode_single_chunk, ZCode::set_erasure, ZCode::decode_single_chunk*, it also shows how to manage data to be feed into these routines.
Example:
~~~
head -n 1 PreOpt/ge_100_03_06_01_1000_weighted_s13.txt | ./zerasure code 16384 7
~~~
will read K,M,W,X,Y from stdin(one line in txt file), then perform strategy-(1,3) using 16384(16KB) as packetsize.

### optimization
Simulated Annealing and Genetic algorithms are provided.

**test_cost_weight** example shows how to obtain the weight of memcpy and XOR operations.

**single** example shows the way to calculate out the cost for one given input array.
Example:
~~~
head -n 1 PreOpt/ge_100_03_06_01_1000_weighted_s13.txt | ./zerasure single 0 7
~~~
will read K,M,W,X,Y from stdin(one line in txt file), then perform strategy-(1,3) using 0 (# of XOR) as cost function.
cost functions:
- 0: # of XOR
- 1: # of total operations
- 2: weighted sum of operations, obtained by previous step, default value memcpy is ~1.5x faster than XOR.


**sa** example is an implementation of Simulated Annealing, using
$$rate_{accept} * exp(-0.05*idx)$$
as the annealing function, you can update it correspondingly.

**ge** example is an implementation of genetic algorithm, has been detailed commented in the code. 

## Code structure
- Algorithm/
    - ZCauchy: Wrapper of Cauchy RS code of jerasure, subclass of ZCode
    - ZCode: abstract class of all codes
    - ZGrouping: Proposed code in paper, subclass of ZCode
    - ZOXC: implementation of huang's paper. [(Link)](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/11/On-Optimizing-XOR-Based-Codes-for-Fault-Tolerant-Storage-Applications.pdf) Have a dependency of the implementation of *maximum cardinality matching* in *LEMON* library.
- Example/
    - ZExample: All examples
- Jerasure-1.2A/
    Partial source code of Jerasure, minor modification for compile purpose.
- Search/
    - ZElement: One step in optimaztion, input([X Y] array) -> output (one number of cost)
    - ZGenetic: Implementation of genetic algorithm 
    - ZRandomArray: Random array generator
    - ZSimulatedAnnealing: Implementation of simulated annealing
- utils: some util function like memory management and timing.

### Notes
1. Scale of vectorization (128/256 bits) are defined in *mfile*, using a makefile variable *VEC*, it could be VEC128/VEC256. We do not have a machine to test the AVX-512 ISA but it will be only few lines codes change as described in the paper.
2. If you want to use 256 bits AVX2 ISA, all the data pointers have to be **32 Bytes aligned**, neither *malloc* in C nor *new* in C++ will provide this. use *aligned_alloc* or *posix_memalign* depends on your OS.
3. The pre-optimized results are obtained using the same polynomial in Galois Field as discribed in [Plank's paper](https://web.eecs.utk.edu/~plank/plank/papers/CS-08-627.pdf), it will need another optimization for using other polynomial.
4. Original paper using *strategy-(i,j)*, in this implementation we use number 0 to 7 for easier programming, it equal to $i\times 4+j$

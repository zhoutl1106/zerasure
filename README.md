# Zerasure
Zerasure is the source code for the FAST'19 paper

**Fast Erasure Coding for Data Storage: A Comprehensive Study of the Acceleration Techniques**
Tianli Zhou and Chao Tian, Texas A&M University

Link: [https://www.usenix.org/conference/fast19/presentation/zhou]

Zerasure is released with new BSD license.

# Usage
## Software dependency
- Jerasure 1.2A: 
http://web.eecs.utk.edu/~plank/plank/www/software.html
Source codes are partially included in the repo for convinient compiling purpose. However, neither the authors nor the Texas A&M University own these codes. All rights are reserved to Dr. Plank, University of Tennessee, Knoxville. 
- LEMON:
https://lemon.cs.elte.hu/trac/lemon
Source codes are not included in repo. Please compile and install via above link. The version tested is *lemon-1.3.1*. Neither the authors nor the Texas A&M University own these codes. All rights are reserved to original developer.

## Compile
Zerasue use **qmake** to organize the project. No *QT* modules, however, are used in the source code. You are free to include the source code in any other form of projects, like *makefile* or *cmake*.
An **qmake** *.pro* file is included in the repo. compilation could be simple run:
~~~~
qmake
make
~~~~
in the root directory in the source code. An executable file *zerasure* will be generated.

## Pre-optimized Cauchy Matrix
Several pre-optimzed $X,Y$ array to define *Cauchy matrix* are provided in **PreOpt/ge_100_03_06_01_1000_weighted_s13.txt**
each row corresponding to one specified $(k,m,w)$ parameters obtained by *genetic algorithm* with *initial population = 100, select rate = 0.3, crossover rate = 0.6, mutation rate = 0.1, maximum population = 1000*. Within one row, first 3 elements are $(k,m,w)$, then $k+m$ rest numbers for $X,Y$ array (see definition of Cauchy matrix). An example of use these numbers will be shown in next section.

## Examples
All examples are included in the *ZExample* class in Example directory, *main* function will parse the parameters and call corresponding routine.

### data coding 
zerasure will not handle any data padding or chunking. Only single frame data (blocksize in source code) will be processed. Assertions are added to assure only fix amount of data is passed.

### optimization
Simulated Annealing and Genetic algorithms are provided.
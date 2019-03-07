OBJS=galois.o \
	jerasure.o \
	reed_sol.o \
	cauchy.o \
	main.o \
	zabstractelement.o \
	zxorelement.o \
	zabstractgenetic.o \
	zxorgenetic.o \
	utils.o

CC=clang
CXX=clang++
CXXFLAGS=-I./Jerasure-1.2A -mavx2 -msse4

zerasure: $(OBJS)
	$(CXX) -o zerasure $(OBJS)

galois.o: Jerasure-1.2A/galois.c Jerasure-1.2A/galois.h
	$(CC) -c Jerasure-1.2A/galois.c

jerasure.o: Jerasure-1.2A/jerasure.c Jerasure-1.2A/jerasure.h
	$(CC) -c Jerasure-1.2A/jerasure.c

reed_sol.o: Jerasure-1.2A/reed_sol.c Jerasure-1.2A/reed_sol.h
	$(CC) -c Jerasure-1.2A/reed_sol.c

cauchy.o: Jerasure-1.2A/cauchy.c Jerasure-1.2A/cauchy.h
	$(CC) -c Jerasure-1.2A/cauchy.c

main.o :
	$(CXX) -c main.cpp $(CXXFLAGS)
zabstractelement.o : Search/zabstractelement.h
	$(CXX) -c Search/zabstractelement.cpp $(CXXFLAGS)
zxorelement.o : Search/zxorelement.h
	$(CXX) -c Search/zxorelement.cpp $(CXXFLAGS)
zabstractgenetic.o : Search/zabstractgenetic.h
	$(CXX) -c Search/zabstractgenetic.cpp $(CXXFLAGS)
zxorgenetic.o : Search/zxorgenetic.h
	$(CXX) -c Search/zxorgenetic.cpp $(CXXFLAGS)
utils.o : utils.h
	$(CXX) -c utils.cpp $(CXXFLAGS)


.PHONY: clean
clean:
	rm zerasure $(OBJS)
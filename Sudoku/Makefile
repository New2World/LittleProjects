CPP		:= g++
CPPFLAG	:= -fPIC -O2

all:
	@ $(CPP) $(CPPFLAG) -c libsudoku.cpp -o libsudoku.o
	@ $(CPP) $(CPPFLAG) -shared libsudoku.o -o libsudoku.so
clean:
	@ rm libsudoku.o libsudoku.so
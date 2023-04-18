CPPFLAGS=-Wall -g
CPP=g++

SRC=udptest.C ../../src/amjComUDP.C ../../src/amjCom.C
OBJ=$(SRC:.C=.o)

all: udptest

udptest: $(OBJ)
	$(CPP) -o $@ $^

clean:
	- rm $(OBJ) udptest

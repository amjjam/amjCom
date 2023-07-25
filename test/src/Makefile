CPPFLAGS=-Wall -g
CPP=g++

UDPTEST_SRC=udptest.C ../../src/amjComUDP.C ../../src/amjCom.C ../../src/amjPacket.C
UDPTEST_OBJ=$(UDPTEST_SRC:.C=.o)

UDPTEST2_SRC=udptest2.C ../../src/amjComUDP.C ../../src/amjCom.C ../../src/amjPacket.C
UDPTEST2_OBJ=$(UDPTEST2_SRC:.C=.o)

MQTEST_SRC=mqtest.C ../../src/amjComMQ.C ../../src/amjCom.C ../../src/amjPacket.C
MQTEST_OBJ=$(MQTEST_SRC:.C=.o)

all: udptest udptest2 mqtest

udptest: $(UDPTEST_OBJ)
	$(CPP) -o $@ $^

udptest2: $(UDPTEST2_OBJ)
	$(CPP) -o $@ $^

mqtest: $(MQTEST_OBJ)
	$(CPP) -o $@ $^ -lrt

clean:
	- rm $(UDPTEST_OBJ) $(UDPTEST2_OBJ) $(MQTEST_OBJ)\
	udptest udptest2 mqtest

CPPFLAGS=-Wall -g -fPIC -I/opt/asio/include -I/opt/amj/include
CPP=g++

UDPTEST_SRC=udptest.C ../../src/amjComUDP.C ../../src/amjCom.C ../../src/amjPacket.C
UDPTEST_OBJ=$(UDPTEST_SRC:.C=.o)

UDPTEST2_SRC=udptest2.C ../../src/amjComUDP.C ../../src/amjCom.C ../../src/amjPacket.C
UDPTEST2_OBJ=$(UDPTEST2_SRC:.C=.o)

MQTEST_SRC=mqtest.C ../../src/amjComMQ.C ../../src/amjCom.C ../../src/amjPacket.C
MQTEST_OBJ=$(MQTEST_SRC:.C=.o)

SHMTEST_SRC=shmtest.C ../../src/amjComSHM.C ../../src/amjCom.C ../../src/amjPacket.C
SHMTEST_OBJ=$(SHMTEST_SRC:.C=.o)

TCPTEST_SRC=tcptest.C ../../src/amjComTCP.C ../../src/amjCom.C ../../src/amjComASIO.C
TCPTEST_OBJ=$(TCPTEST_SRC:.C=.o)

all: udptest udptest2 mqtest shmtest tcptest

udptest: $(UDPTEST_OBJ)
	$(CPP) -o $@ $^

udptest2: $(UDPTEST2_OBJ)
	$(CPP) -o $@ $^

mqtest: $(MQTEST_OBJ)
	$(CPP) -o $@ $^ -lrt

shmtest: $(SHMTEST_OBJ)
	$(CPP) -o $@ $^ -lrt

tcptest: $(TCPTEST_OBJ)
	$(CPP) -o $@ $^ -lrt -L/opt/amj/lib -lamjArg -Wl,-rpath,/opt/amj/lib

clean:
	- rm $(UDPTEST_OBJ) $(UDPTEST2_OBJ) $(MQTEST_OBJ) $(SHMTEST_OBJ) \
	$(TCPTEST_OBJ) \
	udptest udptest2 mqtest tcptest

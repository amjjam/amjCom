FLAGS=-Wall -g -fPIC
CPPFLAGS=$(FLAGS)

AMJCOM_SRC=amjCom.C amjComUDP.C amjComMQ.C amjPacket.C
AMJCOM_OBJ=$(AMJCOM_SRC:.C=.o)

CPP=g++ $(CPPFLAGS)

prefix=/opt/amjCom/

all: libamjCom.so

libamjCom.so: $(AMJCOM_OBJ)
	$(CPP) -shared -Wl,-soname,$@ -lrt -o $@ $^

install: libamjCom.so ../include/amjCom.H ../include/amjComUDP.H ../include/amjComMQ.H ../include/amjPacket.H ../include/amjPacketRW.H
	mkdir -p $(prefix)/lib
	mkdir -p $(prefix)/include
	cp libamjCom.so $(prefix)/lib
	cp ../include/amjCom.H ../include/amjComUDP.H ../include/amjComMQ.H \
	../include/amjPacket.H ../include/amjPacketRW.H $(prefix)/include

uninstall:
	- rm $(prefix)/lib/libamjCom.so
	- rm $(prefix)/include/amjCom.H
	- rm $(prefix)/include/amjComUDP.H
	- rm $(prefix)/include/amjComMQ.H
	- rm $(prefix)/include/amjPacket.H
	- rm $(prefix)/include/amjPacketRW.H
	- rmdir $(prefix)/lib
	- rmdir $(prefix)/include
	- rmdir $(prefix)

clean:
	- rm libamjCom.so $(AMJCOM_OBJ)


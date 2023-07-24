FLAGS=-Wall -g -fPIC
CPPFLAGS=$(FLAGS)

AMJCOM_SRC=amjCom.C amjComUDP.C amjComMQ.C amjPacket.C
AMJCOM_OBJ=$(AMJCOM_SRC:.C=.o)

CPP=g++ $(CPPFLAGS)

all: libamjCom.so

libamjCom.so: $(AMJCOM_OBJ)
	$(CPP) -shared -Wl,-soname,$@ -lrt -o $@ $^

install: libamjCom.so ../include/amjCom.H ../include/amjComUDP.H ../include/amjComMQ.H ../include/amjPacket.H ../include/amjPacketRW.H
	mkdir -p /opt/amjCom/lib
	mkdir -p /opt/amjCom/include
	cp libamjCom.so /opt/amjCom/lib
	cp ../include/amjCom.H ../include/amjComUDP.H ../include/amjComMQ.H \
	../include/amjPacket.H ../include/amjPacketRW.H /opt/amjCom/include

uninstall:
	- rm /opt/amjCom/lib/libamjCom.so
	- rm /opt/amjCom/include/amjCom.H
	- rm /opt/amjCom/include/amjComUDP.H
	- rm /opt/amjCom/include/amjComMQ.H
	- rm /opt/amjCom/include/amjPacket.H
	- rm /opt/amjCom/include/amjPacketRW.H
	- rmdir /opt/amjCom/lib
	- rmdir /opt/amjCom/include
	- rmdir /opt/amjCom

clean:
	- rm libamjCom.so $(AMJCOM_OBJ)


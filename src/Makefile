ifndef prefix
  prefix=/opt/amj
endif

#ifndef asioprefix
#  asioprefix=../include
#endif

#ifndef asioprefix
#  asioprefix=/opt/asio/include
#endif

FLAGS=-Wall -g -fPIC
#-I$(asioprefix)
CPPFLAGS=$(FLAGS)

AMJCOM_SRC=amjCom.C amjComUDP.C amjComMQ.C amjComSHM.C amjPacket.C \
	amjComASIO.C amjComTCP.C
AMJCOM_OBJ=$(AMJCOM_SRC:.C=.o)

CPP=g++ $(CPPFLAGS)

all: libamjCom.so

libamjCom.so: $(AMJCOM_OBJ)
	$(CPP) -shared -Wl,-soname,$@ -lrt -o $@ $^

install: libamjCom.so ../include/amjCom.H ../include/amjComUDP.H ../include/amjComMQ.H ../include/amjComSHM.H ../include/amjPacket.H ../include/amjPacketRW.H ../include/amjComASIO.H ../include/amjComTCP.H
	mkdir -p $(prefix)/lib
	mkdir -p $(prefix)/include
	mkdir -p $(prefix)/include/amjCom
	cp libamjCom.so $(prefix)/lib
	cp ../include/amjCom.H ../include/amjComUDP.H ../include/amjComMQ.H \
	../include/amjComSHM.H ../include/amjPacket.H \
	../include/amjPacketRW.H ../include/amjComASIO.H \
	../include/amjComTCP.H \
	$(prefix)/include/amjCom

uninstall:
	- rm $(prefix)/lib/libamjCom.so
	- rm $(prefix)/include/amjCom/amjCom.H
	- rm $(prefix)/include/amjCom/amjComUDP.H
	- rm $(prefix)/include/amjCom/amjComMQ.H
	- rm $(prefix)/include/amjCom/amjComSHM.H
	- rm $(prefix)/include/amjCom/amjPacket.H
	- rm $(prefix)/include/amjCom/amjPacketRW.H
	- rm $(prefix)/include/amjCom/amjComASIO.H
	- rm $(prefix)/include/amjCom/amjComTCP.H
	- rmdir $(prefix)/lib
	- rmdir $(prefix)/include/amjCom
	- rmdir $(prefix)/include
	- rmdir $(prefix)

clean:
	- rm libamjCom.so $(AMJCOM_OBJ)

FLAGS=-Wall -g -fPIC
CPPFLAGS=$(FLAGS)

AMJCOM_SRC=amjCom.C amjComUDP.C
AMJCOM_OBJ=$(AMJCOM_SRC:.C=.o)

CPP=g++ $(CPPFLAGS)

all: libamjCom.so

libamjCom.so: $(AMJCOM_OBJ)
	$(CPP) -shared -Wl,-soname,$@ -o $@ $^

install: libamjCom.so ../include/amjCom.H ../include/amjComUDP.H
	mkdir -p /opt/amj/lib
	mkdir -p /opt/amj/include
	cp libamjCom.so /opt/amj/lib
	cp ../include/amjCom.H ../include/amjComUDP.H /opt/amj/include

uninstall:
	- rm /opt/amj/lib/libamjCom.so
	- rm /opt/amj/include/amjCom.H
	- rm /opt/amj/include/amjComUDP.H

clean:
	- rm libamjCom.so $(AMJCOM_OBJ)


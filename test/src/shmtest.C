
/*=============================================================================
  This program is for testing amjComSHM shared memory
  communication. It links with the shared object library libamjCom.so
  in ../../src

  The sender generates a packet every time ms. The packet contains
  just an int. The receiver receives the packets, and every second it
  writes the number of packets received.
  
  -s|-r name nbuf size
     if -s is specified then it is a sender. If -r is specified it is a receiver
     name is the name of the shared memory and of the semaphore
     nbuf is the number of buffers

  -t time is the number of millisecond between generated packets on
     the sender side. It is ignored on the receiver side.
  ============================================================================*/

#include "../../include/amjComSHM.H"
#include <cstring>

void parse_args(int argc,char *argv[]);

bool send=false,recv=false;
std::string send_desc,recv_desc;

amjComEndpointSHM *c;
amjPacket p;

int main(int argc, char *argv[]){
  parse_args(argc,argv);

  if(send){
    c=new amjComEndpointSHM(send_desc,"");
    for(int i=0;;i++){
      p.clear();
      memcpy(p.write(sizeof(int)),&i,sizeof(int));
      c.send(p);
    }
  }
  else if(recv){
    int j;
    c=new amjComEndpointSHM("",recv_desc);
    for(int i=0;;i++){
      c.recv(p);
      memcpy(&i,p.read(sizeof(int)),sizeof(int));
      std::cout << i << " " << j << std::endl;
    }
  }
  
  return 0;
}


void parse_args(int argc, char *argv[]){

  for(int i=0;i<argc;i++)
    if(send=(strcmp(argv[i],"-s")==0)||recv=(strcmp(argv[i],"-r")==0)){
      std::string desc;
      i++;
      desc+=argv[i];
      desc+=':';
      i++;
      desc+=argv[i];
      desc+=':';
      i++;
      desc+=argv[i];
      if(sender)
	send_desc=desc;
      else if(receiver)
	recv_desc=desc;
    }
  
}



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
     the sender side. One the receiver side it is the number of
     milliseconds between output. Output is number of packets in the
     interval and average time between packets.
  ============================================================================*/

#include "../../include/amjComSHM.H"
#include <cstring>
#include <iostream>
#include <unistd.h>

void parse_args(int argc,char *argv[]);

bool send=false,recv=false;
std::string send_desc,recv_desc;

amjPacket p;

useconds_t t=1000;

int main(int argc, char *argv[]){
  parse_args(argc,argv);

  if(send){
    amjComEndpointSHM c("",send_desc);
    for(int i=0;;i++){
      p.clear();
      memcpy(p.write(sizeof(int)),&i,sizeof(int));
      c.send(p);
      usleep(t);
    }
  }
  else if(recv){
    int j;
    amjComEndpointSHM c(recv_desc,"");
    for(int i=0;;i++){
      c.receive(p);
      memcpy(&j,p.read(sizeof(int)),sizeof(int));
      std::cout << i << " " << j << std::endl;
    }
  }
  
  return 0;
}


void parse_args(int argc, char *argv[]){

  for(int i=1;i<argc;i++)
    if(strcmp(argv[i],"-s")==0||strcmp(argv[i],"-r")==0){
      send=strcmp(argv[i],"-s")==0;
      recv=strcmp(argv[i],"-r")==0;
      std::string desc;
      i++;
      desc+=argv[i];
      desc+=':';
      i++;
      desc+=argv[i];
      desc+=':';
      i++;
      desc+=argv[i];
      if(send)
	send_desc=desc;
      else if(recv)
	recv_desc=desc;
    }
    else if(strcmp(argv[i],"-t")==0){
      i++;
      t=atoi(argv[i])*1000;
    }
    else{
      std::cout << "error: unknown parameter: " << argv[i] << std::endl;
      abort();
    }
}


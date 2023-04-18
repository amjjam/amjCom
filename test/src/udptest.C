/*========================================================
  udptest -r|-s -my <addr>:<port> -peer <addr>:<port>
  -r will receive in an infinite loop and print messages
  -s will send one message: "Hello from <myaddr>:<myport>"
  -my <addr>:<port> address and port for this. If not set
   the network layer will select.
  -peer <addr>:<port> address and port for peer
  ========================================================*/

#include "../../include/amjComUDP.H"

#include <iostream>
#include <cstring>

void parse_args(int argc, char *argv[]);

bool rtsf;
std::string my;
std::string peer;

int main(int argc, char *argv[]){
  parse_args(argc,argv);
  
  amjComEndpointUDP e(my,peer);
  amjPacket p;
  
  if(rtsf){ // I am a receiver
    for(;;){
      e.receive(p);
      std::cout << std::string(p.data().begin(),p.data().end()) << std::endl;
    }
  }
  else{ // I am a sender
    std::string m="Hello from "+my;
    p.resize(m.size());
    std::copy(m.begin(),m.end(),p.data().begin());
    e.send(p);    
  }
  
  return 0;
}

void parse_args(int argc, char *argv[]){

  for(int i=1;i<argc;i++){
    if(strcmp(argv[i],"-r")==0){
      rtsf=true;
    }
    else if(strcmp(argv[i],"-s")==0){
      rtsf=false;
    }
    else if(strcmp(argv[i],"-my")==0){
      i++;
      my=argv[i];
    }
    else if(strcmp(argv[i],"-peer")==0){
      i++;
      peer=argv[i];
    }
    else{
      std::cout << "error: unknown parameter: " << argv[i] << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  
}

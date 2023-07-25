/* For testing message queue. Make it similar to UDP test */

/*============================================================================
  mqtest -r|-s -qsend <string> -qreceive <string>
  -r will receive in an infinite loop and print messages
  -s will send a message ever 1 s: "Hello from "+send string
  -qsend /<name>:n:size
  -qreceive /<name>:n:size
     for both <name> is the POSIX message queue name, n is the number of
     buffers in the queue, and size is the size of each buffer.
  ============================================================================*/

#include "../../include/amjComMQ.H"

#include <iostream>
#include <cstring>
#include <unistd.h>

void parse_args(int argc, char *argv[]);

bool rtsf;
std::string qsend;
std::string qreceive;

int main(int argc, char *argv[]){
  parse_args(argc,argv);

  amjComEndpointMQ e(qsend,qreceive);
  amjPacket p;

  if(rtsf){ // I am a receiver
    for(;;){
      e.receive(p);
      p.reset();
      std::string message;
      int number;
      p >> number >> message;
      std::cout << number << " " << message << std::endl;
    }
  }
  else{ // I am a sender
    for(int i=0;;i++){
      std::string m="Hello from "+qsend;
      p.clear();
      p << i << m;
      e.send(p);
      usleep(100000);
    }
  }

  return 0;      
}

void parse_args(int argc, char *argv[]){
  for(int i=1;i<argc;i++){
    if(strcmp(argv[i],"-r")==0)
      rtsf=true;
    else if(strcmp(argv[i],"-s")==0)
      rtsf=false;
    else if(strcmp(argv[i],"-qsend")==0){
      i++;
      qsend=argv[i];
    }
    else if(strcmp(argv[i],"-qreceive")==0){
      i++;
      qreceive=argv[i];
    }
    else{
      std::cout << "error: unknown parameter: " << argv[i] << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

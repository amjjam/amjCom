#include "../include/amjComMQ.H"

#include <iostream>

amjComEndpointMQ::amjComEndpointMQ(std::string send, std::string receive){
  qsend=qreceive=-1;
  init(send,SEND,&qsend,&_sendsize);
  init(receive,RECEIVE,&qreceive,&_receivesize);
}

amjComEndpointMQ::~amjComEndpointMQ(){
  if(qsend>=0)
    mq_close(qsend);
  if(qreceive>=0)
    mq_close(qreceive);
}

int amjComEndpointMQ::send(amjPacket &p){
  if(p.size()>_sendsize){
    std::cout << "error: packet too big: " << p.size()
	      << ". Queue size is " << _sendsize << std::endl;
    exit(1);
  }
  int r=mq_send(qsend,p._data(),p.size(),1);
  if(r>=0)
    return r;
  if(r==-1&&errno==EAGAIN)
    return 0;
  perror("amjComEndpointMQ::send error");
  exit(1);
  return 0;
}

int amjComEndpointMQ::receive(amjPacket &p){
  p.resize(_receivesize);
  int r=mq_receive(qreceive,p._data(),_receivesize,nullptr);
  if(r>=0){
    p.resize(r);
    return r;
  }
  perror("amjComEndpointMQ::receive error");
  exit(1);
  return 0;
}

void amjComEndpointMQ::init(std::string description, int direction,
			    mqd_t *q, int *size){
  if(description.size()==0)
    return;
  std::string name=split1(description);
  int n=std::stoi(split1(split2(description)));
  *size=std::stoi(split2(split2(description)));

  // Open the queue in non-blocking mode
  int flags=O_CREAT|O_NONBLOCK|(direction==SEND?O_WRONLY:O_RDONLY);
  //attr.mq_flags=O_NONBLOCK;
  struct mq_attr attr;
  attr.mq_maxmsg=n;
  attr.mq_msgsize=(*size);
  // There are some permission issues which I don't fully 
  // understand, but the code related to umask, which surrounds
  // the call, is supposed to fix this. I found the solution here:
  // http://stackoverflow.com/questions/22780277/
  //                      mq-open-eacces-permission-denied
  mode_t omask;
  omask=umask(0);
  if((*q=mq_open(name.c_str(),flags,(S_IRWXU|S_IRWXG|S_IRWXO),&attr))<0){
    char message[100];
    sprintf(message,"Could not open message queue %s",name.c_str());
    perror(message);
    exit(1);
  }
  umask(omask);

  // Verify queue creation with correct attributes
  mq_getattr(*q,&attr);
  if(attr.mq_maxmsg!=n||attr.mq_msgsize!=*size){
    std::cout << "Error: A message queue with the name " << name
	      << " already exists," << std::endl;
    std::cout << "but with different attributes:" << std::endl;
    std::cout << "Existing queue specs: n=" << attr.mq_maxmsg
	      << ", size=" << attr.mq_msgsize << std::endl;
    std::cout << "Desired queue spects: n=" << n << ", size="
	      << *size << std::endl;
    std::cout << "The solution is to delete the queue and start again."
	      << std::endl;
    std::cout << "Use ipcs to get a list of message queues." << std::endl;
    std::cout << "Use ipcrm to remove the message queue." << std::endl;
    exit(1);
  }

  // If the queue is readable then flush it by reading messages until
  // none are left. 
  std::vector<char> dummy(*size);
  while(mq_receive(*q,dummy.data(),*size,nullptr)>=0);

  // Set the queue to blocking if receiving from it and non-blocking
  // if sending to it.
  attr.mq_flags=(direction==SEND?O_NONBLOCK:0);
  mq_setattr(*q,&attr,nullptr);
}


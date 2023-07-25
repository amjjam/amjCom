#ifndef _AMJCOMMQ_H_
#define _AMJCOMMQ_H_

/*============================================================================
  Format of the strings is "/name:n:size"
  name is a string which is the name of the message queue
  n is an integer which is the number of messages buffers in the queue
  size is an integer which is the size of each message
  If the message queue alreay exists it must be
  ===========================================================================*/

#define SEND 1
#define RECEIVE 2

#include "amjCom.H"

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

class amjComEndpointMQ: amjComEndpoint{
public:
  amjComEndpointMQ(std::string,std::string);
  virtual ~amjComEndpointMQ();
  virtual int send(amjPacket &);
  virtual int receive(amjPacket &);
private:
  int _sendsize;
  int _receivesize;
  mqd_t qsend;
  mqd_t qreceive;
  void init(std::string,int,mqd_t *,int *);
};

#endif /* _AMJCOMMQ_H_ */

#ifndef _AMJCOMSHM_H_
#define _AMJCOMSHM_H_

#include <string>

#include "amjCom.H"
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

/*=============================================================================
  For each of sending and receiving:
  <name>:<nbufs>:<size>

  For two-way communication there will be two shared memory blocks and
  two semaphores.
  
  <name> - a text string name. This is used as the name of named
  semaphore.
  <nbufs> - the number of buffers in the circular fifo
  <size> - the size of each buffer
  
  Creates shared memory circular buffer fifo consisting of <nbufs>
  buffers each of size <size>, for one-way communication. It is a good
  idea to make <nbufs> large enough that it can contain data packets
  sent in case the receiver temporarily lags behind.

  The class creates a named shared memory area which contains:
  int - index for reading next packet
  int - index for writing next packet
  uint8_t[nbufs][size] - nbufs buffers each of size size.

  It also creates a named semaphore which counts the difference
  between the number of packets written and the number of packets
  read.

  Note: If writing is ahead of reading by more than nbufs that means
  that the oldest packets have been overwritten by newest packets. It
  is up to the user to deal with this.
  ============================================================================*/
class amjComEndpointSHM: amjComEndpoint{
public:
  amjComEndpointSHM(std::string,std::string);
  virtual ~amjComEndpointSHM();
  virtual int send(amjPacket &);
  virtual int receive(amjPacket &);
private:
  uint8_t *send_shm;
  sem_t *send_sem;
  int send_nbuf;
  int send_size;
  uint8_t *recv_shm;
  sem_t *recv_sem;
  int recv_nbuf;
  int recv_size;
  void create_shm_sem(std::string s, uint8_t **shm, sem_t **sem,
		      int &nbuf, int &size);
};

#endif /* _AMJCOMSHM_H_ */

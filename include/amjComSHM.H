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
  semaphore which stores number of frames available, and used as basis for a
  named semaphore which serves as a mutex.
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
  //virtual int clear(); // Clears the FIFO and reset sidx and ridx to zero
  virtual int send(amjPacket &);
  virtual int receive(amjPacket &);
private:
  struct shm{
    unsigned int sidx;
    unsigned int ridx;
    unsigned int nbuf;
    unsigned int size;
  } *sshm, *rshm;
  sem_t *ssem;
  sem_t *smutex;
  sem_t *rsem;
  sem_t *rmutex;
  void create_shm_sem(std::string s, struct shm **shm, sem_t **sem,
		      sem_t **mutex, bool init);
  void print();
  void print(struct shm *);
};

#endif /* _AMJCOMSHM_H_ */

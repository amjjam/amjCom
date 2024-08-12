#include "../include/amjComSHM.H"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <iostream>

amjComEndpointSHM::amjComEndpointSHM(std::string r,std::string s){
  create_shm_sem(r,&rshm,&rsem,&rmutex,false);
  create_shm_sem(s,&sshm,&ssem,&smutex,true);
}

amjComEndpointSHM::~amjComEndpointSHM(){
  /* unlink semaphores */
  /* unmap shared memory */
}

void amjComEndpointSHM::create_shm_sem(std::string s, struct shm **shm,
				       sem_t **sem, sem_t **mutex, bool init){

  if(s.size()==0){
    *shm=nullptr;
    *sem=nullptr;
    *mutex=nullptr;
    return;
  }
  
  std::string name=split1(s);
  int nbuf=atoi(split1(split2(s)).c_str());
  int size=atoi(split2(split2(s)).c_str());
  
  int fd;
  if((fd=shm_open(name.c_str(),O_RDWR|O_CREAT,0660))<0){
    perror("shm_open: error:");
    abort();
  }

  int n=sizeof(struct shm)+nbuf*size*sizeof(uint8_t);
  if(ftruncate(fd,n)<0){
    perror("ftruncate: error:");
    abort();
  }
  
  if((*shm=(struct shm *)mmap(nullptr,n,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0))
     ==MAP_FAILED){
    perror("mmap: error:");
    abort();
  };

  // This is the semaphore
  *sem=sem_open(name.c_str(),O_CREAT,S_IRWXU,0);

  // This is the mutex
  std::string name_mutex=name+"_mutex";
  *mutex=sem_open(name_mutex.c_str(),O_CREAT,S_IRWXU,1);
  
  if(!init)
    return;
  
  (*shm)->sidx=0;
  (*shm)->ridx=0;
  (*shm)->nbuf=nbuf;
  (*shm)->size=size;
}

int amjComEndpointSHM::send(amjPacket &p){
  if(p.size()+sizeof(int)>sshm->size){
    std::cout << "send: packet size " << p.size()+sizeof(int)
	      << " larger than buffer size " << sshm->size
	      << std::endl;
  }

  sem_wait(smutex);
  //std::cerr << "send 1:" << std::endl;
  //std::cerr << "packet size: " << p.size() << std::endl;
  //print();
  memcpy((uint8_t *)sshm+sizeof(struct shm)+sshm->sidx*sshm->size,p);
  sshm->sidx=(sshm->sidx+1)%sshm->nbuf;
  //std::cerr << "send 2:" << std::endl;
  //print();
  int sval;
  if(sem_getvalue(ssem,&sval)<0){
    std::cerr << "error: amjComEndpointSHM::send: could not "
	      << "get value of ssem" << std::endl;
    abort();
  }
  if((unsigned int)sval==sshm->nbuf)
    sshm->ridx=(sshm->ridx+1)%sshm->nbuf;
  else
    sem_post(ssem);
  sem_post(smutex);
  return p.size();
}

int amjComEndpointSHM::receive(amjPacket &p){
  //std::cerr << "receive 1:" << std::endl;
  //print();
  sem_wait(rsem);
  //std::cerr << "receive 2:" << std::endl;
  //print();
  sem_wait(rmutex);
  //std::cerr << "offset: " << sizeof(struct shm) << std::endl;
  memcpy(p,(uint8_t *)rshm+sizeof(struct shm)+rshm->ridx*rshm->size);
  //std::cerr << "packet size: " << p.size() << std::endl;
  p.start();
  rshm->ridx=(rshm->ridx+1)%rshm->nbuf;
  sem_post(rmutex);
  return p.size();
}

void amjComEndpointSHM::print(){
  std::cerr << "send:" << std::endl;
  print(sshm);
  std::cerr << "receive:" << std::endl;
  print(rshm);
}

void amjComEndpointSHM::print(struct shm *shm){
  if(shm==nullptr){
    std::cerr << "nullptr" << std::endl;
    return;
  }
  std::cerr << "sidx=" << shm->sidx << std::endl;
  std::cerr << "ridx=" << shm->ridx << std::endl;
}


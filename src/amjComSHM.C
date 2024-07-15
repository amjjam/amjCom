#include "../include/amjComSHM.H"

#include <sys/mman.h>
#include <sys/stat.h>

amjComEndpointSHM::amjComEndpointSHM(std::string s,std::string r){
  create_shm_sem(s,&send_shm,&send_sem,send_nbuf,send_size);
  *(int *)send_shm=0;
  *(int *)(send_shm+sizeof(int))=0;
  create_shm_sem(r,&recv_shm,&recv_sem,recv_nbuf,recv_size);
}

amjComEndpointSHM::~amjComEndpointSHM(){
  /* unlink semaphores */
  /* unmap shared memory */
}

void amjComEndpointSHM::create_shm_sem(std::string s, uint8_t **shm,
				       sem_t **sem,
				       int &nbuf, int &size){
  if(s.size()==0){
    shm=nullptr;
    sem=nullptr;
  }
  std::string name=split1(s);
  nbuf=atoi(split1(split2(s)).c_str());
  size=atoi(split2(split2(s)).c_str());
  
  int fd=shm_open(name.c_str(),O_RDWR|O_CREAT,S_IRWXU);
  *shm=(uint8_t *)mmap(NULL,2*sizeof(int)+nbuf*size,PROT_WRITE,MAP_SHARED,fd,0);
  shm_unlink(name.c_str());
  *sem=sem_open(name.c_str(),O_CREAT,S_IRWXU,0);  
}

int amjComEndpointSHM::send(amjPacket &p){
  uint8_t *pos=send_shm+2*sizeof(int)+*(int *)send_shm*send_size;
  int s=p.size();
  memcpy(pos,&s,sizeof(int));
  memcpy(pos+sizeof(int),(void *)p._data(),s);
  (*(int *)send_shm)++;
  sem_post(send_sem);
  return s;
}

int amjComEndpointSHM::receive(amjPacket &p){
  sem_wait(recv_sem);
  uint8_t *pos=recv_shm+2*sizeof(int)+*(int *)recv_shm*recv_size;
  int s;
  memcpy(&s,pos,sizeof(int));
  p.resize(s);
  memcpy(p._data(),pos+sizeof(int),s);
  (*(int *)recv_shm)++;
  return s;
}

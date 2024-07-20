#include "../include/amjComSHM.H"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <iostream>

amjComEndpointSHM::amjComEndpointSHM(std::string r,std::string s){
  create_shm_sem(s,&sshm,&ssem,true);
  create_shm_sem(r,&rshm,&rsem,false);
}

amjComEndpointSHM::~amjComEndpointSHM(){
  /* unlink semaphores */
  /* unmap shared memory */
}

void amjComEndpointSHM::create_shm_sem(std::string s, struct shm **shm,
				       sem_t **sem, bool init){

  if(s.size()==0){
    shm=nullptr;
    sem=nullptr;
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

  int n=sizeof(struct shm)+nbuf*sizeof(uint8_t *)+nbuf*size*sizeof(uint8_t);
  if(ftruncate(fd,n)<0){
    perror("ftruncate: error:");
    abort();
  }
  
  if((*shm=(struct shm *)mmap(nullptr,n,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0))
     ==MAP_FAILED){
    perror("mmap: error:");
    abort();
  };

  if(init){
    (*shm)->sidx=0;
    (*shm)->ridx=0;
    (*shm)->nbuf=nbuf;
    (*shm)->size=size;
  }

  (*shm)->d=(uint8_t **)((uint8_t *)*shm+sizeof(struct shm));
  for(unsigned int i=0;i<(*shm)->nbuf;i++)
    (*shm)->d[i]=(uint8_t *)*shm+sizeof(struct shm)+
      (*shm)->nbuf*sizeof(uint8_t *)+i*size;
  
  //shm_unlink(name.c_str());
  *sem=sem_open(name.c_str(),O_CREAT,S_IRWXU,0);  
}

int amjComEndpointSHM::send(amjPacket &p){
  if(p.size()+sizeof(int)>sshm->size){
    std::cout << "send: packet size " << p.size()+sizeof(int)
	      << " larger than buffer size " << sshm->size
	      << std::endl;
  }

  memcpy(sshm->d[sshm->sidx],p);
  //int size=p.size();
  //memcpy(sshm->d[sshm->sidx],&size,sizeof(int));
  //memcpy(sshm->d[sshm->sidx]+sizeof(int),p._data(),size);
  sshm->sidx=(sshm->sidx+1)%sshm->nbuf;
  sem_post(ssem);
  return p.size();
}

int amjComEndpointSHM::receive(amjPacket &p){
  sem_wait(rsem);
  memcpy(p,(const void *)rshm->d[rshm->ridx]);
  //int size;
  //memcpy(&size,rshm->d[rshm->ridx],sizeof(int));
  //std::cout << size << std::endl;
  //p.resize(size);
  p.start();
  //memcpy(p._data(),rshm->d[rshm->ridx]+sizeof(int),size);
  rshm->ridx=(rshm->ridx+1)%rshm->nbuf;
  return p.size();
}

#include <time.h>
#include <iostream>
#include <cstring>

int main(int argc, char *argv[]){

  uint16_t a[256*320],b[256*320],c[256*320];

  int n=100000;

  struct timespec t0,t1;
  clock_gettime(CLOCK_MONOTONIC_RAW,&t0);
  
  for(int i=0;i<n;i++){
    memcpy(c,a,256*320*sizeof(uint16_t));
    memcpy(a,b,256*320*sizeof(uint16_t));
    memcpy(b,c,256*320*sizeof(uint16_t));
  }
  
  clock_gettime(CLOCK_MONOTONIC_RAW,&t1);

  
  
  std::cout << "Time per memcpy: " << ((double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_nsec-t0.tv_nsec)/1e9)/n/3 << " s" << std::endl;

}

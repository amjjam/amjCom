#include "../include/amjComUDP.H"

#include <cstring>

amjComEndpointUDP::amjComEndpointUDP(std::string me, std::string peer){
  buffer.resize(MAX_DGRAM);
  myaddr=make_sockaddr_in(me);
  peeraddr=make_sockaddr_in(peer);
  one_peer=(peer.size()>0);
  
  if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  if(bind(sockfd,(const struct sockaddr *)&myaddr,
	  sizeof(struct sockaddr_in))<0){
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
}

int amjComEndpointUDP::send(amjPacket &p){
  return sendto(sockfd,p._data(),p.size(),0,(struct sockaddr *)&peeraddr,
		sizeof(struct sockaddr_in));
}

int amjComEndpointUDP::receive(amjPacket &p){
  struct sockaddr_in senderaddr;
  socklen_t len;
  int n=recvfrom(sockfd,buffer.data(),MAX_DGRAM,MSG_WAITALL,
		 (struct sockaddr *)&senderaddr,&len);
  if(one_peer)
    if(!compare_sockaddr_in(senderaddr,peeraddr)){
      perror("datagram not from peer");
      exit(EXIT_FAILURE);
    }
  p.resize(n);
  memcpy(p._data(),buffer.data(),n);
  return 0;
}


struct sockaddr_in make_sockaddr_in(std::string &s){
  struct sockaddr_in a;
  memset(&a,0,sizeof(struct sockaddr_in));
  a.sin_family=AF_INET;
  if(s.size()==0){
    a.sin_port=0;
    a.sin_addr.s_addr=INADDR_ANY;
  }
  else{
    a.sin_port=std::stoi(split2(s));
    a.sin_addr.s_addr=inet_addr(split1(s).c_str());
  }
  
  return a;
}

#include <iostream>
bool compare_sockaddr_in(struct sockaddr_in &s1, struct sockaddr_in &s2){
  if((s1.sin_port==s2.sin_port)&&(s1.sin_addr.s_addr==s2.sin_addr.s_addr))
    return true;
  return false;
}


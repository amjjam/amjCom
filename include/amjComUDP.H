#ifndef _AMJCOMUDP_H_
#define _AMJCOMUDP_H_

#include "amjCom.H"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_DGRAM 65536

class amjComEndpointUDP: amjComEndpoint{
public:
  amjComEndpointUDP(std::string, std::string);
  virtual ~amjComEndpointUDP(){};
  virtual int send(amjPacket &);
  virtual int receive(amjPacket &);
private:
  struct sockaddr_in myaddr;
  struct sockaddr_in peeraddr;
  int sockfd;
  std::vector<char> buffer;
};

struct sockaddr_in make_sockaddr_in(std::string &s);
bool compare_sockaddr_in(struct sockaddr_in &, struct sockaddr_in &);

#endif /* _AMJCOMUDP_H_ */


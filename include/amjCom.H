#ifndef _AMJCOM_H_
#define _AMJCOM_H_

#include <string>

#include "amjPacket.H"

class amjComEndpoint{
public:
  amjComEndpoint(){};
  virtual ~amjComEndpoint(){};
  virtual int send(amjPacket &)=0;
  virtual int receive(amjPacket &)=0;
private:
};

std::string split1(std::string);
std::string split2(std::string);

#endif /* _AMJCOM_H_ */


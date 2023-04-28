#ifndef _AMJPACKETRW_H_
#define _AMJPACKETRW_H_

class amjPacketRW{
public:
  amjPacketRW(){};
  virtual ~amjPacketRW(){};
  int write(amjPacket &p, unsigned int offset=0) const =0;
  int read(const amjPacket &p, unsigned int offset=0)=0;
private:
};

#endif /* _AMJPACKETRW_H_ */

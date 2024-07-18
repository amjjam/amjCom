#ifndef _AMJPACKET_H_
#define _AMJPACKET_H_

#include <vector>
#include <cstring>
#include <string>
#include <complex>

#include "amjPacketRW.H"

class amjPacket{
public:
  amjPacket(unsigned int n=0, unsigned int o=0);
  virtual ~amjPacket(){};
  void resize(int n);
  void clear(){_d.clear(); offset=0;}
  int seek(unsigned int o=0);
  int reset(){return seek(0);};
  int start(){return seek(0);};
  void reserve(int n){_d.reserve(n);}
  int size() const {return _d.size();}
  std::vector<char> &data(){return _d;}
  char *raw(){return _d.data();}
  char *_data(){return _d.data();}
  const char *_data() const {return _d.data();}
  uint8_t *write(int size);
  uint8_t *read(int size);
  amjPacket &operator<<(const amjPacketRW &);
  amjPacket &operator>>(amjPacketRW &);
  amjPacket &operator<<(int);
  amjPacket &operator>>(int &);
  amjPacket &operator<<(float);
  amjPacket &operator>>(float &);
  amjPacket &operator<<(double);
  amjPacket &operator>>(double &);
  amjPacket &operator<<(std::complex<float>);
  amjPacket &operator>>(std::complex<float> &);
  amjPacket &operator<<(const std::string &);
  amjPacket &operator>>(std::string &);
  amjPacket &operator<<(const std::vector<int> &);
  amjPacket &operator>>(std::vector<int> &);
  amjPacket &operator<<(const std::vector<float> &);
  amjPacket &operator>>(std::vector<float> &);
  amjPacket &operator<<(const std::vector<double> &);
  amjPacket &operator>>(std::vector<double> &);
  amjPacket &operator<<(const std::vector<std::complex<float> > &);
  amjPacket &operator>>(std::vector<std::complex<float> > &);
  amjPacket &operator<<(const std::vector<std::string> &);
  amjPacket &operator>>(std::vector<std::string> &);
private:
  std::vector<char> _d;
  unsigned int offset;
  void bound();
  int push(const void *, unsigned int);
  int pop(void *, unsigned int);
};

int memcpy(amjPacket &,const void *);
int memcpy(void *,const amjPacket &);
	   
#endif /* _AMJPACKET_H_ */


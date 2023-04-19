#ifndef _AMJPACKET_H_
#define _AMJPACKET_H_

#include <vector>

class amjPacket{
public:
  amjPacket(int n=0):_d(n){};
  virtual ~amjPacket(){};
  void resize(int n){_d.resize(n);}
  int size() const {return _d.size();}
  std::vector<char> &data(){return _d;}
  char *_data(){return _d.data();}
private:
  std::vector<char> _d;
};

#endif /* _AMJPACKET_H_ */


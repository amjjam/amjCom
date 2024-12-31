#include "../include/amjCom.H"

namespace amjCom{

  void Packet::resize(size_t s){
    _data.resize(s);
    if(p>=s) p=s-1;
  }

  void Packet::pos(size_t _p){
    if(_p>=_data.size()) _data.resize(_p+1);
    p=_p;
  };

  uint8_t *Packet::write(size_t n){
    size_t pos=p;
    if(p+n>_data.size()) _data.resize(p+n);
    p+=n;
    return &_data[pos];
  };

  uint8_t *Packet::read(size_t n){
    if(p+n>_data.size()) return nullptr;
    return &_data[p];
  };
  
  std::string split1(std::string s){
    return s.substr(0,s.find_first_of(':'));
  }
  
  std::string split2(std::string s){
    return s.substr(s.find_first_of(':')+1);
  }
}

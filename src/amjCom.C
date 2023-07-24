#include "../include/amjCom.H"

std::string split1(std::string s){
  return s.substr(0,s.find_first_of(':'));
}

std::string split2(std::string s){
  return s.substr(s.find_first_of(':')+1);
}

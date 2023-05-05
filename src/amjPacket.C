#include "../include/amjPacket.H"

amjPacket::amjPacket(unsigned int n, unsigned int o):_d(n),offset(o){bound();};

void amjPacket::resize(int n){
  _d.resize(n);
  bound();
}

int amjPacket::seek(unsigned int o){
  offset=o;
  bound();
  return offset;
}

amjPacket &amjPacket::operator<<(const amjPacketRW &d){
  d.write(*this);
  return *this;
}

amjPacket &amjPacket::operator>>(amjPacketRW &d){
  d.read(*this);
  return *this;
}


amjPacket &amjPacket::operator<<(int i){
  push(&i,sizeof(int));
  return *this;
}

amjPacket &amjPacket::operator>>(int &i){
  pop(&i,sizeof(int));
  return *this;
}

amjPacket &amjPacket::operator<<(float f){
  push(&f,sizeof(float));
  return *this;
}

amjPacket &amjPacket::operator>>(float &f){
  pop(&f,sizeof(float));
  return *this;
}


amjPacket &amjPacket::operator<<(double d){
  push(&d,sizeof(double));
  return *this;
}

amjPacket &amjPacket::operator>>(double &d){
  pop(&d,sizeof(double));
  return *this;
}

amjPacket &amjPacket::operator<<(std::complex<float> c){
  float real=c.real(),imag=c.imag();
  push(&real,sizeof(float));
  push(&imag,sizeof(float));
  return *this;
}

amjPacket &amjPacket::operator>>(std::complex<float> &c){
  float real,imag;
  pop(&real,sizeof(float));
  pop(&imag,sizeof(float));
  c.real(real);
  c.imag(imag);
  return *this;
}

amjPacket &amjPacket::operator<<(const std::string &s){
  int n=s.size();
  push(&n,sizeof(int));
  push(s.data(),s.size());
  return *this;
}

amjPacket &amjPacket::operator>>(std::string &s){
  int n;
  pop(&n,sizeof(int));
  s.resize(n);
  pop(s.data(),n);
  return *this;
}

amjPacket &amjPacket::operator<<(const std::vector<int> &i){
  int n=i.size();
  push(&n,sizeof(int));
  push(i.data(),n*sizeof(int));
  return *this;
}

amjPacket &amjPacket::operator>>(std::vector<int> &i){
  int n;
  pop(&n,sizeof(int));
  i.resize(n);
  pop(i.data(),n*sizeof(int));
  return *this;
}

amjPacket &amjPacket::operator<<(const std::vector<float> &f){
  int n=f.size();
  push(&n,sizeof(int));
  push(f.data(),n*sizeof(float));
  return *this;
}

amjPacket &amjPacket::operator>>(std::vector<float> &f){
  int n;
  pop(&n,sizeof(int));
  f.resize(n);
  pop(f.data(),n*sizeof(float));
  return *this;
}

amjPacket &amjPacket::operator<<(const std::vector<double> &d){
  int n=d.size();
  push(&n,sizeof(int));
  push(d.data(),n*sizeof(double));
  return *this;
}

amjPacket &amjPacket::operator>>(std::vector<double> &d){
  int n;
  pop(&n,sizeof(int));
  d.resize(n);
  pop(d.data(),n*sizeof(double));
  return *this;
}

amjPacket &amjPacket::operator<<(const std::vector<std::complex<float> > &c){
  int n=c.size();
  push(&n,sizeof(int));
  float real,imag;
  for(unsigned int i=0;i<c.size();i++){
    real=c[i].real();
    imag=c[i].imag();
    push(&real,sizeof(float));
    push(&imag,sizeof(float));
  }
  return *this;
}

amjPacket &amjPacket::operator>>(std::vector<std::complex<float> > &c){
  int n;
  pop(&n,sizeof(int));
  c.resize(n);
  float real,imag;
  for(unsigned int i=0;i<c.size();i++){
    pop(&real,sizeof(float));
    pop(&imag,sizeof(float));
    c[i].real(real);
    c[i].imag(imag);
  }
  return *this;
}

void amjPacket::bound(){
  offset=offset<0?0:offset;
  offset=offset<_d.size()?offset:_d.size();
}
	
int amjPacket::push(const void *s, unsigned int n){
  if(offset+n>_d.size())
    _d.resize(offset+n);
  memcpy(_d.data()+offset,s,n);
  offset+=n;
  return n;
}

int amjPacket::pop(void *s, unsigned int n){
  if(offset+n>_d.size())
    return -1;
  memcpy(s,_d.data()+offset,n);
  offset+=n;
  return n;
}

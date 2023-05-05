#ifndef _AMJPACKETRW_H_
#define _AMJPACKETRW_H_

/*============================================================================
  All classes that want an object of their type to be writeable to a
  packet like this packet << object; and readable from a packet like
  this packet >> object, must public inherit either amjPacketRW or
  amjPacketRW2 and implement the pure virtual functions.

  amjPacketRW expects the object to use the built-in stream operators
  to write and read packet data. amjPacketRW2 allows the object to
  directly manipulate packet memory.
  
  

  The object can implement memsize to return the number of bytes that
  the packet should reserve in memory. This is for efficiency of
  memory allocation, and if the object does not know its size it can
  return a value zero or less. Then memory will be allocated as elementary
  types are added with <<.

  The function write is implemented to write the object to the
  packet. It can use the stream operators << or it can directly
  manipulate the packet memory. If it is directly manipulating the
  packet memory it must start writing at the offset provided as the
  second paramter. If writing using exclusively stream operators the
  second parameter can be ignored because the stream operators will
  start writing at the correct offset. The function must return the
  number of bytes written.
  
  The function read is implemented to read the object from the
  packet. It can use the stream operators >> or it can directly access
  the packet memory. If is is reading packet memory directly it must
  start reading at the offset provided in the second parameter. If
  reading exclusively using the stream operators the second paramter
  offset can be ignored as those read from the offset position. The
  function must return the number of bytes read.
  ===========================================================================*/

class amjPacket;

class amjPacketRW{
public:
  amjPacketRW(){};
  virtual ~amjPacketRW(){};
  virtual void write(amjPacket &p) const =0;
  virtual void read(amjPacket &p)=0;
private:
};

// class amjPacketRW2{
// public:
//   amjPacketRW(){};
//   virtual ~amjPacketRW(){};
//   virtual int memsize() const =0;
//   virtual int write(amjPacket &p, unsigned int offset=0) const =0;
//   virtual int read(const amjPacket &p, unsigned int offset=0)=0;
// private:
// };

#endif /* _AMJPACKETRW_H_ */

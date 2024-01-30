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

// class amjComServer{
// public:
//   amjComServer(){};
//   virtual ~amjComServer(){};
//   virtual amjComEndpoint* connection();
// private:
// };


/*

  all communication takes place as packets. Data can be packed into
  packets and unpacked from packets. Packets and sent and received. 
  
  An endpoint is the fundamental communication entity. It is one end
  of a connection. Call send on an endpoint to send a packet to the
  remote endpoint. A send can be blocking or non-blocking. Call
  receive on an endpoint to block until a packet is received from the
  remote endpoint. A callback function can be set to be called
  asynchronously when a packet is received. A callback function can be
  set to be called when the status of the connection changes.

  A client is a special case of an endpoint. It can be set to be
  persistent, which means that it will reconnect in case a connection
  is lost.

  A session is a special case of an endpoint. It is what a server
  creates when a connection is made to it. It will not reconnect after
  a disconnect.
  
  A server listens for connections. Call session on a server to block
  until a connection is received. A callback function can be supplied
  which will be called asynchronously when a connection is received. A
  callback function can be supplied which will be called
  asynchronously when the status of the server changes.

  Some special cases:

  Some connections can be made without a server. For example a UDP
  connection. For such connections both the local location and the
  remote location must be specified, and the endpoint will only send
  to the peer specified and will ignore packets not from the peer.

  A UDP server can be used to receive UDP packets from multiple
  peers. The server creates sessions for every new unique remote IP
  address/port. It will forward messages from peers to the sessions
  associated with it. 

*/

#endif /* _AMJCOM_H_ */


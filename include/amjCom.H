#ifndef _AMJCOM_H_
#define _AMJCOM_H_

#include <string>
#include <functional>
#include <memory>

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

namespace amjCom{
  enum Status {NoStatus,Ok,NotOk,NotFound};
  enum Error {NoError,ConnectError,PeerError,WriteError,ReadError,PacketError};

  /********** Packet **********************************************************/
  
  class Packet{
  public:
    Packet():p(0){};
    ~Packet(){};
    void clear(){_data.resize(0); p=0;};
    size_t size(){return _data.size();};
    void resize(size_t s);
    int pos(){return p;};
    void pos(size_t _p);
    void begin(){pos(0);};
    uint8_t *data(){return _data.data();};
    uint8_t *write(size_t n);
    uint8_t *read(size_t n);
  private:
    size_t p;
    std::vector<uint8_t> _data;
  };
  
  /********** Endpoint ********************************************************/
  
  class Endpoint{
  public:
    Endpoint(){}; // If the default construct is called,
		  // callback_receive and callback_status must be
		  // populated in another way
    Endpoint(// Packet receive callback function
	     std::function<void(Packet &)> callback_receive,
	     // Status callback
	     std::function<void(Status &)> callback_status):
      callback_receive(callback_receive),callback_status(callback_status){}
    virtual ~Endpoint(){};
    Status status(){return _status;}
    virtual void send(Packet &)=0;
  protected:
    Status _status;
    std::function<void(Packet &)> callback_receive;
    std::function<void(Status &)> callback_status;
  };
  
  /********** Session *********************************************************/
  
  class Session: public Endpoint{
  public:
    Session(){};
    virtual ~Session(){};
    virtual void start(// Callback for packet receive
		       std::function<void(Packet &)> callback_receive,
		       //Callback for status change
		       std::function<void(Status &)> callback_status)=0;
  };

  typedef std::shared_ptr<Session> pSession;
  
  /********** Server **********************************************************/
  
  class Server{
  public:
    Server(// Callback for new session
	   std::function<void(pSession)> callback_session,
	   // Callback for errors and disconnects
	   std::function<void(Status)> callback_status):
      callback_session(callback_session),callback_status(callback_status){};
    virtual ~Server(){};
  protected:
    std::function<void(pSession)> callback_session;
    std::function<void(Status)> callback_status;
  };

  /********** Client **********************************************************/
  
  class Client: public Endpoint{
  public:
    Client(){};
    Client(// Packet receive callback function
	     std::function<void(Packet &)> callback_receive,
	     // Status callback
	     std::function<void(Status &)> callback_status):
      Endpoint(callback_receive,callback_status){};
    virtual ~Client(){};
  };

  std::string split1(std::string s);
  std::string split2(std::string s);
}

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


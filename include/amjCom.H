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

namespace amjCom{
  /********** Status *********************************************************/

  // State explanations: NoState is the default value in
  // Status. WaitingToConnect means the communicator will try to
  // connect in the future, for example waiting for a timeout before
  // retrying. Connecting means is is actively connecting. Connected
  // means there is a connection. Disconnecting means it is actively
  // disconnecting. Disconnected means it is disconnected and not
  // going to try to reconnect. A typical sequence for a socket might
  // be Connecting -> Connected -> Disconnecting (after an error
  // occurs) -> WaitingToConnect (during reconnect wait) -> Connecting
  // -> -> Connected -> Disconnecting -> Disconnected (when it is
  // shut down for good locally or by peer)
  
  enum State {NoState=0,WaitingToConnect=1,Resolving=2,Connecting=3,
	      Connected=4,Disconnecting=5,Disconnected=6};

  // Error explanations: NoError is the default value in
  // Error. ConnectError means making a connection failed. PeerError
  // means connection is closed, but not cleanly. SendError means an
  // error occurs during a send. ReceiveError means an error occurs
  // during a receive. PacketError means an error occurs while
  // manipulating a package.

  enum Error {NoError=0,ConnectError=1,PeerError=2,SendError=3,ReceiveError=4,
	      PacketError=5};
  
  static std::vector<std::string> statedescriptions=
    {"NoState","WaitingToConnect","Resolving","Connecting","Connected",
     "Disconnecting","Disconnected"};
  static std::vector<std::string> errordescriptions=
    {"None","ConnectError","PeerError","SendError","ReceiveError",
     "PacketError"};
  
  class Status{
  public:
    Status(State state=NoState, Error error=NoError,
	   std::string errormessage=""):
      _state(state),_error(error),_errormessage(errormessage){};
    virtual ~Status(){};
    explicit operator bool(){if(_state==Connected) return true; return false;};
    State state() const {return _state;};
    std::string statedescription() const {return statedescriptions[_state];};
    Error error() const {return _error;};
    void errormessage(std::string e){_errormessage=e;};
    std::string errormessage() const {return _errormessage;};
  private:
    State _state;
    Error _error;
    std::string _errormessage;
  };
  
  /********** Packet *********************************************************/
  
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
  
  /********** Endpoint *******************************************************/
  
  class Endpoint{
  public:
    Endpoint(){};
    virtual ~Endpoint(){};
    Status status() const {return _status;}
    // returns false if not connected in a state that allows sending
    virtual bool send(Packet &)=0;
  protected:
    Status _status;
    virtual void status(Status s){_status=s;}
  };
  
  /********** Session ********************************************************/
  

  // Session is copyable and contains a shared pointer to _Session
  class Session: public Endpoint,
		 public std::enable_shared_from_this<Session>{
  public:
    Session(){};
    virtual ~Session(){};
    virtual void start(// Callback for packet receive
		       std::function<void(std::shared_ptr<Session>, Packet &)>
		       callback_receive,
		       //Callback for status change
		       std::function<void(std::shared_ptr<Session>, Status)>
		       callback_status)=0;
    using Endpoint::status;
  protected:
    void status(std::shared_ptr<Session> session, Status status){
      Endpoint::status(status);
      callback_status(session,status);
    };
    std::function<void(std::shared_ptr<Session>, Packet &)> callback_receive;
    std::function<void(std::shared_ptr<Session>, Status)> callback_status;
  };
  
  typedef std::shared_ptr<Session> pSession;
  
  /********** Server *********************************************************/

  class _Server: public std::enable_shared_from_this<_Server>{
  public:
    _Server(// Callback for new session
	    std::function<void(std::shared_ptr<_Server>, pSession)>
	    callback_session,
	   // Callback for errors and disconnects
	    std::function<void(std::shared_ptr<_Server>, Status)>
	    callback_status):
      callback_session(callback_session),callback_status(callback_status){};
    virtual ~_Server(){};
    virtual void start()=0;
  protected:
    std::function<void(std::shared_ptr<_Server>, pSession)> callback_session;
    std::function<void(std::shared_ptr<_Server>, Status)> callback_status;
  };
  
  using Server=std::shared_ptr<_Server>;
  
  /********** Client *********************************************************/
  
  class _Client: public Endpoint{
  public:
    _Client(){};
    _Client(// Packet receive callback function
	    std::function<void(std::shared_ptr<_Client>, Packet &)>
	    callback_receive,
	    // Status callback
	    std::function<void(std::shared_ptr<_Client>, Status)>
	    callback_status):
      callback_receive(callback_receive),callback_status(callback_status){};
    virtual ~_Client(){};
    virtual void start()=0;
    using Endpoint::status;
  protected:
    void status(std::shared_ptr<_Client> c, Status s){
      Endpoint::status(s);
      callback_status(c,_status);
    };
    std::function<void(std::shared_ptr<_Client>, Packet &)> callback_receive;
    std::function<void(std::shared_ptr<_Client>, Status)> callback_status;
  };

  using Client=std::shared_ptr<_Client>;
  
  std::string split1(std::string s);
  std::string split2(std::string s);
}

#endif /* _AMJCOM_H_ */

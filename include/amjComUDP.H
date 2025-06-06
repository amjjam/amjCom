#ifndef _AMJCOMUDP_H_
#define _AMJCOMUDP_H_

#include "amjCom.H"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_DGRAM 65536

class amjComEndpointUDP: amjComEndpoint{
public:
  amjComEndpointUDP(std::string, std::string);
  virtual ~amjComEndpointUDP(){};
  virtual int send(amjPacket &);
  virtual int receive(amjPacket &);
private:
  struct sockaddr_in myaddr;
  bool one_peer;
  struct sockaddr_in peeraddr;
  int sockfd;
  std::vector<char> buffer;
};

struct sockaddr_in make_sockaddr_in(std::string &s);
bool compare_sockaddr_in(struct sockaddr_in &, struct sockaddr_in &);

// namespace amjCom{
//   namespace UDP{
//     class Common: public std::enable_shared_from_this<Common>{
//     public:
//       Common(IOContext iocontext):
// 	iocontext(iocontext),
// 	_socket_ptr(new asio::ip::udp::socket(iocontext.io_context())){};
//       asio::ip::udp::socket &socket(){return *_socket_ptr;};
//       ~Common();
//     protected:
//       IOContext iocontext;
//       virtual State state()=0;
//       virtual bool send1(Packet &);
//       void receive1();
//       virtual void shutdown();
//       virtual void error_handler(std::string,Error,asio::error_code)=0;
//     private:
      

//     };
    
//     class Server: public Common, public amjCom::Server{
//     public:
//       Server(// Callback for new session
// 	     std::function<void(pSession)> callback_session,
// 	     // Callback for errors and disconnects
// 	     std::function<void(Status)> callback_status):
// 	callback_session(callback_session),callback_status(callback_status){};
//       ~Server();
//     private:
//       std::function<void(pSession)> callback_session;
//       std::function<void(Status)> callback_status;
//     };


//     class Client: public Common, public amjCom::Client{
//     public:
//       Client(// Description of the server
// 	     std::string &server,
// 	     // Packet receive callback function
// 	     std::function<void(Packet &)> callback_receive,
// 	     // Status callback
// 	     std::function<void(Packet &)> callback_status,
// 	     // Optional IOContext
// 	     IOContext iocontext=IOContext());
//       virtual ~Client(){};
//       virtual void start(){resolve();};
//       virtual bool send(Packet &p){return send1(p);};
//     private:
//       std::string server;
//       asio::ip::udp::resolver resolver;
//       asio::ip::udp::resolver::results_type endpoints;
//       asio::ip::udp::endpoint endpoint;
//       virtual State state(){return _status.state();};

//       virtual void error_handler(std::string,Error,std::system_error);
//       virtual void error_handler(std::string,Error,asio::error_code);
//       void resolve();
//       void connect();
//       virtual void callback_connect(asio::error_code, const asio::ip::udp::endpoint);
//       void callback_timer(asio::error_code);
//     };


    

      
//   }
// }

#endif /* _AMJCOMUDP_H_ */


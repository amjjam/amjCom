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


// Below is the code written by ChatGPT

// --- Snippet: Update to amjComUDP.H ---

// #ifndef _AMJCOMUDP_H_
// #define _AMJCOMUDP_H_

// #include <unordered_map>
// #include <memory>
// #include <system_error>
// #include <chrono>

// #include "amjCom.H"
// #include "amjComASIO.H"

// namespace amjCom {
//   namespace UDP {

//     class Common {
//     public:
//       Common(IOContext iocontext);
//       virtual ~Common();
//       asio::ip::udp::socket& socket() { return *_socket_ptr; }
//     protected:
//       IOContext iocontext;
//       std::shared_ptr<asio::ip::udp::socket> _socket_ptr;
//       uint8_t header[4];
//       Packet p;
//       asio::ip::udp::endpoint remote_endpoint;
//       virtual std::shared_ptr<Common> getself() = 0;
//       virtual State state() = 0;
//       virtual void shutdown();
//       virtual void error_handler(std::string, Error, asio::error_code) = 0;
//       virtual void receive4(Packet&, asio::ip::udp::endpoint&) = 0;
//       void receive1();
//       void receive2(asio::error_code, std::size_t);
//       bool send1(Packet& p, const asio::ip::udp::endpoint& to);
//       void send2(asio::error_code, std::size_t);
//     };

//     class _Session : public Common, public amjCom::_Session {
//     public:
//       _Session(IOContext iocontext, asio::ip::udp::endpoint endpoint);
//       virtual ~_Session() {}
//       asio::ip::udp::endpoint endpoint() const { return _remote_endpoint; }
//       bool send(Packet& p) { return send1(p, _remote_endpoint); }
//       void deliver(Packet& p);
//       void update_activity();
//       std::chrono::steady_clock::time_point last_activity() const { return _last_activity; }
//       virtual void start(
//         std::function<void(amjCom::Session, Packet&)> callback_receive,
//         std::function<void(amjCom::Session, Status)> callback_status);
//     protected:
//       std::shared_ptr<Common> getself() override {
//         return std::dynamic_pointer_cast<Common>(shared_from_this());
//       }
//       void status(Status s) override {
//         amjCom::_Session::status(shared_from_this(), s);
//       }
//     private:
//       State state() override { return _status.state(); }
//       void receive4(Packet& p, asio::ip::udp::endpoint&) override {
//         callback_receive(shared_from_this(), p);
//       }
//       void error_handler(std::string, Error, asio::error_code) override;
//       asio::ip::udp::endpoint _remote_endpoint;
//       std::chrono::steady_clock::time_point _last_activity;
//     };

//     using Session = std::shared_ptr<_Session>;

//     class _Server : public amjCom::_Server,
//                     public std::enable_shared_from_this<_Server> {
//     public:
//       static std::shared_ptr<_Server> create(
//         const std::string& address,
//         std::function<void(amjCom::Server, amjCom::Session)> callback_session,
//         std::function<void(amjCom::Server, Status)> callback_status,
//         IOContext iocontext = IOContext(),
//         int timeout_seconds = 60);
//       void start() override;
//       virtual ~_Server() {}
//     private:
//       _Server(const std::string& address,
//               std::function<void(amjCom::Server, amjCom::Session)> callback_session,
//               std::function<void(amjCom::Server, Status)> callback_status,
//               IOContext iocontext,
//               int timeout_seconds);
//       IOContext iocontext;
//       std::unordered_map<std::string, Session> sessions;
//       asio::ip::udp::endpoint remote_endpoint;
//       asio::steady_timer aging_timer;
//       int timeout_seconds;
//       void handle_receive(asio::error_code, std::size_t);
//       void check_session_timeouts();
//       void session_error_handler(std::string, Error, asio::error_code);
//       std::string endpoint_key(const asio::ip::udp::endpoint&);
//     };

//     using Server = std::shared_ptr<_Server>;

//     Server create_server(const std::string& address,
//                          std::function<void(amjCom::Server, amjCom::Session)> callback_session,
//                          std::function<void(amjCom::Server, Status)> callback_status,
//                          IOContext iocontext = IOContext(),
//                          int timeout_seconds = 60);

//     class _Client : public Common, public amjCom::_Client,
//                     public std::enable_shared_from_this<_Client> {
//     public:
//       static std::shared_ptr<_Client> create(
//         const std::string& server,
//         std::function<void(amjCom::Client, Packet&)> callback_receive,
//         std::function<void(amjCom::Client, Status)> callback_status,
//         IOContext iocontext = IOContext());
//       virtual ~_Client() {}
//       virtual void start();
//       virtual bool send(Packet& p);
//     protected:
//       std::shared_ptr<Common> getself() override {
//         return std::static_pointer_cast<Common>(shared_from_this());
//       }
//     private:
//       _Client(const std::string& server,
//               std::function<void(amjCom::Client, Packet&)> callback_receive,
//               std::function<void(amjCom::Client, Status)> callback_status,
//               IOContext iocontext);
//       std::string server;
//       asio::ip::udp::endpoint server_endpoint;
//       asio::ip::udp::resolver resolver;
//       State state() override { return _status.state(); }
//       void receive4(Packet& p, asio::ip::udp::endpoint&) override {
//         callback_receive(shared_from_this(), p);
//       }
//       void error_handler(std::string, Error, asio::error_code) override;
//       void resolve();
//       void on_resolve(asio::error_code, asio::ip::udp::resolver::results_type);
//     };

//     using Client = std::shared_ptr<_Client>;

//     Client create_client(const std::string& server,
//                          std::function<void(amjCom::Client, Packet&)> callback_receive,
//                          std::function<void(amjCom::Client, Status)> callback_status,
//                          IOContext iocontext = IOContext());

//   } // namespace UDP
// } // namespace amjCom

// #endif // _AMJCOMUDP_H_

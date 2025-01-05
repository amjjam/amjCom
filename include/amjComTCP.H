#ifndef _AMJCOMTCP_H_
#define _AMJCOMTCP_H_

#include "amjCom.H"
#include "amjComASIO.H"

namespace amjCom{
  namespace TCP{
    class Common{
    public:
      Common(IOContext iocontext):
	iocontext(iocontext),
	_socket_ptr(new asio::ip::tcp::socket(iocontext.io_context())){};
      virtual ~Common(){};
      asio::ip::tcp::socket &socket(){return *_socket_ptr;};
    protected:
      IOContext iocontext;
      virtual void send1(Packet &);
      void receive1();
      virtual void shutdown();
      virtual void error_handler(std::string,Error,asio::error_code)=0;
    private:
      std::shared_ptr<asio::ip::tcp::socket> _socket_ptr;
      uint8_t header[4];
      Packet p;
      void send2(asio::error_code,std::size_t);
      void receive2(asio::error_code,std::size_t);
      void receive3(asio::error_code,std::size_t);
      // receive4 is defined in Session and Client to call
      // callback_receive because callback_receive is defined in base
      // class Endpoint and unknown in class Common
      virtual void receive4(Packet &p)=0; 
    };
    
    class Session: public Common, public amjCom::Session{
    public:
      Session(IOContext iocontext):Common(iocontext){};
      virtual ~Session(){};
      virtual void start(// Callback for packet receive
			 std::function<void(Packet &)> callback_receive,
			 // Callback for status change
			 std::function<void(Status)> callback_status);
      void send(Packet &p){send1(p);};
    private:
      virtual void receive4(Packet &p){callback_receive(p);};
      virtual void error_handler(std::string, Error, asio::error_code);
    };

    typedef std::shared_ptr<Session> pSession;
    
    class Server: public amjCom::Server{
    public:
      Server(// Description of where to listen, format is "<address>:<port>"
	     const std::string &,
	     // Callback for new session
	     std::function<void(amjCom::pSession)> callback_session,
	     // Callback for errors and disconnects
	     std::function<void(Status)> callback_status,
	     // Optional IOContext
	     IOContext iocontext_=IOContext());
      virtual ~Server(){};
    private:
      IOContext iocontext;
      asio::ip::tcp::acceptor acceptor;
      void accept_connection();
      void accept_callback(pSession, const asio::error_code &);
      void server_error_handler(){/* for the case that server creation
				     fails */};
      void accept_error_handler(){/* for the case that an accept fails */};
    };
    
    class Client: public Common, public amjCom::Client{
    public:
      Client(// Description of the server
	     std::string &server,
	     // Packet receive callback function
	     std::function<void(Packet &)> callback_receive,
	     // Status callback
	     std::function<void(Status)> callback_status,
	     // Optional IOContext
	     IOContext iocontext=IOContext());
      virtual ~Client(){};
      void send(Packet &p){send1(p);};
    private:
      std::string server;
      asio::ip::tcp::resolver resolver;
      asio::steady_timer timer;
      asio::ip::tcp::resolver::results_type endpoints;
      asio::ip::tcp::endpoint endpoint;
      virtual void receive4(Packet &p){callback_receive(p);};
      virtual void error_handler(std::string,Error,asio::error_code);
      void resolve();
      void connect();
      void callback_connect(asio::error_code, const asio::ip::tcp::endpoint);
      void callback_timer(asio::error_code);
    };
  }
}


#endif /* _AMJCOMTCP_H_ */

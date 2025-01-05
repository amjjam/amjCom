#include "../include/amjComTCP.H"

namespace amjCom{
  namespace TCP{
    void Common::send1(Packet &p){
      std::vector<uint8_t> d(4+p.size());
      //std::cout << "send1: p.size()=" << p.size() << std::endl;
      uint32_t s=p.size();
      memcpy(d.data(),&s,sizeof(uint32_t));
      memcpy(d.data()+4,p.data(),p.size());
      asio::async_write(socket(),asio::buffer(d.data(),d.size()),
			[&](asio::error_code e, std::size_t s){send2(e,s);});
    }
    
    void Common::send2(asio::error_code error, std::size_t s){
      /* here is where we handle the status of a send */
      if(error){
	std::cout << "send2: error: " << error.message() << std::endl;
	error_handler("send2: error: ",SendError,error);
      }
    }
    
    void Common::receive1(){
      /* asynchronous read header */
      asio::async_read(socket(),asio::buffer(header,4),
		       [&](asio::error_code e, std::size_t s){
			 receive2(e,s);});
    }
    
    void Common::receive2(asio::error_code error, std::size_t nh){
      /* read header callback */
      if(error||nh!=4){
	std::cout << "amjComTCP::Common::receive2: n=" << nh << ", "
		  << error.message() << std::endl;
	error_handler("receive2: error: ",ReceiveError,error);
	return;
      }
      uint32_t nb;
      memcpy(&nb,header,sizeof(uint32_t));

      /* asynchronous read body */
      p.clear();
      asio::async_read(socket(),asio::buffer(p.write(nb),nb),
		       [&](asio::error_code e, std::size_t s)
		       {receive3(e,s);});
    }

    void Common::receive3(asio::error_code error, std::size_t nb){
      if(error){
	std::cout << "amjComTCP::Common::receive3: error: "
		  << error.message() << std::endl;
	error_handler("receive3: error: ",ReceiveError,error);
	return;
      }
      /* callback to application - via derived class - with packet */
      std::cout << "receive3: p.size()=" << p.size() << std::endl;
      receive4(p);
      
      /* start next receive */
      receive1();
    }

    void Common::shutdown(){
      socket().shutdown(asio::ip::tcp::socket::shutdown_both);
      socket().close();
    }
    
    void Session::start(std::function<void(Packet &)> _callback_receive,
			std::function<void(Status)> _callback_status){
      callback_receive=_callback_receive;
      callback_status=_callback_status;
      receive1();
    }
    
    void Session::error_handler(std::string error_prefix, Error error,
				asio::error_code asio_error){
      Common::shutdown();
      callback_status(Status(Disconnected,error,
			     error_prefix+asio_error.message()));
    }
    
    Server::Server(const std::string &server,
		   std::function<void(amjCom::pSession)> callback_session,
		   std::function<void(Status)> callback_status,
		   IOContext iocontext_):
      amjCom::Server(callback_session,callback_status),iocontext(iocontext_),
      acceptor(iocontext.io_context(),
 	       asio::ip::tcp::endpoint(asio::ip::tcp::v4(),
				       atoi(split2(server).c_str()))){
      std::cout << "Server: port: " << atoi(split2(server).c_str())
		<< std::endl;
      /* somewhere in here should be the test for the server being up
	 and the calling of server_error_handle1() */
      
      accept_connection();
    }
    
    void Server::accept_connection(){
      std::cout << "Server: accept_connection" << std::endl;
      pSession new_session=pSession(new Session(iocontext));
      acceptor.async_accept(new_session->socket(),
       			    [&,new_session](const asio::error_code& error)
       			    {accept_callback(new_session,error);});
    }
    
    void Server::accept_callback(pSession new_session,
				 const asio::error_code& error){
      std::cout << "Server: accept_callback" << std::endl;
      if(error){
	accept_error_handler(); /* Nothing to be done */
	return;
      }
      callback_session(new_session);
      accept_connection();
    }
    
    Client::Client(// Server to connect to <address>:<port>
		   std::string &server,
		   // Packet receive callback function
		   std::function<void(Packet &)> callback_receive,
		   // Status callback
		   std::function<void(Status)> callback_status,
		   // Optional IOContext
		   IOContext iocontext):
      Common(iocontext),amjCom::Client(callback_receive,callback_status),
      server(server),resolver(iocontext.io_context()),
      timer(iocontext.io_context()){
      resolve();
    }
    
    void Client::resolve(){
      /* For the moment do a synchronous resolve and then call
	 connect(). Later I will make this the initiation of a
	 async_resolve, which will call callback_resolve when
	 complete, and from there connect() will be called */
      
      endpoints=resolver.resolve(split1(server),split2(server));
      
      connect();
    }
          
    void Client::connect(){
      /* asynchronous connect */
      
      asio::async_connect(socket(),endpoints,
			  [&](asio::error_code e,
			      asio::ip::tcp::endpoint p){
			    callback_connect(e,p);
			  });
    }
    
    void Client::callback_connect(asio::error_code error,
				  asio::ip::tcp::endpoint ep){
      if(error){
	std::cout << "Client::callback_connect: error: " << error.message()
		  << std::endl;
	error_handler("connect: error: ",ConnectError,error);
	return;
      }
      
      receive1();
    }    
    
    void Client::error_handler(std::string error_prefix,Error error,
			       asio::error_code asio_error){
      Common::shutdown();

      /* report status to application */
      callback_status(Status(Connecting,error,
			     error_prefix+asio_error.message()));

      /* reconnect after a time */
      timer.expires_after(std::chrono::seconds(1));
      timer.async_wait([&](asio::error_code e){callback_timer(e);});
    }
    
    void Client::callback_timer(asio::error_code error){
      if(error){
	std::cout << "Client::callback_timer: error: " << error.message()
		  << std::endl;
	callback_status(Status(Disconnected,ConnectError,
			       "connect: wait timer: error: "+error.message()));
	return;
      }
      connect();
    }
    
  }
}

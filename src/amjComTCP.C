#include "../include/amjComTCP.H"

namespace amjCom{
  namespace TCP{
    Common::~Common(){
      shutdown();
    }
    
    bool Common::send1(Packet &p){
      if(state()!=Connected)
	return false;
      std::vector<uint8_t> d(4+p.size());
      uint32_t s=p.size();
      memcpy(d.data(),&s,sizeof(uint32_t));
      memcpy(d.data()+4,p.data(),p.size());
      auto self=getself();
      asio::async_write(socket(),asio::buffer(d.data(),d.size()),
			[self](asio::error_code e, std::size_t s)
			{self->send2(e,s);});
      return true;
    }
    
    void Common::send2(asio::error_code error, std::size_t s){
      /* here is where we handle the status of a send */
      if(error)
	error_handler("send2: error: ",SendError,error);
    }
    
    void Common::receive1(){
      /* asynchronous read header */
      std::shared_ptr<Common> self=getself();
      asio::async_read(socket(),asio::buffer(header,4),
		       [self](asio::error_code e, std::size_t s){
			 if(self.use_count()==1) return;
			 self->receive2(e,s);});
    }
    
    void Common::receive2(asio::error_code error, std::size_t nh){
      /* read header callback */
      if(error||nh!=4){
	error_handler("receive2: error: n="+std::to_string(nh)+": ",
		      ReceiveError,error);
	return;
      }
      uint32_t nb;
      memcpy(&nb,header,sizeof(uint32_t));
      
      /* asynchronous read body */
      p.clear();
      std::shared_ptr<Common> self=getself();
      asio::async_read(socket(),asio::buffer(p.write(nb),nb),
		       [self](asio::error_code e, std::size_t s)
		       {self->receive3(e,s);});
    }
    
    void Common::receive3(asio::error_code error, std::size_t nb){
      if(error){
	std::cout << "amjComTCP::Common::receive3: error: "
		  << error.message() << std::endl;
	error_handler("receive3: error: ",ReceiveError,error);
	return;
      }
      /* callback to application - via derived class - with packet */
      p.begin();
      receive4(p);
      
      /* start next receive */
      receive1();
    }
    
    void Common::shutdown(){
      try{
	socket().shutdown(asio::ip::tcp::socket::shutdown_both);
	//socket().close();
      }
      catch(const std::exception &e){
	std::cout << "Common::shutdown::catch" << std::endl;
      }
    }
    
    void _Session::start(std::function<void(amjCom::Session, Packet &)>
			 _callback_receive,
			 std::function<void(amjCom::Session, Status)>
			 _callback_status){
      callback_receive=_callback_receive;
      callback_status=_callback_status;
      status(Status(Connected));
      receive1();
    }
    
    void _Session::error_handler(std::string error_prefix, Error error,
				 asio::error_code asio_error){
      Common::shutdown();
      status(Status(Disconnected,error,error_prefix+asio_error.message()));
    }
    
    std::shared_ptr<_Server> _Server::create
    (const std::string &server,
     std::function<void(amjCom::Server, amjCom::Session)> callback_session,
     std::function<void(amjCom::Server, Status)> callback_status,
     IOContext iocontext_){
      
      return std::shared_ptr<_Server>
	(new _Server(server,callback_session,callback_status,iocontext_));
    }
    
    _Server::_Server(const std::string &server,
		     std::function<void(amjCom::Server, amjCom::Session)>
		     callback_session,
		     std::function<void(amjCom::Server, Status)>
		     callback_status,
		     IOContext iocontext_):
      amjCom::_Server(callback_session,callback_status),iocontext(iocontext_),
      acceptor(iocontext.io_context(),
 	       asio::ip::tcp::endpoint(asio::ip::tcp::v4(),
				       atoi(split2(server).c_str()))){
      std::cout << "Server: port: " << atoi(split2(server).c_str())
		<< std::endl;
      /* somewhere in here should be the test for the server being up
	 and the calling of server_error_handle1() */
      
      /* accept_connection(); now done with start() instead */
    }
    
    void _Server::accept_connection(){
      std::cout << "Server: accept_connection" << std::endl;
      Session next_session=std::make_shared<amjCom::TCP::_Session>(iocontext);
      std::shared_ptr<_Server> self=
	std::static_pointer_cast<_Server>(shared_from_this());
      acceptor.async_accept(next_session->socket(),
       			    [self,next_session]
			    (const asio::error_code& error)
       			    {if(self.use_count()==1) return;
			      self->accept_callback(next_session,error);});
    }
    
    void _Server::accept_callback(Session next_session,
				 const asio::error_code& error){
      std::cout << "Server: accept_callback" << std::endl;
      if(error){
	accept_error_handler(); /* Nothing to be done */
	return;
      }
      callback_session(shared_from_this(),next_session);
      accept_connection();
    }


    Server create_server(const std::string &address,
			 std::function<void(amjCom::Server, amjCom::Session)>
			 callback_session,
			 std::function<void(amjCom::Server, Status)>
			 callback_status,
			 IOContext iocontext_){
      return _Server::create(address,callback_session,callback_status,
			     iocontext_);
    }
    
    std::shared_ptr<_Client> _Client::create
    (const std::string &server,
     std::function<void(amjCom::Client, amjCom::Packet &)> callback_receive,
     std::function<void(amjCom::Client, amjCom::Status)> callback_status,
     IOContext iocontext_){
      
      return std::shared_ptr<_Client>
	(new _Client(server,callback_receive,callback_status,iocontext_));
    }

    _Client::_Client(// Server to connect to <address>:<port>
		     const std::string &server,
		     // Packet receive callback function
		     std::function<void(amjCom::Client, amjCom::Packet &)>
		     callback_receive,
		     // Status callback
		     std::function<void(amjCom::Client, amjCom::Status)>
		     callback_status,
		     // Optional IOContext
		     IOContext iocontext):
      Common(iocontext),amjCom::_Client(callback_receive,callback_status),
      server(server),resolver(iocontext.io_context()),
      timer(iocontext.io_context(),std::chrono::seconds(5)){
      /* resolve(); now done with start() instead */
    }
    
    void _Client::resolve(){
      /* For the moment do a synchronous resolve and then call
	 connect(). Later I will make this the initiation of a
	 async_resolve, which will call callback_resolve when
	 complete, and from there connect() will be called */
      status(Status(Resolving));
      try{
	endpoints=resolver.resolve(split1(server),split2(server));
      }
      catch(const std::system_error &error){
	error_handler("resolve: error: ",ConnectError,error);
	return;
      }
      catch(const asio::error_code &error){
	error_handler("resolve: error: ",ConnectError,error);
	return;
      }
      connect();
    }
    
    void _Client::connect(){
      /* asynchronous connect */
      
      status(Status(Connecting));
      std::shared_ptr<_Client> self=
	std::static_pointer_cast<_Client>(shared_from_this());
      asio::async_connect
	(socket(),endpoints,
	 [self](asio::error_code e, asio::ip::tcp::endpoint p){
	   self->callback_connect(e,p);});
    }
    
    void _Client::callback_connect(asio::error_code error,
				  asio::ip::tcp::endpoint ep){
      if(error){
	std::cout << "Client::callback_connect: error: " << error.message()
		  << std::endl;
	error_handler("connect: error: ",ConnectError,error);
	return;
      }
      
      std::cout << "Client::callback_connect: starting receive1" << std::endl;
      status(Status(Connected));
      receive1();
    }    
    
    void _Client::error_handler(std::string error_prefix,Error error,
			       std::system_error system_error){
      Common::shutdown();
      
      /* report status to application */
      status(Status(WaitingToConnect,error,error_prefix+"error code: "+
                    std::to_string(system_error.code().value())+" "
		    +system_error.what()));

      std::cout << "Client::error_handler: starting timer" << std::endl;
      
      /* reconnect after a time */
      timer.expires_after(std::chrono::seconds(5));
      std::shared_ptr<_Client> self=
	std::static_pointer_cast<_Client>(shared_from_this());
      timer.async_wait([self](asio::error_code e){self->callback_timer(e);});
    }
    
    void _Client::error_handler(std::string error_prefix,Error error,
			       asio::error_code asio_error){
      Common::shutdown();
      
      /* report status to application */
      status(Status(WaitingToConnect,error,error_prefix+asio_error.message()));
      
      std::cout << "Client::error_handler: starting timer" << std::endl;
      
      /* reconnect after a time */
      timer.expires_after(std::chrono::seconds(5));
      std::shared_ptr<_Client> self=
	std::static_pointer_cast<_Client>(shared_from_this());
      timer.async_wait([self](asio::error_code e){self->callback_timer(e);});
    }
    
    void _Client::callback_timer(asio::error_code error){
      if(error){
	std::cout << "Client::callback_timer: error: " << error.message()
		  << std::endl;
	sleep(1); // This blocks the thread, but there isn't much else
		  // to do when the timer fails.
	error_handler("Client::callback_timer: error: ",ConnectError,error);
	status(Status(WaitingToConnect,ConnectError,
		      "connect: wait timer: error: "+error.message()));
	return;
      }
      resolve();
    }

    Client create_client(const std::string &server,
			 std::function<void(amjCom::Client, Packet &)>
			 callback_receive,
			 std::function<void(amjCom::Client, amjCom::Status)>
			 callback_status,
			 IOContext iocontext){
      return _Client::create(server,callback_receive,callback_status,
			     iocontext);
    }

  }
}

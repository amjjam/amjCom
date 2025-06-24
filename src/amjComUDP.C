#include "../include/amjComUDP.H"

#include <cstring>

amjComEndpointUDP::amjComEndpointUDP(std::string me, std::string peer){
  buffer.resize(MAX_DGRAM);
  myaddr=make_sockaddr_in(me);
  peeraddr=make_sockaddr_in(peer);
  one_peer=(peer.size()>0);
  
  if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  if(bind(sockfd,(const struct sockaddr *)&myaddr,
	  sizeof(struct sockaddr_in))<0){
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
}

int amjComEndpointUDP::send(amjPacket &p){
  return sendto(sockfd,p._data(),p.size(),0,(struct sockaddr *)&peeraddr,
		sizeof(struct sockaddr_in));
}

int amjComEndpointUDP::receive(amjPacket &p){
  struct sockaddr_in senderaddr;
  socklen_t len=sizeof(sockaddr_in);
  int n;
  if((n=recvfrom(sockfd,buffer.data(),MAX_DGRAM,MSG_WAITALL,
		 (struct sockaddr *)&senderaddr,&len))<0){
    perror("error receiving packet");
    exit(EXIT_FAILURE);
  }
  
  if(one_peer)
    if(!compare_sockaddr_in(senderaddr,peeraddr)){
      perror("datagram not from peer");
      exit(EXIT_FAILURE);
    }
  p.resize(n);
  memcpy(p._data(),buffer.data(),n);
  p.reset();
  return 0;
}


struct sockaddr_in make_sockaddr_in(std::string &s){
  struct sockaddr_in a;
  memset(&a,0,sizeof(struct sockaddr_in));
  a.sin_family=AF_INET;
  if(s.size()==0){
    a.sin_port=htons(0);
    a.sin_addr.s_addr=INADDR_ANY;
  }
  else{
    a.sin_port=htons(std::stoi(amjCom::split2(s)));
    a.sin_addr.s_addr=inet_addr(amjCom::split1(s).c_str());
  }
  
  return a;
}

#include <iostream>
bool compare_sockaddr_in(struct sockaddr_in &s1, struct sockaddr_in &s2){
  if((s1.sin_port==s2.sin_port)&&(s1.sin_addr.s_addr==s2.sin_addr.s_addr))
    return true;
  return false;
}

// Below is the code written by ChatGPT

// --- Implementation: amjComUDP.C ---

// #include "amjComUDP.H"
// #include <iostream>

// namespace amjCom {
//   namespace UDP {

//     Common::Common(IOContext ctx)
//       : iocontext(ctx),
//         _socket_ptr(std::make_shared<asio::ip::udp::socket>(ctx.io_context(), asio::ip::udp::v4())) {}

//     Common::~Common() {
//       shutdown();
//     }

//     void Common::shutdown() {
//       try {
//         socket().close();
//       } catch (...) {}
//     }

//     bool Common::send1(Packet& p, const asio::ip::udp::endpoint& to) {
//       if (state() != Connected) return false;
//       std::vector<uint8_t> d(4 + p.size());
//       uint32_t s = p.size();
//       memcpy(d.data(), &s, sizeof(uint32_t));
//       memcpy(d.data() + 4, p.data(), p.size());
//       auto self = getself();
//       socket().async_send_to(asio::buffer(d), to,
//         [self](asio::error_code ec, std::size_t len) {
//           self->send2(ec, len);
//         });
//       return true;
//     }

//     void Common::send2(asio::error_code error, std::size_t) {
//       if (error) error_handler("send", SendError, error);
//     }

//     void Common::receive1() {
//       auto self = getself();
//       socket().async_receive_from(asio::buffer(header, 4), remote_endpoint,
//         [self](asio::error_code ec, std::size_t len) {
//           if (ec || len != 4) {
//             self->error_handler("recv header", ReceiveError, ec);
//             return;
//           }
//           self->receive2(ec, len);
//         });
//     }

//     void Common::receive2(asio::error_code ec, std::size_t) {
//       uint32_t n;
//       memcpy(&n, header, 4);
//       p.clear();
//       auto self = getself();
//       socket().async_receive_from(asio::buffer(p.write(n), n), remote_endpoint,
//         [self](asio::error_code err, std::size_t nb) {
//           if (!err) {
//             self->p.begin();
//             self->receive4(self->p, self->remote_endpoint);
//           } else {
//             self->error_handler("recv body", ReceiveError, err);
//           }
//           self->receive1();
//         });
//     }

//     _Session::_Session(IOContext ctx, asio::ip::udp::endpoint ep)
//       : Common(ctx), _remote_endpoint(ep) {
//       update_activity();
//     }

//     void _Session::start(std::function<void(amjCom::Session, Packet&)> rcv,
//                          std::function<void(amjCom::Session, Status)> stat) {
//       callback_receive = rcv;
//       callback_status = stat;
//       status(Status(Connected));
//     }

//     void _Session::deliver(Packet& p) {
//       this->p = p;
//       p.begin();
//       receive4(p, _remote_endpoint);
//       update_activity();
//     }

//     void _Session::update_activity() {
//       _last_activity = std::chrono::steady_clock::now();
//     }

//     void _Session::error_handler(std::string msg, Error e, asio::error_code ec) {
//       status(Status(Disconnected, e, msg + ec.message()));
//     }

//     std::string _Server::endpoint_key(const asio::ip::udp::endpoint& ep) {
//       return ep.address().to_string() + ":" + std::to_string(ep.port());
//     }

//     std::shared_ptr<_Server> _Server::create(const std::string& address,
//       std::function<void(amjCom::Server, amjCom::Session)> cb_session,
//       std::function<void(amjCom::Server, Status)> cb_status,
//       IOContext ctx, int timeout_s) {
//       return std::make_shared<_Server>(address, cb_session, cb_status, ctx, timeout_s);
//     }

//     _Server::_Server(const std::string& addr,
//                      std::function<void(amjCom::Server, amjCom::Session)> cb_session,
//                      std::function<void(amjCom::Server, Status)> cb_status,
//                      IOContext ctx, int timeout_s)
//       : amjCom::_Server(cb_session, cb_status),
//         iocontext(ctx),
//         aging_timer(ctx.io_context()),
//         timeout_seconds(timeout_s) {
//       auto port = atoi(split2(addr).c_str());
//       _socket_ptr = std::make_shared<asio::ip::udp::socket>(ctx.io_context(), asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
//     }

//     void _Server::start() {
//       receive1();
//       aging_timer.expires_after(std::chrono::seconds(timeout_seconds));
//       aging_timer.async_wait([self = shared_from_this()](asio::error_code) {
//         self->check_session_timeouts();
//         self->start(); // restart timer
//       });
//     }

//     void _Server::handle_receive(asio::error_code ec, std::size_t len) {
//       if (ec || len != 4) return; // malformed
//       auto key = endpoint_key(remote_endpoint);
//       auto it = sessions.find(key);
//       if (it == sessions.end()) {
//         auto session = std::make_shared<_Session>(iocontext, remote_endpoint);
//         session->start(callback_session, callback_status);
//         sessions[key] = session;
//         callback_session(shared_from_this(), session);
//       }
//       Packet p;
//       uint32_t nb;
//       memcpy(&nb, header, sizeof(uint32_t));
//       p.write(nb);
//       socket().receive_from(asio::buffer(p.data(), nb), remote_endpoint);
//       p.begin();
//       sessions[key]->deliver(p);
//     }

//     void _Server::check_session_timeouts() {
//       auto now = std::chrono::steady_clock::now();
//       for (auto it = sessions.begin(); it != sessions.end(); ) {
//         auto session = it->second;
//         auto age = std::chrono::duration_cast<std::chrono::seconds>(now - session->last_activity());
//         if (age.count() > timeout_seconds) {
//           session->status(Status(Disconnected, TimeoutError, "Session timeout"));
//           it = sessions.erase(it);
//         } else {
//           ++it;
//         }
//       }
//     }

//   } // namespace UDP
// } // namespace amjCom


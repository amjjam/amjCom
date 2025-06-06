#include "../../include/amjComTCP.H"
#include <amjArg.H>

#include <iostream>


amjArg::Help help({
    /*========================================================================*/
    "tcptest",
    "",
    "Test the amjCom::TCP functionality",
    "--server <string> run as server listenning on <string>=address:port",
    "--client <string> run as client connecting to <string>=address:port"
    /*========================================================================*/
  });

void parse_args(int argc, char *argv[]);

bool isServer;
std::string address;
int sessionID=0;

void server_session(amjCom::pSession);
void server_status(amjCom::Status);
void session_receive(int sessionID, amjCom::Packet &);
void session_status(int sessionID, amjCom::Status s);
void client_receive(amjCom::Packet &);
void client_status(amjCom::Status);
std::string content(amjCom::Packet &p);

std::mutex m;
std::vector<amjCom::pSession> sessions;
std::vector<int> sessionIDs;


int main(int argc, char *argv[]){
  parse_args(argc,argv);

  std::shared_ptr<amjCom::Server> server;
  std::shared_ptr<amjCom::Client> client;

  if(isServer)
    server=std::shared_ptr<amjCom::Server>
      (new amjCom::TCP::Server(address,server_session,server_status));
  else
    client=std::shared_ptr<amjCom::Client>
      (new amjCom::TCP::Client(address,client_receive,client_status));
  
  char message[30];
  if(isServer)
    memcpy(message,"Hello from server! ",19);
  else
    memcpy(message,"Hello from client! ",19);
  amjCom::Packet p;

  if(isServer)
    server->start();
  else
    client->start();
  
  for(int i=0;;i++){
    sprintf(message+19,"%d",i);
    p.clear();
    memcpy(p.write(30),message,30);

    if(!isServer)
      if(!client->send(p))
	std::cout << "tcptest: client: could not send" << std::endl;
    
    m.lock();
    for(unsigned int i=0;i<sessions.size();i++)
      if(sessions[i]){
	if(!sessions[i]->send(p)){
	  std::cout << "tcptest: server: session=" << i
		    << ": could not send, deleting." << std::endl;
	  sessions[i].reset();
	}
      }
      else{
	std::cout << "tcptest: server: session=" << i << ": empty" << std::endl;
      }
    m.unlock();
    
    sleep(1);
  }
  
  return 0;
}

void parse_args(int argc, char *argv[]){
  help.help(argc,argv);
  for(int i=1;i<argc;i++){
    if(strcmp(argv[i],"--server")==0){
      isServer=true;
      i++;
      address=argv[i];
    }
    else if(strcmp(argv[i],"--client")==0){
      isServer=false;
      i++;
      address=argv[i];
    }
    else{
      std::cout << "error: unknown option: " << argv[i] << std::endl;
      exit(1);
    }
  }
}

void server_session(amjCom::pSession pS){
  std::cout << "New session:" << sessionID << std::endl;
  int _sessionID=sessionID; // Copy to avoid warning about capture of
                            // non-automatic variable
  pS->start([&,_sessionID](amjCom::Packet p){session_receive(_sessionID,p);},
	    [&,_sessionID](amjCom::Status s){session_status(_sessionID,s);});
  m.lock();
  sessions.push_back(pS);
  sessionIDs.push_back(sessionID);
  m.unlock();
  
  sessionID++;
}

void server_status(amjCom::Status){
  std::cout << "Server: status" << std::endl;
}

void session_receive(int sessionID, amjCom::Packet &p){
  std::cout << "Session " << sessionID << ": packet received: "
	    << content(p) <<  std::endl;
}

void session_status(int sessionID, amjCom::Status s){
  std::cout << "Session" << sessionID << ": status update received"
	    << std::endl;
}

void client_receive(amjCom::Packet &p){
  std::cout << "tcptest: client_receive: packet received: "
	    << content(p) << std::endl;
}

void client_status(amjCom::Status s){
  std::cout << "Client status:" << std::endl;
  std::cout << "  state: " << s.statedescription() << std::endl;
  std::cout << "  Error: " << s.errormessage() << std::endl;
}

std::string content(amjCom::Packet &p){
  std::string s;
  s.resize(p.size());
  //std::cout << "p.size(): " << p.size() << std::endl;
  p.begin();
  memcpy(&s[0],p.read(p.size()),p.size());
  return s;
}

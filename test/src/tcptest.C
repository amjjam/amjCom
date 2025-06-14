#include "../../include/amjComTCP.H"
#include <amjArg.H>

#include <iostream>


amjArg::Help help({
    /*=======================================================================*/
    "tcptest",
    "",
    "Test the amjCom::TCP functionality",
    "--server <string> run as server listenning on <string>=address:port",
    "--client <string> run as client connecting to <string>=address:port"
    /*=======================================================================*/
  });

void parse_args(int argc, char *argv[]);

bool isServer;
std::string address;
int sessionID=0;

void server_session(amjCom::Server, amjCom::pSession);
void server_status(amjCom::Server, amjCom::Status);
void session_receive(int,amjCom::pSession, amjCom::Packet &);
void session_status(int,amjCom::pSession, amjCom::Status s);
void client_receive(amjCom::Client, amjCom::Packet &);
void client_status(amjCom::Client, amjCom::Status);
std::string content(amjCom::Packet &p);
void print_status(const amjCom::Status &s);

std::mutex m;
std::vector<amjCom::pSession> sessions;

int main(int argc, char *argv[]){
  parse_args(argc,argv);

  amjCom::Server server;
  amjCom::Client client;
  //  std::shared_ptr<amjCom::Client> client;

  if(isServer)
    server=amjCom::TCP::create_server(address,server_session,server_status);
  else
    client=amjCom::TCP::create_client(address,client_receive,client_status);
  //client=std::make_shared<amjCom::TCP::Client>(address,client_receive,client_status);
  
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
    memcpy(p.write(strlen(message)),message,strlen(message));

    if(!isServer)
      if(!client->send(p))
	std::cout << "tcptest: client: could not send" << std::endl;
    
    m.lock();
    for(unsigned int i=0;i<sessions.size();i++)
      if(!sessions[i]->send(p)){
	std::cout << "tcptest: server: session=" << i
		  << ": could not send, deleting." << std::endl;
	sessions.erase(sessions.begin()+i);
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

void server_session(amjCom::Server server, amjCom::pSession session){
  std::cout << "New session:" << sessionID << std::endl;
  int _sessionID=sessionID; // Copy to avoid warning about capture of
                            // non-automatic variable
  session->start([&,_sessionID](amjCom::pSession s, amjCom::Packet p)
  {session_receive(_sessionID,s,p);},
    [&,_sessionID](amjCom::pSession S, amjCom::Status s)
    {session_status(_sessionID,S,s);});
  m.lock();
  sessions.push_back(session);
  m.unlock();
  
  sessionID++;
}

void server_status(amjCom::Server server, amjCom::Status s){
  std::cout << "Server: status:" << std::endl;
  print_status(s);
}

void session_receive(int sessionID, amjCom::pSession s, amjCom::Packet &p){
  std::cout << "Session " << sessionID << ": packet received: "
	    << content(p) <<  std::endl;
  char message[40];
  sprintf(message,"server response to sessionID=%d",sessionID);
  p.clear();
  memcpy(p.write(strlen(message)),message,strlen(message));
  s->send(p);
}

void session_status(int sessionID, amjCom::pSession S, amjCom::Status s){
  std::cout << "Session: status:" << std::endl;
  print_status(s);
}

void client_receive(amjCom::Client, amjCom::Packet &p){
  std::cout << "tcptest: client_receive: packet received: "
	    << content(p) << std::endl;
}

void client_status(amjCom::Client, amjCom::Status s){
  std::cout << "Client: status:" << std::endl;
  print_status(s);
}

#include <cstring>
std::string content(amjCom::Packet &p){
  std::string s;
  s.resize(p.size());
  //std::cout << "p.size(): " << p.size() << std::endl;
  p.begin();
  std::memcpy(&s[0],p.read(p.size()),p.size());
  return s;
}

void print_status(const amjCom::Status &s){
  std::cout << "   state: " << s.statedescription() << std::endl;
  std::cout << "   error: " << s.errormessage() << std::endl;
}

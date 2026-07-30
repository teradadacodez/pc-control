#include "TcpServer.hpp"

int main()
{
    tcpserver server(5000) ;
    if(!server.start()) return 1 ;
    server.run() ;
    return 0 ;   
}
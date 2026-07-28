#pragma once


#include <winsock2.h>

class tcpserver
{
    public :
    explicit tcpserver(int port) ;
    ~tcpserver() ;
    bool start() ;
    void run() ;

    private : // using m_ prefix convention for member variables
    int m_port ; 
    SOCKET m_listensock ;
    bool m_wsaready ;
}
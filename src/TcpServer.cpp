#include "TcpServer.hpp"
#include <string>
#include <ws2tcpip.h>
#include <iostream>

tcpserver::tcpserver(int port) : m_port(port), m_listensock(INVALID_SOCKET), m_wsaready(false)
{
    std::cout << "tcpserver() constructor" << std::endl;
}

tcpserver::~tcpserver()
{
    if(m_listensock != INVALID_SOCKET) closesocket(m_listensock) ;
    if(m_wsaready) WSACleanup() ;
}

bool tcpserver::start()
{
    WSAData wsadata ;
    int err = WSAStartup(MAKEWORD(2,2),&wsadata) ;
    if(err != 0)
    {
        std::cerr << "WSA Startup failed!" << std::endl;
        return false ;
    }
    m_wsaready = true ;
    m_listensock = socket(AF_INET, SOCK_STREAM, 0) ;
    if(m_listensock == INVALID_SOCKET)
    {
        std::cerr << "socket() failed!" << std::endl;
        return false ;
    }

    sockaddr_in server_addr{} ;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(static_cast<u_short>(m_port)) ;

    // failure cases
    if(inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr) != 1) // 0.0.0.0 means we are connected to accept connection from anywhere or bind to every ip on this machine !
    {
        std::cerr << "inet_pton() failed : " << WSAGetLastError() << std::endl;
        return false ;
    }

    if(bind(m_listensock, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "bind() failed : " << WSAGetLastError() << std::endl;
        return false;
    }

    if(listen(m_listensock, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "listen() failed : " << WSAGetLastError() << std::endl;
        return false ;
    }

    std::cout << "listening on 0.0.0.0 : " << m_port << std::endl;
    return true ;
}

void tcpserver::run()
{
    while(true)
    {
        std::cout << "waiting for a client..." << std::endl;

        sockaddr_in client_addr{} ;
        int client_addr_size = sizeof(client_addr) ;
        SOCKET clientsock = accept(m_listensock, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_size) ;
        if(clientsock == INVALID_SOCKET)
        {
            std::cerr << "accept() failed : " << WSAGetLastError() << std::endl;
            continue;
        }
        char client_ip[INET_ADDRSTRLEN]{} ;
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip)) ;
        std::cout << "client connected from : " << client_ip << " : " << ntohs(client_addr.sin_port) << std::endl;

        char buff[512];
        while(true)
        {
            int n = recv(clientsock, buff, sizeof(buff), 0) ;
            if(n>0)
            {
                std::cout << "recieved " << n << " bytes : " << std::string(buff,n) ;
                send(clientsock, buff, n, 0) ;
            }
            else if(n==0)
            {
                std::cout << "client disconnected" << std::endl;
                break ;
            }
            else
            {
                std::cerr << "recv() failed : " << WSAGetLastError() << std::endl;
                break ;
            }
        }
        closesocket(clientsock) ;
    }
}
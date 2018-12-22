/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_SOCKET_H_
#define _BB_SOCKET_H_

#if((defined _WIN32) || (defined __CYGWIN32__) || (defined __CYGWIN__))
   #include<winsock2.h>
   #pragma comment(lib, "ws2_32.lib")

   typedef int socklen_t;
   #define IOCTL_TYPE unsigned long
#else
   #include<unistd.h>
   #include<sys/time.h>
   #include<sys/types.h>
   #include<sys/socket.h>
   #include<netinet/in.h>
   #include<cerrno>
   #include<arpa/inet.h>
   #include<netdb.h>
   #include<sys/ioctl.h>

   #define INVALID_SOCKET -1
   #define SOCKET_ERROR -1
   
   #define ioctlsocket ioctl
   #define IOCTL_TYPE char
#endif

#include<NameSpace.h>
#include<Defines.h>
#include<Packet.h>

#ifdef _WIN32
  typedef SOCKET BB_SOCKET;
#else
  typedef int BB_SOCKET;
#endif

DECLARE_ENGINE_NAMESPACE

class Socket
{
   public:
      Socket() { m_socket = INVALID_SOCKET; }
      ~Socket() { Disconnect(); }

      bool SetAsServer(int port, int numPending);
      bool ConnectToServer(char *IP, int port);
      bool AcceptConnection(Socket &client);

      BNETWORKRESULT SendData(Packet &packet);
      BNETWORKRESULT RecieveData(Packet &packet, int *bytesRead);

      void Disconnect();

      bool isConnected() { return (m_socket != INVALID_SOCKET); }

      bool operator==(Socket &s) { return (m_socket == s.m_socket); }
      void operator=(Socket &s) { m_socket = s.m_socket; }

      BB_SOCKET GetSocket() { return m_socket; }

   private:
      BB_SOCKET m_socket;
};

END_ENGINE_NAMESPACE

#endif
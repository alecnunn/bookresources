/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Socket.h>


DECLARE_ENGINE_NAMESPACE


bool Socket::SetAsServer(int port, int numPending)
{
   sockaddr_in address;
   IOCTL_TYPE arg = 1;

   m_socket = socket(AF_INET, SOCK_STREAM, 0);

   if(m_socket == INVALID_SOCKET)
      return false;

   memset(&address, 0, sizeof(sockaddr_in));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = htonl(INADDR_ANY);
   address.sin_port = htons(port);

   ioctlsocket(m_socket, FIONBIO, &arg);

   if(bind(m_socket, (sockaddr*)&address,
      sizeof(sockaddr)) == SOCKET_ERROR)
      {
         Disconnect();
         return false;
      }

   if(listen(m_socket, numPending) == SOCKET_ERROR)
      return false;

   return true;
}


bool Socket::ConnectToServer(char *serverIP, int port)
{
	sockaddr_in addr;
#ifdef _WIN32
	LPHOSTENT lphost;
#else
   struct hostent *lphost;
#endif

	memset(&addr, 0, sizeof(sockaddr_in));
   addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(serverIP);

	if(addr.sin_addr.s_addr == INADDR_NONE)
	   lphost = gethostbyname(serverIP);
	else
	   lphost = gethostbyaddr((const char*)&addr.sin_addr.s_addr,
	                          4, AF_INET);

   if(lphost == NULL)
		{
#ifdef _WIN32
			WSASetLastError(WSAEINVAL);
#endif
			return false;
		}

#ifdef _WIN32
   addr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
#else
   memcpy((char*)&addr.sin_addr.s_addr, lphost->h_addr_list[0], lphost->h_length);
#endif
	addr.sin_port = htons(port);

   m_socket = socket(AF_INET, SOCK_STREAM, 0);	

   if(m_socket == INVALID_SOCKET)
      return false;

	if(connect(m_socket, (struct sockaddr*)&addr,
	   sizeof(sockaddr)) == SOCKET_ERROR)
	   {
		   Disconnect();
		   return false;
	   }

	return true;
}


bool Socket::AcceptConnection(Socket &client)
{
	sockaddr_in addr;
	socklen_t size = sizeof(sockaddr_in);
	memset(&addr, 0, size);

	client.m_socket = accept(m_socket, (struct sockaddr*)&addr,
	                        &size);

	if(client.m_socket == INVALID_SOCKET)
	   return false;

	return true;
}


BNETWORKRESULT Socket::SendData(Packet &packet)
{
   int sent = -1, size = 0;
   void *ptr = (char*)&packet;
   BNETWORKRESULT result = BB_DATA_SUCCESS;

   if(packet.GetPacketSize() <= 0)
      return BB_DATA_FAIL;

   size = packet.GetPacketSize();

   sent = send(m_socket, (char*)ptr, size, 0);

   if(sent == SOCKET_ERROR)
      result = BB_DATA_FAIL;

   if(sent != packet.GetPacketSize())
      result = BB_PARTIAL_DATA;

   return result;
}


BNETWORKRESULT Socket::RecieveData(Packet &packet, int *bytesRead)
{
   int read = 0;
   unsigned long readAmt = 0, maxSize;
   void *ptr = (char*)&packet;
   BNETWORKRESULT result = BB_DATA_SUCCESS;

   maxSize = packet.GetPacketSize();

   if(maxSize <= 0)
      return BB_DATA_FAIL;

   if(ioctlsocket(m_socket, FIONREAD, &readAmt) != 0)
      result = BB_NO_DATA;

   if(readAmt == 0)
      result = BB_NO_DATA;

   if(readAmt > maxSize)
      {
         result = BB_PARTIAL_DATA;
         readAmt = maxSize;
      }

   read = recv(m_socket, (char*)ptr, readAmt, 0);

   if(read <= 0)
      result = BB_DATA_FAIL;

   if(bytesRead)
      *bytesRead = read;

   return result;
}


void Socket::Disconnect()
{
   if(m_socket != INVALID_SOCKET)
#ifdef _WIN32
      closesocket(m_socket);
#else
      close(m_socket);
#endif

   m_socket = INVALID_SOCKET;
}


END_ENGINE_NAMESPACE
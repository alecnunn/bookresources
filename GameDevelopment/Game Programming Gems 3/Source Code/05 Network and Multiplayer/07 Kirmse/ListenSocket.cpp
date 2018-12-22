#include "main.h"

/***************************************************************/
ListenSocket::ListenSocket(uint16 new_from_port) :
      miPort(new_from_port)
{
   mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   
   if (mSocket == INVALID_SOCKET)
   {
      dprintf("Couldn't create socket; error = %s\n", GetLastWinsockErrorString());
      return;
   }
   
   SOCKADDR_IN sin;
   sin.sin_family      = AF_INET;
   sin.sin_port        = htons(miPort);
   sin.sin_addr.s_addr = INADDR_ANY;

   // >1 filter might be on the same port, so this bind could fail
   if (bind(mSocket, (SOCKADDR *) &sin, sizeof sin) != 0)
   {
      dprintf("Could not bind socket on port %d; error = %s\n",
              miPort, GetLastWinsockErrorString());
      mSocket = INVALID_SOCKET;
      return;
   }

   // Set to non-blocking
   if (false == SetAsynchronous(FD_CLOSE | FD_ACCEPT))
      return;

   if (0 != listen(mSocket, SOMAXCONN))
   {
      dprintf("Could not listen on port %d; error = %s\n",
              miPort, GetLastWinsockErrorString());
      closesocket(mSocket);
      mSocket = INVALID_SOCKET;
      return;
   }
   
   sockaddr_in addr;
   int      addr_size = sizeof(addr);
   SOCKET new_sock = accept(mSocket, (sockaddr *) &addr, &addr_size);
   if (new_sock == INVALID_SOCKET)
   {
      if (WSAEWOULDBLOCK != GetLastWinsockError())
      {
         dprintf("Failed to accept connections on port %d, error = %s\n", miPort,
                 GetLastWinsockErrorString());
         closesocket(mSocket);
         mSocket = INVALID_SOCKET;
         return;
      }
   }
   else
   {
      // XXX Bad!
   }
}
/***************************************************************/
ListenSocket::~ListenSocket()
{
}
/***************************************************************/
bool ListenSocket::Send(byte *data, uint32 len_data, Endpoint &endpoint)
{
   return false;
}
/***************************************************************/
void ListenSocket::EventRead()
{
   // Should never get this on a listening socket
   assert(false);
}
/***************************************************************/
Socket *ListenSocket::Accept()
{
   // Accept the new connection
   sockaddr_in addr;
   int      addr_size = sizeof(addr);
   SOCKET new_sock = accept(mSocket, (sockaddr *) &addr, &addr_size);
   if (new_sock != INVALID_SOCKET)
   {
      Socket *connection = new TCPSocket(new_sock, ntohl(addr.sin_addr.s_addr),
                                         miPort);
      return connection;
   }

   return NULL;
}

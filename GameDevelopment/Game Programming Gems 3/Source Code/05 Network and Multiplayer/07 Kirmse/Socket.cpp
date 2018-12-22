#include "main.h"

/***************************************************************/
Socket::Socket() :
      mboolConnected(true)
{
}
/***************************************************************/
Socket::~Socket()
{
   if (mSocket != INVALID_SOCKET)
      closesocket(mSocket);
}
/***************************************************************/
bool Socket::SetAsynchronous(int flags)
{
   // Set to non-blocking
   if (WSAAsyncSelect(mSocket, app->inter->GetWindow(),
                      WM_SOCKET_NOTIFY, flags) != 0)
   {
      dprintf("Error in WSAAsyncSelect; error = %s\n",
              GetLastWinsockErrorString());
      closesocket(mSocket);
      mSocket = INVALID_SOCKET;
      return false;
   }

   return true;
}
/***************************************************************/
bool Socket::Send(Message *msg, Endpoint &endpoint)
{
   return Send(msg->GetData(), msg->GetLength(), endpoint);
}
/***************************************************************/
Socket      *Socket::Accept()
{
   return NULL;
}

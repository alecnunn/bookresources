#include "main.h"

/***************************************************************/
TCPSocket::TCPSocket(SOCKET new_sock, uint32 new_remote_address, uint16 new_from_port)
{
   mSocket         = new_sock;
   miRemoteAddress = new_remote_address;
   msFromPort      = new_from_port;
   SetConnected(true);

   SetAsynchronous(FD_READ | FD_CLOSE);
}
/***************************************************************/
TCPSocket::TCPSocket(Endpoint &endpoint)
{
   msFromPort = endpoint.GetPort();
   miRemoteAddress = endpoint.GetAddress();
   SetConnected(false);

   mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (mSocket == INVALID_SOCKET)
   {
      dprintf("Couldn't create socket; error = %s\n", GetLastWinsockErrorString());
      return;
   }

   SetAsynchronous(FD_CLOSE | FD_CONNECT | FD_READ);

   SOCKADDR_IN sin;
   sin.sin_family      = AF_INET;
   sin.sin_port        = htons(msFromPort);
   sin.sin_addr.s_addr = htonl(miRemoteAddress);
   int result = connect(mSocket, (sockaddr *) &sin, sizeof(sin));
   if (result == SOCKET_ERROR)
   {
      if (GetLastWinsockError() != WSAEWOULDBLOCK)
      {
         dprintf("Couldn't connect socket; error = %s\n", GetLastWinsockErrorString());
         return;
      }
   }
   else
   {
      // XXX Connected right away
      Break();
   }
}
/***************************************************************/
TCPSocket::~TCPSocket()
{
}
/***************************************************************/
bool TCPSocket::Send(byte *data, uint32 len_data, Endpoint &endpoint)
{
   if (send(mSocket, (char *) data, len_data, 0) != (int32) len_data)
   {
      dprintf("send failed; error = %s\n", GetLastWinsockErrorString());
      return false;
   }

   return true;
}
/***************************************************************/
void TCPSocket::EventRead()
{
   byte buf[MAX_MSG_SIZE];

   int32 num_read = recv(mSocket, (char *) buf, sizeof buf, 0);

   if (num_read == SOCKET_ERROR)
   {
      dprintf("receive error = %s\n", GetLastWinsockErrorString());
      return;
   }

   if (num_read == 0)
   {
      dprintf("Got socket shutdown\n");
      return;
   }
   
   Endpoint endpoint;
   endpoint.SetAddress(miRemoteAddress);
   endpoint.SetPort(msFromPort);

   Message *msg = new Message(num_read);
   memcpy(msg->GetData(), buf, num_read);
   
   app->network->MessageReceived(msg, this, TCP, endpoint);
}

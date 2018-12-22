#include "main.h"

/***************************************************************/
UDPSocket::UDPSocket(uint16 new_from_port)
{
   miFromPort = new_from_port;
   mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if (mSocket == INVALID_SOCKET)
   {
      dprintf("Couldn't create socket; error = %s\n", GetLastWinsockErrorString());
      return;
   }
   
   SOCKADDR_IN sin;
   sin.sin_family      = AF_INET;
   sin.sin_port        = htons(miFromPort);
   sin.sin_addr.s_addr = INADDR_ANY;

   // >1 filter might be on the same port, so this bind could fail
   if (bind(mSocket, (SOCKADDR *) &sin, sizeof sin) != 0)
   {
      dprintf("Could not bind socket on port %d; error = %s\n",
              miFromPort, GetLastWinsockErrorString());
      mSocket = INVALID_SOCKET;
      return;
   }

   SetAsynchronous(FD_READ | FD_CLOSE);
}
/***************************************************************/
UDPSocket::~UDPSocket()
{
}
/***************************************************************/
bool UDPSocket::Send(byte *data, uint32 len_data, Endpoint &endpoint)
{
   SOCKADDR_IN sin;
   sin.sin_family      = AF_INET;
   sin.sin_port        = htons(endpoint.GetPort());
   sin.sin_addr.s_addr = htonl(endpoint.GetAddress());

   if (sendto(mSocket, (char *) data, len_data, 0,
              (struct sockaddr *) &sin, sizeof sin) != (int32) len_data)
   {
      dprintf("sendto failed; error = %s\n", GetLastWinsockErrorString());
      return false;
   }

   return true;
}
/***************************************************************/
void UDPSocket::EventRead()
{
   byte buf[MAX_MSG_SIZE];

   SOCKADDR_IN sin;
   int32 len = sizeof sin;
   
   int32 num_read = recvfrom(mSocket, (char *) buf, sizeof buf, 0,
                             (struct sockaddr *) &sin, (int *) &len);

   if (num_read == SOCKET_ERROR)
   {
      // Under Windows, a WSAECONNRESET on a UDP socket means that
      // a previous send generated an ICMP host unreachable.  Why this
      // should be an error on the next receive (or at all) is very odd.
      if (GetLastWinsockError() != WSAECONNRESET)
         dprintf("receive error = %s\n", GetLastWinsockErrorString());
      return;
   }

   if (num_read == 0)
   {
      dprintf("Got socket shutdown\n");
      return;
   }
   
   Endpoint endpoint;
   endpoint.SetAddress(htonl(sin.sin_addr.s_addr));
   endpoint.SetPort(miFromPort);

   Message *msg = new Message(num_read);
   memcpy(msg->GetData(), buf, num_read);
   
   app->network->MessageReceived(msg, this, UDP, endpoint);
}

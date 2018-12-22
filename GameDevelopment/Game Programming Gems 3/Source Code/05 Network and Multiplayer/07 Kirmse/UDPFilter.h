#ifndef _UDPFILTER_H
#define _UDPFILTER_H

class UDPFilter : public Filter
{
   typedef Filter super;
   
public:

   UDPFilter(Endpoint &new_from, Endpoint &new_to);
   virtual ~UDPFilter();

   virtual void    RunCycle(float now);
   virtual Filter *Clone();
   
   virtual void MessageReceived(Socket *sock, Message *pMessage);
   virtual void SocketAccepted (Socket *sock);
   virtual void SocketConnected(Socket *sock, int error_code);
   virtual void SocketClosed   (Socket *sock);
   virtual void TryCreateSocket(uint16 port);

private:

   Socket *mpFromSocket;

   typedef list<Message *> message_list_type;
   message_list_type messages;

   // Insert message in sorted order
   void InsertMessage(Message *pMessage);

   void SendPacket   (Message *pMessage);
};

#endif

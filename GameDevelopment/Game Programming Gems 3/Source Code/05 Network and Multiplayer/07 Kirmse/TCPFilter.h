#ifndef _TCPFILTER_H
#define _TCPFILTER_H

class TCPFilter : public Filter
{
   typedef Filter super;
   
public:

   TCPFilter(Endpoint &new_from, Endpoint &new_to);
   virtual ~TCPFilter();

   virtual void    RunCycle(float now);
   virtual Filter *Clone();
   
   virtual void MessageReceived(Socket *sock, Message *msg);
   virtual void SocketAccepted (Socket *sock);
   virtual void SocketConnected(Socket *sock, int error_code);
   virtual void SocketClosed   (Socket *sock);
   virtual void TryCreateSocket(uint16 port);

private:

   typedef vector<Socket *> SocketVector;
   typedef list<Message *>  MessageList;
   struct SocketInfo
   {
      Socket      *to_socket;
      MessageList  from_messages;  // Messages from->to
      MessageList  to_messages;    // Messages to->from
      bool         pending_delete; // true means delete when send queue empty
   };
   
   typedef map<Socket *, SocketInfo *> SocketMap;

   
   void InsertMessage(MessageList &messages, Message *msg);
   void DeleteSocketInfo(SocketInfo *info);
   void DeleteConnection(Socket *sock);

   ListenSocket *mpListenSocket;
   SocketMap     mSockets;  // Maps from socket to info
   float         mfLastCycle;
};

#endif

#include "main.h"

/***************************************************************/
TCPFilter::TCPFilter(Endpoint &new_from, Endpoint &new_to) :
      super(new_from, new_to),
      mpListenSocket(NULL)
{
   SetProtocol(TCP);
   mfLastCycle = app->timer->GetDuration();
}
/***************************************************************/
TCPFilter::~TCPFilter()
{
   SocketMap::iterator it;
   for (it = mSockets.begin(); it != mSockets.end(); ++it)
   {
      DeleteSocketInfo((*it).second);

      // Delete incoming side
      delete (*it).first;
   }

   // Delete listening socket
   // XXX Only if last one listening on this port?
   delete mpListenSocket;
}
/***************************************************************/
Filter *TCPFilter::Clone()
{
   return new TCPFilter(*this);
}
/***************************************************************/
void TCPFilter::MessageReceived(Socket *sock, Message *msg)
{
   bool from_direction = true;  // Is message from the "from" socket?
   Socket *pSendSocket;

   // Look for coming from "from" socket
   SocketMap::iterator it = mSockets.find(sock);
   if (it == mSockets.end())
   {
      // Look for coming from "to" socket
      for (it = mSockets.begin(); it != mSockets.end(); ++it)
      {
         if ((*it).second->to_socket == sock)
         {
            from_direction = false;
            pSendSocket = (*it).first;
            break;
         }
      }

      // Found?
      if (true == from_direction)
      {
         dprintf("Message received from unknown socket\n");
         delete msg;
         return;
      }
   }
   else
      pSendSocket = (*it).second->to_socket;

   SocketInfo *info = (*it).second;

   // Which direction are we sending?
   const Endpoint *from_endpoint, *to_endpoint;
   const byte *pData = msg->GetData();
   int len = msg->GetLength();
   if (true == from_direction)
   {
      from_endpoint = &GetFromEndpoint();
      to_endpoint   = &GetToEndpoint();
   }
   else
   {
      from_endpoint = &GetToEndpoint();
      to_endpoint   = &GetFromEndpoint();
   }
   
   int32 latency = GenerateLatency();

   // Add sender's transmission latency (if any)
   Host *host = app->network->FindHostByAddress(from_endpoint->GetAddress());
   if (host != NULL)
      latency += host->MessageSent(msg, TCP);

   // Add receiver's reception latency (if any)
   host = app->network->FindHostByAddress(to_endpoint->GetAddress());
   if (host != NULL)
      latency += host->MessageReceived(msg, TCP);
   
   // Add message to list to be sent later
   msg->SetSendTime(app->timer->GetDuration() + latency);

   if (true == from_direction)
        InsertMessage(info->from_messages, msg);
   else
      InsertMessage(info->to_messages, msg);

   if (true == IsListed())
   {
      if (true == pSendSocket->IsConnected())
      {
         if (0 == latency)
            app->inter->DisplayPacket(msg, TCP,
                                      *from_endpoint, *to_endpoint,
                                      Action(Action::SEND_NOW));
         else
            app->inter->DisplayPacket(msg, TCP,
                                      *from_endpoint, *to_endpoint,
                                      Action(Action::DELAY, latency));
      }
      else
         app->inter->DisplayPacket(msg, TCP,
                                   *from_endpoint, *to_endpoint,
                                   Action(Action::WAIT_CONNECT));
   }
}
/***************************************************************/
void TCPFilter::SocketAccepted (Socket *sock)
{
   // Connect to other side
   Socket *to_socket = new TCPSocket(GetToEndpoint());
   app->network->AddSocket(to_socket, TCP);

   SocketInfo *info = new SocketInfo;
   info->to_socket      = to_socket;
   info->pending_delete = false;
   
   // Remember this pair of sockets
   mSockets[sock] = info;
}
/***************************************************************/
void TCPFilter::SocketConnected(Socket *sock, int error_code)
{
   // Did connection fail?
   if (error_code != 0)
   {
      dprintf("Connection failed; error = %d\n", error_code);

      // Disconnect other end
      SocketMap::iterator it;
      for (it = mSockets.begin(); it != mSockets.end(); ++it)
      {
         if ((*it).second->to_socket == sock)
            break;
      }
      
      if (it != mSockets.end())
      {
         DeleteSocketInfo((*it).second);

         Socket *from_sock = (*it).first;
         app->network->RemoveSocket(from_sock);
         delete from_sock;

         mSockets.erase(it);
      }

      return;
   }

   // Remember that it's connected
   sock->SetConnected(true);
}
/***************************************************************/
void TCPFilter::SocketClosed   (Socket *sock)
{
   // Is it an incoming socket?
   SocketMap::iterator it = mSockets.find(sock);
   if (it != mSockets.end())
   {
      // If there's pending data, wait for that to be sent
      if (true == (*it).second->from_messages.empty())
         DeleteConnection(sock);
      else
         (*it).second->pending_delete = true;
   }
   else
   {
      // Is it an outgoing socket?
      for (it = mSockets.begin(); it != mSockets.end(); ++it)
      {
         SocketInfo *info = (*it).second;

         if (info->to_socket == sock)
         {
            // If there's pending data, wait for that to be sent
            if (true == (*it).second->to_messages.empty())
               DeleteConnection(sock);
            else
               (*it).second->pending_delete = true;
            break;
         }
      }
   }
}
/***************************************************************/
void TCPFilter::RunCycle(float now)
{
   // How much time has passed?
   float dt = now - mfLastCycle;
   mfLastCycle = now;
   
   // Loop over all sockets
   SocketMap::iterator it;
   SocketVector DeleteSockets;
   for (it = mSockets.begin(); it != mSockets.end(); ++it)
   {
      SocketInfo *info = (*it).second;

      //
      // to->from messages
      // 
      while (!info->to_messages.empty())
      {
         Message *msg = info->to_messages.front();
         
         if (msg->GetSendTime() > now)
            break;

         info->to_messages.pop_front();
         
         (*it).first->Send(msg, GetFromEndpoint());
         delete msg;
      }

      //
      // from->to messages
      // 
      
      // If sending end isn't connected yet, wait
      if (false == info->to_socket->IsConnected())
         continue;
      
      while (!info->from_messages.empty())
      {
         Message *msg = info->from_messages.front();
         
         if (msg->GetSendTime() > now)
            break;
         
         info->from_messages.pop_front();
         
         info->to_socket->Send(msg, GetToEndpoint());
         delete msg;
      }

      // Delete this guy if his from queue has emptied
      if (true == info->pending_delete &&
          true == info->from_messages.empty() &&
          true == info->to_messages.empty())
      {
         DeleteSockets.push_back((*it).first);
      }
   }

   // Perform pending deletions
   SocketVector::iterator sit;
   for (sit = DeleteSockets.begin(); sit != DeleteSockets.end(); ++sit)
   {
      DeleteConnection(*sit);
   }
}
/***************************************************************/
void TCPFilter::InsertMessage(MessageList &messages, Message *pMessage)
{
   // Must preserve order
   if (false == messages.empty())
   {
      // Want to send before last packet?
      Message *pLastMessage = messages.back();
      if (pLastMessage->GetSendTime() > pMessage->GetSendTime())
         pMessage->SetSendTime(pLastMessage->GetSendTime());
   }
   
   MessageList::iterator it;
   for (it = messages.begin(); it != messages.end(); ++it)
   {
      if (pMessage->GetSendTime() < (*it)->GetSendTime())
         break;
   }
   messages.insert(it, pMessage);
}
/***************************************************************/
void TCPFilter::TryCreateSocket(uint16 port)
{
   mpListenSocket = new ListenSocket(port);
   app->network->AddSocket(mpListenSocket, TCP);
}
/***************************************************************/
void TCPFilter::DeleteSocketInfo(SocketInfo *info)
{
   app->network->RemoveSocket(info->to_socket);
   delete info->to_socket;
   delete info;
}
/***************************************************************/
void TCPFilter::DeleteConnection(Socket *sock)
{
   // Is it an incoming socket?
   SocketMap::iterator it = mSockets.find(sock);
   if (it != mSockets.end())
   {
      // Delete outgoing end
      DeleteSocketInfo((*it).second);
      
      app->network->RemoveSocket(sock);

      // Blow socket away
      delete sock;

      mSockets.erase(it);
   }
   else
   {
      // Is it an outgoing socket?
      for (it = mSockets.begin(); it != mSockets.end(); ++it)
      {
         SocketInfo *info = (*it).second;
         
         if (info->to_socket == sock)
         {
            // Delete outgoing end
            DeleteSocketInfo((*it).second);
            
            app->network->RemoveSocket((*it).first);
            
            // Blow socket away
            delete (*it).first;
            
            mSockets.erase(it);
            break;
         }
      }
   }
}


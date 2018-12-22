#include "main.h"

/***************************************************************/
UDPFilter::UDPFilter(Endpoint &new_from, Endpoint &new_to) :
      super(new_from, new_to)
{
   SetProtocol(UDP);
}
/***************************************************************/
UDPFilter::~UDPFilter()
{
   message_list_type::iterator it;
   for (it = messages.begin(); it != messages.end(); ++it)
      delete *it;
}
/***************************************************************/
Filter *UDPFilter::Clone()
{
   return new UDPFilter(*this);
}
/***************************************************************/
void UDPFilter::MessageReceived(Socket *sock, Message *pMessage)
{
   // Is this our "from" socket?
   if (sock != mpFromSocket)
      return;
   
   // Drop packet?
   if ((rand() % 100) < GetPacketLoss())
   {
      if (IsListed())
         app->inter->DisplayPacket(pMessage, UDP,
                                   GetFromEndpoint(), GetToEndpoint(),
                                   Action::DROP);
      delete pMessage;
      return;
   }

   // Duplicate packet?
   if (rand() % 100 < GetDuplicationProbability())
   {
      Message *pDuplicateMsg = new Message(*pMessage);
      pDuplicateMsg->SetAction(Action::DUPLICATE);

      SendPacket(pDuplicateMsg);
   }

   SendPacket(pMessage);
}
/***************************************************************/
void UDPFilter::SocketAccepted (Socket *sock)
{
   assert(false);
}
/***************************************************************/
void UDPFilter::SocketConnected(Socket *sock, int error_code)
{
   assert(false);
}
/***************************************************************/
void UDPFilter::SocketClosed   (Socket *sock)
{
   assert(false);
}
/***************************************************************/
void UDPFilter::RunCycle(float now)
{
   while (!messages.empty())
   {
      Message *msg = messages.front();

      if (msg->GetSendTime() > now)
         return;

      messages.pop_front();

      mpFromSocket->Send(msg, GetToEndpoint());
      delete msg;
   }
}
/***************************************************************/
void UDPFilter::InsertMessage(Message *pMessage)
{
   // To preserve order, we have to put the message at the end of the
   // list.  If we are allowed to reorder, we just insert it in the
   // correct place.
   if (false == messages.empty())
   {
      // Want to send before last packet?
      Message *pLastMessage = messages.back();
      if (pLastMessage->GetSendTime() > pMessage->GetSendTime())
      {
         if (rand() % 100 < GetReorderProbability())
         {
            // Record the reorder
            pMessage->SetAction(Action::REORDER);
         }
         else
         {
            // Can't reorder?  Then set it to send just after last packet
            pMessage->SetSendTime(pLastMessage->GetSendTime());
         }
      }
   }

   // Put the packet in sorted order of send time
   message_list_type::iterator it;
   for (it = messages.begin(); it != messages.end(); ++it)
   {
      if (pMessage->GetSendTime() < (*it)->GetSendTime())
         break;
   }
   messages.insert(it, pMessage);
}
/***************************************************************/
void UDPFilter::SendPacket(Message *pMessage)
{
   int32 latency = GenerateLatency();

   // Add sender's transmission latency (if any)
   Host *host = app->network->FindHostByAddress(GetFromEndpoint().GetAddress());
   if (host != NULL)
      latency += host->MessageSent(pMessage, UDP);

   // Add receiver's reception latency (if any)
   host = app->network->FindHostByAddress(GetToEndpoint().GetAddress());
   if (host != NULL)
      latency += host->MessageReceived(pMessage, UDP);
   
   // Add message to list to be sent later
   pMessage->SetSendTime(app->timer->GetDuration() + latency);

   InsertMessage(pMessage);   

   if (true == IsListed())
   {
      // Don't override any existing action
      if (Action::NONE == pMessage->GetAction().GetType())
      {
         if (0 == latency)
            pMessage->SetAction(Action(Action::SEND_NOW));
         else
            pMessage->SetAction(Action(Action::DELAY, latency));
      }         

      // Show packet on interface
      app->inter->DisplayPacket(pMessage, UDP,
                                GetFromEndpoint(), GetToEndpoint(),
                                pMessage->GetAction());
   }
}
/***************************************************************/
void UDPFilter::TryCreateSocket(uint16 port)
{
   mpFromSocket = new UDPSocket(port);
   app->network->AddSocket(mpFromSocket, UDP);
}

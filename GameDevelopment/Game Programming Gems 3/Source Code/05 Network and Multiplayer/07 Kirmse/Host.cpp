#include "main.h"

const uint32 PPP_HEADER_SIZE = 7;  // PPP header = 3, compressed IP = ~4
const uint32 IP_HEADER_SIZE  = 20;
const uint32 TCP_HEADER_SIZE = 8;  // XXX Check VJ compression header size
const uint32 UDP_HEADER_SIZE = 8;

// Recompute averages every this many milliseconds
const uint32 AVERAGE_SAMPLE_TIME = 3000;

/***************************************************************/
Host::Host(uint32 new_address)
{
   miAddress = new_address;

   miSendBandwidth    = 0xffffffff;
   miReceiveBandwidth = 0xffffffff;

   mfSendQueueLength    = 0.0;
   mfReceiveQueueLength = 0.0;

   mboolUsingPPP = true;
   miCompression = 0;
   
   mfLastCycle = app->timer->GetDuration();
   mfLastAverageTime = mfLastCycle;

   miSentBytes            = 0;
   miReceivedBytes        = 0;
   miAverageSentBytes     = 0;
   miAverageReceivedBytes = 0;
}
/***************************************************************/
Host::~Host()
{
}
/***************************************************************/
void Host::RunCycle(float now)
{
   // Decrease send queue by the amount the host would have sent
   // since the last cycle
   float dt = now - mfLastCycle;
   float sent = dt * miSendBandwidth / 1000.0f;

   if (sent > mfSendQueueLength)
      mfSendQueueLength = 0;
   else
      mfSendQueueLength -= sent;

   // Same with the receive queue
   float received = dt * miReceiveBandwidth / 1000.0f;
   if (received > mfReceiveQueueLength)
      mfReceiveQueueLength = 0;
   else
      mfReceiveQueueLength -= received;

   if (now - mfLastAverageTime >= AVERAGE_SAMPLE_TIME)
      RecomputeAverages(now);
   
   mfLastCycle = now;
}
/***************************************************************/
void Host::AddressToString(char *str)
{
   sprintf(str, "%d.%d.%d.%d", (miAddress & 0xff000000) >> 24,
           (miAddress & 0x00ff0000) >> 16,
           (miAddress & 0x0000ff00) >> 8,
           (miAddress & 0x000000ff));
}
/***************************************************************/
uint32 Host::MessageReceived(Message *msg, Protocol protocol)
{
   // Time taken to receive message = (message size + queue size) / bandwidth
   uint32 len = (100 - miCompression) *
      (msg->GetLength() + GetHeaderSize(protocol)) / 100;
   uint32 ms = (uint32) (1000.0 * (len + mfReceiveQueueLength)
                         / miReceiveBandwidth);
   
   mfReceiveQueueLength += len;

   miReceivedBytes += len;
   
   return ms;
}
/***************************************************************/
uint32 Host::MessageSent(Message *msg, Protocol protocol)
{
   // Time taken to send message = (message size + queue size) / bandwidth
   uint32 len = (100 - miCompression) *
      (msg->GetLength() + GetHeaderSize(protocol)) / 100;
   uint32 ms = (uint32) (1000.0 * (len + mfSendQueueLength)
                         / miSendBandwidth);
   
   mfSendQueueLength += len;

   miSentBytes += len;
   
   return ms;
}
/***************************************************************/
void Host::RecomputeAverages(float now)
{
   const float alpha = 0.5;
   uint32 new_average;

   new_average = 1000 * miSentBytes / (now - mfLastAverageTime);
   miAverageSentBytes = (uint32) (alpha * miAverageSentBytes +
      (1.0 - alpha) * new_average);

   new_average = 1000 * miReceivedBytes / (now - mfLastAverageTime);
   miAverageReceivedBytes = (uint32) (alpha * miAverageReceivedBytes +
      (1.0 - alpha) * new_average);
   
   miSentBytes     = 0;
   miReceivedBytes = 0;

   mfLastAverageTime = now;
}
/***************************************************************/
uint32 Host::GetHeaderSize(Protocol protocol)
{
   if (protocol == TCP)
      return IP_HEADER_SIZE + TCP_HEADER_SIZE;
   
   if (mboolUsingPPP)
      return PPP_HEADER_SIZE + UDP_HEADER_SIZE;
   else
      return IP_HEADER_SIZE + UDP_HEADER_SIZE;
}

/*
 * Host
 *
 * Per-machine settings, such as bandwidth.
 *
 * When created, a host has infinite bandwidth.
 */

#ifndef _HOST_H
#define _HOST_H

enum Protocol
{
   UDP, TCP
};


class Host
{
public:

   Host(uint32 new_address);
   ~Host();

   void RunCycle(float now);
   
   uint32 GetAddress()
   {
      return miAddress;
   }

   uint32 GetSendBandwidth()
   {
      return miSendBandwidth;
   }
   void   SetSendBandwidth(uint32 b)
   {
      miSendBandwidth = b;
   }

   uint32 GetReceiveBandwidth()
   {
      return miReceiveBandwidth;
   }
   void   SetReceiveBandwidth(uint32 b)
   {
      miReceiveBandwidth = b;
   }

   bool   IsUsingPPP()
   {
      return mboolUsingPPP;
   }
   void   SetUsingPPP(bool b)
   {
      mboolUsingPPP = b;
   }

   uint32 GetCompression()
   {
      return miCompression;
   }
   void   SetCompression(uint32 c)
   {
      miCompression = c;
   }

   // Get smoothed averages of bandwidth usage
   uint32 GetSendAverage()
   {
      return miAverageSentBytes;
   }
   uint32 GetReceiveAverage()
   {
      return miAverageReceivedBytes;
   }
   
   // Put user-readable version of address in str
   void AddressToString(char *str);

   // Called when this host sent us a message;
   // return the number of milliseconds to delay the message to
   // simulate the time it took to transmit it.
   uint32 MessageSent(Message *msg, Protocol protocol);

   // Called when this host is to receive a message;
   // return the number of milliseconds to delay the message to
   // simulate the time it takes to receive it.   
   uint32 MessageReceived(Message *msg, Protocol protocol);

private:

   uint32 miAddress;
   uint32 miSendBandwidth;
   uint32 miReceiveBandwidth;

   // # of bytes waiting to send on the simulated host.
   // Need to keep track of this in floating point because
   // we might simulate a send of < 1 byte in each call to RunCycle().
   float  mfSendQueueLength;

   // # of bytes waiting to be received on the simulated host
   float  mfReceiveQueueLength;
   
   // Last time RunCycle() was called
   float  mfLastCycle;

   // true to use PPP header size
   bool   mboolUsingPPP;

   // Simulated compression (percent)
   uint32 miCompression;

   // Periodically compute a smoothed average of sent and received byte rates.
   uint32 miSentBytes;
   uint32 miReceivedBytes;
   uint32 miAverageSentBytes;
   uint32 miAverageReceivedBytes;
   float  mfLastAverageTime;
   

   void RecomputeAverages(float now);
   
   // Return the size of the network layer (IP, UDP) header in messages
   // to this host.
   uint32 GetHeaderSize(Protocol protocol);   
};

#endif

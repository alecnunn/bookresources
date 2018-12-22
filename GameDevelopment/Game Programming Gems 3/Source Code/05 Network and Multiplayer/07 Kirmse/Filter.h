/*
 * Filter
 *
 * A description of what to do with packets going from one location to another.
 *
 * When created, a filter by default enabled, listed, with 0 latency
 * and 0 packet loss.
 */

#ifndef _FILTER_H
#define _FILTER_H

class Filter
{
public:

   Filter(Endpoint &new_from, Endpoint &new_to);
   Filter(const Filter &f);   
   virtual ~Filter();

   Endpoint &GetFromEndpoint()
   {
      return mFromEndpoint;
   }
   Endpoint &GetToEndpoint()
   {
      return mToEndpoint;
   }
   void      SwapEndpoints();
   
   void      Enable(bool new_enabled)
   {
      mboolEnabled = new_enabled;
   }
   bool      IsEnabled()
   {
      return mboolEnabled;
   }

   void      List(bool new_listed)
   {
      mboolListed = new_listed;
   }
   bool      IsListed()
   {
      return mboolListed;
   }

   int32     GetLatencyMean()
   {
      return miLatencyMean;
   }
   void      SetLatencyMean(int32 mean)
   {
      miLatencyMean = mean;
   }

   int32     GetLatencyVariance()
   {
      return miLatencyVariance;
   }
   void      SetLatencyVariance(int32 var)
   {
      miLatencyVariance = var;
   }

   // Packet loss is a percentage 0-100
   int32     GetPacketLoss()
   {
      return miPacketLoss;
   }
   void      SetPacketLoss(int32 loss)
   {
      miPacketLoss = loss;
   }

   // Reorder probability is a percentage 0-100
   int32     GetReorderProbability()
   {
      return miReorderProbability;
   }
   void      SetReorderProbability(int32 value)
   {
      miReorderProbability = value;
   }

   // Duplication probability is a percentage 0-100
   int32     GetDuplicationProbability()
   {
      return miDuplicationProbability;
   }
   void      SetDuplicationProbability(int32 value)
   {
      miDuplicationProbability = value;
   }

   Protocol  GetProtocol()
   {
      return mProtocol;
   } 
   void      SetProtocol(Protocol p)
   {
      mProtocol = p;
   } 
   
   virtual void    RunCycle(float now)  = 0;
   virtual Filter *Clone   ()           = 0;
   
   // Called when bytes arrive from our from endpoint.
   virtual void MessageReceived(Socket *socket, Message *msg)   = 0;

   // Called when a new connection is made
   virtual void SocketAccepted (Socket *sock)    = 0;

   // Called when a connect succeeds or fails
   virtual void SocketConnected(Socket *sock, int error_code) = 0;

   // Called when a socket closes
   virtual void SocketClosed   (Socket *sock) = 0;

   // Create a socket listening on the given port
   virtual void TryCreateSocket(uint16 port) = 0;

   // Get/set values to/from string
   void         ToString(char *str);
   
protected:
   
   int32 GenerateLatency();

private:

   Endpoint mFromEndpoint;
   Endpoint mToEndpoint;
   int32    miLatencyMean;
   int32    miLatencyVariance;
   int32    miPacketLoss;
   int32    miReorderProbability;
   int32    miDuplicationProbability;
   bool     mboolEnabled;
   bool     mboolListed;

   Protocol mProtocol;

   Filter &operator=(const Filter &that);
};

#endif

#include "main.h"

/***************************************************************/
Filter::Filter(Endpoint &new_from, Endpoint &new_to)
{
   mFromEndpoint = new_from;
   mToEndpoint   = new_to;
   mboolEnabled  = true;
   mboolListed   = true;
   mProtocol     = UDP;

   miLatencyMean            = 0;
   miLatencyVariance        = 0;
   miPacketLoss             = 0;
   miReorderProbability     = 0;
   miDuplicationProbability = 0;
}
/***************************************************************/
Filter::Filter(const Filter &f)
{
   *this = f;
   mToEndpoint    = f.mToEndpoint;
   mFromEndpoint  = f.mFromEndpoint;
}
/***************************************************************/
Filter::~Filter()
{
}
/***************************************************************/
void Filter::SwapEndpoints()
{
   Endpoint temp = mToEndpoint;
   mToEndpoint   = mFromEndpoint;
   mFromEndpoint = temp;
}
/***************************************************************/
Filter &Filter::operator=(const Filter &that)
{
   mFromEndpoint     = that.mFromEndpoint;
   mToEndpoint       = that.mToEndpoint;
   mboolEnabled      = that.mboolEnabled;
   mboolListed       = that.mboolListed;

   miLatencyMean     = that.miLatencyMean;
   miLatencyVariance = that.miLatencyVariance;
   miPacketLoss      = that.miPacketLoss;
   mProtocol         = that.mProtocol;

   return *this;
}
/***************************************************************/
int32 Filter::GenerateLatency()
{
   /* See Knuth TAOCP Section 3.4.1 Algorithm P */
   float u1, u2, v1, v2, s, x1;
   while (true)
   {
      u1 = ((float) rand()) / RAND_MAX;
      u2 = ((float) rand()) / RAND_MAX;
      v1 = 2*u1 - 1;
      v2 = 2*u2 - 1;
      s = v1 * v1 + v2 * v2;
      if (s <= 1.0)
         break;
   }

   x1 = v1 * (float) sqrt(-2.0 * log(s) / s);

   int32 latency = (int32) (miLatencyMean + x1 * miLatencyVariance);
   if (latency < 0)
      latency = 0;
   return latency;
}
/***************************************************************/
void Filter::ToString(char *str)
{
   char str1[200], str2[200];
   GetFromEndpoint().ToString(str1);
   GetToEndpoint().ToString(str2);
   sprintf(str, "%s,%s,%s,%d,%d,%d,%d,%d,%d,%d", str1, str2,
           GetProtocol() == UDP ? "U" : "T",
           IsListed(),
           GetLatencyMean(), GetLatencyVariance(),
           GetPacketLoss(), IsEnabled(),
           GetDuplicationProbability(), GetReorderProbability());
}

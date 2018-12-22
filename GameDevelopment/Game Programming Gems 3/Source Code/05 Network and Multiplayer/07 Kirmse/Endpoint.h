/*
 * Endpoint
 *
 * Encapsulates an IP address and port number.
 */

#ifndef _ENDPOINT_H
#define _ENDPOINT_H

class Endpoint
{
public:

   Endpoint() {}
   
   Endpoint(uint32 new_address, uint16 new_port)
   {
      miAddress = new_address;
      miPort = new_port;
   }
   
   ~Endpoint() {}

   bool operator==(const Endpoint &that)
   {
      return miAddress == that.miAddress && miPort == that.miPort;
   }

   Endpoint &operator=(const Endpoint &that)
   {
      miAddress = that.miAddress;
      miPort    = that.miPort;
      return *this;
   }

   void   SetAddress(uint32 new_address) { miAddress = new_address; }
   uint32 GetAddress() const
   {
      return miAddress;
   }
   
   void   SetPort(uint16 new_port)       { miPort = new_port; }
   uint16 GetPort() const
   {
      return miPort;
   }

   // Put user-readable version of address in str
   void ToString(char *str) const
   {
      sprintf(str, "%d.%d.%d.%d:%d", (miAddress & 0xff000000) >> 24,
              (miAddress & 0x00ff0000) >> 16,
              (miAddress & 0x0000ff00) >> 8,
              (miAddress & 0x000000ff),
              miPort);
   }

   // Return true iff we read IP address and port from the string.
   bool FromString(char *str)
   {
      uint32 a1, a2, a3, a4;
      char *ptr;
      if (sscanf(str, "%d.%d.%d.%d", &a1, &a2, &a3, &a4) != 4)
         return false;

      ptr = strchr(str, ':');
      if (ptr != NULL)
         if (sscanf(ptr, ":%d", &miPort) != 1)
            return false;

      miAddress = (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;
      return true;
   }

protected:

   uint32 miAddress;
   uint16 miPort;
   
};

#endif

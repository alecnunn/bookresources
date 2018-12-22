#ifndef _UDPSOCKET_H
#define _UDPSOCKET_H

class UDPSocket : public Socket
{
public:

   UDPSocket(uint16 new_from_port);
   virtual ~UDPSocket();


   virtual bool Send(byte *data, uint32 len_data, Endpoint &endpoint);

   virtual void EventRead();
   
   virtual uint16  GetFromPort() const
   {
      return miFromPort;
   }
   
   virtual uint32  GetRemoteAddress() const
   {
      return 0;
   }
   
private:

   uint16  miFromPort;
};

#endif

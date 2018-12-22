#ifndef _LISTENSOCKET_H
#define _LISTENSOCKET_H

class ListenSocket : public Socket
{
public:

   ListenSocket(uint16 new_from_port);
   virtual ~ListenSocket();

   virtual void    EventRead();

   virtual bool    Send(byte *data, uint32 len_data, Endpoint &endpoint);
   
   virtual Socket *Accept();

   virtual uint16  GetFromPort() const
   {
      return miPort;
   }

   virtual uint32  GetRemoteAddress() const
   {
      return 0;
   }
   
protected:

   uint16          miPort;
};

#endif

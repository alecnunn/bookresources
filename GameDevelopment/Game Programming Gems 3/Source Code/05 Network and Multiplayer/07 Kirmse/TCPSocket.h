#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H

class TCPSocket : public Socket
{
public:

   // Constructor for newly accepted connection
   TCPSocket(SOCKET new_sock, uint32 new_remote_address, uint16 new_from_port);

   // Constructor for connecting to remote endpoint
   TCPSocket(Endpoint &endpoint);
   
   virtual ~TCPSocket();

   virtual void EventRead();

   virtual bool Send(byte *data, uint32 len_data, Endpoint &endpoint);
   
   virtual uint16  GetFromPort() const
   {
      return msFromPort;
   }

   virtual uint32  GetRemoteAddress() const
   {
      return miRemoteAddress;
   }
   
protected:

   uint32 miRemoteAddress;
   uint16 msFromPort;
};

#endif

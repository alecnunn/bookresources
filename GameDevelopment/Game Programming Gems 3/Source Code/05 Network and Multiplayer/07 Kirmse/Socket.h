/*
 * Socket
 *
 * Encapsulates a socket.
 */

#ifndef _SOCKET_H
#define _SOCKET_H

class Socket
{
public:

   Socket();
   virtual ~Socket();

   bool IsValid() const
   {
      return mSocket != INVALID_SOCKET;
   }
   
   SOCKET   GetSocket() const
   {
      return mSocket;
   }

   bool    IsConnected() const
   {
      return mboolConnected;
   }

   void    SetConnected(bool connected)
   {
      mboolConnected = connected;
   }

   // Called in response to events on the socket
   virtual void    EventRead()        = 0;

   virtual uint16  GetFromPort() const      = 0;
   virtual uint32  GetRemoteAddress() const = 0;
   
   virtual bool    Send(byte *data, uint32 len_data, Endpoint &endpoint) = 0;
   bool            Send(Message *msg, Endpoint &endpoint);
   
   virtual Socket *Accept();
   
protected:

   bool SetAsynchronous(int flags);
   
   enum
   {
      MAX_MSG_SIZE = 2000,
   };
   
   SOCKET   mSocket;

private:
   
   bool     mboolConnected;
};

#endif

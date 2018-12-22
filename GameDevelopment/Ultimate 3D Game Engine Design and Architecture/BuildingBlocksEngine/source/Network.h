/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_NETWORK_H_
#define _BB_NETWORK_H_

#include<Socket.h>
#include<NameSpace.h>
#include<Packet.h>


DECLARE_ENGINE_NAMESPACE


class Network
{
   public:
      Network(int maxConnections)
      {
         m_isConnected = 0,
         m_callback = NULL;
         m_networkType = BB_NETWORK_NULL;
         m_totalUsers = 0;
         m_users = NULL;

         if(maxConnections > 0)
            {
               m_users = new Socket[maxConnections];
               m_maxConnections = maxConnections;
            }
      }

      ~Network()
      {
         if(m_users != NULL)
            {
               delete[] m_users;
               m_users = NULL;
            }
      }

      BRESULT Host(int port, int numPending);
      BNETWORKRESULT Connect(char *ip, int port);

      void Disconnect();
      void DisconnectClient(int index);

      BNETWORKRESULT Send(Packet &packet)
      {
         // ONLY SYSTEM MESSAGES CAN HAVE USAGE OF 0.
         if(packet.GetUsage() == 0)
            return BB_DATA_FAIL;

         return SendPacket(packet);
      }
      
      BNETWORKRESULT Send(Packet &packet, int sendTo)
      {
         // ONLY SYSTEM MESSAGES CAN HAVE USAGE OF 0.
         if(packet.GetUsage() == 0)
            return BB_DATA_FAIL;

         return SendPacket(packet, sendTo);
      }

   private:
      BNETWORKRESULT SendPacket(Packet &packet);
      BNETWORKRESULT SendPacket(Packet &packet, int sendTo);

   public:
      void Process();

      void SetCallBackFunction(BB_NETWORK_CALLBACK(funcPtr))
      {
         m_callback = funcPtr;
      }

      bool isConnected() { return m_isConnected; }
      int GetMaxConnections() { return m_maxConnections; }
      int GetTotalConnections() { return m_totalUsers; }
      BB_NETWORK_TYPE GetNetworkType() { return m_networkType; }

   private:
      BRESULT AcceptConnection();

   private:
      bool m_isConnected;
      BB_NETWORK_TYPE m_networkType;

      Socket m_connection;
      Socket *m_users;
      int m_totalUsers;
      int m_maxConnections;

      BB_NETWORK_CALLBACK(m_callback);
};

END_ENGINE_NAMESPACE

#endif
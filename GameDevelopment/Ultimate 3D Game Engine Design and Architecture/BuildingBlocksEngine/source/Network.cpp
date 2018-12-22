/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Network.h>


DECLARE_ENGINE_NAMESPACE


BRESULT Network::Host(int port, int numPending)
{
   if(m_connection.SetAsServer(port, numPending) == false)
      return BB_FAIL;

   m_isConnected = true;
   m_networkType = BB_NETWORK_SERVER;

   return BB_SUCCESS;
}


BNETWORKRESULT Network::Connect(char *ip, int port)
{
   if(m_connection.ConnectToServer(ip, port) == false)
      return BB_DATA_FAIL;

   m_isConnected = true;
   m_networkType = BB_NETWORK_CLIENT;

   return BB_DATA_SUCCESS;
}


void Network::Disconnect()
{
   int i = 0;
   BB_NETWORK_MESSAGE_TYPE type;

   if(m_networkType == BB_NETWORK_NULL)
      return;

   if(m_networkType == BB_NETWORK_SERVER)
      type = BB_MESSAGE_SERVER_DISCONNECT;
   else
      type = BB_MESSAGE_CLIENT_DISCONNECT;

   Packet packet(BB_NETWORK_SYSTEM_USAGE, type, NULL, 0);
   SendPacket(packet);

   for(; i < m_totalUsers; i++)
      {
         if(m_users[i].isConnected())
            m_users[i].Disconnect();
      }

   m_connection.Disconnect();
}


void Network::DisconnectClient(int index)
{
   BB_NETWORK_MESSAGE_TYPE type;

   if(m_networkType != BB_NETWORK_SERVER)
      return;

   type = BB_MESSAGE_FORCE_DISCONNECT;

   Packet packet(BB_NETWORK_SYSTEM_USAGE, type, NULL, 0);
   SendPacket(packet, index);

   if(index < m_totalUsers)
      {
         if(m_users[index].isConnected())
            m_users[index].Disconnect();
      }
}


BNETWORKRESULT Network::SendPacket(Packet &packet)
{
   int i = 0;

   if(m_networkType == BB_NETWORK_NULL)
      return BB_DATA_FAIL;

   if(m_networkType == BB_NETWORK_SERVER)
      {
         for(; i < m_totalUsers; i++)
            {
               if(SendPacket(packet, i) != BB_DATA_SUCCESS)
                  {
                     if(m_callback)
                        {
                           m_callback(BB_NETWORK_SYSTEM_USAGE,
                                      BB_MESSAGE_SEND_ERROR,
                                      (char*)&i, sizeof(int));
                        }
                  }
            }
      }
   else
      {
         if(m_connection.SendData(packet) != BB_DATA_SUCCESS)
            return BB_DATA_FAIL;
      }

   return BB_DATA_SUCCESS;
}


BNETWORKRESULT Network::SendPacket(Packet &packet, int sendTo)
{
   if(sendTo < 0 || sendTo >= m_totalUsers)
      return BB_DATA_FAIL;

   if(m_networkType != BB_NETWORK_SERVER)
      return BB_DATA_FAIL;

   if(m_users[sendTo].isConnected() == false)
      return BB_NO_DATA;

   if(m_users[sendTo].SendData(packet) != BB_DATA_SUCCESS)
      return BB_DATA_FAIL;

   return BB_DATA_SUCCESS;
}


void Network::Process()
{
   int read = 0;
   Packet packet(0, 0, NULL, 0);

   if(m_networkType == BB_NETWORK_NULL)
      return;

   if(AcceptConnection())
      {
         if(m_callback)
            {
               m_callback(BB_NETWORK_SYSTEM_USAGE,
                          BB_MESSAGE_CONNECT_ACCEPTED,
                          NULL, 0);
            }
      }

   if(m_networkType == BB_NETWORK_SERVER)
      {
         for(int i = 0; i < m_totalUsers; i++)
            {
               if(m_users[i].isConnected() == false)
                  continue;

               m_users[i].RecieveData(packet, &read);
               
               if(read > 0)
                  {
                     if(m_callback)
                        {
                           m_callback((int)packet.GetUsage(),
                                    (int)packet.GetID(),
                                    packet.GetData(),
                                    packet.GetDataSize());
                        }
                  }
            }
      }
   else
      {
         m_connection.RecieveData(packet, &read);
         
         if(read > 0)
            {
               if(m_callback)
                  {
                     m_callback((int)packet.GetUsage(),
                              (int)packet.GetID(),
                              packet.GetData(),
                              packet.GetDataSize());
                  }
            }
      }
}


BRESULT Network::AcceptConnection()
{
   if(m_networkType != BB_NETWORK_SERVER)
      return BB_FAIL;

   if(m_totalUsers >= m_maxConnections)
      return BB_FAIL;

   if(m_connection.AcceptConnection(m_users[m_totalUsers]) == false)
      return BB_FAIL;

   m_totalUsers++;

   return BB_SUCCESS;
}

END_ENGINE_NAMESPACE

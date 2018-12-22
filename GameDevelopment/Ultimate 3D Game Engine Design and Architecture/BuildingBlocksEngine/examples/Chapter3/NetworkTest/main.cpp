/*
   Network Test Demo - Chapter 3
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<Network.h>
#include<string>

using namespace std;

void Server(int PortNumber);
void Client(char *ServerIP, int PortNumber);


bbe::Network g_network(32);
bool g_disconnect = false;

#define NETWORK_STATUS_TYPE   1
#define NETWORK_VERIFY        1

#define NETWORK_ACTION_TYPE   2
#define NETWORK_PRINT         1


void NetworkCallback(int usage, int type, char *data, int size)
{
   switch(usage)
      {
         case BB_NETWORK_SYSTEM_USAGE:
            if(type == BB_MESSAGE_SERVER_DISCONNECT)
               {
                  cout << "Server Disconnected." << endl;
                  g_disconnect = true;
               }
            else if(type == BB_MESSAGE_CLIENT_DISCONNECT)
               {
                  cout << "Client Disconnected." << endl;
                  g_disconnect = true;
                  
               }
            break;

         case NETWORK_STATUS_TYPE:
            if(type == NETWORK_VERIFY)
               {
                  cout << "Client Connected." << endl;
               }
            break;

         case NETWORK_ACTION_TYPE:
            if(type == NETWORK_PRINT)
               {
                  if(data != NULL)
                     cout << "Message: " << data << endl;

                  g_disconnect = true;
               }
            break;
      }
}


int main(int args, char **argc)
{
   string connectionType;
   char IP[] = {"127.0.0.1"};
   char PORT_NUMBER[] = {"6000"};

#ifdef _WIN32
   WSADATA WinSockData;
   WSAStartup(MAKEWORD(2, 0), &WinSockData);
#endif

   cout << "Sending/Recieving Data test" << endl;
   cout << "Connecting to IP address " << IP << endl;
   cout << "Connecting to port " << PORT_NUMBER << endl << endl;

   cout << "Type in server to act as a server " <<
           "or client to act as a client --> ";

   getline(cin, connectionType);

   g_network.SetCallBackFunction(NetworkCallback);

   if(connectionType == "server")
      {
         Server(atoi(PORT_NUMBER));
      }
   else if(connectionType == "client")
      {
         Client(IP, atoi(PORT_NUMBER));
      }
   else
      {
         cout << "You didn't choose server or client" << endl;
      }

#ifdef _WIN32
   WSACleanup();
#endif
   return 1;
}


void Server(int PortNumber)
{
   bbe::Packet packet(0, 0, 0, 0);
   cout << "Server listening for a connection..." << endl;

   if(g_network.Host(PortNumber, 32))
      {
         while(g_disconnect == false)
            g_network.Process();

         g_network.Disconnect();
         cout << "Server Disconnected" << endl;
      }
   else
      {
         cout << "Error hosting." << endl;
      }
}


void Client(char *ServerIP, int PortNumber)
{
   string str("Hello Server!");

   bbe::Packet verifyPacket(NETWORK_STATUS_TYPE,
                            NETWORK_VERIFY, 0, 0);

   bbe::Packet messagePacket(NETWORK_ACTION_TYPE,
                             NETWORK_PRINT,
                             str.c_str(), str.length());

   cout << "Client: connecting to " <<  ServerIP <<
           " and port " << PortNumber << endl;

   if(g_network.Connect(ServerIP, PortNumber))
      {
         cout << "Client: Connection Success" << endl;

         g_network.Send(verifyPacket);
         g_network.Send(messagePacket);

         while(g_disconnect == false)
            g_network.Process();

         g_network.Disconnect();
         cout << "Disconnected." << endl;
      }
   else
      {
         cout << "ERROR: Connection Failed!" << endl;
      }
}
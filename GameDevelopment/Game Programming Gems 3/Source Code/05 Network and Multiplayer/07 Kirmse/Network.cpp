#include "main.h"

static const int16 WINSOCK_MAJOR_VERSION = 1;
static const int16 WINSOCK_MINOR_VERSION = 1;

static const char *config_num_filters = "NumFilters";
static const char *config_filter_base = "Filter";
static const char *config_num_hosts   = "NumHosts";
static const char *config_host_base   = "Host";

/***************************************************************/
Network::Network()
{
   mboolInitialized = false;
   
   WSADATA data;
   
   // Start Winsock
   if (WSAStartup(MAKEWORD(WINSOCK_MAJOR_VERSION, WINSOCK_MINOR_VERSION), &data) != 0)
   {
      dprintf("Could not load Winsock");
      return;
   }

   if (LOBYTE(data.wVersion) != WINSOCK_MAJOR_VERSION ||
       HIBYTE(data.wVersion) != WINSOCK_MINOR_VERSION)
   {
      dprintf("Found Winsock with bad version %d.%d",
              LOBYTE(data.wVersion), HIBYTE(data.wVersion));
      WSACleanup();
      return;
   }

   mboolInitialized = true;
}
/***************************************************************/
Network::~Network()
{
   if (mboolInitialized)
      WSACleanup();

   AddToConfig();
   
   // Delete filters
   filter_set_type::iterator it;
   for (it = mFilters.begin(); it != mFilters.end(); ++it)
      delete *it;

   // Delete hosts
   host_set_type::iterator hit;
   for (hit = mHosts.begin(); hit != mHosts.end(); ++hit)
      delete *hit;
}
/***************************************************************/
void Network::AddSocket(Socket *s, Protocol protocol)
{
   if (protocol == UDP)
      mUDPSockets[s->GetSocket()] = s;
   else
      mTCPSockets[s->GetSocket()] = s;
}
/***************************************************************/
void Network::RemoveSocket(Socket *s)
{
   mUDPSockets.erase(s->GetSocket());
   mTCPSockets.erase(s->GetSocket());
}
/***************************************************************/
Socket *Network::FindSocketByPort(uint16 port, Protocol protocol)
{
   socket_set_type *sockets = &mTCPSockets;
   if (protocol == UDP)
      sockets = &mUDPSockets;
   
   socket_set_type::iterator it;
   for (it = sockets->begin(); it != sockets->end(); ++it)
   {
      Socket *socket = (*it).second;
      if (socket->GetFromPort() == port)
         return socket;
   }
   return NULL;
}
/***************************************************************/
void Network::AddFilter(Filter *filter)
{
   mFilters.insert(filter);

   // See if we need to create a new socket to listen on
   uint16 port = filter->GetFromEndpoint().GetPort();
   Protocol protocol = filter->GetProtocol();
   if (FindSocketByPort(port, protocol) == NULL)
      filter->TryCreateSocket(port);
}
/***************************************************************/
void Network::RemoveFilter(Filter *filter)
{
   mFilters.erase(filter);
   delete filter;
}
/***************************************************************/
void Network::AddHost(Host *host)
{
   mHosts.insert(host);
}
/***************************************************************/
void Network::RemoveHost(Host *host)
{
   mHosts.erase(host);
   delete host;
}
/***************************************************************/
void Network::MessageReceived(Message *msg, Socket *sock,
                              Protocol protocol, Endpoint &endpoint)
{
   // Pass message on to appropriate filter(s)
   Filter *filter = FindEnabledFilterByFromEndpoint(endpoint, protocol);

   if (filter != NULL)
      filter->MessageReceived(sock, msg);
   
   filter = FindEnabledFilterByToEndpoint(endpoint, protocol);
   if (filter != NULL)
      filter->MessageReceived(sock, msg);
}
/***************************************************************/
void Network::On_WM_SOCKET_NOTIFY(SOCKET sock, int32 event, int32 error)
{
   socket_set_type::iterator it = mUDPSockets.find(sock);
   if (it == mUDPSockets.end())
   {
      it = mTCPSockets.find(sock);

      if (it == mTCPSockets.end())
      {
         dprintf("Got socket event for unknown socket %d\n", sock);
         return;
      }
   }

   Socket *socket = (*it).second;
   Filter *filter;
   switch (event)
   { 
   case FD_CLOSE:
   {
      // Tell any filters about it
      Endpoint endpoint(socket->GetRemoteAddress(), socket->GetFromPort());
      filter = FindEnabledFilterByFromEndpoint(endpoint, TCP);
      
      if (filter != NULL)
         filter->SocketClosed(socket);

      filter = FindEnabledFilterByToEndpoint(endpoint, TCP);
      if (filter != NULL)
         filter->SocketClosed(socket);

      break;
   }

   case FD_READ:
      socket->EventRead();
      break;

   case FD_ACCEPT:
   {
      if (error != 0)
      {
         dprintf("Got socket error %d\n", error);
         return;
      }

      // Keep accepting until there's nothing left
      Socket *new_sock;
      do
      {
         new_sock = socket->Accept();
         if (new_sock != NULL)
         {
            AddSocket(new_sock, TCP);
            
            // Tell filter about the new socket
            Endpoint from_endpoint(new_sock->GetRemoteAddress(), socket->GetFromPort());
            Filter *filter = FindEnabledFilterByFromEndpoint(from_endpoint, TCP);
            
            if (filter != NULL)
               filter->SocketAccepted(new_sock);
         }
      } while (new_sock != NULL);

      break;
   }

   case FD_CONNECT:
   {
      // Tell filter the result
      Endpoint endpoint(socket->GetRemoteAddress(), socket->GetFromPort());
      filter = FindEnabledFilterByToEndpoint(endpoint, TCP);
      filter->SocketConnected(socket, error);
      break;
   }
   }
}
/***************************************************************/
void Network::RunCycle(float now)
{
   filter_set_type::iterator it;
   for (it = mFilters.begin(); it != mFilters.end(); ++it)
      (*it)->RunCycle(now);

   host_set_type::iterator hit;
   for (hit = mHosts.begin(); hit != mHosts.end(); ++hit)
      (*hit)->RunCycle(now);
}
/***************************************************************/
void Network::GetHostAddresses(char *str)
{
   char hostname[200];
   gethostname(hostname, sizeof hostname);
   HOSTENT *hostent = gethostbyname(hostname);

   *str = 0;
   int32 adapter = 0;
   while (hostent->h_addr_list[adapter] != NULL)
   {
      char buf[50];
      uint32 address = ntohl(* ((uint32 *) hostent->h_addr_list[adapter]));
      sprintf(buf, "(%d.%d.%d.%d)   ", (address >> 24), (address >> 16) & 0xff,
              (address >> 8) & 0xff, (address) & 0xff);
      strcat(str, buf);
      adapter++;
   }
}
/***************************************************************/
Host *Network::FindHostByAddress(uint32 address)
{
   host_set_type::iterator it;
   for (it = mHosts.begin(); it != mHosts.end(); ++it)
      if ((*it)->GetAddress() == address)
         return *it;
   return NULL;
}
/***************************************************************/
Filter *Network::FindEnabledFilterByFromEndpoint(Endpoint &endpoint, Protocol protocol)
{
   filter_set_type::iterator it;
   for (it = mFilters.begin(); it != mFilters.end(); ++it)
      if ((*it)->GetFromEndpoint() == endpoint &&
          (*it)->GetProtocol() == protocol &&
         (*it)->IsEnabled())
         return *it;
   return NULL;
}
/***************************************************************/
Filter *Network::FindEnabledFilterByToEndpoint(Endpoint &endpoint, Protocol protocol)
{
   filter_set_type::iterator it;
   for (it = mFilters.begin(); it != mFilters.end(); ++it)
      if ((*it)->GetToEndpoint() == endpoint &&
          (*it)->GetProtocol() == protocol &&
         (*it)->IsEnabled())
         return *it;
   return NULL;
}
/***************************************************************/
void Network::ReadFromConfig()
{
   // Load filters
   int32 num_filters;
   if (!app->config->LookupInt32(config_num_filters, &num_filters))
      return;

   int32 i;
   char name[100], *str, *ptr;
   for (i = 0; i < num_filters; i++)
   {
      sprintf(name, "%s%d", config_filter_base, i);
      if (!app->config->LookupString(name, &str))
         continue;

      Endpoint e1, e2;
      ptr = strchr(str, ',');
      if (ptr == NULL)
         continue;

      ptr++;
      if (!e1.FromString(str) || !e2.FromString(ptr))
         continue;
      
      Filter *filter;

      int32 b;
      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;

      ptr++;
      if (*ptr == 'U')
         filter = new UDPFilter(e1, e2);
      else
         filter = new TCPFilter(e1, e2);

      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;
      
      ptr++;
      if (sscanf(ptr, "%d", &b) == 1)
         filter->List(b != 0);
      
      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;

      ptr++;
      if (sscanf(ptr, "%d", &b) == 1)
         filter->SetLatencyMean(b);
      
      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;
      
      ptr++;
      if (sscanf(ptr, "%d", &b) == 1)
         filter->SetLatencyVariance(b);
                  
      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;

      ptr++;
      if (sscanf(ptr, "%d", &b) == 1)
         filter->SetPacketLoss(b);
      
      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;

      ptr++;
      if (sscanf(ptr, "%d", &b) == 1)
         filter->Enable(b != 0);

      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;

      ptr++;
      if (sscanf(ptr, "%d", &b) == 1)
         filter->SetDuplicationProbability(b);

      ptr = strchr(ptr, ',');
      if (ptr == NULL)
         continue;

      ptr++;
      if (sscanf(ptr, "%d", &b) == 1)
         filter->SetReorderProbability(b);
      
      app->inter->AddFilter(filter);
   }

   // Load hosts
   int32 num_hosts;
   if (!app->config->LookupInt32(config_num_hosts, &num_hosts))
      return;

   for (i = 0; i < num_hosts; i++)
   {
      sprintf(name, "%s%d", config_host_base, i);
      if (!app->config->LookupString(name, &str))
         continue;

      Endpoint e;
      if (!e.FromString(str))
         continue;
      
      Host *host = new Host(e.GetAddress());
      
      int32 b;
      ptr = strchr(str, ',');
      if (ptr != NULL)
      {
         ptr++;
         if (sscanf(ptr, "%d", &b) == 1)
            host->SetSendBandwidth(b);

         ptr = strchr(ptr, ',');
         if (ptr != NULL)
         {
            ptr++;
            if (sscanf(ptr, "%d", &b) == 1)
               host->SetReceiveBandwidth(b);

            ptr = strchr(ptr, ',');
            if (ptr != NULL)
            {
               ptr++;
               if (sscanf(ptr, "%d", &b) == 1)
                  host->SetUsingPPP(b != 0);

               ptr = strchr(ptr, ',');
               if (ptr != NULL)
               {
                  ptr++;
                  if (sscanf(ptr, "%d", &b) == 1)
                     host->SetCompression(b);
               }
            }
         }
      }
      
      app->inter->AddHost(host);
   }
}
/***************************************************************/
void Network::AddToConfig()
{
   int32 num_filters = mFilters.size();
   app->config->SetInt32(config_num_filters, num_filters);

   char name[100], str1[Config::MAX_LINE_LENGTH];
   char str[Config::MAX_LINE_LENGTH];
   int32 i = 0;
   filter_set_type::iterator it;
   for (it = mFilters.begin(); it != mFilters.end(); ++it)
   {
      Filter *filter = *it;
      
      sprintf(name, "%s%d", config_filter_base, i);

      filter->ToString(str);
      app->config->SetString(name, str);
      i++;
   }

   int32 num_hosts = mHosts.size();
   app->config->SetInt32(config_num_hosts, num_hosts);
   i = 0;
   host_set_type::iterator hit;
   for (hit = mHosts.begin(); hit != mHosts.end(); ++hit)
   {
      Host *host = *hit;

      sprintf(name, "%s%d", config_host_base, i);

      host->AddressToString(str1);
      sprintf(str, "%s,%d,%d,%d,%d", str1, host->GetSendBandwidth(),
              host->GetReceiveBandwidth(),
              host->IsUsingPPP(),
              host->GetCompression());
      
      app->config->SetString(name, str);
      i++;
   }
}

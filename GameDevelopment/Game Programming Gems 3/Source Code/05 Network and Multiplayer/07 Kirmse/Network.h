/*
 * Network
 *
 * Initializes the network and keeps track of connections.
 */

#ifndef _NETWORK_H
#define _NETWORK_H

class Network
{
public:

   Network();
   ~Network();

   void ReadFromConfig();
   
   void AddFilter   (Filter *filter);
   void RemoveFilter(Filter *filter);

   void AddHost   (Host *host);
   void RemoveHost(Host *host);
   
   void AddSocket   (Socket *s, Protocol protocol);
   void RemoveSocket(Socket *s);
   
   void MessageReceived(Message *ms, Socket *sock, Protocol protocol,
                        Endpoint &endpoint);
   
   void On_WM_SOCKET_NOTIFY(SOCKET sock, int32 event, int32 error);

   void RunCycle(float now);

   // Put a text-readable version of our machine's IP address(es) into str.
   void GetHostAddresses(char *str);

   // Return the host with the given address, or NULL if none exists.
   Host *FindHostByAddress(uint32 address);
   
private:
   
   typedef map<SOCKET, Socket *> socket_set_type;
   socket_set_type mUDPSockets;
   socket_set_type mTCPSockets;

   typedef set<Filter *> filter_set_type;
   filter_set_type mFilters;

   typedef set<Host *> host_set_type;
   host_set_type mHosts;
   
   bool mboolInitialized;

   Socket *FindSocketByPort(uint16 port, Protocol protocol);

   // Return enabled filter with given "from" endpoint, or NULL if none.
   Filter *FindEnabledFilterByFromEndpoint(Endpoint &endpoint, Protocol protocol);

   Filter *FindEnabledFilterByToEndpoint(Endpoint &endpoint, Protocol protocol);

   void AddToConfig();
};

#endif

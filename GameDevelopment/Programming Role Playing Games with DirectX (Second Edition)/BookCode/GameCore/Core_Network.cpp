/**************************************************
WinMain.cpp
GameCore Component

Programming Role-Playing Games with DirectX
by Jim Adams (Jan 2004)
**************************************************/

#include "Core_Global.h"

// Default application GUID
GUID g_ApplicationGUID = { 0x71118060, 0xfdb4, 0x11d4, { 0x90, 0x89, 0xeb, 0x5b, 0x75, 0x97, 0xe5, 0x70 } };

// This function sets a new application GUID
BOOL SetApplicationGUID(GUID *ApplicationGUID)
{
  if(ApplicationGUID == NULL)
    return FALSE;
  memcpy(&g_ApplicationGUID, ApplicationGUID, sizeof(GUID));
  return TRUE;
}

cNetworkAdapter::cNetworkAdapter()
{
  m_AdapterList = NULL;
}

cNetworkAdapter::~cNetworkAdapter()
{
  Shutdown();
}

BOOL cNetworkAdapter::Init()
{
  IDirectPlay8Client *pDPClient = NULL;
  DWORD dwSize = 0;
  HRESULT hr;

  Shutdown();

  // Create DirectPlay Client component
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Client, NULL,    \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Client,           \
                             (void**)&pDPClient)))
    return FALSE;

  // Assign a message handler to network component
  if(FAILED(pDPClient->Initialize(NULL, NetMsgHandler, 0))) {
    pDPClient->Release();
    return FALSE;
  }

  // Query the required size of the data buffer
  if(pDPClient->EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, m_AdapterList, &dwSize, &m_NumAdapters, 0) != DPNERR_BUFFERTOOSMALL) {
    pDPClient->Release();
    return FALSE;
  }

  // Allocate a buffer
  if((m_AdapterList = (DPN_SERVICE_PROVIDER_INFO*)new BYTE[dwSize]) == NULL) {
    pDPClient->Release();
    return FALSE;
  }

  // Enumerate again
  hr = pDPClient->EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, m_AdapterList, &dwSize, &m_NumAdapters, 0);

  // Release network interface
  pDPClient->Release();

  if(FAILED(hr))
    return FALSE;

  return TRUE;
}

BOOL cNetworkAdapter::Shutdown()
{
  delete[] m_AdapterList;
  m_AdapterList = NULL;
  m_NumAdapters = 0;

  return TRUE;
}

long cNetworkAdapter::GetNumAdapters()
{
  return m_NumAdapters;
}

BOOL cNetworkAdapter::GetName(unsigned long Num, char *Buf)
{
  if(m_AdapterList == NULL)
    return FALSE;
  if(Num >= m_NumAdapters)
    return FALSE;

  // Buf must be at least MAX_PATH bytes in size
  wcstombs(Buf, m_AdapterList[Num].pwszName, MAX_PATH);

  return TRUE;
}

GUID *cNetworkAdapter::GetGUID(unsigned long Num)
{
  if(m_AdapterList == NULL)
    return NULL;
  if(Num >= m_NumAdapters)
    return NULL;

  return &m_AdapterList[Num].guid;
}

HRESULT WINAPI cNetworkAdapter::NetMsgHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
  return S_OK;
}

cNetworkServer::cNetworkServer()
{
  m_pDPServer          = NULL;
  m_Connected          = FALSE;
  m_Port               = 0;
  m_SessionName[0]     = 0;
  m_SessionPassword[0] = 0;
  m_MaxPlayers         = 0;
  m_NumPlayers         = 0;
}

cNetworkServer::~cNetworkServer()
{
  Shutdown();
}

IDirectPlay8Server *cNetworkServer::GetServerCOM()
{ 
  return m_pDPServer;
}

BOOL cNetworkServer::Init()
{
  Shutdown();

  // Create the Server object
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Server, NULL,    \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Server,           \
                             (void**)&m_pDPServer)))
    return FALSE;

  return TRUE;
}

BOOL cNetworkServer::Shutdown()
{
  Disconnect();

  // Release Server object
  ReleaseCOM(m_pDPServer);

  // Clear all member variables to default
  m_pDPServer          = NULL;
  m_Connected          = FALSE;
  m_Port               = 0;
  m_SessionName[0]     = 0;
  m_SessionPassword[0] = 0;
  m_MaxPlayers         = 0;
  m_NumPlayers         = 0;

  return TRUE;
}

BOOL cNetworkServer::Host(GUID *guidAdapter, long Port, char *SessionName, char *Password, long MaxPlayers)
{
  DPN_APPLICATION_DESC dpad;
  IDirectPlay8Address *pDPAddress;
  WCHAR wSessionName[MAX_PATH], wPassword[MAX_PATH];

  // Disconnect from current connection
  Disconnect();

  // Error checking
  if(m_pDPServer == NULL)
    return FALSE;
  if(SessionName == NULL)
    return FALSE;

  // Get port selection
  if(!(m_Port = Port))
    return FALSE;

  // Initialize the Server object
  if(FAILED(m_pDPServer->Initialize((PVOID)this, NetworkMessageHandler, 0)))
    return FALSE;

  // Create an address object
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Address, NULL,   \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Address,          \
                             (void**)&pDPAddress)))
    return FALSE;

  // Set protocol
  if(FAILED(pDPAddress->SetSP(&CLSID_DP8SP_TCPIP))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Set the port - must not be 0
  if(FAILED(pDPAddress->AddComponent(DPNA_KEY_PORT, &m_Port, sizeof(DWORD), DPNA_DATATYPE_DWORD))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Set adapter (if any)
  if(guidAdapter != NULL) {
    if(FAILED(pDPAddress->AddComponent(DPNA_KEY_DEVICE, guidAdapter, sizeof(GUID), DPNA_DATATYPE_GUID))) {
      pDPAddress->Release();
      return FALSE;
    }
  }

  // Record the session Name and Password
  strcpy(m_SessionName, SessionName);
  mbstowcs(wSessionName, SessionName, MAX_PATH);
  if(Password != NULL) {
    strcpy(m_SessionPassword, Password);
    mbstowcs(wPassword, Password, MAX_PATH);
  }

  // Setup the application description structure
  ZeroMemory(&dpad, sizeof(DPN_APPLICATION_DESC));
  dpad.dwSize          = sizeof(DPN_APPLICATION_DESC);
  dpad.dwFlags         = DPNSESSION_CLIENT_SERVER;
  dpad.guidApplication = g_ApplicationGUID;
  dpad.pwszSessionName = wSessionName;
  if(Password != NULL) {
    dpad.pwszPassword = wPassword;
    dpad.dwFlags |= DPNSESSION_REQUIREPASSWORD;
  }
  dpad.dwMaxPlayers = (m_MaxPlayers = MaxPlayers);

  // Start the hosting
  if(FAILED(m_pDPServer->Host(&dpad, &pDPAddress, 1, NULL, NULL, NULL, 0))) {
    pDPAddress->Release();
    return FALSE;
  }

  pDPAddress->Release();

  m_Connected = TRUE;

  return TRUE;
}

BOOL cNetworkServer::Disconnect()
{
  // Perform disconnect
  if(m_pDPServer != NULL) {
    if(FAILED(m_pDPServer->Close(0)))
      return FALSE;
  }
  m_Connected = FALSE;
  m_Port = 0;

  m_SessionName[0]     = 0;
  m_SessionPassword[0] = 0;

  m_MaxPlayers = 0;
  m_NumPlayers = 0;

  return TRUE;
}

BOOL cNetworkServer::IsConnected()
{
  return m_Connected;
};

BOOL cNetworkServer::Send(DPNID dpnidPlayer, void *Data, unsigned long Size, unsigned long Flags)
{
  DPNHANDLE       hAsync;
  DPN_BUFFER_DESC dpbd;

  // Error checking
  if(m_pDPServer == NULL)
    return FALSE;

  // Build a data structure
  if((dpbd.dwBufferSize = Size) == 0)
    return FALSE;
  dpbd.pBufferData  = (BYTE*)Data;

  m_pDPServer->SendTo(dpnidPlayer, &dpbd, 1, 0, NULL, &hAsync, Flags);

  return TRUE;
}

BOOL cNetworkServer::SendText(DPNID dpnidPlayer, char *Text, unsigned long Flags)
{
  DPNHANDLE       hAsync;
  DPN_BUFFER_DESC dpbd;

  // Error checking
  if(m_pDPServer == NULL)
    return FALSE;

  // Build a data structure
  dpbd.dwBufferSize = strlen(Text)+1;
  dpbd.pBufferData  = (BYTE*)Text;

  m_pDPServer->SendTo(dpnidPlayer, &dpbd, 1, 0, NULL, &hAsync, Flags);

  return TRUE;
}

BOOL cNetworkServer::DisconnectPlayer(long PlayerId)
{
  // Error checking
  if(m_pDPServer == NULL)
    return FALSE;

  // Try to disconnect the specified player
  if(FAILED(m_pDPServer->DestroyClient(PlayerId, NULL, 0, 0)))
    return FALSE;
  return TRUE;

  return TRUE;
}

BOOL cNetworkServer::GetIP(char *IPAddress, unsigned long PlayerNum)
{
  WSADATA  WSAData;
  HOSTENT *HostEnt;
  char     LocalName[MAX_PATH], *LocalIP;
  in_addr *LocalIPAddr;
  IDirectPlay8Address *pDPAddress;
  WCHAR ClientAddress[MAX_PATH];
  DWORD Size, Flags;

  // Get host IP address if PlayerNum == 0
  if(!PlayerNum) {
    if(WSAStartup(MAKEWORD(1,0), &WSAData))
      return FALSE;
    gethostname(LocalName, MAX_PATH);
    if((HostEnt = gethostbyname(LocalName)) == NULL) {
      WSACleanup();
      return FALSE;
    }
    LocalIPAddr = (in_addr*)HostEnt->h_addr_list[0];
    LocalIP = inet_ntoa(*LocalIPAddr);
    WSACleanup();

    if(LocalIP == NULL)
      return FALSE;

    strcpy(IPAddress, LocalIP);

    return TRUE;
  }

  if(m_pDPServer == NULL)
    return FALSE;
  if(m_Connected == FALSE)
    return FALSE;

  // Retrieve the player information
  if(FAILED(m_pDPServer->GetClientAddress((DPNID)PlayerNum, &pDPAddress, 0)))
    return FALSE;

  Size = MAX_PATH;
  Flags = DPNA_DATATYPE_STRING;
  if(FAILED(pDPAddress->GetComponentByName(DPNA_KEY_HOSTNAME, ClientAddress, &Size, &Flags))) {
    pDPAddress->Release();
    return FALSE;
  }

  wcstombs(IPAddress, ClientAddress, MAX_PATH);

  pDPAddress->Release();

  return TRUE;
}

BOOL cNetworkServer::GetName(char *Name, unsigned long PlayerId)
{
  DPN_PLAYER_INFO *dppi;
  DWORD Size;
  HRESULT hr;

  if(m_pDPServer == NULL)
    return FALSE;

  dppi = NULL;
  Size = 0;
  hr = m_pDPServer->GetClientInfo(PlayerId, dppi, &Size, 0);
  if(FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL)
    return FALSE;
  dppi = (DPN_PLAYER_INFO *)new char[Size];
  dppi->dwSize = sizeof(DPN_PLAYER_INFO);
  if(FAILED(m_pDPServer->GetClientInfo(PlayerId, dppi, &Size, 0))) {
    delete[] dppi;
    return FALSE;
  }
  wcstombs(Name, dppi->pwszName, MAX_PATH);
  delete[] dppi;

  return TRUE;
}

long cNetworkServer::GetPort()
{
  return m_Port;
}

BOOL cNetworkServer::GetSessionName(char *Buf)
{
  if(Buf == NULL)
    return FALSE;
  strcpy(Buf, m_SessionName);
  return TRUE;
}

BOOL cNetworkServer::GetSessionPassword(char *Buf)
{
  if(Buf == NULL)
    return FALSE;
  strcpy(Buf, m_SessionPassword);
  return TRUE;
}

long cNetworkServer::GetMaxPlayers()
{
  return m_MaxPlayers;
}

long cNetworkServer::GetNumPlayers()
{
  return m_NumPlayers;
}

HRESULT WINAPI cNetworkServer::NetworkMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
  cNetworkServer *Network;

  if((Network = (cNetworkServer*)pvUserContext) == NULL)
    return E_FAIL;

  // Pass message to user-defined function


  switch(dwMessageId) {
    case DPN_MSGID_ADD_PLAYER_TO_GROUP:
      if(Network->AddPlayerToGroup((DPNMSG_ADD_PLAYER_TO_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_ASYNC_OP_COMPLETE:
      if(Network->AsyncOpComplete((DPNMSG_ASYNC_OP_COMPLETE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CLIENT_INFO:
      if(Network->ClientInfo((DPNMSG_CLIENT_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CONNECT_COMPLETE:
      if(Network->ConnectComplete((DPNMSG_CONNECT_COMPLETE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CREATE_GROUP:
      if(Network->CreateGroup((DPNMSG_CREATE_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CREATE_PLAYER:
      if(Network->CreatePlayer((DPNMSG_CREATE_PLAYER *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_DESTROY_GROUP:
      if(Network->DestroyGroup((DPNMSG_DESTROY_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_DESTROY_PLAYER:
      if(Network->DestroyPlayer((DPNMSG_DESTROY_PLAYER *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_ENUM_HOSTS_QUERY:
      if(Network->EnumHostsQuery((DPNMSG_ENUM_HOSTS_QUERY *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_ENUM_HOSTS_RESPONSE:
      if(Network->EnumHostsResponse((DPNMSG_ENUM_HOSTS_RESPONSE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_GROUP_INFO:
      if(Network->GroupInfo((DPNMSG_GROUP_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_HOST_MIGRATE:
      if(Network->HostMigrate((DPNMSG_HOST_MIGRATE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_INDICATE_CONNECT:
      if(Network->IndicateConnect((DPNMSG_INDICATE_CONNECT *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_INDICATED_CONNECT_ABORTED:
      if(Network->IndicatedConnectAborted((DPNMSG_INDICATED_CONNECT_ABORTED *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_PEER_INFO:
      if(Network->PeerInfo((DPNMSG_PEER_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_RECEIVE:
      if(Network->Receive((DPNMSG_RECEIVE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP:
      if(Network->RemovePlayerFromGroup((DPNMSG_REMOVE_PLAYER_FROM_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_RETURN_BUFFER:
      if(Network->ReturnBuffer((DPNMSG_RETURN_BUFFER *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_SEND_COMPLETE:
      if(Network->SendComplete((DPNMSG_SEND_COMPLETE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_SERVER_INFO:
      if(Network->ServerInfo((DPNMSG_SERVER_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_TERMINATE_SESSION:
      if(Network->TerminateSession((DPNMSG_TERMINATE_SESSION *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
  }

  return E_FAIL;
}

cNetworkClient::cNetworkClient()
{
  m_pDPClient          = NULL;
  m_Connected          = FALSE;
  m_Port               = 0;
  m_IPAddress[0]       = 0;
  m_Name[0]            = 0;
  m_SessionName[0]     = 0;
  m_SessionPassword[0] = 0;
}

cNetworkClient::~cNetworkClient()
{
  Shutdown();
}

IDirectPlay8Client *cNetworkClient::GetClientCOM()
{
  return m_pDPClient;
}

BOOL cNetworkClient::Init()
{
  Shutdown();

  // Create the Client object
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Client, NULL,    \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Client,           \
                             (void**)&m_pDPClient)))
    return FALSE;

  return TRUE;
}

BOOL cNetworkClient::Shutdown()
{
  Disconnect();

  // Release Client object
  ReleaseCOM(m_pDPClient);

  // Clear all member variables to default
  m_pDPClient                = NULL;
  m_Connected          = FALSE;
  m_Port               = 0;
  m_IPAddress[0]       = 0;
  m_Name[0]            = 0;
  m_SessionName[0]     = 0;
  m_SessionPassword[0] = 0;

  return TRUE;
}

BOOL cNetworkClient::Connect(GUID *guidAdapter, char *IP, long Port, char *PlayerName, char *SessionName, char *SessionPassword)
{
  DPN_APPLICATION_DESC dpad;
  DPN_PLAYER_INFO      dppi;
  IDirectPlay8Address *pDPAddress, *pDPDevice;
  WCHAR wszPlayerName[MAX_PATH], wszIP[MAX_PATH];
  WCHAR wSessionName[MAX_PATH], wPassword[MAX_PATH];
  DPNHANDLE hAsync;

  Disconnect();

  // Do error checking
  if(m_pDPClient == NULL)
    return FALSE;
  if(SessionName == NULL || PlayerName == NULL)
    return FALSE;

  // Get port selection
  if(!(m_Port = Port))
    return FALSE;

  // Initialize the Client object
  if(FAILED(m_pDPClient->Initialize((PVOID)this, NetworkMessageHandler, 0)))
    return FALSE;

  // Assign client information
  ZeroMemory(&dppi, sizeof(DPN_PLAYER_INFO));
  dppi.dwSize = sizeof(DPN_PLAYER_INFO);
  dppi.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
  mbstowcs(wszPlayerName, PlayerName, strlen(PlayerName)+1); 
  dppi.pwszName = wszPlayerName;
  m_pDPClient->SetClientInfo(&dppi, NULL, NULL, DPNSETCLIENTINFO_SYNC);

  // Create address objects
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Address, NULL,   \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Address,          \
                             (void**)&pDPAddress)))
    return FALSE;
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Address, NULL,   \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Address,          \
                             (void**)&pDPDevice))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Set protocols
  pDPAddress->SetSP(&CLSID_DP8SP_TCPIP);
  pDPDevice->SetSP(&CLSID_DP8SP_TCPIP);

  // Set the port - must not be 0
  if(FAILED(pDPAddress->AddComponent(DPNA_KEY_PORT, &m_Port, sizeof(DWORD), DPNA_DATATYPE_DWORD))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Set the host
  mbstowcs(wszIP, IP, strlen(IP)+1);
  pDPAddress->AddComponent(DPNA_KEY_HOSTNAME, wszIP, (wcslen(wszIP)+1)*sizeof(WCHAR), DPNA_DATATYPE_STRING);

  // Set the adapter
  pDPAddress->AddComponent(DPNA_KEY_DEVICE, guidAdapter, sizeof(GUID), DPNA_DATATYPE_GUID);
  pDPDevice->AddComponent(DPNA_KEY_DEVICE, guidAdapter, sizeof(GUID), DPNA_DATATYPE_GUID);

  // Record the session Name and Password
  strcpy(m_SessionName, SessionName);
  mbstowcs(wSessionName, SessionName, MAX_PATH);
  if(SessionPassword != NULL) {
    strcpy(m_SessionPassword, SessionPassword);
    mbstowcs(wPassword, SessionPassword, MAX_PATH);
  }

  // Setup the application description structure
  ZeroMemory(&dpad, sizeof(DPN_APPLICATION_DESC));
  dpad.dwSize          = sizeof(DPN_APPLICATION_DESC);
  dpad.dwFlags         = DPNSESSION_CLIENT_SERVER;
  dpad.guidApplication = g_ApplicationGUID;
  dpad.pwszSessionName = wSessionName;
  if(SessionPassword != NULL) {
    dpad.pwszPassword = wPassword;
    dpad.dwFlags |= DPNSESSION_REQUIREPASSWORD;
  }

  // Connect
  if(FAILED(m_pDPClient->Connect(&dpad, pDPAddress, pDPDevice, NULL, NULL, NULL, 0, NULL, &hAsync, 0))) {
    pDPAddress->Release();
    pDPDevice->Release();
    return FALSE;
  }

  pDPAddress->Release();
  pDPDevice->Release();

  return TRUE;
}

BOOL cNetworkClient::Disconnect()
{
  // Perform disconnect
  if(m_pDPClient != NULL) {
    if(FAILED(m_pDPClient->Close(0)))
      return FALSE;
  }

  m_Connected          = FALSE;
  m_Port               = 0;
  m_IPAddress[0]       = 0;
  m_Name[0]            = 0;
  m_SessionName[0]     = 0;
  m_SessionPassword[0] = 0;

  return TRUE;
}

BOOL cNetworkClient::IsConnected()
{
  return m_Connected;
};

BOOL cNetworkClient::Send(void *Data, unsigned long Size, unsigned long Flags)
{
  DPNHANDLE       hAsync;
  DPN_BUFFER_DESC dpbd;

  // Error checking
  if(m_pDPClient == NULL)
    return FALSE;

  // Build a data structure
  dpbd.dwBufferSize = Size;
  dpbd.pBufferData  = (BYTE*)Data;

  m_pDPClient->Send(&dpbd, 1, 0, NULL, &hAsync, Flags);

  return TRUE;
}

BOOL cNetworkClient::SendText(char *Text, unsigned long Flags)
{
  DPNHANDLE       hAsync;
  DPN_BUFFER_DESC dpbd;

  // Error checking
  if(m_pDPClient == NULL)
    return FALSE;

  // Build a data structure
  dpbd.dwBufferSize = strlen(Text)+1;
  dpbd.pBufferData  = (BYTE*)Text;

  m_pDPClient->Send(&dpbd, 1, 0, NULL, &hAsync, Flags);

  return TRUE;
}

BOOL cNetworkClient::GetIP(char *IPAddress)
{
  WSADATA  WSAData;
  HOSTENT *HostEnt;
  char     LocalName[MAX_PATH], *LocalIP;
  in_addr *LocalIPAddr;

  if(WSAStartup(MAKEWORD(1,0), &WSAData))
    return FALSE;
  gethostname(LocalName, MAX_PATH);
  if((HostEnt = gethostbyname(LocalName)) == NULL) {
    WSACleanup();
    return FALSE;
  }
  LocalIPAddr = (in_addr*)HostEnt->h_addr_list[0];
  LocalIP = inet_ntoa(*LocalIPAddr);
  WSACleanup();

  if(LocalIP == NULL)
    return FALSE;

  strcpy(IPAddress, LocalIP);

  return TRUE;
}

long cNetworkClient::GetPort()
{
  return m_Port;
}

BOOL cNetworkClient::GetName(char *Name)
{
  if(Name == NULL)
    return FALSE;
  strcpy(Name, m_Name);
  return TRUE;
}

BOOL cNetworkClient::GetSessionName(char *Buf)
{
  if(Buf == NULL)
    return FALSE;
  strcpy(Buf, m_SessionName);
  return TRUE;
}

BOOL cNetworkClient::GetSessionPassword(char *Buf)
{
  if(Buf == NULL)
    return FALSE;
  strcpy(Buf, m_SessionPassword);
  return TRUE;
}

HRESULT WINAPI cNetworkClient::NetworkMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
  cNetworkClient *Network;
  DPNMSG_CONNECT_COMPLETE *pCCMsg;

  if((Network = (cNetworkClient*)pvUserContext) == NULL)
    return E_FAIL;

  switch(dwMessageId) {
    case DPN_MSGID_ADD_PLAYER_TO_GROUP:
      if(Network->AddPlayerToGroup((DPNMSG_ADD_PLAYER_TO_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_ASYNC_OP_COMPLETE:
      if(Network->AsyncOpComplete((DPNMSG_ASYNC_OP_COMPLETE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CLIENT_INFO:
      if(Network->ClientInfo((DPNMSG_CLIENT_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CONNECT_COMPLETE:
      // Update connection information
      pCCMsg = (DPNMSG_CONNECT_COMPLETE*)pMsgBuffer;
      if(pCCMsg->hResultCode == S_OK)
        Network->m_Connected = TRUE;

      if(Network->ConnectComplete((DPNMSG_CONNECT_COMPLETE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CREATE_GROUP:
      if(Network->CreateGroup((DPNMSG_CREATE_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_CREATE_PLAYER:
      if(Network->CreatePlayer((DPNMSG_CREATE_PLAYER *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_DESTROY_GROUP:
      if(Network->DestroyGroup((DPNMSG_DESTROY_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_DESTROY_PLAYER:
      if(Network->DestroyPlayer((DPNMSG_DESTROY_PLAYER *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_ENUM_HOSTS_QUERY:
      if(Network->EnumHostsQuery((DPNMSG_ENUM_HOSTS_QUERY *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_ENUM_HOSTS_RESPONSE:
      if(Network->EnumHostsResponse((DPNMSG_ENUM_HOSTS_RESPONSE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_GROUP_INFO:
      if(Network->GroupInfo((DPNMSG_GROUP_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_HOST_MIGRATE:
      if(Network->HostMigrate((DPNMSG_HOST_MIGRATE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_INDICATE_CONNECT:
      if(Network->IndicateConnect((DPNMSG_INDICATE_CONNECT *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_INDICATED_CONNECT_ABORTED:
      if(Network->IndicatedConnectAborted((DPNMSG_INDICATED_CONNECT_ABORTED *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_PEER_INFO:
      if(Network->PeerInfo((DPNMSG_PEER_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_RECEIVE:
      if(Network->Receive((DPNMSG_RECEIVE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP:
      if(Network->RemovePlayerFromGroup((DPNMSG_REMOVE_PLAYER_FROM_GROUP *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_RETURN_BUFFER:
      if(Network->ReturnBuffer((DPNMSG_RETURN_BUFFER *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_SEND_COMPLETE:
      if(Network->SendComplete((DPNMSG_SEND_COMPLETE *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_SERVER_INFO:
      if(Network->ServerInfo((DPNMSG_SERVER_INFO *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
    case DPN_MSGID_TERMINATE_SESSION:
      Network->m_Connected = FALSE;
      if(Network->TerminateSession((DPNMSG_TERMINATE_SESSION *)pMsgBuffer) == TRUE)
        return S_OK;
      break;
  }

  return E_FAIL;
}

/**************************************************
WinMain.cpp
GameCore Component

Programming Role-Playing Games with DirectX
by Jim Adams (01 Jan 2002)
**************************************************/

#ifndef _CORE_NETWORK_H_
#define _CORE_NETWORK_H_

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }

// Function to change to new GUID
BOOL SetApplicationGUID(GUID *ApplicationGUID);

class cNetworkAdapter;
class cNetworkServer;
class cNetworkClient;

class cNetworkAdapter
{
  protected:
    DPN_SERVICE_PROVIDER_INFO *m_AdapterList;
    unsigned long m_NumAdapters;

    static HRESULT WINAPI NetMsgHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer);

  public:
    cNetworkAdapter();
    ~cNetworkAdapter();

    BOOL  Init();
    BOOL  Shutdown();
    long  GetNumAdapters();
    BOOL  GetName(unsigned long Num, char *Buf);
    GUID *GetGUID(unsigned long Num);
};

class cNetworkServer
{
  protected:
    IDirectPlay8Server *m_pDPServer;

    BOOL                m_Connected;

    long                m_Port;

    char                m_SessionName[MAX_PATH];
    char                m_SessionPassword[MAX_PATH];

    long                m_MaxPlayers;
    long                m_NumPlayers;

    static HRESULT WINAPI NetworkMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer);

    virtual BOOL AddPlayerToGroup(DPNMSG_ADD_PLAYER_TO_GROUP *Msg) { return TRUE; }
    virtual BOOL AsyncOpComplete(DPNMSG_ASYNC_OP_COMPLETE *Msg) { return TRUE; }
    virtual BOOL ClientInfo(DPNMSG_CLIENT_INFO *Msg) { return TRUE; }
    virtual BOOL ConnectComplete(DPNMSG_CONNECT_COMPLETE *Msg) { return TRUE; }
    virtual BOOL CreateGroup(DPNMSG_CREATE_GROUP *Msg) { return TRUE; }
    virtual BOOL CreatePlayer(DPNMSG_CREATE_PLAYER *Msg) { return TRUE; }
    virtual BOOL DestroyGroup(DPNMSG_DESTROY_GROUP *Msg) { return TRUE; }
    virtual BOOL DestroyPlayer(DPNMSG_DESTROY_PLAYER *Msg) { return TRUE; }
    virtual BOOL EnumHostsQuery(DPNMSG_ENUM_HOSTS_QUERY *Msg) { return TRUE; }
    virtual BOOL EnumHostsResponse(DPNMSG_ENUM_HOSTS_RESPONSE *Msg) { return TRUE; }
    virtual BOOL GroupInfo(DPNMSG_GROUP_INFO *Msg) { return TRUE; }
    virtual BOOL HostMigrate(DPNMSG_HOST_MIGRATE *Msg) { return TRUE; }
    virtual BOOL IndicateConnect(DPNMSG_INDICATE_CONNECT *Msg) { return TRUE; }
    virtual BOOL IndicatedConnectAborted(DPNMSG_INDICATED_CONNECT_ABORTED *Msg) { return TRUE; }
    virtual BOOL PeerInfo(DPNMSG_PEER_INFO *Msg) { return TRUE; }
    virtual BOOL Receive(DPNMSG_RECEIVE *Msg) { return TRUE; }
    virtual BOOL RemovePlayerFromGroup(DPNMSG_REMOVE_PLAYER_FROM_GROUP *Msg) { return TRUE; }
    virtual BOOL ReturnBuffer(DPNMSG_RETURN_BUFFER *Msg) { return TRUE; }
    virtual BOOL SendComplete(DPNMSG_SEND_COMPLETE *Msg) { return TRUE; }
    virtual BOOL ServerInfo(DPNMSG_SERVER_INFO *Msg) { return TRUE; }
    virtual BOOL TerminateSession(DPNMSG_TERMINATE_SESSION *Msg) { return TRUE; }

  public:
    cNetworkServer();
    ~cNetworkServer();

    IDirectPlay8Server *GetServerCOM();

    BOOL Init();
    BOOL Shutdown();

    BOOL Host(GUID *guidAdapter, long Port, char *SessionName, char *Password = NULL, long MaxPlayers = 0);
    BOOL Disconnect();
    BOOL IsConnected();

    BOOL Send(DPNID dpnidPlayer, void *Data, unsigned long Size, unsigned long Flags = 0);
    BOOL SendText(DPNID dpnidPlayer, char *Text, unsigned long Flags = 0);

    BOOL DisconnectPlayer(long PlayerId);

    BOOL GetIP(char *IPAddress, unsigned long PlayerId = 0);
    BOOL GetName(char *Name, unsigned long PlayerId);
    long GetPort();
    BOOL GetSessionName(char *Buf);
    BOOL GetSessionPassword(char *Buf);
    long GetMaxPlayers();
    long GetNumPlayers();
};

class cNetworkClient
{
  protected:
    IDirectPlay8Client *m_pDPClient;

    BOOL                m_Connected;

    char                m_IPAddress[MAX_PATH];
    long                m_Port;

    char                m_Name[MAX_PATH];
    
    char                m_SessionName[MAX_PATH];
    char                m_SessionPassword[MAX_PATH];


    static HRESULT WINAPI NetworkMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer);

    virtual BOOL AddPlayerToGroup(DPNMSG_ADD_PLAYER_TO_GROUP *Msg) { return TRUE; }
    virtual BOOL AsyncOpComplete(DPNMSG_ASYNC_OP_COMPLETE *Msg) { return TRUE; }
    virtual BOOL ClientInfo(DPNMSG_CLIENT_INFO *Msg) { return TRUE; }
    virtual BOOL ConnectComplete(DPNMSG_CONNECT_COMPLETE *Msg) { return TRUE; }
    virtual BOOL CreateGroup(DPNMSG_CREATE_GROUP *Msg) { return TRUE; }
    virtual BOOL CreatePlayer(DPNMSG_CREATE_PLAYER *Msg) { return TRUE; }
    virtual BOOL DestroyGroup(DPNMSG_DESTROY_GROUP *Msg) { return TRUE; }
    virtual BOOL DestroyPlayer(DPNMSG_DESTROY_PLAYER *Msg) { return TRUE; }
    virtual BOOL EnumHostsQuery(DPNMSG_ENUM_HOSTS_QUERY *Msg) { return TRUE; }
    virtual BOOL EnumHostsResponse(DPNMSG_ENUM_HOSTS_RESPONSE *Msg) { return TRUE; }
    virtual BOOL GroupInfo(DPNMSG_GROUP_INFO *Msg) { return TRUE; }
    virtual BOOL HostMigrate(DPNMSG_HOST_MIGRATE *Msg) { return TRUE; }
    virtual BOOL IndicateConnect(DPNMSG_INDICATE_CONNECT *Msg) { return TRUE; }
    virtual BOOL IndicatedConnectAborted(DPNMSG_INDICATED_CONNECT_ABORTED *Msg) { return TRUE; }
    virtual BOOL PeerInfo(DPNMSG_PEER_INFO *Msg) { return TRUE; }
    virtual BOOL Receive(DPNMSG_RECEIVE *Msg) { return TRUE; }
    virtual BOOL RemovePlayerFromGroup(DPNMSG_REMOVE_PLAYER_FROM_GROUP *Msg) { return TRUE; }
    virtual BOOL ReturnBuffer(DPNMSG_RETURN_BUFFER *Msg) { return TRUE; }
    virtual BOOL SendComplete(DPNMSG_SEND_COMPLETE *Msg) { return TRUE; }
    virtual BOOL ServerInfo(DPNMSG_SERVER_INFO *Msg) { return TRUE; }
    virtual BOOL TerminateSession(DPNMSG_TERMINATE_SESSION *Msg) { return TRUE; }

  public:
    cNetworkClient();
    ~cNetworkClient();

    IDirectPlay8Client *GetClientCOM();

    BOOL Init();
    BOOL Shutdown();

    BOOL Connect(GUID *guidAdapter, char *IP, long Port, char *PlayerName, char *SessionName, char *SessionPassword = NULL);
    BOOL Disconnect();
    BOOL IsConnected();

    BOOL Send(void *Data, unsigned long Size, unsigned long Flags = 0);
    BOOL SendText(char *Text, unsigned long Flags = 0);

    BOOL GetIP(char *IPAddress);
    long GetPort();
    BOOL GetName(char *Name);
    BOOL GetSessionName(char *Buf);
    BOOL GetSessionPassword(char *Buf);
};

#endif

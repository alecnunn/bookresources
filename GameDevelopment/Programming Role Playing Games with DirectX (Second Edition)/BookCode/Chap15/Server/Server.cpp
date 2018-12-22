#include <windows.h>
#include <stdio.h>

#include "Core_Global.h"
#include "Server.h"

BOOL cServer::CreatePlayer(DPNMSG_CREATE_PLAYER *Msg)
{
  return TRUE;
}

BOOL cServer::DestroyPlayer(DPNMSG_DESTROY_PLAYER *Msg)
{
  return TRUE;
}

BOOL cServer::Receive(DPNMSG_RECEIVE *Msg)
{
  return TRUE;
}

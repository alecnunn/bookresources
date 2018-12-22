# gameserverdemo.py
#
# Main driver program for a demo game server created
# for the purpose of demonstrating the programming principles
# illustrated in the articles,
#   Precise Game Event Broadcasting with Python
#   Creating a "Safe Sandbox" for Game Scripting
#
# In the book, "Massively Multiplayer Game Development".
#
# Implements server initialization and the main server loop.
# 
# Author: Matthew Walker
#         mwalker@softhome.net
#

import gameserver.gameserver
import gameserver.deferred
import gameserver.objmgr
import gameserver.gameobs


def InitGame():
  """
  Perform any necessary startup functions.
  """
  # init the safe sandbox classes
  import gameserver.safesandbox
  sandboxclasses = []
  sandboxclasses.append(('gameserver.sandboxmodules.thetrap', 'TheTrap'))
  sandboxclasses.append(('gameserver.sandboxmodules.battle', 'Battle'))
  sandboxclasses.append(('gameserver.sandboxmodules.town', 'Town'))

  gameserver.safesandbox._InitSandboxClasses(sandboxclasses)

  # create the "world" areas; i.e. the sandboxes
  gameserver.gameobs.CreateRoom('TheTrap')
  gameserver.gameobs.CreateRoom('Battle')
  gameserver.gameobs.CreateRoom('Town')


def ProcessRequests():
  """
  Handle all waiting calls whose time has come.
  Return the time until our next iteration.
  """
  waitTime = None
  try:
    waitTime = gameserver.deferred.ExecuteDeferredCalls()
  except:
    # anything can happen here
    import traceback
    traceback.print_exc()
  return waitTime


def GameLoop (timeout=1.0, use_poll=0, map=None):
  """
  Main server loop for asynchronous processing.
  Steal the functionality straight from asyncore and
  modify it for our own purposes.
  This function handles both client requests and 
  deferred calls.
  """
  import asyncore
  if map is None:
    map=asyncore.socket_map

  # pick the asyncore polling function to use
  if use_poll:
    if hasattr (select, 'poll'):
      WaitForRequests = asyncore.poll3
    else:
      WaitForRequests = asyncore.poll2
  else:
    WaitForRequests = asyncore.poll

  # loop forever, or until we are done
  waitTime = timeout
  while map:
    # receive client requests via asyncore
    WaitForRequests(waitTime, map)

    # fire waiting deferred calls
    waitTime = ProcessRequests()

    if not waitTime:
      waitTime = timeout


#
# command-line interface:
# usage: python gameserverdemo.py <port>
# 
if __name__ == '__main__':
  import sys
  import string
  if len(sys.argv) < 2:
    print 'Usage: %s <server-port>' % sys.argv[0]
  else:
    game = gameserver.gameserver.GameServer(string.atoi (sys.argv[1]))
    InitGame()
    GameLoop()



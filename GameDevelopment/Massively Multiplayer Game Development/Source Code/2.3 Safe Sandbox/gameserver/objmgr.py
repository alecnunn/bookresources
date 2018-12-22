# objmgr.py
#
# Implements the ability to address objects by an 
# id, avoiding the need to hold on to object references.
# This prevents reference counting and garbage collection
# problems associated with circular references.
# 
# The deferred call implementation uses object ids to
# identify the recipient of a deferred method call.
#
# Author: Matthew Walker
#         mwalker@softhome.net
#

import string
import types

def __ModuleInit__():
  """
  Set up module-level attributes
  """
  global g_gameobjects
  global g_ids
  global g_players_clients
  global g_clients_players
  global g_rooms

  g_gameobjects = {}
  g_ids = 0
  g_players_clients = {}
  g_clients_players = {}
  g_rooms = {}

__ModuleInit__()  # at first import



#
# Module functions - for managing all objects in the game
#
def NextId():
  """
  Generate a new object id.
  """
  global g_ids
  g_ids = g_ids + 1
  return g_ids


def AddObject(ob, id):
  """
  Add a game object to the manager using the id provided.
  """
  assert(type(id) == types.IntType)
  g_gameobjects[id] = ob


def RemoveObject(ob):
  """
  Remove an object from the game, None if it fails.
  """
  try:
    del g_gameobjects[ob.id]
    return ob.id
  except KeyError:
    print 'Cannot remove %s: not in map.' % (ob,)
    return None
 

def GetObject(id):
  """
  Return the object with the requested id, or None 
  if it doesn't exist.
  """
  try:
    return g_gameobjects[id]
  except KeyError:
    import traceback
    traceback.print_exc()
    print 'Invalid object id: %s' % (id,)
    return None


def RegisterPlayer(clientid, playerid):
  """
  Associate a player with a specific client.
  """
  if g_clients_players.get(clientid) or g_players_clients.get(playerid):
    raise RuntimeError("Duplicate player registration.")
  g_clients_players[clientid] = playerid
  g_players_clients[playerid] = clientid


def UnregisterPlayer(clientid, playerid):
  """
  Dis-associate a player with a specific client.
  """
  del g_players_clients[playerid]
  del g_clients_players[clientid]


def GetPlayerByClientId(clientid):
  """
  Get a player object based on a client id.
  """
  playerid = g_clients_players[clientid]
  player = GetObject(playerid)
  return player


def GetPlayerByName(name):
  """
  Get a player object based on its name.
  NOTE: there is no enforcement of unique names.
  """
  for playerid in g_players_clients.keys():
    player = GetObject(playerid)
    if player.name == name:
      return player
  return None


def GetObjectByName(name):
  """
  Get a generic object based on its name. Player
  objects are also included.
  NOTE: there is no enforcement of unique names.
  """
  for objectid in g_gameobjects.keys():
    object = GetObject(objectid)
    try:
      if object.name == name:
        return object
    except:
      # some objects don't have a name - that's okay
      pass
  return None


def RegisterRoom(room):
  """
  Provide the ability to identify a room by its name.
  """
  if g_rooms.get(string.upper(room.name)):
    raise RuntimeError('Registering duplicate room %s' % (room.name,))
  g_rooms[string.upper(room.name)] = room.id


def GetRoomIds():
  """
  Return a list of all room ids.
  """
  return g_rooms.values()


def GetRoomByName(roomname):
  """
  Get a room (sandbox) object based on its name.
  NOTE: there is no enforcement of unique names.
  """
  roomid = g_rooms[string.upper(roomname)]
  room = GetObject(roomid)
  return room



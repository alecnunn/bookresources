# request.py
#
# Implements a high level request dispatcher that 
# validates user input and maps recognized command 
# strings to functions that invoke game behavior.
#
# Author: Matthew Walker
#         mwalker@softhome.net
#


import objmgr
import gameobs
import string
import re
import os
import deferred
import gameserver


# special object id for request handler
ID_REQUEST = -200


class Request:
  """
  Responsible for handling requests from clients and
  invoking the appropriate action in response. Treat
  this as a singleton: create only one.
  """
  sepRE = re.compile(r"\s+")
  global commands
  commands = {}
  global argusage
  argusage = {}

  def __init__(self):
    global __singleton__
    __singleton__ = self
    self.id = ID_REQUEST  # enables addressing by deferred calls
    objmgr.AddObject(self, self.id)

  def Handle(self, clientid, request):
    """
    Decipher a request into a command and arguments.

    clientid - id of the client making the request
    request - request string to be parsed
    """
    # remove leading and trailing whitespace
    request = string.strip(request) 

    # first token is command; remainder is argument string
    try:
      actionlist = self.sepRE.split(request, 1)
    except ValueError:
      print 'Invalid request: %s' % (request,)
      return

    # parse argstring into arg list
    try:
      action, argstr = actionlist
    except ValueError:
      action = actionlist[0]
      argstr = ''

    args = self.sepRE.split(argstr)

    # convert to tuple
    if args == ['']:
      args = () # no args
    else:
      args = tuple(args)  

    retval = _ExecuteCommand(action, clientid, args)
  
    # notify client of response via deferred call to server
    deferred.Call(gameserver.ID_SERVER, 'respond', (retval,))


def _ExecuteCommand(action, clientid, args):
  """ Dispatch command to appropriate action method. """
  action = string.upper(action)
  try:
    retval = apply(commands[action], (clientid,) + args)
  except KeyError:
    import traceback
    traceback.print_exc()
    retval = Response(clientid, 'Invalid command: ' + repr(action) + os.linesep + \
                      'Type \'help\' for more information.')
  except TypeError:
    import traceback
    traceback.print_exc()
    retval = Response(clientid, 'Usage: %s %s' % (action, argusage[action]))

  return retval



#
# Global command functions, entered into a dictionary
# for easy dispatching without the use of a large
# if .. elif .. else block. The commands dictionary
# maps the command keyword to the function reference,
# and the argusage dictionary maps the command keyword
# to a string that describes the required argument for
# the command (if any).
# 
# The docstrings (first comment after function declaration)
# are written for display purposes - type 'help' in a 
# client to see them in action.
#

def ListCommands(clientid):
  """ Show the list of available commands.  """
  commandstr = ''
  commandNames = commands.keys()
  commandNames.sort() # alpha sort
  for command in commandNames:
    if command[0] == '_':  # private command; not for players
      continue
    func = commands[command]
    args = argusage[command]
    commandstr += '%#-20s- %s%s' % (command + ' ' + args, func.__doc__, os.linesep)

  return Response(clientid, 'Command List: ' + os.linesep + commandstr)
commands['HELP'] = ListCommands
argusage['HELP'] = ''
    

def Logout(clientid):
  """ Remove the Player from the game.  """
  player = objmgr.GetPlayerByClientId(clientid)
  objmgr.UnregisterPlayer(clientid, player.id)
  objmgr.RemoveObject(player)

  deferred.Call(gameserver.ID_SERVER, 'pre_close_client', (clientid,))

  return Response(clientid, 'Goodbye, %s. Hope you had fun!' % (player.name,))
commands['LOGOUT'] = Logout
argusage['LOGOUT'] = ''


def EnterRoom(clientid, roomname):
  """ Enter the room identified by name. """
  try:
    room = objmgr.GetRoomByName(roomname)
  except KeyError:
    return Response(clientid, 'Invalid room name: %s' % (roomname,))

  player = objmgr.GetPlayerByClientId(clientid)
  room.EnterRoom(player)

  return Response(clientid, 'You are now in %s' % (room.name,))
commands['ENTER'] = EnterRoom
argusage['ENTER'] = '<roomname>'


def ExitRoom(clientid):
  """ Leave the current room. """
  player = objmgr.GetPlayerByClientId(clientid)
  room = objmgr.GetObject(player.currentRoom)
  if room:
    room.ExitRoom(player)
    return Response(clientid, 'You have left %s' % (room.name,))
  else:
    return Response(clientid, 'You are not in a room.')
commands['EXIT'] = ExitRoom
argusage['EXIT'] = ''


def ListRooms(clientid):
  """ Return a list of available rooms.  """
  roomids = objmgr.GetRoomIds()
  roomstr = ''
  for roomid in roomids:
    room = objmgr.GetObject(roomid)
    roomstr += '%-10s- %s%s' % (room.name, room.sandbox.__doc__, os.linesep)

  return Response(clientid, 'Avaliable Rooms:' + os.linesep + roomstr)
commands['ROOMS'] = ListRooms
argusage['ROOMS'] = ''


def Wave(clientid, targetname):
  """ Wave at a player."""
  target = objmgr.GetPlayerByName(targetname)
  if target is None:
    return Response(clientid, '%s is not in the game.%s' % (targetname, os.linesep))
    
  player = objmgr.GetPlayerByClientId(clientid)
  player._Wave(target.id)

  return Response(clientid, 'You waved at %s.%s' % (targetname, os.linesep))
commands['WAVE'] = Wave
argusage['WAVE'] = '<targetname>'


def Attack(clientid, targetname):
  """ Attack a player."""
  target = objmgr.GetPlayerByName(targetname)
  if target is None:
    return Response(clientid, '%s is not in the game.%s' % (targetname, os.linesep))
    
  player = objmgr.GetPlayerByClientId(clientid)
  player._Attack(target.id)

  return Response(clientid, 'You attacked %s.%s' % (targetname, os.linesep))
commands['ATTACK'] = Attack
argusage['ATTACK'] = '<targetname>'


def Take(clientid, itemname):
  """ Take an item identified by its name. """
  item = objmgr.GetObjectByName(itemname)
  if item is None:
    return Response(clientid, '%s is not in the game.%s' % (itemname, os.linesep))
    
  player = objmgr.GetPlayerByClientId(clientid)
  if not player._AcquireItem(item):
    return Response(clientid, '%s is not in this room.%s' % (itemname, os.linesep))

  return Response(clientid, 'You took %s.%s' % (itemname, os.linesep))
commands['TAKE'] = Take
argusage['TAKE'] = '<itemname>'


def Put(clientid, itemname):
  """ Put an item identified by its name. """
  item = objmgr.GetObjectByName(itemname)
  if item is None:
    return Response(clientid, '%s is not in the game.%s' % (itemname, os.linesep))
    
  player = objmgr.GetPlayerByClientId(clientid)
  if not player._ReleaseItem(item):
    return Response(clientid, 'You do not have %s.%s' % (itemname, os.linesep))

  return Response(clientid, 'You returned %s.%s' % (itemname, os.linesep))
commands['PUT'] = Put
argusage['PUT'] = '<itemname>'


def Equip(clientid, itemname):
  """ Arm yourself with the weapon identified by its name. """
  item = objmgr.GetObjectByName(itemname)
  if item is None:
    return Response(clientid, '%s is not in the game.%s' % (itemname, os.linesep))
    
  player = objmgr.GetPlayerByClientId(clientid)
  if not isinstance(item, gameobs.Weapon):
    return Response(clientid, '%s is not a weapon.%s' % (itemname, os.linesep))

  if not player._EquipWeapon(item):
    return Response(clientid, '%s is not in your inventory.%s' % (itemname, os.linesep))

  return Response(clientid, 'You equipped a %s.%s' % (itemname, os.linesep))
commands['EQUIP'] = Equip
argusage['EQUIP'] = '<itemname>'


def ShowInventory(clientid):
  """ Display your inventory. """
  player = objmgr.GetPlayerByClientId(clientid)
  itemstr = 'You have:' + os.linesep
  for itemid in player.inventory:
    item = objmgr.GetObject(itemid)
    itemstr += '%s%s' % (item.name, os.linesep)

  return Response(clientid, itemstr)
commands['INV'] = ShowInventory
argusage['INV'] = ''


#
# Private commands
#
def Login(clientid, name):
  """ Create a new Player for the given name.  """
  playerid = objmgr.NextId()
  player = gameobs.Player(name, playerid, clientid)  
  objmgr.AddObject(player, playerid)
  objmgr.RegisterPlayer(clientid, playerid)
  return Response(clientid, 'Welcome to the Game, %s. Type \'help\' for a list of commands.' % (name,))
commands['_LOGIN'] = Login
argusage['_LOGIN'] = '<playername>'


#
# Utility functions
#

def ClientMessage(playerid, msg):
  """
  Sends a message to the player associates with the 
  clientid using a deferred call.
  """
  player = objmgr.GetObject(playerid)
  response = Response(player.clientid, msg)
  deferred.Call(gameserver.ID_SERVER, 'respond', (response,))


#
# Utility class
#
class Response:
  """
  Simple structure for returing information to 
  a client.
  """
  def __init__(self, clientid, msg):
    self.clientid = clientid
    self.msg = msg

  def __str__(self):
    return os.linesep + self.msg + os.linesep


def __ModuleInit__():
  """
  Create a single instance of the handler, automatically.
  """
  Request()   # assigns self to __singleton__

__ModuleInit__()  # called at import time



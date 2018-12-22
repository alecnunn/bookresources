# gameobs.py
#
# Implements various game objects used by the demo
# server. Most important are the Player and Room
# classes. All game objects derive from GameObject,
# which has an id that is used for addressing 
# deferred method calls.
# 
# Author: Matthew Walker
#         mwalker@softhome.net
#

import safesandbox
import objmgr
import eventbroadcaster
import gameevents
import gameeventkeys
import request

#
# Basic game objects
#
class GameObject:
  """
  Base class for all game objects. Has object id
  for tracking by the objmgr module.
  """
  def __init__(self, id):
    self.id = id


class Player(GameObject):
  """
  Represents a player in the game. This class is intended
  to be used both outside and inside a SafeSandbox. When
  used within a sandbox, it is protected by a Bastion
  wrapper class, which prevents direct access to attributes
  and methods that are prefixed by an underscore ('_') 
  character.
  """
  def __init__(self, name, id, clientid):
    GameObject.__init__(self, id)
    self.name = name
    self.currentRoom = None
    self.clientid = clientid
    self.health = 100
    self.weaponid = None
    self.inventory = []

  def GetName(self):
    """
    Provide access to the name from within a Bastion-protected instance.
    """
    return self.name

  def GetHealth(self):
    """
    Provide access to health from within a Bastion-protected instance.
    """
    return self.health

  def _AddToInventory(self, item):
    """
    Protected.
    Put an item into inventory.
    """
    self.inventory.append(item.id)

  def _RemoveFromInventory(self, item):
    """
    Protected.
    Remove an item into inventory.
    """
    self.inventory.remove(item.id)

  def _AcquireItem(self, item):
    """
    Protected.
    Take ownership of an item.
    """
    if self.currentRoom:
      room = objmgr.GetObject(self.currentRoom)
      if item.id in room.itemids:
        room.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.ACTION_TAKE_OBJECT, self.id, item.id), 0.0)
        return 1
      else:
        return 0

  def _ReleaseItem(self, item):
    """
    Protected.
    Release ownership of an item.
    """
    if self.currentRoom:
      room = objmgr.GetObject(self.currentRoom)
      if item.id in self.inventory:
        room.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.ACTION_PLACE_OBJECT, self.id, item.id), 0.0)
        return 1
      else:
        return 0

  def _OnEnterRoom(self, room):
    """
    Protected.
    Called by the room we enter; register handlers.
    """
    self.currentRoom = room.id
    room.eventManager.RegisterHandler(gameeventkeys.ByEventAndSubject(gameevents.ACTION_WAVE, self.id), self._OnWaveAtMe)
    room.eventManager.RegisterHandler(gameeventkeys.ByEventAndSubject(gameevents.ACTION_ATTACK_SUCCESS, self.id), self._OnAttackMe)
    room.eventManager.RegisterHandler(gameeventkeys.ByEventAndSubject(gameevents.PLAYER_EXITS_ROOM, self.id), self._OnExitRoom2)

  def _OnExitRoom(self, room):
    """
    Protected.
    Called by the room we exit; unregister handlers.
    """
    self.currentRoom = None
    room.eventManager.UnregisterHandler(gameeventkeys.ByEventAndSubject(gameevents.ACTION_WAVE, self.id), self._OnWaveAtMe)
    room.eventManager.UnregisterHandler(gameeventkeys.ByEventAndSubject(gameevents.ACTION_ATTACK_SUCCESS, self.id), self._OnAttackMe)
    room.eventManager.UnregisterHandler(gameeventkeys.ByEventAndSubject(gameevents.PLAYER_EXITS_ROOM, self.id), self._OnExitRoom2)

  def _OnExitRoom2(self, key):
    """
    Event handler for when I leave a room.
    """
    self.health = 100  # regain health

  def _EquipWeapon(self, weapon):
    """
    Protected.
    Arm ourself.
    """
    if weapon.id in self.inventory:
      self.weaponid = weapon.id
      return 1
    else:
      return 0

  def _GetWeapon(self):
    """
    Protected.
    Access our weapon.
    """
    return objmgr.GetObject(self.weaponid)

  def _Wave(self, targetPlayerId):
    """
    Protected.
    Wave at the targetPlayer.
    """
    if self.currentRoom:
      room = objmgr.GetObject(self.currentRoom)
      room.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.ACTION_WAVE, self.id, targetPlayerId), 0.0)
   
  def _OnWaveAtMe(self, key):
    """
    Protected.
    Event handler activated when someone waves at me.
    """
    srcname = objmgr.GetObject(key.src).name
    request.ClientMessage(self.id, '%s just waved at you.' % (srcname,))

  def _Attack(self, targetPlayerId):
    """
    Protected.
    Attack the target player with your weapon.
    """
    if self.currentRoom:
      room = objmgr.GetObject(self.currentRoom)
      weapon = self._GetWeapon() 
      if not weapon:
        request.ClientMessage(self.id, 'You have no weapon equipped.')
        return

      targetPlayer = objmgr.GetObject(targetPlayerId)
      if targetPlayer.currentRoom != self.currentRoom:
        request.ClientMessage(self.id, 'You are not in the same room as %s' % (targetPlayer.name))
        return
       
      print 'here'
      # compute success
      import random
      if random.random() > weapon.tohit:
        # compute damage
        damage = random.randint(weapon.mindamage, weapon.maxdamage)
        # notify whoever cares; note that damage is an arg
        room.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.ACTION_ATTACK_SUCCESS, self.id, targetPlayerId), 0.0, damage)
        request.ClientMessage(self.id, 'You attacked %s for %s damage.' % (targetPlayer.name, damage))
      else:
        room.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.ACTION_ATTACK_FAILURE, self.id, targetPlayerId), 0.0)
        request.ClientMessage(self.id, 'You missed.')

  def _OnAttackMe(self, key, damage):
    """
    Protected.
    Event handler activated when someone attacks me.
    """
    srcname = objmgr.GetObject(key.src).name
    self.health -= damage
    if self.health <= 0:
      request.ClientMessage(self.id, 'You were killed by %s.' % (srcname,))
      room = objmgr.GetObject(self.currentRoom)
      room.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.PLAYER_DIES, self.id), 0.0)
      room.ExitRoom(self)
    else: 
      request.ClientMessage(self.id, '%s just attacked you for %s.' % (srcname, damage))




class Item(GameObject):
  """
  Represents an item you can touch.
  This class is protected by the Bastion when inside
  a SafeSandbox.
  """
  def __init__(self, name, id):
    GameObject.__init__(self, id)
    self.name = name
    self.currentRoom = None
    self.ownerid = None

  def _AssignOwner(self, ownerid):
    self.ownerid = ownerid

  def GetName(self):
    # provide access to the name from within a Bastion-protected instance.
    return self.name


class Weapon(Item):
  """
  You can hurt somebody with this.
  """
  def __init__(self, name, id):
    Item.__init__(self, name, id)
    self.mindamage = 5
    self.maxdamage = 20
    self.tohit = 0.50
    self.range = 2


#
# Environment / world game object
#
class Room(GameObject):
  """
  Identifies a region in the game in which players
  can interact with each other and the 'world'.
  Has a SafeSandbox which prevents game errant code from
  disrupting the integrity of the server.
  """

  def __init__(self, name, id):
    GameObject.__init__(self, id)

    # for tracking items and players in this room
    self.playerids = []
    self.itemids = []

    # add event broadcaster to global object mananger so it can receive deferred calls
    eventManagerId = objmgr.NextId()
    self.eventManager = eventbroadcaster.EventBroadcaster(eventManagerId)
    objmgr.AddObject(self.eventManager, eventManagerId) 

    # our safe sandbox, where game code runs
    self.sandbox = safesandbox.CreateSafeSandbox(self, self.eventManager, name)
    self.name = self.sandbox.__class__.__name__


  def EnterRoom(self, player):
    if not player.id in self.playerids:
      self.playerids.append(player.id)
      player._OnEnterRoom(self)

      # post a game event to notify whoever cares; spoof the sandbox as the source,
      # because restricted code may handle this event.
      self.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.PLAYER_ENTERS_ROOM, self.sandbox, player.id), 0.0)
    else: 
      raise RuntimeError('Player %s is already in the room', (playerid,))


  def ExitRoom(self, player):
    if not player.id in self.playerids:
      raise RuntimeError('Player %s is not in the room', (playerid,))
    else: 
      self.playerids.remove(player.id)
      player._OnExitRoom(self)

      # post a game event to notify whoever cares; spoof the sandbox as the source,
      # because restricted code may handle this event.
      self.eventManager.PostGameEvent(gameeventkeys.GameEventKey(gameevents.PLAYER_EXITS_ROOM, self.sandbox, player.id), 0.0)


#
# Utility
#
def CreateRoom(roomname):
  """
  Factory function: Create an instance of the 
  room identified by the roomname. 
  """
  roomid = objmgr.NextId()
  room = Room(roomname, roomid)
  objmgr.AddObject(room, roomid)
  objmgr.RegisterRoom(room) 



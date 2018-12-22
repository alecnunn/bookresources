"""
psmserver.py - PSM Demo Server

A server to demonstrate the concept of Parallel-state machines
"""

import sys
import os
path, ignored = os.path.split(os.getcwd())
sys.path.append(path)
import asyncore
import socket
import string, stat
import marshal
import struct
import traceback
import exceptions
import errno
import actor
import shared
from shared.msgtypes import *
import shared.casting as _casting
import shared.fighting as _fighting
import shared.dead as _dead
import random

# globals
g_lastActorId = 100     # starting actorId
g_actorDict = {}        # client to actor instance mapping
g_port = 4701           # the port to listen on

g_programName = 'PSM Demo Server: '


"""
asynpsmlistener - listener class, handles incoming
connections from clients.  If a client connects,
creates a client class instance to handle any
subsequent interactions
"""
class asynpsmlistener(asyncore.dispatcher):
  def __init__(self, port):
    asyncore.dispatcher.__init__(self)
    self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
    self.bind(("", port))
    self.listen(20) # Allow up to 20 connections

  def handle_accept(self):
    client, addr = self.accept()
    print g_programName+'connection from ', addr
    return asyncpsmclient(client, addr)

"""
The client class, one allocated for each
client connection.  It handles the network
read/write activity to the client program.
"""
class asyncpsmclient(asyncore.dispatcher):
  def __init__(self, sock = None, addr = None):
    asyncore.dispatcher.__init__(self, sock)
    self.got_request = 0
    self.clientSocket = sock
    self.addr = addr

  def read_exactly(self, neededBytes):
    result = ""
    while neededBytes > 0:
      try:
        data = self.clientSocket.recv(neededBytes)
      except socket.error:
        errval, errStr = sys.exc_info()[1]
        if errval == errno.EWOULDBLOCK:
          continue
        else:
          raise exceptions.RuntimeError, "Problem with recv()"

      if len(data) == 0:
        # trouble -- connection was closed!
        print g_programName+'Connection closed with %s' % repr(self.addr)
        return None
      else:
        result      = result + data
        neededBytes = neededBytes - len(data)
    # end: while
    return result

    
  def handle_read(self):
    # Get the message header and dispatch based on the message type
    header = self.read_exactly(MSG_HDR_SIZE)
    if header is None:
      self.close()
      return

    (msgType,) = struct.unpack("<B", header)

    if msgType == MSG_TYPE_LOGIN:
        data = _LoginNewClient(self)
        self.send(data)
    elif msgType == MSG_TYPE_STATE_CHANGE_REQUEST:
        data = _ActorStateChange(self)
    else:
      print g_programName+'Unexpected msg type %s from %s' % (msgType, repr(self.addr))
      self.close()    

  def close(self):
    """
    Get rid of the client, actor entry when the
    connection is closed, let all remaining
    clients know that this actor has left.
    """
    global g_actorDict
    if g_actorDict.has_key(self):
      actor = g_actorDict[self]
      del g_actorDict[self]
      for currentClient, currentActor in g_actorDict.items():
        currentClient.send(struct.pack("<BB", MSG_TYPE_ACTOR_LEFT, actor.GetId()))
      actor = None
    asyncore.dispatcher.close(self)        
        
  def writeable(self):
    return 0

  def handle_write(self):
    pass


def _ParseStateName(state):
  """
  Return a more readable name from the module itself
  """
  return state.__name__[7:]


def _PrintActorList():
  """
  Prints the list of current actors and their states
  """
  print ''
  print 'Actor List'
  print '-----------------------------------------------------------------------------'
  for currentActor in g_actorDict.values():
    print 'Actor %s: Posture [%s] Movement [%s] Action [%s]' % (currentActor.GetId(),
                                                                _ParseStateName(currentActor.GetPostureState()),  \
                                                                _ParseStateName(currentActor.GetMovementState()), \
                                                                _ParseStateName(currentActor.GetActionState()))
  print '-----------------------------------------------------------------------------'

    
def _LoginNewClient(client):
  """
  A client logs in.  assign it an actor id and send
  that in a login acknowldegement.  Add a new actor
  message for each current actor for the new client,
  and tell all current clients about the new actor
  that logged in. store the new actor in a mapping
  from client to actor instance for future use.
  """
  global g_lastActorId
  global g_actorDict
  
  newActor = actor.Actor(g_lastActorId)
  g_lastActorId += 1
  data = struct.pack("<BB", MSG_TYPE_LOGIN_ACK, newActor.GetId())
  for priorClient, priorActor in g_actorDict.items():
    p, m, a = priorActor.GetAllStateIds()
    data += struct.pack("<BBBBB", MSG_TYPE_NEW_ACTOR, priorActor.GetId(), p, m, a)  # their current states
    p, m, a = newActor.GetAllStateIds()
    priorClient.send(struct.pack("<BBBBB", MSG_TYPE_NEW_ACTOR, newActor.GetId(), p, m, a)) # others about this new actor      
  g_actorDict[client] = newActor # store the new actor
  print g_programName+'new actor [%s] logged in and created' % newActor.GetId()
  _PrintActorList()
  return data


def _BroadcastStateChange(client, actor):
  """
  Broadcast the state change of the specfied actor to
  all clients except the one specfified, 
  """
  for otherClient in g_actorDict.keys():
    if client != otherClient:
      p, m, a = actor.GetAllStateIds()
      otherClient.send(struct.pack("<BBBBB", MSG_TYPE_STATE_CHANGE, actor.GetId(), p, m, a))


def _BroadcastFrozenToAll(actor):
  """
  Broadcast that an actor got frozen to all clients
  """
  for client in g_actorDict.keys():
    client.send(struct.pack("<BB", MSG_TYPE_ACTOR_FROZEN, actor.GetId()))


def _BroadcastDeathToAll(actor):
  """
  Broadcast that an actor died to all clients
  """
  for client in g_actorDict.keys():
    client.send(struct.pack("<BB", MSG_TYPE_ACTOR_DEAD, actor.GetId()))    


def _GetOtherActor(notThisActor):
  """
  return a random actor that isn't the one passed in
  """
  candidateList = []
  for actor in g_actorDict.values():
    if actor == notThisActor:
      pass
    else:
      candidateList.append(actor)

  if len(candidateList) == 1:
    actor = candidateList[0]
  else:
    index = random.randint(0, len(candidateList)-1)
    actor = candidateList[index]

  return actor  


def _HandleCasting(client, actor):
  """
  If there is someone to freeze, the freeze them and
  let everyone know about it.
  """
  print g_programName+'actor [%s] attempting to cast FREEZE spell...' % (actor.GetId())
  if len(g_actorDict) == 1:
    print g_programName+'no target found, the spell fizzles...'
    return

  otherActor = _GetOtherActor(actor)
  print g_programName+'the spell freezes actor [%s] and prevents their movement!' % (otherActor.GetId(),)
  # they can't move any more
  otherActor.ForceDefaultStates()
  otherActor.BlockMovement()
  # let everyone know
  _BroadcastFrozenToAll(otherActor) 
      

def _HandleFighting(client, actor):
  """
  If there is someone to fight, fight them to the death.
  Let everyone know who died.
  """
  print g_programName+'actor [%s] attempting to fight...' % (actor.GetId())
  if len(g_actorDict) == 1:
    print g_programName+'no other actor found! The air is harmlessly punched...'
    return
  
  deadActor = None
  otherActor = _GetOtherActor(actor)
  print g_programName+'and proceeds to attack actor [%s]' % (actor.GetId(),)
  print g_programName+'The fighting is fierce...'
  roll = random.randint(1,10)
  # arbitrarily, the attacker has the upper hand
  if roll < 7:
    deadActor = otherActor
    print g_programName+'and the opponent is overcome, actor [%s] dies' % (otherActor.GetId(),)
  else:
    deadActor = actor
    print g_programName+'and the opponent overcomes the attacker, actor [%s]' % (actor.GetId())

  # attacker is dead
  deadActor.TransitionTo(_dead)
  # NOTE: to force a correction to happen, you can comment
  # out this next line, and then try to do a state change
  # on the dead actor client.  You will see a correction,
  # and subsequent failure on state change request, since
  # the blocks were transmitted in the _SendStateCorrection
  # method and the dead actor client is synced up.
  _BroadcastDeathToAll(deadActor)      
      

def _SendStateCorrection(client, actor):
  """
  Sends the state correction back to the client,
  because they could not go into the state they
  requested and this syncs the client back to
  the server known state.  Note that here we
  are sending the server known blocked states
  for completeness in this demo.  Depending
  on how the character state manager is used
  and how clients know that they need to block
  transitions, this may not be necessary.  For
  example, within this source, when an actor
  dies, we simply broacast that fact.  The
  appropriate blocking occurs at the client,
  and the client and server remain in sync
  block wise.
  """
  p, m, a = actor.GetAllStateIds()
  data = struct.pack("<BBBB", MSG_TYPE_STATE_CORRECTION, p, m, a)
  blockedDict = actor.GetBlockedStateDict()
  data += struct.pack("<B", len(blockedDict)) # the number of entries
  for (state, blockCount) in blockedDict.items():
    data += struct.pack("<BB", state.GetStateId(), blockCount) # append each entry
  client.send(data)
  

def _ActorStateChange(client):
  """
  Handles an actor state change request.  If it is
  valid, broadcast the change to all other clients.
  if it is not, send a correction back to the
  originating client.
  """
  if g_actorDict.has_key(client):
    actor = g_actorDict[client] 
    data = client.read_exactly(1)
    if data is None:
      client.close()
      return
    (stateId,) = struct.unpack("B", data)
    state = shared._GetStateModuleById(stateId)
    print g_programName+'state change request from actor [%s] to state [%s]' % (actor.GetId(),_ParseStateName(state))
    if actor.RequestStateChange(state):
      print g_programName+'state change ok, broadcasting'
      _BroadcastStateChange(client, actor)
      if state == _casting:
        _HandleCasting(client, actor)
      elif state == _fighting:
        _HandleFighting(client, actor)
    else:
      print g_programName+'state change denied, sending correction'
      _SendStateCorrection(client, actor)
    _PrintActorList()
  else:
    print g_programName+'unable to retrieve client entry %s' % (client,)
    

def _SendShutdownMsg():
  """
  We got shutdown, let any active clients know
  """
  print g_programName+'sending shutdown messages'
  data = struct.pack("<B", MSG_TYPE_SHUTDOWN)
  for client in g_actorDict.keys():
    client.send(data)

def main():
  """
  main() - main program loop
  """  
  print g_programName+'starting...'
  
  # Parse the port argument if passed
  global g_port
  if len(sys.argv) > 1:
    value = int(sys.argv[1])
    if value > 0:
      g_port = value

  # Create the listener
  a = asynpsmlistener(g_port)
  print g_programName+'ready for connections on port [%s], press <ctrl-c> to exit' % (g_port,)
  
  # Poll forever
  try:
    asyncore.loop(1)
  except KeyboardInterrupt:
    # handle ctrl-c exit
    _SendShutdownMsg()
      
  print g_programName+'exited'
  del a

# run main()  
if __name__ == '__main__':
 main()  


    

    


"""
psmclient.py - PSM Demo Client

A client to demonstrate the concept of Parallel-state machines
"""

import sys
import os
path, ignored = os.path.split(os.getcwd())
sys.path.append(path)
import actor
import socket
import select
import threading
import struct
import string
import shared.characterstatemgr
import shared.moveforward as _moveforward
import shared.moveleft as _moveleft
import shared.movebackward as _movebackward
import shared.moveright as _moveright
import shared.stopped as _stopped
import shared.standing as _standing
import shared.sitting as _sitting
import shared.layingdown as _layingdown
import shared.dead as _dead
import shared.idle as _idle
import shared.casting as _casting
import shared.fighting as _fighting
import shared.crafting as _crafting
import shared.gesturing as _gesturing
from shared.msgtypes import *

# globals
g_actorId = None           # id assigned by server for this clients actor
g_actorDict = {}           # mapping between actorId and actor instance      
g_userInput = 0            # flag indicating whether user input occured
g_inputValue = None        # the actual input value
g_port = 4701              # default port the server listens on
g_serverName = 'localhost' # default server to connect to

g_programName = 'PSM Client: '
g_inputPrompt = g_programName+'states=[w,a,s,d,x] [c,v,b] [i,u,y,o,p], 9=status, h=help, 0=exit'


def _ParseStateName(state):
  """
  Return a more readable name from the module itself
  """
  return state.__name__[7:]


def _GetInput():
  """
  Loop reading the console for input.  Returns
  when input signals to exit
  """
  
  global g_userInput
  global g_inputValue

  while 1:
    c = sys.stdin.read(1)
    if c in ['w','a','s','d','x','c','v','b','y','u','i','o','p','9','h']:
      a = threading.Lock()
      a.acquire()
      g_userInput = 1
      g_inputValue = c
      a.release()
    elif c == '0':
      return


def _RequestStateChange(s, stateToRequest):
  """
  Processes the state change request by the user.
  finds to local actor instance and then requests
  the specified state change on the actor itself.
  if succesful, make the request to the server,
  otherwise report the failure
  """
  
  actor = g_actorDict[g_actorId]
  if actor.RequestStateChange(stateToRequest):
    print g_programName+'Transitioned to state [%s], sending server request' % (_ParseStateName(stateToRequest),)
    print '(L)Actor %s: Posture [%s] Movement [%s] Action [%s]' % (g_actorId,
                                                                   _ParseStateName(actor.GetPostureState()),  \
                                                                   _ParseStateName(actor.GetMovementState()), \
                                                                   _ParseStateName(actor.GetActionState()))
    data = struct.pack("<BB", MSG_TYPE_STATE_CHANGE_REQUEST, stateToRequest.GetStateId())
    s.send(data)
  else:
    print g_programName+'Blocked from transitioning to state [%s]' % (_ParseStateName(stateToRequest),)
    print '(L)Actor %s: Posture [%s] Movement [%s] Action [%s]' % (g_actorId,
                                                                   _ParseStateName(actor.GetPostureState()),  \
                                                                   _ParseStateName(actor.GetMovementState()), \
                                                                   _ParseStateName(actor.GetActionState()))    
  print ''
  print g_inputPrompt


def _ActorStatus(s):
  """
  Displays this clients current states for each actor,
  tagging the local actor with an "(L)"
  """
  print ''
  print 'Actor Status'
  print '-----------------------------------------------------------------------------'
  for actorId, actor in g_actorDict.items():
    if g_actorId == actorId:
      print '(L)Actor %s: Posture [%s] Movement [%s] Action [%s]' % (actorId, _ParseStateName(actor.GetPostureState()),  \
                                                                   _ParseStateName(actor.GetMovementState()), \
                                                                   _ParseStateName(actor.GetActionState()))
    else:
      print '   Actor %s: Posture [%s] Movement [%s] Action [%s]' % (actorId, _ParseStateName(actor.GetPostureState()),  \
                                                                   _ParseStateName(actor.GetMovementState()), \
                                                                   _ParseStateName(actor.GetActionState()))
  print '-----------------------------------------------------------------------------'
  print ''
  print g_inputPrompt


def _ShowHelp(s):
  """
  prints extended help
  """

  print 'Press the specified key and [Enter] to cause the local actor to transition to'
  print 'the state desired. If valid to do so in the current state, a request is sent to'
  print 'the server. The resulting local actor state is displayed after each request.'
  print ''
  print 'Options:        Movement             Posture             Action'
  print '                =================    =======             ======'
  print '                w = moveforward      c = standing        i = idle'
  print '                a = moveleft         v = sitting         u = casting'
  print '                s = movebackward     b = layingdown      y = crafting'
  print '                d = moveright                            o = fighting'
  print '                x = stopped                              p = gesturing'
  print ''
  print 'Two actions are given additional functionality if there is more that 1 client'
  print 'connected.  Select casting "u" will attempt to cast a FREEZE spell on another'
  print 'actor. If successfull, it will render the affected actor unable to move.'
  print 'Selecting fighting "f" will cause an actor to attack another actor.  The fight'
  print 'is to the death, and the initiator of the attack has a hihger probability of'
  print 'success. If the initiator loses he himself will die.'
  print ''
  print 'Entering "9" causes the display of an overall status of each actor'
  print 'that is connected; an actor is assigned to each client that is logged in.'
  print 'The actor aasociated with a client is tagged with an (L). "0" exits.'
  print ''
  print g_inputPrompt


"""
This global shows up here so that we can use the
direct method reference in the dictionary.
"""

g_methodDict = {
#   input   method               args
#   ----   -------               -----
    'w'  : (_RequestStateChange, (_moveforward,)),
    'a'  : (_RequestStateChange, (_moveleft,)),
    's'  : (_RequestStateChange, (_movebackward,)),
    'd'  : (_RequestStateChange, (_moveright,)),
    'x'  : (_RequestStateChange, (_stopped,)),
    'c'  : (_RequestStateChange, (_standing,)),
    'v'  : (_RequestStateChange, (_sitting,)),
    'b'  : (_RequestStateChange, (_layingdown,)),
    'i'  : (_RequestStateChange, (_idle,)),
    'u'  : (_RequestStateChange, (_casting,)),
    'y'  : (_RequestStateChange, (_crafting,)),
    'o'  : (_RequestStateChange, (_fighting,)),
    'p'  : (_RequestStateChange, (_gesturing,)),
    '9'  : (_ActorStatus, ()),
    'h'  : (_ShowHelp, ()),
}
  
def _InvokeMethod(input, s):
  """
  Utility method that inovokes the appropriate
  method doing a lookup based on user input.
  Any args specified are passed through to
  the method.
  """
  global g_methodDict
  if g_methodDict.has_key(input):
    method, args = g_methodDict[input]
    # print 'method to call is %s with %s' % (method,s)
    apply(method, (s,)+args)


def Recv_ActorLeft(data):
  """
  A client disconnected from the server.  Remove
  that actor from this clients lookup so that is
  in sync with the servers actors.
  """
  global g_actorDict
  (actorId,) = struct.unpack("<B", data[0])
  assert g_actorId != actorId, 'server telling us we left!'
  print g_programName+'actor left, id is %s' % (actorId,)
  del g_actorDict[actorId]
  print g_inputPrompt
  return data[1:]


def Recv_NewActor(data):
  """
  A new client logged into the server. Add it to
  our actor lookup and set the initial states to those
  specified on the server
  """
  global g_actorDict
  newActorId, p, m, a = struct.unpack("<BBBB", data[0:4])
  print g_programName+'new actor id is %s' % (newActorId,)
  newActor = actor.Actor()
  newActor.SetAllStateIds([p,m,a])
  g_actorDict[newActorId] = newActor
  print g_inputPrompt
  return data[4:]


def Recv_StateCorrection(data):
  """
  A state correction has been recieved from
  the server.  This means this client allowed
  an action for the local actor that was not
  allowed on the server, so immediately correct
  the local actor to reflect the server specified
  state.
  """
  actor = g_actorDict[g_actorId]
  p, m, a = struct.unpack("<BBB", data[0:3])
  data = data[3:]
  blockedDict = {}
  (numEntries,) = struct.unpack("<B", data[0])
  data = data[1:]
  for i in range(1, numEntries+1):
    stateId, blockCount = struct.unpack("<BB", data[0:2])
    blockedDict[shared._GetStateModuleById(stateId)] = blockCount
    data = data[2:]

  actor.SetAllStateIds([p,m,a])
  actor.SetBlockedStateDict(blockedDict)
  print g_programName+'received correction to [%s] [%s] [%s] for local actor [%s]' % (_ParseStateName(actor.GetPostureState()),  \
                                                                                      _ParseStateName(actor.GetMovementState()), \
                                                                                      _ParseStateName(actor.GetActionState()), \
                                                                                      g_actorId) 
  print g_inputPrompt
  return data


def Recv_StateChange(data):
  """
  A normal state change has occured for the
  specified actor (who is not local). Set that
  actors states to the ones specified in the
  message.
  """
  actorId, p, m, a = struct.unpack("<BBBB", data[0:4]) #TODO ONLY ONE STATE
  actor = g_actorDict[actorId]
  actor.SetAllStateIds([p,m,a])
  print g_programName+'actor [%s] changed state to [%s] [%s] [%s]' % (actorId, _ParseStateName(actor.GetPostureState()),  \
                                                                               _ParseStateName(actor.GetMovementState()), \
                                                                               _ParseStateName(actor.GetActionState()))
  return data[4:]


def Recv_ActorFrozen(data):
  """
  An actor got frozen.  Transition them to the 
  states then prevents them from moving any more
  """
  global g_actorDict
  (frozenActorId,) = struct.unpack("<B", data[0:1])
  if g_actorId == frozenActorId:
    print g_programName+'Local actor [%s] is frozen!' % (frozenActorId,)
  else:
    print g_programName+'Actor [%s] is frozen!' % (frozenActorId,)
  frozenActor = g_actorDict[frozenActorId]
  frozenActor.ForceDefaultStates()
  frozenActor.BlockMovement()
  print g_inputPrompt
  return data[1:]


def Recv_ActorDead(data):
  """
  An actor died.  Transition them to the dead
  state which makes sure they won't be able to
  do anything.
  """
  global g_actorDict
  (deadActorId,) = struct.unpack("<B", data[0:1])
  if g_actorId == deadActorId:
    print g_programName+'Local actor [%s] is dead!' % (deadActorId,)
  else:
    print g_programName+'Actor [%s] is dead!' % (deadActorId,)
  deadActor = g_actorDict[deadActorId]
  deadActor.TransitionTo(_dead)
  print g_inputPrompt
  return data[1:]


def Recv_LoginAck(data):
  """
  Login acknowledgement from server.  Assign
  the local actor id to the value passed in
  the message and create the actor instance.
  The instance is stored for subsequent use
  """
  global g_actorId
  global g_actorDict
  (g_actorId,) = struct.unpack("<B", data[0])
  print g_programName+'Login ok, local actor id is %s' % (g_actorId,)
  print g_inputPrompt
  g_actorDict[g_actorId] = actor.Actor()
  return data[1:]


def _ReadNetwork(s):
  """
  Selects on the specifid socket s, if any data
  comes through, parse the message type and
  send the remaining data along to the method
  called for the particular message type.  The
  handling method must return any data it did not
  process.
  """
  input, ignored, ignored = select.select([s], [], [], 0)
  for sock in input:
    data = sock.recv(1024)
    while len(data):
      (msgType,) = struct.unpack("<B", data[0])
      data = data[1:]
      if msgType == MSG_TYPE_LOGIN_ACK:
        data = Recv_LoginAck(data)
      elif msgType == MSG_TYPE_NEW_ACTOR:
        data = Recv_NewActor(data)
      elif msgType == MSG_TYPE_ACTOR_LEFT:
        data = Recv_ActorLeft(data)
      elif msgType == MSG_TYPE_STATE_CORRECTION:
        data = Recv_StateCorrection(data)
      elif msgType == MSG_TYPE_STATE_CHANGE:
        data = Recv_StateChange(data)
      elif msgType == MSG_TYPE_ACTOR_FROZEN:
        data = Recv_ActorFrozen(data)
      elif msgType == MSG_TYPE_ACTOR_DEAD:
        data = Recv_ActorDead(data)
      elif msgType == MSG_TYPE_SHUTDOWN:
        return 0
      else:
        print g_programName+'unknown message type %s, abort' % (msgType,)
        data = ''
  
  return 1



def main():
  """
  main() - main program loop
  """
  # Parse the port argument if passed
  global g_port
  global g_serverName
  if len(sys.argv) > 1:
    g_serverName = sys.argv[1]
    if len(sys.argv) > 2:
      value = int(sys.argv[2])
      if value > 0:
        g_port = value
  
  global g_userInput
  global g_inputValue

  # connect to the server and send it a login message
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  try:
    print g_programName+'connecting to [%s] on port [%s]...' % (g_serverName, g_port)
    s.connect((g_serverName, g_port))
    data = struct.pack("<B", MSG_TYPE_LOGIN)
    s.send(data)
    s.setblocking(0)
  except:
    # server not up, we go down...
    print g_programName + 'unable to connect, is server running?'
    return

  # start a separate thread to get user input
  inputThread = threading.Thread(None, _GetInput)
  inputThread.start()

  # loop forever checking for user input and messages from the server
  try:
    while 1:
      if inputThread.isAlive():
        # checking for user input
        if g_userInput:
          a = threading.Lock()
          a.acquire()
          # valid user input, call the appropriate method
          _InvokeMethod(g_inputValue, s)
          g_userInput = 0
          g_inputValue = None
          a.release()

        # check for incoming network messages          
        if _ReadNetwork(s) == 0:
          print g_programName+'got shutdown message. Press [Enter] to continue.'
          inputThread._Thread__stop()
          s.close()
          break
      else:
        # user selected to exit program
        print g_programName+'exiting'
        s.close()
        break
  except KeyboardInterrupt:
    # if the user decides to ctrl-c to exit
    print g_programName+'exiting'
    s.close()
    
  del inputThread
  

# run main()  
if __name__ == '__main__':
 print g_programName+'starting up...'
 main()
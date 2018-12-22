# eventbroadcaster.py
#
# Implements the game event management API
# for registering and unregistering for game
# events, and dispatching events when they
# occur.
#
# Author: Matthew Walker
#         mwalker@softhome.net
#

import deferred
import gameeventkeys

class EventBroadcaster:
  def __init__(self, objId):
    self.id = objId
    self.handlers = {}

  def RegisterHandler(self, key, handlerRef):
    key.RegisterHandler(self.handlers, handlerRef)
  
  def UnregisterHandler(self, key, handlerRef):
    key.UnregisterHandler(self.handlers, handlerRef)

  def PostGameEvent(self, key, delay, *args):
    kArgs = (key,) + args
    deferred.Call(self.id, 'Dispatch', kArgs, delay, None, None)
  
  def Dispatch(self, key, *args):
    #print 'Dispatch( %s, %s )' % (key, args)
    handlerList = \
      gameeventkeys.GetHandlers(key, self.handlers)
    for handler in handlerList:
      apply(handler, (key,) + args)



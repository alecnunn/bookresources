# gameeventkeys.py
#
# Implements the event key objects, which 
# allows precise registration for events 
# based on specific criteria.
# 
# Author: Matthew Walker
#         mwalker@softhome.net
#

class GameEventKey:
  """
  Event key base class.
  Responsible for providing the equality and hashing
  operators which allow this object and its derived
  classes to be used as dictionary keys. Also 
  responsible for implementing the event registration
  and lookup functionality, so that if desired, derived
  classes may further specialize this behavior.
  """
  def __init__(self, event, src=None, subj=None):
    self.event = event
    self.src   = src
    self.subj  = subj

  def __eq__(self, other):
    # equality test
    return (self.event == other.event) and \
           (self.src   == other.src)   and \
           (self.subj  == other.subj)

  def __hash__(self):
    # hash function 
    return hash((self.event, self.src, self.subj))

  def __repr__(self):
    # supports debug display of this object
    import gameevents
    return '<%s : event [ %s ], source [ %s ], subject [ %s ]>' % (self.__class__.__name__, gameevents.byvalue[self.event], self.src, self.subj)

  def RegisterHandler(self, handlers, handlerRef):
    # this key maps to a dictionary of handlers
    hDict = handlers.get(self, {})
    if not hDict:
      handlers[self] = hDict
    hDict[handlerRef] = 1  # dictionary as a set

  def UnregisterHandler(self, handlers, handlerRef):
    # this key maps to a dictionary of handlers
    hDict = handlers[self]
    del hDict[handlerRef]
    if len(hDict) == 0:
      # remove unneeded key entries
      del handlers[self]

  def Lookup(self, handlers):
    # return the dict for this key, or an empty one
    return handlers.get(self, {})


#
# GameEventKey derived classes; used for
# registering handlers according to specific
# criteria.
#
class ByEvent(GameEventKey):
  def __init__(self, event):
    GameEventKey.__init__(self, event)

class ByEventAndSource(GameEventKey):
  def __init__(self, event, src):
    GameEventKey.__init__(self, event, src)

class ByEventAndSubject(GameEventKey):
  def __init__(self, event, subj):
    GameEventKey.__init__(self, event, None, subj)

class ByEventSourceAndSubject(GameEventKey):
  def __init__(self, event, src, subj):
    GameEventKey.__init__(self, event, src, subj)



def GetHandlers(key, handlers):
  """
  Retrieve a list of handlers for all possible
  combinations of key criteria. We use a dictionary
  so that each handler will be called only once.
  """
  hDict = {}
  hDict.update(ByEvent (key.event).Lookup(handlers))
  hDict.update(ByEventAndSource(key.event, 
               key.src).Lookup(handlers))
  hDict.update(ByEventAndSubject(key.event, 
               key.subj).Lookup(handlers))
  hDict.update(ByEventSourceAndSubject(key.event, 
               key.src, key.subj).Lookup(handlers))

  # since hDict is used as a set, the data we need
  # is in the keys, not the values
  return hDict.keys()  # list of handler references



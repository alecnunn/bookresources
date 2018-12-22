# deferred.py
#
# Implement a deferred (asynchronous) call framework.
# Allows the storing of method and function calls in
# a list, sorted by the time in which they are to be 
# called. Using this module, a client may schedule a
# call on any object that is addressable via the objmgr
# module (i.e. has an id).
#
# Author: Matthew Walker
#         mwalker@softhome.net
#

import time
import bisect  # efficient list operations
import objmgr

# module-scoped list; protect against reload
try:
  deferredCalls
except NameError:
  deferredCalls = []


class DeferredCall:
  """
  Responsible for representing the state of a method or
  function call that is intended to be executed at some
  time in the future.
  """
  def __init__(self, id, call, args, t, cb, cbArgs):
    self.targetId  = id
    self.call      = call
    self.args      = args
    self.time      = t
    self.callback  = cb
    self.cbArgs    = cbArgs

  def __cmp__(self, other):
    return cmp(self.time, other.time)

  def __call__(self):
    """
    Make this class a callable object (a.k.a. a functor),
    which acts just like a fuction or method.
    """
    target = objmgr.GetObject(self.targetId)
    if target is not None:
      try:
        method = getattr(target, self.call)
      except AttributeError:
        print "No %s on %s" % (self.call, target)
        return

      apply(method, self.args) # make the call

      if self.callback is not None:
        # notify that call is complete
        apply(self.callback, self.cbArgs)
    else:
      print 'No valid target for %s' % (self,)

  def __repr__(self):
    """
    Return a string representation of this object.
    """
    return '<%s: call [%s] args [%s] time [%s] cb [%s] cbArgs [%s]>' % \
      (self.__class__.__name__, self.call, self.args, self.time, self.callback, self.cbArgs)


def Call(target, call, args, delay=0, cb=None, cbArgs=()):
  """
  Schedule a call for later execution. This 
  is the API for making a deferred call.
  """
  #print 'Call( %s, %s, %s, %s, %s, %s )' % (target, call, args, delay, cb, cbArgs)
  callTime = time.time() + float(delay) / 1000.0
  dCall = DeferredCall(target, call, args,
                       callTime, cb, cbArgs)
  bisect.insort_right(deferredCalls, dCall)


def ExecuteDeferredCalls():
  """
  Run deferred method calls whose time has come.
  """
  dCall = None
  now = time.time()
  while deferredCalls:
    dCall = deferredCalls.pop(0) # front of the list
    if dCall.time > now:
      # not time yet, put it back at front of the list
      deferredCalls.insert(0, dCall)
      break
    
    dCall() # execute the call, and any callbacks

  # now return how long in ms until next call
  next = None  # forever
  if dCall is not None:
    next = int((dCall.time - time.time()) * 1000.0)
    if next < 0:
      next = 0

  return next


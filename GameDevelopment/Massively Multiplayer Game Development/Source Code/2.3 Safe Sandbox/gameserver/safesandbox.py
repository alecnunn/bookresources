# safesandbox.py
#
# Implements an environment in which protected code
# can run. The code in this module is not restricted,
# but the code in derived classes of SafeSandbox is
# restricted.
#
# Author: Matthew Walker
#         mwalker@softhome.net
#

import sandboxrexec
import Bastion
import string
import gameserver
import objmgr
import deferred
import gameobs
import gameevents
import gameeventkeys


# a global instance of our restricted environment
g_re = sandboxrexec.SandboxRExec()

# a global dictionary of protected sandbox classes
g_classDict = {}

def _GetProtectedObject(object):
  "Helper to return a protected object."
  return Bastion.Bastion(object)

def _GetProtectedModule(modulePath):
  "Helper to return a protected module."
  return g_re.r_import(modulePath)

def _InitSandboxClasses(sandboxModules):
  """
  Build a dictionary of SafeSandbox derived class objects which is 
  used by the CreateSafeSandbox() factory function. Called at server 
  startup time.

  Classes derived from SafeSandbox cannot directly import this module,
  because this module indirectly imports modules that are not allowed
  inside a SafeSandbox instance. Instead, we create the inheritance 
  relationship at runtime, after importing the derived class's module 
  via our SandboxRExec object.

  The sandboxModules parameter is a list of tuples of sandbox module 
  and class names to be loaded.
  """
  for module in sandboxModules:
    # expand the row tuple into its elements
    moduleName, className = module

    # import the module via the restricted environment
    try:
      module = _GetProtectedModule(moduleName)
    except ImportError:
      import traceback
      traceback.print_exc()
      print 'SafeSandbox module [ %s ] cannot be imported.' % (moduleName,)
      continue

    # certain common modules may be needed within a sandbox,
    # but may themselves import restricted objects; we insert
    # them into our protected module from the 'outside' to avoid
    # problems with trying to import them from within the sandbox.
    import gameevents
    setattr(module, 'gameevents', gameevents)
    import gameeventkeys
    setattr(module, 'gameeventkeys', gameeventkeys)

    # get the class object from the sandbox module
    safeSandboxClass = getattr(module, className)

    # Because we manually create the inheritance relationship with
    # the SafeSandbox base class, we cannot assume that implementors
    # will write constructors that call the base constructor, so 
    # we disallow it entirely.
    assert not hasattr(safeSandboxClass, '__init__'), \
       'Class [ %s ] must not define an __init__ method!' % (safeSandboxClass,)

    # Insert the SafeSandbox base class into the list of bases,
    # thereby creating inheritance without the derived class needing to 
    # import this module.
    bases = safeSandboxClass.__bases__
    safeSandboxClass.__bases__ = bases + (SafeSandbox,)

    # store the sandbox class object indexed by its name
    g_classDict[string.upper(className)] = safeSandboxClass

    print 'Created SafeSandbox class: %s' % (safeSandboxClass,)


def CreateSafeSandbox(worldRegion, eventManager, className):
  """
  Factory function that returns an instance of the derived
  class of SafeSandbox, as indicated by the className. The
  worldRegion parameter is an object containing information
  about the physical game world and the objects it contains. 
  The eventManager parameter provides the ability to register
  for and post asynchronous game events.
  """
  try:
    sandbox = g_classDict[string.upper(className)](worldRegion, eventManager)
    sandbox._FrameworkInit()
    return sandbox
  except KeyError:
    raise RuntimeError('No sandbox class for id [ %s ]' % (className,))


#
# SafeSandbox base class.
#
class SafeSandbox:
  """
  Defines the logical space within which ad-hoc game code runs.
  Provides controlled access to specific services via wrapper 
  methods available to derived classes.
  """

  def __init__(self, worldRegion, eventManager):
    """
    Constructor. Derived classes are not permitted to override this.
    The worldRegion contains the world geometry and game objects.
    There is one event manager per sandbox, allowing for a clean 
    partitioning of events in different sandboxes.
    """
    self.__worldRegion = worldRegion
    self.__eventManager = eventManager


  def _FrameworkInit(self):
    """
    Standard initalization. Calls derived class implementation if it exists.
    This is in leiu of permitting derived classes to implement __init__().
    """
    if hasattr(self, 'Init'):
      self.Init()

    # common initialization
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.PLAYER_ENTERS_ROOM), self._OnPlayerEnter)
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.PLAYER_EXITS_ROOM), self._OnPlayerExit)
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.ACTION_TAKE_OBJECT), self._OnTakeItem)
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.ACTION_PLACE_OBJECT), self._OnPlaceItem)
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.PLAYER_DIES), self._OnPlayerDies)


  def __getattr__(self, name):
    """
    Simulate direct member access for protected members.
    Posting of game events is not allowed within the sandbox, so 
    we only expose registration and unregistration methods.
    """
    if name == 'RegisterHandler':
      return self.__eventManager.RegisterHandler
    elif name == 'UnregisterHandler':
      return self.__eventManager.UnregisterHandler
    elif name == 'playerids':
      return self.__worldRegion.playerids[:]  # return a copy, to avoid mutating list
    elif name == 'itemids':
      return self.__worldRegion.itemids[:]  # return a copy, to avoid mutating list
    else:
      raise AttributeError(name)


  def CreateItem(self, klassname, name):
    """
    Create an object of the given class with the name requested.
    """
    # resolve the class object from the class name
    klass = getattr(gameobs, klassname)
    assert(issubclass(klass, gameobs.Item))

    # create a new instance and add to object mgr
    id = objmgr.NextId()
    ob = klass(name, id)
    objmgr.AddObject(ob, id)

    # keep track of it
    self.__worldRegion.itemids.append(id)

    # return a protected version
    return self.GetGameObject(id)  


  def GetGameObject(self, obId):
    """
    Return a protected game object from the world region, based
    on its object id.
    """
    gameobject = objmgr.GetObject(obId)
    return _GetProtectedObject(gameobject)


  def ClientMessage(self, playerid, msg):
    """
    Sends a message to the player associates with the 
    clientid using a deferred call.
    """
    import request
    request.ClientMessage(playerid, msg)

 
  # 
  # Default event handlers common to all sandboxes
  #

  def _OnPlayerEnter(self, key):
    """
    Private; always called in the base class; 
    derived class can implement OnPlayerEnter()
    (i.e. not virtual) to extend.
    """
    print '_OnPlayerEnter( %s )' % (key,)    
    player = self.GetGameObject(key.subj)
    seemsg = 'You see:\n'
    for playerid in self.playerids:
      if playerid != key.subj:
        # don't notify player who entered
        self.ClientMessage(playerid, '%s has entered %s.' % (player.GetName(),self.__worldRegion.name))

        # tell player who is around us
        other = self.GetGameObject(playerid)
        seemsg = seemsg + ('%s\n' % (other.GetName(),))

    # also tell of items you see
    for itemid in self.itemids:
      item = self.GetGameObject(itemid)
      seemsg = seemsg + ('%s\n' % (item.GetName(),))
    
    self.ClientMessage(key.subj, seemsg)

    if hasattr(self, 'OnPlayerEnter'):
      self.OnPlayerEnter(key)


  def _OnPlayerExit(self, key):
    """
    Private; always called in the base class; 
    derived class can implement OnPlayerExit()
    (i.e. not virtual) to extend.
    """
    print 'OnPlayerExit( %s )' % (key,)    
    player = self.GetGameObject(key.subj)
    for playerid in self.playerids:
      if playerid != key.subj:
        # don't notify player who left
        self.ClientMessage(playerid, '%s has left Town.' % (player.GetName(),))

    if hasattr(self, 'OnPlayerExit'):
      self.OnPlayerExit(key)


  def _OnTakeItem(self, key):
    """
    Private; always called in the base class; 
    derived class can implement OnTakeItem()
    (i.e. not virtual) to extend.
    """
    print '_OnTakeItem( %s )' % (key,)    
    player = objmgr.GetObject(key.src)
    item = objmgr.GetObject(key.subj)
    player._AddToInventory(item)
    item._AssignOwner(player.id)
    self.__worldRegion.itemids.remove(key.subj)

    if hasattr(self, 'OnTakeItem'):
      self.OnTakeItem(key)


  def _OnPlaceItem(self, key):
    """
    Private; always called in the base class; 
    derived class can implement OnPlaceItem()
    (i.e. not virtual) to extend.
    """
    print '_OnPlaceItem( %s )' % (key,)    
    player = objmgr.GetObject(key.src)
    item = objmgr.GetObject(key.subj)
    player._RemoveFromInventory(item)
    item._AssignOwner(None)
    self.__worldRegion.itemids.append(key.subj)

    if hasattr(self, 'OnPlaceItem'):
      self.OnPlaceItem(key)
    

  def _OnPlayerDies(self, key):
    """
    Notify the world when a player dies.
    """
    print '_OnPlaceItem( %s )' % (key,)    
    player = objmgr.GetObject(key.src)

    for playerid in self.playerids:
      if playerid != key.subj:
        # don't notify player who died - he already knows
        self.ClientMessage(playerid, '%s has died.' % (player.GetName(),))


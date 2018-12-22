# Server actor instance
#
import shared.characterstatemgr as _characterstatemgr
import shared.moveforward as _moveforward
import string

class Actor(_characterstatemgr.CharacterStateMgr):

  def __init__(self, actorId):
    _characterstatemgr.CharacterStateMgr.__init__(self)
    self.__actorId = actorId

  def GetId(self):
    return self.__actorId

  def RequestStateChange(self, state):
    return self.RequestTransition(state)  
    

  

 

# Client actor instance
#
import shared.characterstatemgr as _characterstatemgr
import shared.moveforward as _moveforward
import string

class Actor(_characterstatemgr.CharacterStateMgr):

  def __init__(self):
    _characterstatemgr.CharacterStateMgr.__init__(self)

  def RequestStateChange(self, state):
    return self.RequestTransition(state)

    

  

 

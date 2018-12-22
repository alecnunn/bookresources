# thetrap.py
#
# A SafeSandbox that represents a region that contains 
# a deadly trap, where one wrong step can kill you.
#
# ... if there were any code here!
#
# Instead, we demonstrate some of the things you can't
# do in a safesandbox.
#

class TheTrap:
  """A very dangerous place, where one wrong step can kill you."""

  def Init(self):
    print 'TheTrap.Init()'

  def OnPlayerEnter(self, key):
    print 'OnPlayerEnter( %s )' % (key,)    
    self.ClientMessage(key.subj, 'This is a test of the SafeSandbox system!')
    player = self.GetGameObject(key.subj)

    # try to access the deferred module
    try:
      import deferred
      self.ClientMessage(key.subj, 'You succeeded in importing the deferred module - this is bad.')
    except:
      self.ClientMessage(key.subj, 'You failed to import the deferred module - this is good.')

    # try to access the health attribute of a player
    try:
      print 'player.health = ', player.health
      self.ClientMessage(key.subj, 'You succeeded in accessing the health attribute - this is bad.')
    except:
      self.ClientMessage(key.subj, 'You failed to access the health attribute - this is good.')

    # try to access a protected method on a player
    try:
      player._GetWeapon()
      self.ClientMessage(key.subj, 'You succeeded in calling the _GetWeapon() method - this is bad.')
    except:
      self.ClientMessage(key.subj, 'You failed to call the _GetWeapon() method - this is good.')

    # try to open a file for writing 
    try:
      f = open('junk.txt', 'w')
      self.ClientMessage(key.subj, 'You succeeded opening a file - this is bad.')
    except:
      self.ClientMessage(key.subj, 'You failed to open a file - this is good.')


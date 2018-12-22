# town.py
#
# A SafeSandbox that represents a town, where no 
# fighting is allowed, and players can trade with
# one another.
#


class Town:
  """A charming location to meet people."""
  def Init(self):
    print 'Town.Init()'
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.ACTION_WAVE), self.OnWave)

    # create some useless stuff
    self.CreateItem('Item', 'House')
    self.CreateItem('Item', 'Rock')
    self.CreateItem('Item', 'Sign')


  def OnWave(self, key):
    print 'OnWave( %s )' % (key,)
    source = self.GetGameObject(key.src)
    target = self.GetGameObject(key.subj)
    for playerid in self.playerids:
      if playerid != key.subj and playerid != key.src:
        # don't notify source or target - they already know
        self.ClientMessage(playerid, '%s is greeting %s.' % (source.GetName(),target.GetName()))




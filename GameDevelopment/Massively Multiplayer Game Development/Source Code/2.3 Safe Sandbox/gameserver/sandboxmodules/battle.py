# battle.py
#
# A SafeSandbox that represents a combat arena, where 
# fighting is allowed.
#

class Battle:
  """A place where you can engage in combat."""
  def Init(self):
    print 'Battle.Init()'
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.ACTION_ATTACK_SUCCESS), self.OnAttackSuccess)
    self.RegisterHandler(gameeventkeys.ByEvent(gameevents.ACTION_ATTACK_FAILURE), self.OnAttackFailure)

    # create some stuff
    self.CreateItem('Weapon', 'Longsword')
    self.CreateItem('Weapon', 'Shortsword')

  def OnPlayerEnter(self, key):
    print 'OnPlayerEnter( %s )' % (key,)    
    self.ClientMessage(key.subj, 'Let the battle begin!')

  def OnPlayerExit(self, key):
    print 'OnPlayerExit( %s )' % (key,)    
    player = self.GetGameObject(key.subj)
    if player.GetHealth() > 0:
      self.ClientMessage(key.subj, 'You are lucky to escape alive.')
    else:
      self.ClientMessage(key.subj, 'You suffered a crushing defeat.')

  def OnAttackSuccess(self, key, damage):
    print 'OnAttackSuccess( %s, %s )' % (key, damage)
    source = self.GetGameObject(key.src)
    target = self.GetGameObject(key.subj)
    for playerid in self.playerids:
      if playerid != key.subj and playerid != key.src:
        # don't notify or target - they already know
        self.ClientMessage(playerid, '%s attacked %s for %s damage.' % (source.GetName(), target.GetName(), damage))

  def OnAttackFailure(self, key):
    print 'OnAttackFailure( %s )' % (key,)
    source = self.GetGameObject(key.src)
    target = self.GetGameObject(key.subj)
    for playerid in self.playerids:
      if playerid != key.subj and playerid != key.src:
        # don't notify source or target - they already know
        self.ClientMessage(playerid, '%s attacked %s and missed.' % (source.GetName(), target.GetName()))




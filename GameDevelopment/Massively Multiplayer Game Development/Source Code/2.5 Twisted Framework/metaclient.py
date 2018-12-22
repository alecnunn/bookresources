from twisted.spread.pb import authIdentity, getObjectAt, portno
from twisted.spread.flavors import Referenceable
from twisted.internet import reactor
from twisted.internet.defer import DeferredList

class BugClient(Referenceable):
    def gotPerspective(self, pref):
        print 'got bug'
        pref.callRemote("goToPsychologist","The Tick").addCallback(self.notify)
        pref.callRemote("psychoanalyze").addCallback(self.notify)
    def notify(self, text):
        print 'bug:', text
    def remote_healed(self, healedMessage):
        print 'Healed:', healedMessage

class CellClient(Referenceable):
    def gotPerspective(self, pref):
        print 'got cell'
        # pref.callRemote()
    def remote_connected(self):
        print 'Cell Connected'
    def remote_hear(self, text):
        print 'Phone:', text
    def remote_ring(self, callerID):
        print "Your phone is ringing.  It's a call from", callerID

bugClient = BugClient()
phoneClient = CellClient()

# Log-In Information
username = "bob"
password = "bobpass"
bugName = "franz"
phoneNumber = "222-303-8485"
    
# A little magic to get us connected...
getObjectAt("localhost", portno).addCallback(
    # challenge-response authentication
    lambda r: authIdentity(r, username, password)).addCallback(
    # connecting to each perspective with 'attach' method of remote identity
    lambda i: DeferredList([
    i.callRemote("attach", "metamorph", bugName, bugClient),
    i.callRemote("attach", "cellphone", phoneNumber, phoneClient)])
    # connecting perspectives to client-side objects
    ).addCallback(lambda l: (bugClient.gotPerspective(l[0][1]),
                             phoneClient.gotPerspective(l[1][1])))

reactor.callLater(5, reactor.stop) # In five seconds, log out.
reactor.run() # Start the main loop.

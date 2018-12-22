# cell phone service
from twisted.spread.pb import Service, Perspective
class Cellphone(Perspective):
    def attached(self, remoteEar, identity):
        self.remoteEar = remoteEar
        self.caller = None
        self.talkingTo = None
        return Perspective.attached(self, remoteEar, identity)

    def detached(self, remoteEar, identity):
        del self.remoteEar
        self.caller = None
        self.talkingTo = None
        return Perspective.detached(self, remoteEar, identity)

    def hear(self, text):
        self.remoteEar.callRemote('hear', text)

    def perspective_dial(self, phoneNumber):
        otherPhone = self.service.getPerspectiveNamed(phoneNumber)
        otherPhone.ring(self)

    callerID = True
    def ring(self, otherPhone):
        self.caller = otherPhone
        if self.callerID:
            displayNumber = otherPhone.perspectiveName
        else:
            displayNumber = "000-555-1212"
        self.remoteEar.callRemote('ring', displayNumber)

    def perspective_pickup(self):
        if self.caller:
            self.caller.phoneConnected(self)
            self.phoneConnected(self.caller)
            self.caller = None

    def phoneConnected(self, otherPhone):
        self.talkingTo = otherPhone
        self.remoteEar.callRemote('connected')

    def perspective_talk(self, message):
        if self.talkingTo:
            self.talkingTo.hear(message)

class PhoneCompany(Service):
    perspectiveClass = Cellphone

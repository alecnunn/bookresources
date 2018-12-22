from twisted.spread.pb import Perspective, Service
class Bug(Perspective):
    angst = 0
    def attached(self, remoteBugWatcher, identity):
        self.remoteBugWatcher = remoteBugWatcher
        self.psychologist = None
        self.angst += 5 # It's hard to get up in the morning.
        return Perspective.attached(self, remoteBugWatcher, identity)
    def detached(self, remoteBugWatcher, identity):
        self.remoteBugWatcher = None
        if self.psychologist:
            self.psychologist.leaveTherapy(self)
            self.psychologist = None
        return Perspective.detached(self, remoteBugWatcher, identity)

    # Remote methods.
    def perspective_goToPsychologist(self, name):
        psychologist = self.service.psychologists.get(name)
        if psychologist:
            psychologist.joinTherapy(self)
            self.psychologist = psychologist
            return "You made it to group therapy."
        else:
            return "There's no such psychologist."

    def perspective_psychoanalyze(self):
        if self.psychologist:
            self.psychologist.requestTherapy(self)
            return "Therapy requested!"
        else:
            return "You're not near a therapist."

    # Local methods.
    def heal(self, points):
        self.angst -= points
        if points > 0:
            feeling = "better"
        else:
            feeling = "worse"
        self.remoteBugWatcher.callRemote(
            "healed",
            "You feel %s points %s.  Your angst is now: %s." %
            (abs(points), feeling, self.angst))

class Psychologist:
    def __init__(self, name, skill, world):
        self.name = name
        self.skill = skill
        self.group = []
        world.psychologists[name] = self
    def joinTherapy(self, bug):
        self.group.append(bug)
    def leaveTherapy(self, bug):
        self.group.remove(bug)
    def requestTherapy(self, bug):
        for bug in self.group:
            bug.heal(self.skill + len(self.group))

class BuggyWorld(Service):
    perspectiveClass = Bug
    def __init__(self, *args, **kw):
        Service.__init__(self, *args, **kw)
        self.psychologists = {}
        Psychologist("Freud", -5, self)
        Psychologist("Pavlov", 1, self)
        Psychologist("The Tick", 10, self)

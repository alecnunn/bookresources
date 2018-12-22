# Import Twisted code
from twisted.spread.pb import AuthRoot, BrokerFactory, portno
from twisted.internet.app import Application
from twisted.cred.authorizer import DefaultAuthorizer
# Import our own library
import cellphone
import metamorph
# Create root-level object and authorizer
app = Application("Metamorphosis")
auth = DefaultAuthorizer(app)
# Create our services (inside the App directory)
phoneCompany = cellphone.PhoneCompany("cellphone", app, auth)
buggyWorld = metamorph.BuggyWorld("metamorph", app, auth)
# Create Identities for Joe and Bob.

def makeAccount(userName, phoneNumber, bugName, pw):
    # Create a cell phone for the player, so they can chat.
    phone = phoneCompany.createPerspective(phoneNumber)
    # Create a bug for the player, so they can play the game.
    bug = buggyWorld.createPerspective(bugName)
    # Create an identity for the player, so they can log in.
    i = auth.createIdentity(userName)
    i.setPassword(pw)
    # add perspectives to identity we created
    i.addKeyForPerspective(phone)
    i.addKeyForPerspective(bug)
    # Finally, commit the identity back to its authorizer.
    i.save()

# Create both Bob's and Joe's accounts.
makeAccount("joe", "222-303-8484", "fritz", "joepass")
makeAccount("bob", "222-303-8485", "franz", "bobpass")

app.listenTCP(portno, BrokerFactory(AuthRoot(auth)))
from twisted.web.server import Site
from twisted.web.static import File
from twisted.web.script import ResourceScript
f = File('.')
f.processors = { '.rpy': ResourceScript }
app.listenTCP(8080, Site(f))
app.run()

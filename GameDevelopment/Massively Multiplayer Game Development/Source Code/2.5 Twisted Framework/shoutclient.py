from twisted.internet.protocol import Protocol, ClientFactory
class Shout(Protocol):             # Class designed to manage a connection.
    stringToShout = "Twisted is great!" # a string to send the echo server
    def connectionMade(self):      # Method called when connection established.
        self.buf = ""              # create an empty buffer
        print "Shouting:", repr(self.stringToShout)
        self.transport.write(self.stringToShout)
    def dataReceived(self, data):  # Method called when data is received.
        self.buf += data           # buffer any received data
        if self.buf == self.stringToShout: # if we've received the full message
            self.transport.loseConnection() # then close the connection.
            print "Echoed:", repr(self.stringToShout)
            reactor.stop()
class ShoutClientFactory(ClientFactory):
    def buildProtocol(self, address):
        return Shout()              # Build Protocols on incoming connections
from twisted.internet import reactor
# connect to local port 1234 and expect an echo.
reactor.connectTCP("localhost", 1234, ShoutClientFactory())
reactor.run() # run the main loop until the user interrupts it

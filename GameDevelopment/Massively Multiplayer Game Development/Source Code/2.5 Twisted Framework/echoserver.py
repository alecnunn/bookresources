from twisted.internet.protocol import Protocol, Factory
class Echo(Protocol):              # Class designed to manage a connection.
    def dataReceived(self, data):  # Method called when data is received.
        self.transport.write(data) # When we receive data, write it back out.
class EchoFactory(Factory):           
    def buildProtocol(self, address):
        return Echo()              # Build Protocols on incoming connections
from twisted.internet import reactor
reactor.listenTCP(1234, EchoFactory()) # Bind TCP Port 1234 to an EchoFactory
reactor.run() # run the main loop until the user interrupts it

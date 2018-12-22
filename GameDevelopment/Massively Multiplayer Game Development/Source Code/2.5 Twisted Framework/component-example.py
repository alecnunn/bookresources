from twisted.python.components import getAdapter, registerAdapter, Interface

class IA(Interface):                    # Define an interface that implements 
    def a(self):                        # a sample method method, "a".
        "A sample method."

class A:                                # define an adapter class
    __implements__ = IA                 # that implements our IA interface
    def __init__(self, original):       # keep track of the object being wrapped
        self.original = original
    def a(self):                        # define the method required by our
        print 'a',                      # interface, and have it print 'a'
        self.original.b()               # then call back to the object we're adapting

class B:                                # the hapless B class doesn't know anything
    def b(self):                        # about its adapter, and it defines one
        print 'b'                       # method which displays 'b'

registerAdapter(A, B, IA)               # register A to adapt B for interface IA
adapter = getAdapter(B(), IA, None)     # adapt a new B instance with an A instance
adapter.a()                             # call the method defined by interface IA

Parallel State Machines Demonstration
=====================================

This demonstration illustrates the use of parallel state
machines in a client-server implementation. Up to 20
clients may be connected to a server, which validates
and broadcasts state change request from clients on
behalf of the actor they represent.

The server is executed by using entering the "server" folder 
via a command prompt and typing:

python psmserver.py <portNum>

where <portNum> is the optional port number to listen on.
This assumes you have python installed (if not, go to 
www.python.org to download), and that it is in your path. 
If python.exe it's not in your path, you'll have to fully
qualify the Python location. Once the server is running,
it will print a message indicating it is ready for connections.


With the server running, enter the "client" folder with a 
separate command prompt and type:

python psmclient.py <server> <portNum>

where <server> is an optional hostname or ip address where
the server is running (defaults to localhost) and <portNum>
is the optional port number to connect to. The client will
automatically attempt to connect to the running
server.  Upon successful connection, the client will be 
assigned an actorId by the server.  The client is able to
control this actor using the various options presented at
the prompt.  Press 'h' and enter to get further details.

Up to 20 clients can be connected simultaneously. Each client
controls its own actor in the 'world' and as their states
change, the server broadcasts the updates to all connected
clients. Watch the output from the server and client(s)
to see the results of your interactions.  Note that if
you choose to run more than one client from the same 
machine, there may be a delay in the second and subsequent
client's startup times.  However, once connected, all
ativity will work correctly.

The shared folder contains the PSM source.  Both the client
and server in this demo shared this code.


The source for this demo is organized into 3 folders. The 
contents of these folders are as follows:

folder		description
=======		==========================================
shared		All of the PSM related files, including 
		characterstatemgr.py and all of the state 
                module files.  
		
client		The source for the client program in 
                psmclient.py and a simplistic client actor
                class.	

server		The source for the server program in 
                psmserver.py and a simplistic server actor
                class.
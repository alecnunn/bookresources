import java.net.*;
import java.lang.*;
import java.util.*;
import GameGroup;

public class GameServerThread extends Thread {
ServerSocket servSock = null;
Vector v;

GameServerThread(int port) {
        try {
            servSock = new ServerSocket(port);
        } catch (Exception e) {
            System.out.println("Could not initialize. Exiting.");
            System.exit(1);
        }
System.out.println("Server successfully initialized.  Waiting for connection on port "+port);
v = new Vector();
}

public void run() {
GameGroup tempGroup=null;

while(servSock != null ) {
Socket tempSock;
        try {
	tempSock = servSock.accept();
	System.out.println("Received New Connection.");
	if( !v.isEmpty() ) {
		tempGroup = (GameGroup)v.lastElement();
		if( tempGroup.full() ) 			
			v.addElement(  new GameGroup( tempSock ) );
		else {
			tempGroup.addClient(tempSock);
			if( tempGroup.full() ) 
				tempGroup.start();	
		}
	} else 
		v.addElement(new GameGroup( tempSock ) );
for( int x=0; x<v.size()-1;x++) 
	if( !((GameGroup)v.elementAt(x)).isAlive() )
		v.removeElementAt(x);

        } catch (Exception e) {
            System.out.println("New Connection Failure.  Exiting.\n"+e);
          System.exit(1);
        }

try{ sleep(100); } catch (Exception e);
}
}

public void finalize() {
	try {
		servSock.close();
	} catch(Exception e);
	servSock = null;
}


	
}

import java.net.*;
import java.lang.*;
import sClientGroup;
import sClientThread;

public class ChatServerThread extends Thread {
ServerSocket servSock = null;
sClientGroup group;

ServerThread() {
        try {
            servSock = new ServerSocket(1123);
        } catch (Exception e) {
            System.out.println("Could not initialize. Exiting.");
            System.exit(1);
        }
group = new sClientGroup();
}

public void run() {


while(servSock != null ) {
Socket tempSock;
        try {
	tempSock = servSock.accept();
	System.out.println("New Connection.");
        } catch (Exception e) {
            System.out.println("New Connection Failure.  Exiting.");
            System.exit(1);
        }

tempThread = new sClientThread(tempSock);
group.addClient( tempThread );
}


public void finalize() {
	try {
		servSock.close();
	} catch(Exception e);
	servSock = null;
}


	
}

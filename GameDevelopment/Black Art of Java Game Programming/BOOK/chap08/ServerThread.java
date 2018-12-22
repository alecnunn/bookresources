import java.net.*;
import java.lang.*;
import java.io.*;
import java.util.*;
import HighScoreList;

public class ServerThread extends Thread {
ServerSocket servSock = null;
long lastChange;
long lastWrite;
HighScoreList L;




ServerThread(HighScoreList theList) {
        try {
            servSock = new ServerSocket(2357);
        } catch (Exception e) {
            System.out.println("Could not listen on port: " + 2357 + ", " + e);
            System.exit(1);
        }
L = theList;


}

public void run() {


lastChange = System.currentTimeMillis();
while(servSock != null && L != null ) {

        Socket sock = null;
        try {
	sock = servSock.accept();
	System.out.println("Got connection: "+2357);
        } catch (Exception e) {
            System.out.println("Accept failed: " + 2357 + ", " + e);
            System.exit(1);
        }

        try {
            DataInputStream is = new DataInputStream(
                                 new BufferedInputStream(sock.getInputStream()));
            PrintStream os = new PrintStream(
                             new BufferedOutputStream(sock.getOutputStream(), 1024), false);
        
            String inputLine=null, outputLine = "\n";


String command = "blah";

while( command != null && !command.startsWith("bye") ) {
StringTokenizer st1 = null;

inputLine = is.readLine().trim();

if(inputLine != null) {
	System.out.println("Got: "+inputLine);
	st1 = new StringTokenizer(inputLine, "::");
	}

if(st1 != null && st1.hasMoreTokens()) {

command = st1.nextToken();

if (command.startsWith("request")) {
	if (st1.hasMoreTokens() && Long.valueOf( st1.nextToken()).longValue() < lastChange )
		outputLine = L.toDataString();
	else
		outputLine = "none";
            os.println(outputLine);
            os.flush();
}

if(command.startsWith("update")) {

	if (L.tryScore( st1.nextToken() ) != null)
		lastChange = System.currentTimeMillis();
	}
}
}

            os.close();
            is.close();
            sock.close();


        } catch (Exception e) {
            e.printStackTrace();
        }
}
}

public void finalize() {
	try {
		servSock.close();
	} catch(Exception e);
	servSock = null;
}


	
}

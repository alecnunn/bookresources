import java.net.*;
import java.lang.*;
import java.io.*;
import java.util.*;
import GameGroup;

public class GameClientThread extends Thread {
GameGroup parent;
Socket theSock;
DataInputStream dis;
PrintStream ps;
String alias;

GameClientThread(Socket s, GameGroup p) {
theSock = s;
parent = p;
}

public void run() {

	try {
		dis = new DataInputStream( new BufferedInputStream( theSock.getInputStream()));
            		ps = new PrintStream( theSock.getOutputStream());
	} catch (Exception e) {
		stop();
	}


while (theSock !=null) {
String input = null;
try {
input = dis.readLine().trim();
if(input != null) {
	parent.output(input);         
	if(input.equals("bye"))
		stop();
	}
} catch (Exception e) {
	stop();
	theSock = null;
}
try{ sleep(100); } catch(Exception e);
}
}

public boolean message(String str) {
boolean flag = false;

while (!flag)
try {
	ps.println(str);
	flag = true;
} catch (Exception e) {
	flag = false;
}
return true;
}

public void finalize() {
	try {
		ps.close();
		dis.close();
		theSock.close();
	} catch(Exception e);
theSock = null;

}

public void setAlias(String str) {
alias = str;
}

public String getAlias() {

return alias;
}
	
}

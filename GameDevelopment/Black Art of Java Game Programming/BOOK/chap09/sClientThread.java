import java.net.*;
import java.lang.*;
import java.io.*;
import java.util.*;
import sClientGroup;

public class sClientThread extends Thread {
sClientGroup parent;
Socket theSock;
DataInputStream dis;
PrintStream ps;
String alias;

sClientThread(Socket s, sClientGroup p) {
theSock = s;
parent = p;
}

public void run() {

	try {
		dis = new DataInputStream( theSock.getInputStream());
            		ps = new PrintStream( theSock.getOutputStream());
	} catch (Exception e);


while (theSock !=null) {
String input = null;
try {
input = dis.readLine().trim();
if(input != null)
	parent.handleInput(input, this);         

} catch (Exception e);
}
}

public boolean message(String str) {
try {
	ps.println(str);
} catch (Exception e) {
	return false;
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

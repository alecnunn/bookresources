import java.net.*;
import java.io.*;
import java.util.*;
import java.lang.*;

import sClientThread;

public class sClientGroup extends Thread {
Vector theGroup;


sClientGroup() {
theGroup = new Vector();
}

public void addClient(Socket s) {
sClientThread tempThread;
	tempThread = new sClientThread( s, this );
	theGroup.addElement(tempThread);
	tempThread.start();
}

public void run() {

while( true ) {
try{ sleep(30000); } catch (Exception e);
cleanHouse();
}

}

/* send a message "msg", of type "type", to all Clients */
public void sendMessage(String msg, String type) { 
int x;

for(x=0; x<theGroup.size(); x++) 
	((sClientThread)theGroup.elementAt(x)).message(type+"||"+msg);
/* remember that the format for messages is "type||message" */
}


/* send a message "msg", of type "type", to the Client with alias "target" */
public void sendMessage(String msg, String target, String type) { 
int x;
sClientThread tempThread;

for(x=0; x<theGroup.size(); x++) {
	tempThread=(sClientThread)theGroup.elementAt(x);
	if( tempThread.getAlias().equals(target) )
		tempThread.message(type+"||"+msg);
}
}

/* here is where we handle any input received from a Client */
/* This method is called by sClientThread directly */
public void handleInput(String str, sClientThread T) {
StringTokenizer st;

/* this next line is for debugging only.  You would not include it in the final product */
System.out.println("Got: "+str+" from "+T.getAlias());

/* command to disconnect = "bye" */
if( str.startsWith("bye") ) {
	T.stop();
	return;
	}

st = new StringTokenizer( str, "||");

if(st != null ) {
String cmd, val;

cmd = st.nextToken();
val = st.nextToken();

/* "login" = a new person is logging in.  Set the alias, send a welcome message, and then send everyone an updated list of Client names */
if(cmd.equals("login")) {
	T.setAlias( val );
	sendMessage(T.getAlias()+"||"+T.getAlias()+" has entered the room.", cmd);
	sendMessage(calcList(), "list");
	return ;
	}

/* "logout" = one of our clients is finished and wants to disconnect.  Let everyone know that and then stop the connection.  The garbage collection method will take care of removing them from the list. */
if(cmd.equals("logout")) {
	sendMessage(T.getAlias()+" has left the room.", cmd);
	T.stop();
	return ;
	}

/* someone wants to "say" something to the whole room */
if(cmd.equals("say")) {
	sendMessage(T.getAlias()+" says: "+ val, cmd);
	return ;
	}
/* someone wants to whisper something to a specific person only */
if(cmd.equals("whisper")) {
	sendMessage(T.getAlias()+" whispers to you: "+val,st.nextToken(),cmd);
	return ;
	}
}

}

/* return a list of all currently connected users in the form "name1&name2&name3" */
public String calcList() {
int x;
StringBuffer buf = new StringBuffer();
String temp;
for(x=0; x<theGroup.size(); x++) {
	temp = ((sClientThread) (theGroup.elementAt(x))).getAlias();
	if(temp != null)
		buf.append(temp).append('&');
}
if (buf.length() >0 )
	buf.setLength(buf.length()-1);
return buf.toString();
}
	

/* go through the Vector, and search for "dead" Threads (which are disconnected) and then remove them from the list */
public void cleanHouse() {
int x;
sClientThread tempThread;

for (x=0; x<theGroup.size(); x++) {
	tempThread = (sClientThread)theGroup.elementAt(x);
	if( tempThread==null ||  ! tempThread.isAlive() )
		 theGroup.removeElement( tempThread );
	}
}

public void finalize() {
Thread t;

for( int x=0;x<theGroup.size();x++) {
	t = (Thread)theGroup.elementAt(x);
	if( t!=null && t.isAlive() )
		t.stop();
	}
}

/* END OF CLASS */
}

import java.net.*;
import java.io.*;
import HighScoreList;
import ServerThread;
import SaveFileThread;

class HighScoreServer{

public static void main(String args[]) {

HighScoreList theList = null;
File theFile = new File("data.txt");
String temp = null;

if (!theFile.exists())
	System.out.println("No such file\nWill create a new one");
else {
try {
	DataInputStream dis = new DataInputStream( new FileInputStream( theFile) );
	temp = dis.readLine();
} catch (Exception e) {
System.out.println("Unable to read from file. Terminating...");
System.exit(1);
}
}
if(temp != null)
	theList = new HighScoreList(Integer.valueOf(args[0]).intValue(),temp);	
else
	theList = new HighScoreList(Integer.valueOf(args[0]).intValue());

if(theList == null) {
	System.out.println("Unable to initzialize. Terminating...");
	System.exit(1);
}
	new ServerThread(theList).start();
	new SaveFileThread(theList, theFile).start();
System.out.println("Server initialized.  Tracking "+args[0]+" scores.\nUsing "+args[1]);
}

}

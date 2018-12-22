import java.net.*;
import java.lang.*;
import java.io.*;

public class inThread extends Thread {
DataInputStream dis;
Object parent;

inThread(DataInputStream d, Object p) {
	dis = d;
	parent = p;
}

public void run() {
String str;
	while (dis != null) {
	str = null;
		try {
			str = dis.readLine();
		} catch (IOException e) {
			try {
			dis.close();
			} catch (Exception e2);
			dis = null;
		}
		if(str != null)
			parent.handleInput(str);
	}
}
		
}
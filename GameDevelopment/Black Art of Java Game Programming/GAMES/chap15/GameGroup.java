import java.lang.*;
import java.net.*;
import java.io.*;
import java.util.*;
import GameClientThread;

public class GameGroup extends Thread {

GameClientThread arr[];
final int SIZE=2;

GameGroup ( Socket s ) {
	arr = new GameClientThread[SIZE];		
	addClient( s );
}

public void addClient( Socket s ) {
int x;

for( x=0; x<SIZE; x++) 
	if( arr[x] == null || !arr[x].isAlive() ) {
		arr[x] = new GameClientThread(s,this);
		arr[x].start();
		return ;
		}

}

public void run() {
int x;

arr[0].message("start||white");
arr[1].message("start||black");
	

while( true ) {
	for(x=0;x<SIZE;x++) 
		if( arr[x] == null || !arr[x].isAlive() ) {
			output("bye");
			stop();
			} 
try{ sleep( 10000 ); } catch(Exception e);
}
}

public void finalize() {
int x;

output("bye");
try {
for(x=0; x<SIZE; x++) 
	if( arr[x] != null ) {
		arr[x].stop();
		}
} catch(Exception e);

}

public void output(String str) {
int x;

for(x=0;x<SIZE;x++) 
	if(arr[x] != null)
		arr[x].message(str);
	
}

public boolean full() {
int x;

for(x=0;x<SIZE;x++)
	if( arr[x] == null )
		return false;
return true;
}

}
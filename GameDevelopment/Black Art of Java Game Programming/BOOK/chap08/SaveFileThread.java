import java.lang.*;
import java.io.*;
import HighScoreList;

public class SaveFileThread extends Thread {
HighScoreList L;
final int FILE_DELAY = 100000;
File f;

SaveFileThread(HighScoreList theList, File theFile) {
L = theList;
f = theFile;
}

public void run() {

while( L != null ) {



if(f.exists())
	f.renameTo(new File("test.bak"));

try {
PrintStream ps = new PrintStream(new FileOutputStream(f));
ps.println( L.toDataString() );
System.out.println("Writing High Scores to file "+f.getName());
ps.close();
} catch (Exception e);

try{ sleep(FILE_DELAY); } catch (Exception e);
}
}

}
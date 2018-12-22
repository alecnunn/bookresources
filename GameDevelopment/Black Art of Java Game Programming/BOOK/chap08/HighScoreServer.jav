
import java.net.*;
import java.io.*;

class HighScoreServer {
    public static void main(String args[]) {
        ServerSocket servSock = null;

        try {
            servSock = new servSock(2357);
        } catch (IOException e) {
            System.out.println("Could not listen on port: " + 2357 + ", " + e);
            System.exit(1);
        }

        Socket clientSocket = null;
        try {
            clientSocket = servSock.accept();
        } catch (IOException e) {
            System.out.println("Accept failed: " + 2357 + ", " + e);
            System.exit(1);
        }

        try {
            DataInputStream is = new DataInputStream(
                                 new BufferedInputStream(clientSocket.getInputStream()));
            PrintStream os = new PrintStream(
                             new BufferedOutputStream(clientSocket.getOutputStream(), 1024), false);
        
            String inputLine, outputLine;

            outputLine = "Eric&1000&eries@netcom.com||Dude&2&blah@cool||Darn&0&darn@lame.com||THeGuy&24235&bhffh||Whoever&234";

            os.println(outputLine);
            os.flush();

            os.close();
            is.close();
            clientSocket.close();
            servSock.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

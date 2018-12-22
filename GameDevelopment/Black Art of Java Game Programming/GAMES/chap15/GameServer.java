import GameServerThread;

class GameServer {
public static void main(String args[]) {
int thePort;

try{
	thePort = Integer.valueOf(args[0]).intValue();
} catch (Exception e) {
	thePort = 2001;
}

new GameServerThread(thePort).start();
}
}
package server;

import java.net.*;
import java.io.*;
import java.util.Date;

import common.Config;

class FullServerException extends Exception {}

class Server {
  public static ServerSocket main_sock = null;

  public static void main (String args[]) {
    try {
      PlayerRecord.init(Config.server_dbase);
    } catch (IOException e) {
      System.out.println(e);
      System.exit(1);
    }
    try {
      int server_port = Config.server_port;
      if (args.length > 0) {
	try {
	  server_port = Integer.parseInt(args[0]);
	} catch (NumberFormatException e) {
	}
      }
      main_sock = new ServerSocket(server_port);
    } catch (IOException e) {
      System.out.println("Can't listen on port.");
      return;
    }
    new WatchDog(80000).start();
    System.out.println("MahJong Server ready for connections, " + new Date());
    for (;;) {
      try {
	Socket s = main_sock.accept();
	DataInputStream i = new DataInputStream(s.getInputStream());
	DataOutputStream o = new DataOutputStream(s.getOutputStream());
	System.out.println("Got connection...");
	(new Player(s, i, o)).start();
      } catch (Exception e) {
	System.out.println("Failed to accept connection, " + new Date());
      }
    }
  }
}


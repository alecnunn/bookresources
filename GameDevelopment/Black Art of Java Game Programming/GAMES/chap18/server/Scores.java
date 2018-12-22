package server;

import java.util.Date;
import java.io.IOException;

import common.Config;

public class Scores {
  public static void main (String args[]) {
    StringBuffer buf = new StringBuffer(500);
    try {
      PlayerRecord.highscores(Config.server_dbase, buf, 10);
    } catch (IOException e) {
      System.out.println(e);
      System.exit(1);
    }
    System.out.println("<h2>Top 10 scorers as of " + new Date() + "</h2>");
    System.out.println("<PRE>");
    System.out.print(buf.toString());
    System.out.println("</PRE>");
  }
}

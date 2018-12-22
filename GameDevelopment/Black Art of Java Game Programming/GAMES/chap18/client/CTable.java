package client;

import common.TableOptions;
import common.Config;

class CTable {
  public static String seat_names[] = { "EAST", "SOUTH", "WEST", "NORTH" };

  public int id = -1;
  public CPlayer players[] = new CPlayer[Config.max_table_players];
  public TableOptions opt;
  public String print_text = null;

  public static int nextTurn (int s) {
    return (s + 3) % 4;
  }

  public static int roundWind (int round) {
    return (round + 3) % 4;
  }

  public static int seatWind (int round, int dealer, int seat) {
    return (7 + round - seat + dealer) % 4;
  }

  public String toString () {
    StringBuffer buf = new StringBuffer(80);
    buf.append('<').append(id).append(">:");
    for (int i = 7 - buf.length(); i > 0; i--) buf.append(' ');
    buf.append(opt.toString());
    for (int i = 16 - buf.length(); i > 0; i--) buf.append(' ');
    buf.append("  players: ");
    int n = buf.length();
    for (int s = 0; s < 4; s++) {
      if (players[s] != null) {
	buf.append('[').append(players[s].id).append(']');
      }
      for (int i = n + (s + 1) * 5 - buf.length(); i > 0; i--) buf.append(' ');
    }
    int m = 0;
    for (int s = 4; s < players.length; s++) {
      if (players[s] != null) {
	m++;
      }
    }
    if (m != 0) {
      buf.append("" + m + " spec.\n");
    } else {
      buf.append("\n");
    }
    return print_text = buf.toString();
  }
}


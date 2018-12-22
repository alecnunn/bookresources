package common;

import java.io.*;

public class Packet extends DataOutputStream {
  ByteArrayOutputStream b_out;

  public Packet (ByteArrayOutputStream buf) {
    super(buf);
    b_out = buf;
  }
  
  private byte[] buf () {
    return b_out.toByteArray();
  }

  private static Packet New (int size) {
    ByteArrayOutputStream buf = new ByteArrayOutputStream(size);
    return new Packet(buf);
  }

  public static Hand readHand (DataInputStream S, Hand h) throws IOException {
    int len = S.readInt(), i = 0;
    if (h == null) {
      h = new Hand(len);
    }
    h.setLength(0);
    for (i = 0; i < len; i++) {
      byte v = S.readByte();
      byte g = S.readByte();
      boolean s = (g & (byte)16) != 0;
      h.addTile(v, (byte)(g & 15), s);
    }
    return h;
  }

  public static void writeHand (DataOutputStream S, Hand h) throws IOException {
    int i = 0;
    S.writeInt(h.length());
    for (i = 0; i < h.length(); i++) {
      S.writeByte(h.tileAt(i));
      S.writeByte(h.groupAt(i) | (byte)(h.isSelected(i) ? 16 : 0));
    }
  }

  // Server packet types.
  public static final byte SP_YOU_ARE = 0;
  public static final byte SP_LOGIN = 1;
  public static final byte SP_JOIN = 2;
  public static final byte SP_LEAVE = 3;
  public static final byte SP_TABLE_OPT = 4;
  public static final byte SP_TABLE_PLAYER = 5;
  public static final byte SP_MESSAGE = 6;
  public static final byte SP_GAME = 7;
  public static final byte SP_TURN = 8;
  public static final byte SP_STAT = 9;
  public static final byte SP_DISCARD = 10;
  public static final byte SP_TILES = 11;
  public static final byte SP_YOUR_PLAY = 12;
  public static final byte SP_YOUR_PRIVATE = 13;
  public static final byte SP_YOUR_DRAW = 14;
  public static final byte SP_OUT_TILES = 15;
  public static final byte SP_WIN = 16;
  public static final byte SP_WHO_PLAYED = 17;
  public static final byte SP_YOUR_CHOICES = 18;
  public static final byte SP_URGE = 19;
  public static final byte SP_PREFERENCE = 20;
  public static final byte SP_FINGER = 21;

  // Client packet types.
  public static final byte CP_LOGIN = 0;
  public static final byte CP_MESSAGE = 1;
  public static final byte CP_TABLE_JOIN = 2;
  public static final byte CP_TABLE_SERVE = 3;
  public static final byte CP_PLAY = 4;
  public static final byte CP_QUIT = 5;
  public static final byte CP_PING = 6;
  public static final byte CP_URGE = 7;
  public static final byte CP_PREFERENCE = 8;
  public static final byte CP_FINGER = 9;
  public static final byte CP_CHANGE_FINGER = 10;
  public static final byte CP_CHANGE_PASSWD = 11;

  public static byte[] SPYouAre (int id) {
    try {
      Packet p = New(2);
      p.writeByte(SP_YOU_ARE);
      p.writeByte(id);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final byte LOGIN_OK = 0;
  public static final byte LOGIN_TWICE = 1;
  public static final byte LOGIN_BADPASS = 2;
  public static final byte LOGIN_EXIST = 3;
  public static final byte LOGIN_NONEXIST = 4;

  public static byte[] SPLogin (byte result) {
    try {
      Packet p = New(2);
      p.writeByte(SP_LOGIN);
      p.writeByte(result);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPJoin (int p_id, String name, String host, int bal, boolean join) {
    try {
      Packet p = New(16);
      p.writeByte(SP_JOIN);
      p.writeByte(p_id);
      p.writeUTF(name);
      p.writeUTF(host);
      p.writeInt(bal);
      p.writeBoolean(join);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final String[] leave_reasons = {
    "disconnected",
    "connection timed out",
    "connection severed on read",
    "connection severed on write",
    "garbage read in",
  };

  public static byte[] SPLeave (int p_id, int reason) {
    try {
      Packet p = New(3);
      p.writeByte(SP_LEAVE);
      p.writeByte(p_id);
      p.writeByte(reason);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPTableOpt (int t_id, TableOptions opt) {
    try {
      Packet p = New(10);
      p.writeByte(SP_TABLE_OPT);
      p.writeByte(t_id);
      opt.write(p);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPTablePlayer (int t_id, int seat, int p_id) {
    try {
      Packet p = New(4);
      p.writeByte(SP_TABLE_PLAYER);
      p.writeByte(t_id);
      p.writeByte(seat);
      p.writeByte(p_id);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final byte MSG_GOD = 0;
  public static final byte MSG_TABLE = 1;
  public static final byte MSG_ALL = 2;

  public static byte[] SPMessage (byte type, int p_id, String text) {
    try {
      Packet p = New(16);
      p.writeByte(SP_MESSAGE);
      p.writeByte(type);
      p.writeByte(p_id);
      p.writeUTF(text);
      return p.buf();
    } catch (IOException e) { return null; }
  }
  
  public static final byte GAME_START = 0;
  public static final byte GAME_END = 1;
  public static final byte GAME_PROGRESS = 2;

  public static byte[] SPGame (int p_id, int round, int seat, byte action) {
    try {
      Packet p = New(5);
      p.writeByte(SP_GAME);
      p.writeByte(p_id);
      p.writeByte(round);
      p.writeByte(seat);
      p.writeByte(action);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final byte TURN_NONE = 0;
  public static final byte TURN_DRAW = 1;
  public static final byte TURN_GRAB = 2;
  public static final byte TURN_FOURTH = 3;

  public static byte[] SPTurn (byte action, int seat) {
    try {
      Packet p = New(3);
      p.writeByte(SP_TURN);
      p.writeByte(action);
      p.writeByte(seat);
      return p.buf();
    } catch (IOException e) { return null; }
  }
  
  public static byte[] SPStat (int p_id, int bal) {
    try {
      Packet p = New(6);
      p.writeByte(SP_STAT);
      p.writeByte(p_id);
      p.writeInt(bal);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final byte DISCARD_DISCARD = 0;
  public static final byte DISCARD_FOURTH = 1;

  public static byte[] SPDiscard (byte tile, byte action ) {
    try {
      Packet p = New(3);
      p.writeByte(SP_DISCARD);
      p.writeByte(tile);
      p.writeByte(action);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPTiles (int p_id, int seat, int n_priv, Hand h1, Hand h2, boolean ss) {
    try {
      Packet p = New(16);
      p.writeByte(SP_TILES);
      p.writeByte(p_id);
      p.writeByte(seat);
      p.writeByte(n_priv);
      Hand h = new Hand(h1.length() + h2.length());
      h.append(h1);
      if (ss) {
	h.append(h2);
      } else {
	for (int i = 0; i < h2.length(); i++) {
	  h.addTile((byte)0, h2.groupAt(i), false);
	}
      }
      writeHand(p, h);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final byte YPLAY_DISCARD = 0;
  public static final byte YPLAY_FOURTH = 1;
  public static final byte YPLAY_BID = 2;
  public static final byte YPLAY_CLAIM = 3;

  public static final String[] play_errors = {
    "Illegal play",
    "You cannot play twice in one turn!",
    "You do not have that tile!",
    "You must already have a triplet of those tiles!",
    "You do not have enough tiles to make a quadruplet!",
    "You are not the next in turn!",
    "You cannot make a connection that way!",
    "You cannot make a triplet that way!",
  };    

  public static byte[] SPYourPlay (byte action, boolean ok, int reason) {
    try {
      Packet p = New(4);
      p.writeByte(SP_YOUR_PLAY);
      p.writeByte(action);
      p.writeBoolean(ok);
      p.writeByte(reason);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPYourPrivate (Hand h) {
    try {
      Packet p = New(16);
      p.writeByte(SP_YOUR_PRIVATE);
      writeHand(p, h);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPYourDraw (byte tile) {
    try {
      Packet p = New(2);
      p.writeByte(SP_YOUR_DRAW);
      p.writeByte(tile);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPOutTiles (int p_id, int seat, Hand h) {
    try {
      Packet p = New(16);
      p.writeByte(SP_OUT_TILES);
      p.writeByte(p_id);
      p.writeByte(seat);
      writeHand(p, h);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPWin (int p_id, int seat, int score, int feeder) {
    try {
      Packet p = New(8);
      p.writeByte(SP_WIN);
      p.writeByte(p_id);
      p.writeByte(seat);
      p.writeInt(score);
      p.writeByte(feeder);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPWhoPlayed (byte actions[]) {
    int flags = 0;
    for (int i = 0; i < 4; i++) {
      if (actions[i] != -1)
	flags |= (1 << i);
    }
    try {
      Packet p = New(2);
      p.writeByte(SP_WHO_PLAYED);
      p.writeByte(flags);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPYourChoices (int flags, int win_score) {
    try {
      Packet p = New(9);
      p.writeByte(SP_YOUR_CHOICES);
      p.writeInt(flags);
      p.writeInt(win_score);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPUrge () {
    byte[] buf = { SP_URGE };
    return buf;
  }

  public static byte[] SPPreference (String pref) {
    try {
      Packet p = New(16);
      p.writeByte(SP_PREFERENCE);
      p.writeUTF(pref);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] SPFinger (int p_id, String name, String mail, String home, String plan) {
    try {
      Packet p = New(16);
      p.writeByte(SP_FINGER);
      p.writeByte(p_id);
      p.writeUTF(name);
      p.writeUTF(mail);
      p.writeUTF(home);
      p.writeUTF(plan);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final byte LOGIN_NEW = 0;
  public static final byte LOGIN_OLD = 1;

  public static byte[] CPLogin (byte action, String name, String passwd) {
    try {
      Packet p = New(16);
      p.writeByte(CP_LOGIN);
      p.writeByte(action);
      p.writeUTF(name);
      p.writeUTF(passwd);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] CPMessage (byte type, String text) {
    try {
      Packet p = New(16);
      p.writeByte(CP_MESSAGE);
      p.writeByte(type);
      p.writeUTF(text);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final int TABLE_LEAVE = -1;
  public static final int JOIN_PLAY = 0;
  public static final int JOIN_SPEC = 1;

  public static byte[] CPTableJoin (int t_id, byte mode) {
    try {
      Packet p = New(3);
      p.writeByte(CP_TABLE_JOIN);
      p.writeByte(t_id);
      p.writeByte(mode);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] CPTableServe (TableOptions opt) {
    try {
      Packet p = New(16);
      p.writeByte(CP_TABLE_SERVE);
      opt.write(p);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static final byte PLAY_PASS = 0;
  public static final byte PLAY_DRAW = 1;
  public static final byte PLAY_CONN_UP = 2;
  public static final byte PLAY_CONN_MD = 3;
  public static final byte PLAY_CONN_DN = 4;
  public static final byte PLAY_TRIP = 5;
  public static final byte PLAY_QUAD = 6;
  public static final byte PLAY_DISCARD = 7;
  public static final byte PLAY_FOUR = 8;
  public static final byte PLAY_WIN = 9;

  public static byte[] CPPlay (byte action, byte tile) {
    try {
      Packet p = New(3);
      p.writeByte(CP_PLAY);
      p.writeByte(action);
      p.writeByte(tile);
      return p.buf();
    } catch (IOException e) { return null; }
  }
   
  public static byte[] CPQuit () {
    byte[] buf = { CP_QUIT };
    return buf;
  }

  public static byte[] CPPing () {
    byte[] buf = { CP_PING };
    return buf;
  }

  public static byte[] CPUrge () {
    byte[] buf = { CP_URGE };
    return buf;
  }

  public static byte[] CPPreference (String pref) {
    try {
      Packet p = New(16);
      p.writeByte(CP_PREFERENCE);
      p.writeUTF(pref);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] CPFinger (int p_id) {
    try {
      Packet p = New(2);
      p.writeByte(CP_FINGER);
      p.writeByte(p_id);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] CPChangeFinger (String mail, String home, String plan) {
    try {
      Packet p = New(16);
      p.writeByte(CP_CHANGE_FINGER);
      p.writeUTF(mail);
      p.writeUTF(home);
      p.writeUTF(plan);
      return p.buf();
    } catch (IOException e) { return null; }
  }

  public static byte[] CPChangePasswd (String passwd) {
    try {
      Packet p = New(16);
      p.writeByte(CP_CHANGE_PASSWD);
      p.writeUTF(passwd);
      return p.buf();
    } catch (IOException e) { return null; }
  }

}

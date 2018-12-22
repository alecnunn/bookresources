package server;

import java.net.*;
import java.io.*;
import java.util.Date;
import java.util.StringTokenizer;

import common.*;
import types.Queue;

class PlayerOutput extends Thread {
  private static Player the_player;
  private DataOutputStream out;
  private Queue pkt_queue = new Queue();

  public PlayerOutput (Player p, DataOutputStream S) {
    the_player = p;
    out = S;
  }

  public void output (byte p[]) {
    pkt_queue.push(p);
  }

  public void run () {
  loop:
    while (true) {
      byte p[] = (byte[])pkt_queue.pop();
      if (p != null && p.length > 0) {
	try {
	  out.write(p, 0, p.length);
	} catch (IOException e) {
	  break loop;
	}
      }
    }
    the_player.StopThread();
    System.out.println("OutputThread: ghostbust " + the_player.id + "...");
    the_player.GB(3);
    the_player.Disconnect();
  }
}

class Player extends Thread {
  public static Player global[] = new Player[Config.max_players];

  public boolean connected = true;
  public int id = -1;
  public PlayerRecord stat = null;
  public boolean ping = true;
  public PlayerOutput out_thread;

  private Socket sock;
  private DataInputStream in;
  private DataOutputStream out;
  private Integer doing_work = new Integer(0);

  public String name = null;
  public String host = null;
  public Table table = null;
  public int seat = -1;

  private int leave_reason = 0;

  public Player (Socket s, DataInputStream in, DataOutputStream out) throws FullServerException {
    sock = s;
    this.in = in;
    this.out = out;
    host = s.getInetAddress().getHostName();
    System.out.println("Find slot...");
    synchronized (global) {
      for (int i = 0; i < global.length; i++) {
	if (global[i] == null) {
	  id = i;
	  global[i] = this;
	  break;
	}
      }
      if (id == -1) throw new FullServerException();
    }
    (out_thread = new PlayerOutput(this, out)).start();
    output(Packet.SPYouAre(id));
  }

  public static void outputAll (byte[] p) {
    System.out.print("Sending packet to all...");
    synchronized (global) {
      for (int i = 0; i < global.length; i++) {
	Player him = global[i];
	if (him != null && him.stat != null && him.connected) {
	  him.output(p); 
	}
      }
    }
    System.out.println("done.");
  }

  public void output (byte[] p) {
    if (connected) {
      out_thread.output(p);
    }
  }

  public synchronized void Disconnect () {
    if (id == -1)
      return;
    connected = false;
    if (table != null) {
      table.Leave(this);
    }
    synchronized (global) {
      global[id] = null;
    }
    if (stat != null) {
      PlayerRecord.updateAll(stat);
      outputAll(Packet.SPLeave(id, leave_reason));
      stat = null;
    }
    try {
      in.close();
      out.close();
      sock.close();
    } catch (IOException e) {
    }
    System.out.println("Disconnected: id = " + id + ", reason = " + leave_reason);
    id = -1;
  }

  public void StopThread () {
    connected = false;
    synchronized (doing_work) {
      stop();
    }
  }

  public void GB (int reason) {
    connected = false;
    leave_reason = reason;
  }

  public void run () {
    int n_pings = 0;
  loop:
    while (connected) {
      try {
	byte type = in.readByte();
//	System.out.println("Packet[" + type + "]");
	synchronized (doing_work) {  // so that StopThread() won't miss it.
	  ping = true;
	  switch (type) {
	  case Packet.CP_QUIT:
	    System.out.println("Quit: id = " + id);
	    GB(0);
	    break loop;
	  case Packet.CP_URGE:
	    if (table != null && seat < 4 && table.AllSeated()) {
	      table.urge(this);
	    } else {
	      output(Packet.SPMessage(Packet.MSG_GOD, 0, "Chill out!"));
	    }
	    break;
	  case Packet.CP_PING:
	    if (table != null && seat < 4 && table.AllSeated() && 
		table.actions[seat] == -1 && table.action_masks[seat] != 0) {
	      n_pings++;
	      if (n_pings == 2 || n_pings == 3) {
		output(Packet.SPUrge());
	      }
	      if (n_pings >= 4) {
		table.Leave(this);
	      }
	    } else {
	      n_pings = 0;
	    }
	    break;
	  case Packet.CP_LOGIN:
	    hdLogin();
	    break;
	  case Packet.CP_MESSAGE:
	    n_pings = 0;
	    hdMessage();
	    break;
	  case Packet.CP_TABLE_JOIN:
	    n_pings = 0;
	    hdTableJoin();
	    break;
	  case Packet.CP_TABLE_SERVE:
	    n_pings = 0;
	    hdTableServe();
	    break;
	  case Packet.CP_PLAY:
	    n_pings = 0;
	    hdPlay();
	    break;
	  case Packet.CP_PREFERENCE:
	    hdPref();
	    break;
	  case Packet.CP_FINGER:
	    hdFinger();
	    break;
	  case Packet.CP_CHANGE_FINGER:
	    hdChangeFinger();
	    break;
	  case Packet.CP_CHANGE_PASSWD:
	    hdChangePasswd();
	    break;
	  default:
	    System.out.println("Unknown Packet: id = " + id + ", type = " +
			       type + ", " + new Date());
	    GB(4);
	    break loop;
	  }
	}
      }
      catch (IOException e) {
	GB(2);
	break loop;
      }
    }
    out_thread.stop();
    System.out.println("RunThread: ghostbust " + id + "...");
    Disconnect();
  }

  // Packet handlers go here.

  private void hdLogin () throws IOException {
    byte action = in.readByte();
    String l_name = in.readUTF();
    String l_passwd = in.readUTF();

    PlayerRecord S = null;
    if (l_name.length() == 0) {
      output(Packet.SPLogin(Packet.LOGIN_TWICE));
      return;
    }
    switch (action) {
    case Packet.LOGIN_NEW:
      if ((S = PlayerRecord.create(l_name, l_passwd, 2000)) == null) {
	output(Packet.SPLogin(Packet.LOGIN_EXIST));
	return;
      }
      break;
    case Packet.LOGIN_OLD:
      if ((S = PlayerRecord.query(l_name)) == null) {
	output(Packet.SPLogin(Packet.LOGIN_NONEXIST));
	return;
      }
      if (l_passwd.length() > PlayerRecord.NAME_LEN &&
	  !l_passwd.regionMatches(0, S.passwd, 0, S.passwd.length()) ||
	  !l_passwd.equals(S.passwd)) {
	output(Packet.SPLogin(Packet.LOGIN_BADPASS));
	return;
      }
      System.out.println("Looking for name [" + S.name + "]...");
      synchronized (global) {
	for (int i = 0; i < global.length; i++) {
	  if ((global[i] != null) && S.name.equals(global[i].name)) {
	    output(Packet.SPLogin(Packet.LOGIN_TWICE));
	    return;
	  }
	}
      }
      break;
    }
    System.out.println("Login: id = " + id + ", [" + S.name + "], " + new Date());

    // the orders here are important.
    if (!S.preferences.equals(""))
      output(Packet.SPPreference(S.getPref()));
    output(Packet.SPLogin(Packet.LOGIN_OK));

    System.out.println("Reporting all players to " + id + "...");
    synchronized (global) {
      for (int i = 0; i < global.length; i++) {
	Player him = global[i];
	if (him != null && him.stat != null && i != id) {
	  output(Packet.SPJoin(i, him.name, him.host, 
			       him.stat.getBalance(), false));
	}
      }
    }

    System.out.println("Reporting all tables to " + id + "...");
    synchronized (Table.global) {
      for (int i = 0; i < Table.global.length; i++) {
	Table t = Table.global[i];
	if (t != null) {
	  output(Packet.SPTableOpt(i, t.opt));
	  t.Update(this);
	}
      }
    }
    System.out.println("Finsihed login sequence for " + id + ".");
    name = S.name;
    stat = S;
    outputAll(Packet.SPJoin(id, name, host, S.getBalance(), true));
    output(Packet.SPMessage(Packet.MSG_GOD, 0, "Welcome to the Internet MahJong Server!"));
  }

  void hdMessage () throws IOException {
    byte type = in.readByte();
    String text = in.readUTF();

    switch (type) {
    case Packet.MSG_TABLE:
      if (table != null) {
	table.outputTable(Packet.SPMessage(Packet.MSG_TABLE, id, text));
      }
      break;
    case Packet.MSG_ALL:
      outputAll(Packet.SPMessage(Packet.MSG_ALL, id, text));
      break;
    case Packet.MSG_GOD:
      if (text.length() == 0)
	break;
      if (text.charAt(0) == '#') {
/*
	Hand h = new Hand(text.substring(1, text.length()));
	if (table != null) {
	  table.pri_hands[seat] = h;
	  table.outputHands(this, seat, true);
	}
	System.out.println("Hand: " + h.toString());
*/
      } else if (text.charAt(0) == '/') {            // commands
	StringTokenizer st = new StringTokenizer(text);
	String token = st.nextToken();
      all_cases: 
	{
	  if (token.equals("/serve")) {
	    if (table != null) {
	      output(Packet.SPMessage(Packet.MSG_GOD, 0, 
				      "You must leave a table first!"));
	      return;
	    }
	    try {
	      table = new Table(new TableOptions(), true, this);
	    } catch (FullServerException e) {
	      output(Packet.SPMessage(Packet.MSG_GOD, 0,
				      "Cannot create a new table."));
	    }
	    return;
	  }
	  else if (token.equals("/invite")) {
	    boolean add = true;
	    int i = 0, invited = 0;
	    Player him = null;
	    if (table == null) {
	      output(Packet.SPMessage(Packet.MSG_GOD, 0, 
				      "You are not on a table!"));
	      return;
	    }
	    if (seat > 3) {
	      output(Packet.SPMessage(Packet.MSG_GOD, 0,
				      "Spectators cannot invite players!"));
	      return;
	    }
	    while (st.hasMoreTokens()) {
	      token = st.nextToken();
	      if (token.charAt(0) == '-') {
		i = 1;
		add = false;
	      } else if (token.charAt(0) == '+') {
		i = 1;
	      }
	      try {
		invited = Integer.parseInt(token.substring(i));
	      } catch (NumberFormatException e) {
		output(Packet.SPMessage(Packet.MSG_GOD, 0,
					"Bad command format."));
		return;
	      }
	      table.Invite(this, invited, add);
	    }
	    output(Packet.SPMessage(Packet.MSG_GOD, 0, "Invited players:" +
				    table.GetInvited()));
	    return;
	  }
	}
	output(Packet.SPMessage(Packet.MSG_GOD, 0, "Unrecognized command."));
      } else {
	System.out.println("Log (" + name + "):: " + text);
      }
      break;
    }
  }
  
  void hdTableJoin () throws IOException {
    int t_id = (int)in.readByte();
    byte mode = in.readByte();
    
    if (t_id == Packet.TABLE_LEAVE) {  // leave
      if (table == null) {
	output(Packet.SPMessage(Packet.MSG_GOD, 0, "You are not on a table."));
	return;
      }
      table.Leave(this);
    } else {  // join
      if (table != null) {
	output(Packet.SPMessage(Packet.MSG_GOD, 0, 
				"You must leave a table first!"));
	return;
      }
      synchronized (Table.global) {
	Table t = null;
	if (t_id < 0 || t_id >= Table.global.length ||
	    (t = Table.global[t_id]) == null) {
	  output(Packet.SPMessage(Packet.MSG_GOD, 0, "Invalid table ID."));
	  return;
	}
	t.Join(this, mode);
      }
    }
  }
  
  void hdTableServe () throws IOException {
    TableOptions opt = new TableOptions(in);

    if (table == null) {              // serve new table
      try {
	table = new Table(opt, false, this);
      } catch (FullServerException e) {
	output(Packet.SPMessage(Packet.MSG_GOD, 0, "Server is full."));
      }
    } else {                          // change table options
      table.SetOptions(opt, this);
    }
  }

  void hdPlay () throws IOException {
    byte action = in.readByte();
    byte tile = in.readByte();

    if (table == null) {
      output(Packet.SPMessage(Packet.MSG_GOD, 0, 
			   "You must join a table first."));
      return;
    }
    if (seat < 0 || seat > 3) {
      output(Packet.SPMessage(Packet.MSG_GOD, 0,
			   "You can observe the game only."));
      return;
    }
    table.Play(this, seat, action, tile);
  }
  
  void hdPref () throws IOException {
    String pref = in.readUTF();

    stat.setPref(pref);
    PlayerRecord.updateAll(stat);
  }

  void hdFinger () throws IOException {
    int p_id = (int)in.readByte();
    
    Player him = global[p_id];
    if (him == null) {
      output(Packet.SPMessage(Packet.MSG_GOD, 0, "No such player."));
      return;
    }
    output(Packet.SPFinger(p_id, him.name, him.stat.getMail(), him.stat.getURL(), him.stat.getPlan()));
  }
  
  void hdChangeFinger () throws IOException {
    String mail = in.readUTF();
    String home = in.readUTF();
    String plan = in.readUTF();
    
    stat.setInfo(mail, home, plan);
    PlayerRecord.updateAll(stat);
  }

  void hdChangePasswd () throws IOException {
    String passwd = in.readUTF();
    
    stat.setPasswd(passwd);
    PlayerRecord.updateAll(stat);
  }
}


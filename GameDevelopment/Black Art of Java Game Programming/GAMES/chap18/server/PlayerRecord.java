package server;

import types.RecordFile;
import java.io.*;
import java.util.Hashtable;
import java.util.BitSet;

class PlayerRecord {  
  public static final int NAME_LEN = 20;

  static RecordFile File;
  static Hashtable Hash;
  static PlayerRecord last_rec = null;
  static ByteArrayOutputStream rec_out;

  private int block_number;        // block of this record
  public int balance = 0;          // stat data
  public int reserved1;
  public int reserved2;
  public int reserved3;
  public int reserved4;
  public static final int NAME_DISP = 20;
  public String name;
  public String passwd;
  public String preferences = "";
  public String e_mail = "";
  public String home_url = "";
  public String plan = "";

  public static void init (String f_name) throws IOException {
    File = new RecordFile(f_name);
    Hash = new Hashtable();
    BitSet bs = File.scanBlocks();
    int num_blocks = File.numBlocks();
    for (int i = 0; i < num_blocks; i++) {
      if (!bs.get(i)) {
	Integer block_number = new Integer(i);
	byte[] name_bytes = File.readPartial(i, NAME_DISP, 64);
	String name = new DataInputStream(new ByteArrayInputStream(name_bytes)).readUTF();
	Hash.put(name, block_number);
      }
    }
    rec_out = new ByteArrayOutputStream(NAME_DISP);
  }

  public static void highscores (String f_name, StringBuffer result, int n) throws IOException {
    String names[] = new String[n];
    int scores[] = new int[n];
    File = new RecordFile(f_name);
    BitSet bs = File.scanBlocks();
    int num_blocks = File.numBlocks();
    for (int i = 0; i < num_blocks; i++) {
      if (!bs.get(i)) {
	byte[] stat_bytes = File.readPartial(i, 0, NAME_DISP + 64);
	DataInputStream in = new DataInputStream(new ByteArrayInputStream(stat_bytes));
	int score = in.readInt();
	for (int j = 0; j < n; j++) {
	  if (names[j] == null) {
	    in.skipBytes(16);
	    names[j] = in.readUTF();
	    scores[j] = score;
	    break;
	  }
	  if (score > scores[j]) {
	    for (int k = n - 1; k > j; k--) {
	      names[k] = names[k - 1];
	      scores[k] = scores[k - 1];
	    }
	    in.skipBytes(16);
	    names[j] = in.readUTF();
	    scores[j] = score;
	    break;
	  }
	}
      }
    }
    for (int j = 0; j < n; j++) {
      if (names[j] != null) {
	result.append("  Rank: " + (j + 1) + "   ");
	if (j < 9) result.append(" ");
	result.append(names[j]);
	for (int i = 0; i < 24 - names[j].length(); i++)
	  result.append(" ");
	result.append("Points: " + scores[j] + "\n");
      }
    }
  }

  public static synchronized PlayerRecord query (String name) {
    if (name.length() > NAME_LEN)
      name = name.substring(0, NAME_LEN);
    Object rec_hashobj = Hash.get(name);
    if (rec_hashobj == null) {
//      System.out.println("Query [" + name + "]: not found.");
      return null;
    }
//    System.out.println("Query [" + name + "]: found");
    PlayerRecord rec = new PlayerRecord();
    try {
      rec.block_number = ((Integer)rec_hashobj).intValue();
      byte[] rec_bytes = File.readAll(rec.block_number);
      DataInputStream in = new DataInputStream(new ByteArrayInputStream(rec_bytes));
      rec.balance = in.readInt();
      rec.reserved1 = in.readInt();
      rec.reserved2 = in.readInt();
      rec.reserved3 = in.readInt();
      rec.reserved4 = in.readInt();
      rec.name = in.readUTF();
      rec.passwd = in.readUTF();
      rec.preferences = in.readUTF();
      rec.e_mail = in.readUTF();
      rec.home_url = in.readUTF();
      rec.plan = in.readUTF();
    } catch (IOException e) {
      return null;
    }
    // TBA: replace hashobj by the PlayerRecord?
//    System.out.println("Query [" + name + "]: done");
    return rec;
  }
  
  public static synchronized void updateStat (PlayerRecord rec) {
    if (rec == null)
      return;
    try {
      rec_out.reset();
      DataOutputStream ro = new DataOutputStream(rec_out);
      ro.writeInt(rec.balance);
      ro.writeInt(rec.reserved1);
      ro.writeInt(rec.reserved2);
      ro.writeInt(rec.reserved3);
      ro.writeInt(rec.reserved4);
      File.writePartial(rec.block_number, rec_out.toByteArray(), 0, NAME_DISP);
    } catch (IOException e) {
    }
  }
  
  public static synchronized void updateAll (PlayerRecord rec) {
    if (rec == null)
      return;
    try {
      rec_out.reset();
      DataOutputStream ro = new DataOutputStream(rec_out);
      ro.writeInt(rec.balance);
      ro.writeInt(rec.reserved1);
      ro.writeInt(rec.reserved2);
      ro.writeInt(rec.reserved3);
      ro.writeInt(rec.reserved4);
      ro.writeUTF(rec.name);
      ro.writeUTF(rec.passwd);
      ro.writeUTF(rec.preferences);
      ro.writeUTF(rec.e_mail);
      ro.writeUTF(rec.home_url);
      ro.writeUTF(rec.plan);
      File.writeAll(rec.block_number, rec_out.toByteArray());
    } catch (IOException e) {
    }
  }
  
  public static synchronized PlayerRecord create (String name, String passwd, int balance) {
    if (name.length() > NAME_LEN)
      name = name.substring(0, NAME_LEN);
    if (Hash.get(name) != null)
      return null;
    PlayerRecord rec = new PlayerRecord();
    try {
      rec.block_number = File.newBlock();
      rec_out.reset();
      DataOutputStream ro = new DataOutputStream(rec_out);
      ro.writeInt(rec.balance = balance);
      ro.writeInt(rec.reserved1);
      ro.writeInt(rec.reserved2);
      ro.writeInt(rec.reserved3);
      ro.writeInt(rec.reserved4);
      ro.writeUTF(rec.name = name);
      ro.writeUTF(rec.passwd = passwd);
      ro.writeUTF(rec.preferences);
      ro.writeUTF(rec.e_mail);
      ro.writeUTF(rec.home_url);
      ro.writeUTF(rec.plan);
      File.writeAll(rec.block_number, rec_out.toByteArray());
    } catch (IOException e) {
      return null;
    }
    Hash.put(name, new Integer(rec.block_number));  // TBA: put the PlayerRec?
    return rec;
  }

  public synchronized void addBalance (int n) {
    balance += n;
  }

  public synchronized void subBalance (int n) {
    balance -= n;
  }
  
  public synchronized int getBalance () {
    return balance;
  }

  public synchronized void setPasswd (String pwd) {
    passwd = pwd;
  }

  public synchronized String getPref () {
    return preferences;
  }

  public synchronized void setPref (String pref) {
    preferences = pref;
  }

  public synchronized void setInfo (String m, String h, String p) {
    e_mail = m;
    home_url = h;
    plan = p;
  }
  
  public synchronized String getMail () {
    return e_mail;
  }

  public synchronized String getURL () {
    return home_url;
  }
  
  public synchronized String getPlan () {
    return plan;
  }
}
    

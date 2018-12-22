package server;

import java.io.*;
import types.RecordFile;

class Reformat {
  static final int NAME_LEN = 20;
  static final int REC_LEN = (NAME_LEN * 2 + 1) * 2 + 4 * 6;
  
  public static void main (String args[]) {
    try {
      RandomAccessFile o_file = new RandomAccessFile(args[0], "rw");
      RecordFile n_file = new RecordFile(args[1]);
      long length = o_file.length();
      int n_entries = (int)(o_file.length() / REC_LEN);
      StringBuffer name = new StringBuffer(NAME_LEN);
      StringBuffer passwd = new StringBuffer(NAME_LEN);
      ByteArrayOutputStream buf = new ByteArrayOutputStream(60);
      while (n_entries-- > 0) {
	int str_len = (int)o_file.readByte();
	name.setLength(str_len);
	for (int i = 0; i < str_len; i++) {
	  name.setCharAt(i, o_file.readChar());
	}
	o_file.skipBytes((NAME_LEN - str_len) * 2);
	str_len = (int)o_file.readByte();
	passwd.setLength(str_len);
	for (int i = 0; i < str_len; i++) {
	  passwd.setCharAt(i, o_file.readChar());
	}
	o_file.skipBytes((NAME_LEN - str_len) * 2);
	int score = o_file.readInt() + 2000;
	o_file.skipBytes(20);
	buf.reset();
	DataOutputStream out = new DataOutputStream(buf);
	out.writeInt(score);
	out.writeInt(0);
	out.writeInt(0);
	out.writeInt(0);
	out.writeInt(0);
	out.writeUTF(name.toString());
	out.writeUTF(passwd.toString());
	out.writeUTF("");
	out.writeUTF("");
	out.writeUTF("");
	out.writeUTF("");
	System.out.println("Converting " + name);
	n_file.writeAll(n_file.newBlock(), buf.toByteArray());
      }
    } catch (IOException e) {
      System.out.println(e);
    }
  }
}

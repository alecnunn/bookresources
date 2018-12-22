package common;

import java.io.*;

public class TableOptions {
  public boolean count_flowers;
  public boolean multiple_wins;
  public int min_fan;
  public int max_fan;

  public TableOptions () {
    count_flowers = false;
    multiple_wins = false;
    min_fan = 0;
    max_fan = 13;
  }
  public TableOptions (DataInputStream S) throws IOException {
    read(S);
  }
  public void read (DataInputStream S) throws IOException {
    count_flowers = S.readBoolean();
    multiple_wins = S.readBoolean();
    min_fan = S.readInt();
    max_fan = S.readInt();
  }
  public void write (DataOutputStream S) throws IOException {
    S.writeBoolean(count_flowers);
    S.writeBoolean(multiple_wins);
    S.writeInt(min_fan);
    S.writeInt(max_fan);
  }
  public void copy (TableOptions opt) {
    count_flowers = opt.count_flowers;
    multiple_wins = opt.multiple_wins;
    min_fan = opt.min_fan;
    max_fan = opt.max_fan;
  }
  public String toString() {
    StringBuffer buf = new StringBuffer(20);
    buf.append("options: {");
    if (count_flowers) buf.append('F');
    if (multiple_wins) buf.append('M');
    buf.append("" + min_fan);
    buf.append('}');
    for (int i = 16 - buf.length(); i > 0; i--) buf.append(' ');
    return buf.toString();
  }
}

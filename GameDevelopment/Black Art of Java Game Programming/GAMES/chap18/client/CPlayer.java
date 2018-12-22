package client;

class CPlayer {
  public int id = -1;
  public String name = null;
  public String host = null;
  public CTable table = null;
  public int seat = -1;
  public int balance = 0;
  public String print_text = null;

  public String toString () {
    int i = 0, n = 0, l = 0;
    StringBuffer buf = new StringBuffer(80);
    buf.append('[').append(id).append(']');
    l = buf.length();
    for (i = 0; i < 7 - l; i++) buf.append(' ');
    n = name.length() > 20 ? 20 : name.length();
    buf.append(name.substring(0, n));
    for (i = 0; i < 23 - n; i++) buf.append(' ');
    buf.append(" " + balance);
    n = buf.length();
    for (i = 0; i < 39 - n; i++) buf.append(' ');
    if (table != null && seat >= 0 && seat < 4) {
      buf.append("Table ").append(table.id).append("  ");
    }
    n = buf.length();
    for (i = 0; i < 50 - n; i++) buf.append(' ');
    buf.append('@').append(host);
    buf.append('\n');
    return print_text = buf.toString();
  }
}
